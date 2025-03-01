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
#include "banks.hpp"
#include "Button.hpp"
#include "Configuration.hpp"
#include "fetch.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "io.hpp"
#include "loader.hpp"
#include "nlohmann/json.hpp"
#include "PkmUtils.hpp"
#include "printerator.hpp"
#include "random.hpp"
#include "revision.h"
#include "thread.hpp"
#include "TitleLoadScreen.hpp"
#include "utils/crypto.hpp"
#include "utils/format.hpp"
#include "website.h"
#include <3ds.h>
#include <array>
#include <atomic>
#include <format>
#include <malloc.h>
#include <stdio.h>
#include <sys/stat.h>

// #include <chrono>

namespace
{
    u32 old_time_limit;
    Handle hbldrHandle;
    std::atomic_flag moveIcon     = ATOMIC_FLAG_INIT;
    std::atomic_flag doCartScan   = ATOMIC_FLAG_INIT;
    std::atomic_flag continueI18N = ATOMIC_FLAG_INIT;

    struct asset
    {
        std::string url;
        std::string path;
        decltype(pksm::crypto::sha256({})) hash;
    };

    asset assets[2] = {
        {CDN_URL "assets/pkm_spritesheet.t3x",   "/3ds/PKSM/assets/pkm_spritesheet.t3x",
         {0xc5, 0x4b, 0x46, 0x4d, 0xe9, 0xe5, 0x6f, 0x5b, 0x04, 0xc7, 0xd6, 0x79, 0xbd, 0xf0,
                0xb9, 0xb6, 0xc8, 0x4d, 0xbe, 0xa5, 0x55, 0x5b, 0xb7, 0xae, 0x62, 0x86, 0x2b, 0x18,
                0x62, 0x08, 0x10, 0x32}},
        {CDN_URL "assets/types_spritesheet.t3x", "/3ds/PKSM/assets/types_spritesheet.t3x",
         {0x9f, 0xba, 0xa1, 0x0f, 0xe2, 0x05, 0xce, 0x57, 0xcf, 0x87, 0x32, 0xc3, 0x7f, 0x72,
                0x42, 0x02, 0x04, 0xf9, 0x06, 0xd7, 0x5c, 0x65, 0xff, 0xae, 0xe8, 0xbf, 0x61, 0x5a,
                0x08, 0xe4, 0x86, 0x85}}
    };

    bool matchSha256HashFromFile(
        const std::string& path, const decltype(pksm::crypto::sha256({}))& sha)
    {
        bool match = false;
        auto in    = Archive::sd().file(path, FS_OPEN_READ);
        if (in)
        {
            size_t size = in->size();
            char* data  = new char[size];
            in->read(data, size);
            auto hash = pksm::crypto::sha256({(u8*)data, size});
            delete[] data;
            match = sha == hash;
            in->close();
        }
        return match;
    }

    bool assetsMatch(void)
    {
        for (const auto& item : assets)
        {
            if (!io::exists(item.path))
            {
                return false;
            }
            if (!matchSha256HashFromFile(item.path, item.hash))
            {
                return false;
            }
        }
        return true;
    }

    Result downloadAdditionalAssets(void)
    {
        Result res = 0;

        for (const auto& item : assets)
        {
            bool downloadAsset = true;
            if (io::exists(item.path))
            {
                if (matchSha256HashFromFile(item.path, item.hash))
                {
                    downloadAsset = false;
                }
                else
                {
                    std::remove(item.path.c_str());
                }
            }
            if (downloadAsset)
            {
                u32 status;
                ACU_GetWifiStatus(&status);
                if (status == 0)
                {
                    return -1;
                }
                Result res1 = Fetch::download(item.url, item.path);
                if (R_FAILED(res1))
                {
                    return res1;
                }
            }
        }
        return res;
    }

