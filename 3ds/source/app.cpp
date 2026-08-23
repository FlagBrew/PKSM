/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2025 Bernardo Giordano, Admiral Fish, piepie62
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
 *       * Requiring preservation of specified reasonable legal notices or
 *         author attributions in that material or in the Appropriate Legal
 *         Notices displayed by works containing it.
 *       * Prohibiting misrepresentation of the origin of that material,
 *         or requiring that modified versions of such material be marked in
 *         reasonable ways as different from the original version.
 */

#include "app.hpp"
#include "appIcon.hpp"
#include "Archive.hpp"
#include "AssetStore.hpp"
#include "banks.hpp"
#include "Button.hpp"
#include "Configuration.hpp"
#include "fetch.hpp"
#include "gui.hpp"
#include "GuiPresenter.hpp"
#include "i18n_ext.hpp"
#include "loader.hpp"
#include "nlohmann/json.hpp"
#include "PkmUtils.hpp"
#include "printerator.hpp"
#include "random.hpp"
#include "revision.h"
#include "ScreenStack.hpp"
#include "Subsystems.hpp"
#include "thread.hpp"
#include "TitleLoadScreen.hpp"
#include "utils/format.hpp"
#include "utils/logging.hpp"
#include "utils/server.hpp"
#include "website.h"
#include <3ds.h>
#include <array>
#include <atomic>
#include <cstdlib>
#include <format>
#include <malloc.h>
#include <stdio.h>
#include <string_view>
#include <sys/stat.h>

// #include <chrono>

namespace
{
    u32 old_time_limit;
    // Never opened: the hb:ldr connection above HBLDR_SetTarget is commented out, so this
    // stays zero and the chainload path fails rather than closing a handle it never had.
    Handle hbldrHandle;
    std::atomic_flag moveIcon         = ATOMIC_FLAG_INIT;
    std::atomic_flag doCartScan       = ATOMIC_FLAG_INIT;
    std::atomic<bool> iconThreadAlive = false;
    std::atomic<bool> cartScanAlive   = false;
    // Held for as long as soc is up, and freed with it. It used to be allocated and never
    // released at all.
    u32* socketBuffer = nullptr;

    // Everything PKSM brings up, in the order it came up. Acquiring records the teardown,
    // so App::exit unwinds exactly this much and no more - including when startup stopped
    // partway, which is a routine path (no wifi, missing assets, corrupt ext data).
    pksm::Subsystems subsystems;

    // Signal the splash/icon thread to stop and block until it has actually
    // finished its last framebuffer write and buffer swap. Drawing to the top
    // screen (or touching the GPU) before the thread is gone races with it and
    // corrupts the top screen.
    void stopIconThread()
    {
        moveIcon.clear();
        while (iconThreadAlive.load())
        {
            svcSleepThread(100'000); // 0.1 ms
        }
    }

    // Clearing a flag only asks a thread to stop, and its teardown runs the moment this
    // returns, so wait for it to actually be gone. Bounded, because a thread that never
    // started - a worker that could not be spawned, a task still queued - would otherwise
    // hang the exit path forever.
    bool waitForStop(const std::atomic<bool>& alive)
    {
        constexpr int MAX_WAIT_TICKS = 20000; // 0.1 ms each, so two seconds
        for (int tick = 0; alive.load() && tick < MAX_WAIT_TICKS; tick++)
        {
            svcSleepThread(100'000);
        }
        return !alive.load();
    }

    void stopCartScan()
    {
        doCartScan.clear();
        if (!waitForStop(cartScanAlive))
        {
            Logging::warning("The cart scan thread did not stop");
        }
    }

    // What the console error screen says when a step does not come up. The subsystem's own
    // name carries it, so adding a step to the sequence needs nothing here.
    std::string startupErrorMessage(std::string_view name)
    {
        if (name == "assets")
        {
            return "Additional assets download failed.\n\nAlways make sure you're connected to "
                   "the internet and on the lastest version.";
        }
        return std::format("{:s} failed to initialize.", name);
    }

    void logSubsystemEvent(std::string_view name, pksm::Subsystems::Event event)
    {
        Logging::startupLog(
            std::string(name), event == pksm::Subsystems::Event::Acquired ? "init ok" : "exited");
    }

    // Everything about the downloaded spritesheets: what they should be, and whether
    // they are. Asked once on the way up, and again after the GUI is drawing with them -
    // the second time from the memo, without touching the SD card.
    AssetStore assetStore;

    Result consoleDisplayError(const std::string& message, Result res)
    {
        stopIconThread();
        consoleInit(GFX_TOP, nullptr);

        std::format_to(Printerator{}, "\x1b[2;16H\x1b[34mPKSM v{:d}.{:d}.{:d}-{:s}\x1b[0m",
            VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO, GIT_REV);
        std::format_to(
            Printerator{}, "\x1b[5;1HError during startup: \x1b[31m0x{:08X}\x1b[0m", (u32)res);
        std::format_to(Printerator{}, "\x1b[8;1HDescription: \x1b[33m{:s}\x1b[0m", message);
        std::format_to(Printerator{}, "\x1b[29;16HPress START to exit.");
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
        while (aptMainLoop() && !(hidKeysDown() & KEY_START))
        {
            hidScanInput();
        }
        return res;
    }

    // Shown when the PKSM ext data archive is corrupt (#1558). A resets and recreates it
    // (losing any banks stored in ext data, which are already unreadable), START exits.
    bool confirmExtdataReset()
    {
        stopIconThread();
        consoleInit(GFX_TOP, nullptr);

        std::format_to(Printerator{}, "\x1b[2;16H\x1b[34mPKSM v{:d}.{:d}.{:d}-{:s}\x1b[0m",
            VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO, GIT_REV);
        std::format_to(Printerator{}, "\x1b[5;1HPKSM ext data appears to be corrupted.");
        std::format_to(
            Printerator{}, "\x1b[8;1HPress \x1b[32mA\x1b[0m to reset it and continue.\x1b[0m");
        std::format_to(Printerator{}, "\x1b[9;1HStorage banks kept in ext data will be lost.");
        std::format_to(Printerator{}, "\x1b[29;16HPress \x1b[31mSTART\x1b[0m to exit.\x1b[0m");
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
        while (aptMainLoop())
        {
            hidScanInput();
            u32 down = hidKeysDown();
            if (down & KEY_A)
            {
                return true;
            }
            if (down & KEY_START)
            {
                return false;
            }
        }
        return false;
    }

    Result HBLDR_SetTarget(const char* path)
    {
        u32 pathLen = strlen(path) + 1;
        u32* cmdbuf = getThreadCommandBuffer();

        cmdbuf[0] = IPC_MakeHeader(2, 0, 2); // 0x20002
        cmdbuf[1] = IPC_Desc_StaticBuffer(pathLen, 0);
        cmdbuf[2] = (u32)path;

        Result rc = svcSendSyncRequest(hbldrHandle);
        if (R_SUCCEEDED(rc))
        {
            rc = cmdbuf[1];
        }
        return rc;
    }

    void backupExtData()
    {
        Archive::copyDir(Archive::data(), u"/", Archive::sd(), u"/3ds/PKSM/extDataBackup");
    }

    void backupBanks()
    {
        Archive::copyDir(Archive::sd(), u"/3ds/PKSM/banks", Archive::sd(), u"/3ds/PKSM/banksBkp");
    }

    bool update(std::string execPath)
    {
        u32 status;
        ACU_GetWifiStatus(&status);
        if (status == 0)
        {
            return false;
        }

        execPath        = execPath.substr(execPath.find(':') + 1);
        std::string url = "", path = "";

        stopIconThread();
        Gui::waitFrame(i18n::localize("UPDATE_CHECKING"), ScreenTarget::TOP);
        auto response = Fetch::get("https://api.github.com/repos/FlagBrew/PKSM/releases/latest");
        if (!response.ok())
        {
            Gui::error(i18n::localize("CURL_ERROR"), response.code);
        }
        else
        {
            switch (response.status)
            {
                case 200:
                {
                    nlohmann::json retJson = nlohmann::json::parse(response.body, nullptr, false);
                    if (retJson.is_discarded() || !retJson.contains("tag_name") ||
                        !retJson["tag_name"].is_string())
                    {
                        Gui::warn(i18n::localize("UPDATE_CHECK_ERROR_BAD_JSON_1") + '\n' +
                                  i18n::localize("UPDATE_CHECK_ERROR_BAD_JSON_2"));
                    }
                    else
                    {
                        std::string newVersion = retJson["tag_name"].get<std::string>();
                        size_t pos             = 0;
                        size_t pos2            = 0;
                        int newMajor           = std::stoi(newVersion, &pos);
                        int newMinor           = std::stoi(newVersion.substr(pos + 1), &pos2);
                        int newMicro           = std::stoi(newVersion.substr(pos + pos2 + 2));

                        if (newMajor > VERSION_MAJOR ||
                            (newMajor == VERSION_MAJOR && newMinor > VERSION_MINOR) ||
                            (newMajor == VERSION_MAJOR && newMinor == VERSION_MINOR &&
                                newMicro > VERSION_MICRO))
                        {
                            url = "https://github.com/FlagBrew/PKSM/releases/download/" +
                                  newVersion + "/PKSM";
                            if (execPath != "")
                            {
                                url += ".3dsx";
                                path = execPath + ".new";
                            }
                            else
                            {
                                url += ".cia";
                                path = "/3ds/PKSM/PKSM.cia";
                            }
                        }
                    }
                    break;
                }
                case 502:
                    Gui::error(i18n::localize("HTTP_OFFLINE"), response.status);
                    break;
                default:
                    Gui::error(i18n::localize("HTTP_UNKNOWN_ERROR"), response.status);
                    break;
            }
        }
        if (!url.empty())
        {
            Gui::waitFrame(i18n::localize("UPDATE_FOUND_BACKUP"), ScreenTarget::TOP);
            backupExtData();
            backupBanks();
            Gui::waitFrame(i18n::localize("UPDATE_FOUND_DOWNLOAD"), ScreenTarget::TOP);
            std::string fileName = path.substr(path.find_last_of('/') + 1);
            Result res           = Fetch::download(url, path, [&fileName](u64 now, u64 total)
                          { Gui::showDownloadProgress(fileName, now / 1024, total / 1024); });
            if (R_FAILED(res))
            {
                Gui::error(i18n::localize("UPDATE_FOUND_BUT_FAILED_DOWNLOAD"), res);
                Archive::sd().deleteFile(path);
                return false;
            }

            Gui::waitFrame(i18n::localize("UPDATE_INSTALLING"), ScreenTarget::TOP);
            if (execPath != "")
            {
                // Stop using the 3DSX
                romfsExit();
                if (R_FAILED(Archive::moveFile(Archive::sd(), path, Archive::sd(), execPath)))
                {
                    // RUN, THE INSTALL FAILED
                    romfsInit();
                    Archive::sd().deleteFile(path);
                    return false;
                }
                else
                {
                    // No need to reinit ROMFS, as we're definitely about to reboot
                    // And if we don't reboot, then catastrophic errors are likely. Honestly,
                    // probably a good thing
                    return true;
                }
            }
            else
            {
                // Adapted from https://github.com/joel16/3DShell/blob/master/source/cia.c
                AM_TitleEntry title;
                Handle dstHandle;
                auto ciaFile = Archive::sd().file(path, FS_OPEN_READ);
                if (ciaFile)
                {
                    if (R_FAILED(res = AM_GetCiaFileInfo(
                                     MEDIATYPE_SD, &title, std::get<0>(ciaFile->getRawHandle()))))
                    {
                        Gui::error(i18n::localize("BAD_CIA_FILE"), res);
                        ciaFile->close();
                        return false;
                    }

                    if (R_FAILED(res = AM_StartCiaInstall(MEDIATYPE_SD, &dstHandle)))
                    {
                        Gui::error(i18n::localize("CIA_INSTALL_START_FAIL"), res);
                        ciaFile->close();
                        return false;
                    }

                    size_t bufSize = 0x10000;
                    u8* buf        = (u8*)memalign(0x1000, bufSize);
                    u8 backupBuf[0x1000];
                    if (!buf)
                    {
                        buf     = backupBuf;
                        bufSize = sizeof(backupBuf);
                    }
                    u32 bytesWritten, bytesRead;
                    u64 offset          = 0;
                    bool ciaInstallGood = true;
                    do
                    {
                        memset(buf, 0, bufSize);

                        bytesRead = ciaFile->read(buf, bufSize);
                        if (R_FAILED(ciaFile->result()))
                        {
                            Gui::error(i18n::localize("CIA_UPDATE_READ_FAIL"), ciaFile->result());
                            ciaFile->close();
                            FSFILE_Close(dstHandle);
                            return false;
                        }

                        if (R_FAILED(res = FSFILE_Write(dstHandle, &bytesWritten, offset, buf,
                                         bytesRead, FS_WRITE_FLUSH)))
                        {
                            Gui::error(i18n::localize("CIA_UPDATE_WRITE_FAIL"), res);
                            ciaFile->close();
                            FSFILE_Close(dstHandle);
                            return false;
                        }

                        if (bytesWritten != bytesRead)
                        {
                            ciaInstallGood = false;
                        }

                        offset += bytesRead;
                    }
                    while (offset < ciaFile->size() && ciaInstallGood);

                    if (buf != backupBuf)
                    {
                        free(buf);
                    }

                    if (!ciaInstallGood)
                    {
                        AM_CancelCIAInstall(dstHandle);
                        ciaFile->close();
                        Gui::warn("Bytes written doesn't match bytes read:\n" +
                                  std::to_string(bytesWritten) + " vs " +
                                  std::to_string(bytesRead));
                        return false;
                    }

                    if (R_FAILED(res = AM_FinishCiaInstall(dstHandle)))
                    {
                        Gui::error(i18n::localize("CIA_INSTALL_FINISH_FAIL"), res);
                        ciaFile->close();
                        return false;
                    }

                    ciaFile->close();

                    Archive::sd().deleteFile(path);

                    return true;
                }
            }
        }
        return false;
    }

    void cartScan()
    {
        bool oldCardIn;
        FSUSER_CardSlotIsInserted(&oldCardIn);

        while (doCartScan.test())
        {
            bool cardIn = false;

            FSUSER_CardSlotIsInserted(&cardIn);
            if (cardIn != oldCardIn)
            {
                bool power;
                FSUSER_CardSlotGetCardIFPowerStatus(&power);
                if (cardIn)
                {
                    if (!power)
                    {
                        FSUSER_CardSlotPowerOn(&power);
                    }
                    while (!power && doCartScan.test())
                    {
                        FSUSER_CardSlotGetCardIFPowerStatus(&power);
                    }
                    svcSleepThread(500'000'000);
                    for (size_t i = 0; i < 10; i++)
                    {
                        if ((oldCardIn = TitleLoader::scanCard()))
                        {
                            break;
                        }
                    }
                }
                else
                {
                    FSUSER_CardSlotPowerOff(&power);
                    TitleLoader::scanCard();
                    oldCardIn = false;
                }
            }
        }

        cartScanAlive = false;
    }

    void iconThread()
    {
        u16 w, h;
        int xIcon = 176, yIcon = 96, splashIconMargin = 4, glowWidth = 2;
        float time        = 0.0f;
        const float speed = 0.025f;

        // Add particles for visual interest
        constexpr int NUM_PARTICLES = 40;

        struct Particle
        {
            float x, y;
            float speed;
            float size;
            float alpha;
        };

        Particle particles[NUM_PARTICLES];

        // Initialize particles
        for (int i = 0; i < NUM_PARTICLES; i++)
        {
            particles[i].x     = rand() % 400;
            particles[i].y     = rand() % 240;
            particles[i].speed = 0.2f + (rand() % 30) / 100.0f;
            particles[i].size  = 1 + (rand() % 3);
            particles[i].alpha = 0.3f + (rand() % 70) / 100.0f;
        }

        while (moveIcon.test())
        {
            u8* fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &w, &h);

            // Draw gradient pattern
            for (int x = 0; x < 400; x += 2)
            {
                float xRatio = (float)x / 400.0f;
                float xWave  = sin(xRatio * 6.0f + time) * 0.5f + 0.5f;

                for (int y = 0; y < 240; y += 2)
                {
                    // Skip pixels in the icon area
                    if (x >= xIcon && x < xIcon + 48 && y >= yIcon && y < yIcon + 48)
                    {
                        continue;
                    }

                    bool glow = false;
                    if ((x >= xIcon - splashIconMargin && x < xIcon + 48 + splashIconMargin &&
                            y >= yIcon - splashIconMargin && y < yIcon + 48 + splashIconMargin))
                    {
                        if ((x >= xIcon - glowWidth && x < xIcon + 48 + glowWidth &&
                                y >= yIcon - glowWidth && y < yIcon + 48 + glowWidth))
                        {
                            glow = true;
                        }

                        u8 r, g, b;
                        if (glow)
                        {
                            float highlight_multiplier =
                                fmax(0.0, fabs(fmod(time, 1.0) - 0.5) / 0.5);
                            r                = COLOR_SELECTOR.r;
                            g                = COLOR_SELECTOR.g;
                            b                = COLOR_SELECTOR.b;
                            PKSM_Color color = PKSM_Color(r + (255 - r) * highlight_multiplier,
                                g + (255 - g) * highlight_multiplier,
                                b + (255 - b) * highlight_multiplier, 255);
                            r                = color.r;
                            g                = color.g;
                            b                = color.b;
                        }
                        else
                        {
                            r = g = b = 0;
                        }

                        // Set 2x2 pixel blocks for better performance
                        for (int dx = 0; dx < 2 && x + dx < 400; dx++)
                        {
                            for (int dy = 0; dy < 2 && y + dy < 240; dy++)
                            {
                                u8* pixel = fb + (x + dx) * 3 * 240 + (y + dy) * 3;
                                pixel[0]  = r;
                                pixel[1]  = g;
                                pixel[2]  = b;
                            }
                        }
                        continue;
                    }

                    float yRatio = (float)y / 240.0f;

                    // Create smooth color transitions
                    u8 r = (u8)(255 * (sin(time + xRatio * 3.14f) * 0.5f + 0.5f));
                    u8 g = (u8)(255 * (cos(time * 0.7f + yRatio * 3.14f) * 0.5f + 0.5f));
                    u8 b =
                        (u8)(255 *
                             (xWave * (sin(xRatio * yRatio * 10.0f + time * 1.1f) * 0.5f + 0.5f)));

                    // Set 2x2 pixel blocks for better performance
                    for (int dx = 0; dx < 2 && x + dx < 400; dx++)
                    {
                        for (int dy = 0; dy < 2 && y + dy < 240; dy++)
                        {
                            u8* pixel = fb + (x + dx) * 3 * 240 + (y + dy) * 3;
                            pixel[0]  = r;
                            pixel[1]  = g;
                            pixel[2]  = b;
                        }
                    }
                }
            }

            // Draw particles
            for (int i = 0; i < NUM_PARTICLES; i++)
            {
                // Update particle position
                particles[i].y -= particles[i].speed;
                if (particles[i].y < 0)
                {
                    particles[i].y = 240;
                    particles[i].x = rand() % 400;
                }

                // Skip particles in icon area
                if (particles[i].x >= xIcon - splashIconMargin &&
                    particles[i].x < xIcon + 48 + splashIconMargin &&
                    particles[i].y >= yIcon - splashIconMargin &&
                    particles[i].y < yIcon + 48 + splashIconMargin)
                {
                    continue;
                }

                // Draw particle with alpha blending
                int size  = particles[i].size;
                int alpha = (int)(particles[i].alpha * 255);
                for (int px = 0; px < size && (int)particles[i].x + px < 400; px++)
                {
                    for (int py = 0; py < size && (int)particles[i].y + py < 240; py++)
                    {
                        u8* p = fb + ((int)particles[i].x + px) * 3 * 240 +
                                ((int)particles[i].y + py) * 3;
                        p[0] = (p[0] * (255 - alpha) + 255 * alpha) / 255;
                        p[1] = (p[1] * (255 - alpha) + 255 * alpha) / 255;
                        p[2] = (p[2] * (255 - alpha) + 255 * alpha) / 255;
                    }
                }
            }

            int xOff = 0;
            for (const auto& line : bootSplash)
            {
                std::copy(line.begin(), line.end(),
                    gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &w, &h) + (xIcon + xOff++) * 3 * 240 +
                        yIcon * 3);
            }

            time += speed;

            gfxFlushBuffers();
            gfxSwapBuffersGpu();
            gspWaitForVBlank();
        }

        iconThreadAlive = false;
    }