    Result consoleDisplayError(const std::string& message, Result res)
    {
        moveIcon.clear();
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
        std::string url = "", path = "", retString = "";
        if (auto fetch = Fetch::init("https://api.github.com/repos/FlagBrew/PKSM/releases/latest",
                true, &retString, nullptr, ""))
        {
            moveIcon.clear();
            Gui::waitFrame(i18n::localize("UPDATE_CHECKING"));
            auto res = Fetch::perform(fetch);
            if (res.index() == 1)
            {
                if (std::get<1>(res) != CURLE_OK)
                {
                    Gui::error(i18n::localize("CURL_ERROR"), std::get<1>(res) + 100);
                }
                else
                {
                    long status_code;
                    fetch->getinfo(CURLINFO_RESPONSE_CODE, &status_code);
                    switch (status_code)
                    {
                        case 200:
                        {
                            nlohmann::json retJson =
                                nlohmann::json::parse(retString, nullptr, false);
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
                                int newMinor = std::stoi(newVersion.substr(pos + 1), &pos2);
                                int newMicro = std::stoi(newVersion.substr(pos + pos2 + 2));

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
                            Gui::error(i18n::localize("HTTP_OFFLINE"), status_code);
                            break;
                        default:
                            Gui::error(i18n::localize("HTTP_UNKNOWN_ERROR"), status_code);
                            break;
                    }
                }
            }
        }
        if (!url.empty())
        {
            Gui::waitFrame(i18n::localize("UPDATE_FOUND_BACKUP"));
            backupExtData();
            backupBanks();
            Gui::waitFrame(i18n::localize("UPDATE_FOUND_DOWNLOAD"));
            std::string fileName = path.substr(path.find_last_of('/') + 1);
            Result res           = Fetch::download(
                url, path, "",
                [](void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal,
                    curl_off_t ulnow)
                {
                    Gui::showDownloadProgress(*(std::string*)clientp, dlnow / 1024, dltotal / 1024);
                    return 0;
                },
                &fileName);
            if (R_FAILED(res))
            {
                Gui::error(i18n::localize("UPDATE_FOUND_BUT_FAILED_DOWNLOAD"), res);
                Archive::sd().deleteFile(path);
                return false;
            }

            Gui::waitFrame(i18n::localize("UPDATE_INSTALLING"));
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

        while (doCartScan.test_and_set())
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
                    while (!power && doCartScan.test_and_set())
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

        while (moveIcon.test_and_set())
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
    }

    void i18nThread()
    {
        static constexpr pksm::Language languages[] = {pksm::Language::JPN, pksm::Language::ENG,
            pksm::Language::FRE, pksm::Language::ITA, pksm::Language::GER, pksm::Language::SPA,
            pksm::Language::KOR, pksm::Language::CHS, pksm::Language::CHT, pksm::Language::NL,
            pksm::Language::PT, pksm::Language::RU, pksm::Language::RO};
        for (const auto& i : languages)
        {
            if (continueI18N.test_and_set())
            {
                i18n::init(i);
            }
            else
            {
                return;
            }
        }
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

    Result res;

    srand(time(NULL));

    hidInit();
    gfxInitDefault();
    Threads::init(0, 2);

    moveIcon.test_and_set();
    Threads::create(iconThread);

    if (R_FAILED(res = svcConnectToPort(&hbldrHandle, "hb:ldr")))
    {
        return consoleDisplayError(
            "Rosalina sysmodule has not been found.\n\nMake sure you're running latest Luma3DS.",
            res);
    }
    APT_GetAppCpuTimeLimit(&old_time_limit);
    APT_SetAppCpuTimeLimit(30);

    if (R_FAILED(res = cfguInit()))
    {
        return consoleDisplayError("cfguInit failed.", res);
    }
    if (R_FAILED(res = romfsInit()))
    {
        return consoleDisplayError("romfsInit failed.", res);
    }
    if (R_FAILED(res = Archive::init(execPath)))
    {
        return consoleDisplayError("Archive::init failed.", res);
    }
    if (R_FAILED(res = pxiDevInit()))
    {
        return consoleDisplayError("pxiDevInit failed.", res);
    }
    if (R_FAILED(res = amInit()))
    {
        return consoleDisplayError("amInit failed.", res);
    }
    if (R_FAILED(res = acInit()))
    {
        return consoleDisplayError("acInit failed.", res);
    }
    if (R_FAILED(res = nsInit()))
    {
        return consoleDisplayError("nsInit failed.", res);
    }

    u32* socketBuffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);
    if (socketBuffer == NULL)
    {
        return consoleDisplayError("Failed to create socket buffer.", -1);
    }
    if (socInit(socketBuffer, SOC_BUFFERSIZE))
    {
        return consoleDisplayError("socInit failed.", -1);
    }

    if (CURLcode code = curl_global_init(CURL_GLOBAL_NOTHING))
    {
        return consoleDisplayError("cURL init failed", (Result)code);
    }

    if (R_FAILED(Fetch::initMulti()))
    {
        return consoleDisplayError("Initializing network connection failed.", -1);
    }

    // link3dsStdio();

    if (R_FAILED(res = downloadAdditionalAssets()))
    {
        return consoleDisplayError("Additional assets download failed.\n\nAlways make sure you're "
                                   "connected to the internet and on the lastest version.",
            res);
    }
    if (R_FAILED(res = Gui::init()))
    {
        return consoleDisplayError("Gui::init failed.", res);
    }

    i18n::addCallbacks(i18n::initGui, i18n::exitGui);
    moveIcon.clear();
    i18n::init(Configuration::getInstance().language());

    PkmUtils::initDefaults();

    if (!assetsMatch())
    {
        Gui::warn("Additional assets are not correct.\nPress A to start PKSM update...");
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

    if (Configuration::getInstance().autoUpdate() && update(execPath))
    {
        return rebootToPKSM(execPath);
    }

    if (R_FAILED(res = Banks::init()))
    {
        return consoleDisplayError("Banks::init failed.", res);
    }

    TitleLoader::init();

    Threads::executeTask(TitleLoader::scanTitles);
    TitleLoader::scanSaves();

    doCartScan.test_and_set();
    Threads::create(cartScan);

    continueI18N.test_and_set();
    Threads::executeTask(i18nThread);

    Gui::setScreen(std::make_unique<TitleLoadScreen>());
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
    moveIcon.clear();
    continueI18N.clear();
    svcCloseHandle(hbldrHandle);
    TitleLoader::exit();
    Gui::exit();
    Fetch::exitMulti();
    curl_global_cleanup();
    socExit();
    nsExit();
    acExit();
    doCartScan.clear();
    Threads::exit();
    i18n::exit();
    amExit();
    pxiDevExit();
    Archive::exit();
    romfsExit();
    cfguExit();

    if (old_time_limit != UINT32_MAX)
    {
        APT_SetAppCpuTimeLimit(old_time_limit);
    }

    gfxExit();
    hidExit();

    return 0;
}

void App::end() {}