    Result rebootToPKSM(const std::string& execPath)
    {
        Result res = -1;
        if (execPath.empty())
        {
            aptSetChainloaderToSelf();
            res = 0;
        }
        else
        {
            std::string path = execPath.substr(execPath.find('/'));
            res              = HBLDR_SetTarget(path.c_str());
        }
        if (R_FAILED(res))
        {
            Gui::warn(
                i18n::localize("UPDATE_SUCCESS_1") + '\n' + i18n::localize("UPDATE_SUCCESS_2"));
        }
        return -1;
    }
}

Result App::init(const std::string& execPath)
{
    // auto start = std::chrono::high_resolution_clock::now();

    srand(time(NULL));

    // if (R_FAILED(res = svcConnectToPort(&hbldrHandle, "hb:ldr")))
    // {
    //     return consoleDisplayError(
    //         "Rosalina sysmodule has not been found.\n\nMake sure you're running latest Luma3DS.",
    //         res);
    // }

    // The order below is the whole startup sequence, and the only place it is written down.
    // Chained with &&, so the first step that does not come up stops the rest; whatever did
    // come up is held, and App::exit releases it in reverse. The console is up first so
    // there is something to report a failure on.
    const bool platformUp =
        subsystems.acquire("hid", hidInit, hidExit) &&
        subsystems.acquire("gfx", gfxInitDefault, gfxExit) &&
        // The observer logs every later step, so it goes in as soon as there is a log to
        // write to. hid and gfx come up before it and are not logged, as before.
        subsystems.acquire(
            "logging",
            []
            {
                Logging::init();
                subsystems.observe(logSubsystemEvent);
            },
            Logging::exit) &&
        subsystems.acquire(
            "cpu",
            []
            {
                APT_GetAppCpuTimeLimit(&old_time_limit);
                APT_SetAppCpuTimeLimit(30);
            },
            []
            {
                if (old_time_limit != UINT32_MAX)
                {
                    APT_SetAppCpuTimeLimit(old_time_limit);
                }
            }) &&
        subsystems.acquire("cfgu", cfguInit, cfguExit) &&
        subsystems.acquire("romfs", romfsInit, romfsExit) &&
        subsystems.acquire(
            "archive", [&execPath] { return Archive::init(execPath, confirmExtdataReset); },
            Archive::exit) &&
        subsystems.acquire("file logging", Logging::initFileLogging) &&
        // Threads comes up after everything a worker touches on its way out, so joining
        // them at teardown happens while the archive and the services are still there.
        subsystems.acquire(
            "threads", [] { return Threads::init(0, 2); }, Threads::exit) &&
        subsystems.acquire(
            "splash",
            []
            {
                moveIcon.test_and_set();
                iconThreadAlive = true;
                if (!Threads::create(iconThread))
                {
                    iconThreadAlive = false;
                }
            },
            stopIconThread) &&
        subsystems.acquire("pxiDev", pxiDevInit, pxiDevExit) &&
        subsystems.acquire("am", amInit, amExit) && subsystems.acquire("ac", acInit, acExit) &&
        subsystems.acquire("ns", nsInit, nsExit) &&
        subsystems.acquire(
            "socket buffer",
            []
            {
                socketBuffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);
                return socketBuffer != nullptr;
            },
            []
            {
                free(socketBuffer);
                socketBuffer = nullptr;
            }) &&
        subsystems.acquire(
            "soc", [] { return socInit(socketBuffer, SOC_BUFFERSIZE) == 0; }, socExit) &&
        subsystems.acquire("network", Fetch::init, Fetch::exit) &&
        subsystems.acquire("server", Server::init, Server::exit) &&
        subsystems.acquire("assets", [] { return assetStore.ensure(); }) &&
        subsystems.acquire("gui", Gui::init, Gui::exit) &&
        subsystems.acquire(
            "i18n",
            []
            {
                i18n::addCallbacks(i18n::initGui, i18n::exitGui);
                // Nothing may draw to the top screen while the splash thread still owns it.
                // Stopping it here rather than at teardown costs nothing: the handshake is
                // idempotent, so the token's release is a no-op afterwards.
                stopIconThread();
                // Only the language the user reads. The other twelve materialize on first
                // use through checkInitialized, which is what the interface already does
                // for a save written in a language that is not this one.
                i18n::init(Configuration::getInstance().language());
            },
            i18n::exit) &&
        // Only now is there something to draw with and a language to draw in. Installing the
        // presenter also drains whatever the domain reported on the way up - Configuration
        // parses before any of this exists, and its warnings have been waiting here since.
        subsystems.acquire(
            "presenter", [] { pksm::present::install(GuiPresenter::create()); },
            pksm::present::uninstall) &&
        subsystems.acquire("pkm", PkmUtils::initDefaults);

    if (!platformUp)
    {
        const auto& failure = *subsystems.failure();
        return consoleDisplayError(startupErrorMessage(failure.name), failure.status);
    }

    // Not lifetime: two decisions that can end this run by rebooting into a new build.
    if (assetStore.verdict() != AssetStore::Verdict::Ok)
    {
        Gui::warn("Additional assets are not correct.\nPress A to start PKSM update");
        if (!update(execPath))
        {
            Gui::warn("PKSM update failed.\nTry downloading assets manually before restarting.");
            return -1;
        }
        else
        {
            return rebootToPKSM(execPath);
        }
    }
    Logging::startupLog("gui", "assets match");

    if (Configuration::getInstance().autoUpdate() && update(execPath))
    {
        return rebootToPKSM(execPath);
    }

    // The rest of the sequence, resumed now that this run is staying.
    const bool contentUp =
        subsystems.acquire("banks", Banks::init) &&
        subsystems.acquire("titles", TitleLoader::init, TitleLoader::exit) &&
        subsystems.acquire("title scan", [] { Threads::executeTask(TitleLoader::scanTitles); }) &&
        subsystems.acquire("save scan", TitleLoader::scanSaves) &&
        subsystems.acquire(
            "cart scan",
            []
            {
                doCartScan.test_and_set();
                cartScanAlive = true;
                if (!Threads::create(cartScan))
                {
                    cartScanAlive = false;
                }
            },
            stopCartScan);

    if (!contentUp)
    {
        const auto& failure = *subsystems.failure();
        return consoleDisplayError(startupErrorMessage(failure.name), failure.status);
    }

    // reinitialize both screens. The top screen format is restored explicitly because the
    // ext data corruption prompt (confirmExtdataReset) may have switched it to console mode.
    gfxSetScreenFormat(GFX_TOP, GSP_BGR8_OES);
    gfxSetDoubleBuffering(GFX_TOP, true);
    gfxSetScreenFormat(GFX_BOTTOM, GSP_BGR8_OES);
    gfxSetDoubleBuffering(GFX_BOTTOM, true);
    gfxSwapBuffersGpu();
    gspWaitForVBlank();

    ScreenStack::push(std::make_unique<TitleLoadScreen>());
    // uncomment when needing to debug with GDB
    // consoleDebugInit(debugDevice_SVC);

    // auto end = std::chrono::high_resolution_clock::now();
    // printf("Startup completed in: %sus\n",
    // std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end -
    // start).count()).c_str());
    return 0;
}

Result App::exit(void)
{
    Logging::info("Exiting PKSM");
    subsystems.releaseAll();
    return 0;
}

void App::end() {}
