/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2019 Bernardo Giordano, Admiral Fish, piepie62
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
#include "Configuration.hpp"
#include "TitleLoadScreen.hpp"
#include "appIcon.hpp"
#include "archive.hpp"
#include "banks.hpp"
#include "fetch.hpp"
#include "gui.hpp"
#include "i18n.hpp"
#include "loader.hpp"
#include "random.hpp"
#include "revision.h"
#include "sha256.h"
#include "thread.hpp"
#include <3ds.h>
#include <stdio.h>

// increase the stack in order to allow quirc to decode large qrs
int __stacksize__ = 64 * 1024;

static u32 old_time_limit;
static Handle hbldrHandle;

struct asset
{
    std::string url;
    std::string path;
    unsigned char hash[SHA256_BLOCK_SIZE];
};

static bool matchSha256HashFromFile(const std::string& path, unsigned char* sha)
{
    bool match = false;
    FSStream in(Archive::sd(), path, FS_OPEN_READ);
    if (in.good())
    {
        size_t size = in.size();
        char* data  = new char[size];
        in.read(data, size);
        char hash[SHA256_BLOCK_SIZE];
        sha256((unsigned char*)hash, (unsigned char*)data, size);
        delete[] data;
        match = memcmp(sha, hash, SHA256_BLOCK_SIZE) == 0;
    }
    in.close();
    return match;
}

static Result downloadAdditionalAssets(void)
{
    Result res      = 0;
    asset assets[2] = {
        {"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/pkm_spritesheet.t3x", "/3ds/PKSM/assets/pkm_spritesheet.t3x",
            {0xa5, 0x0e, 0x59, 0x75, 0x00, 0xf0, 0xe1, 0x6a, 0x6e, 0xe9, 0xd4, 0x5b, 0xb3, 0x3b, 0x9c, 0x08, 0xe8, 0x69, 0xc0, 0x1d, 0x10, 0x53, 0x3f,
                0xe0, 0xbe, 0x7e, 0x2c, 0xa4, 0xe7, 0x6d, 0xcc, 0x48}},
        {"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/types_spritesheet.t3x",
            "/3ds/PKSM/assets/types_spritesheet.t3x",
            {0xea, 0x7f, 0x92, 0x86, 0x0a, 0x9b, 0x4d, 0x50, 0x3a, 0x0c, 0x2a, 0x6e, 0x48, 0x60, 0xfb, 0x93, 0x1f, 0xd3, 0xd7, 0x7d, 0x6a, 0xbb, 0x1d,
                0xdb, 0xac, 0x59, 0xeb, 0xf1, 0x66, 0x34, 0xa4, 0x91}}};

    for (auto item : assets)
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
#if !CITRA_DEBUG
            u32 status;
            ACU_GetWifiStatus(&status);
            if (status == 0)
                return -1;
#endif
            Result res1 = Fetch::download(item.url, item.path);
            if (R_FAILED(res1))
                return res1;
            if (!matchSha256HashFromFile(item.path, item.hash))
            {
                std::remove(item.path.c_str());
                return -1;
            }
        }
    }
    return res;
}

static Result consoleDisplayError(const std::string& message, Result res)
{
    consoleInit(GFX_TOP, nullptr);
    printf("\x1b[2;16H\x1b[34mPKSM v%d.%d.%d-%s\x1b[0m", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO, GIT_REV);
    printf("\x1b[5;1HError during startup: \x1b[31m0x%08lX\x1b[0m", res);
    printf("\x1b[8;1HDescription: \x1b[33m%s\x1b[0m", message.c_str());
    printf("\x1b[29;16HPress START to exit.");
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
    while (aptMainLoop() && !(hidKeysDown() & KEY_START))
    {
        hidScanInput();
    }
    return res;
}

static Result HBLDR_SetTarget(const char* path)
{
    u32 pathLen = strlen(path) + 1;
    u32* cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(2, 0, 2); // 0x20002
    cmdbuf[1] = IPC_Desc_StaticBuffer(pathLen, 0);
    cmdbuf[2] = (u32)path;

    Result rc = svcSendSyncRequest(hbldrHandle);
    if (R_SUCCEEDED(rc))
        rc = cmdbuf[1];
    return rc;
}

static int progress_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    static auto oldTime = osGetTime();
    auto time           = osGetTime();
    if (dltotal != 0 && time >= oldTime + 1000)
    {
        Gui::showDownloadProgress(*(std::string*)clientp, dlnow / 1024, dltotal / 1024);
        oldTime = time;
    }
    return 0;
}

static bool update(std::string execPath)
{
    u32 status;
    ACU_GetWifiStatus(&status);
    if (status == 0)
    {
        return false;
    }
    execPath        = execPath.substr(execPath.find(':') + 1);
    std::string url = "", path = "", retString = "";
    const std::string patronCode = Configuration::getInstance().patronCode();
    if (Configuration::getInstance().alphaChannel() && !patronCode.empty())
    {
        if (auto fetch = Fetch::init("https://flagbrew.org/patron/updateCheck", true, true, &retString, nullptr, "code=" + patronCode))
        {
            Gui::waitFrame(i18n::localize("UPDATE_CHECKING"));
            CURLcode res = fetch->perform();
            if (res != CURLE_OK)
            {
                Gui::error(i18n::localize("CURL_ERROR"), abs(res));
            }
            else
            {
                long status_code;
                fetch->getinfo(CURLINFO_RESPONSE_CODE, &status_code);
                switch (status_code)
                {
                    case 200:
                        if (retString.substr(0, 8) != GIT_REV)
                        {
                            url = "https://flagbrew.org/patron/downloadLatest/";
                            if (execPath.empty())
                            {
                                url += "cia";
                                path = "/3ds/PKSM/PKSM.cia";
                            }
                            else
                            {
                                url += "3dsx";
                                path = execPath + ".new";
                            }
                        }
                        break;
                    case 401:
                        Gui::warn(i18n::localize("NOT_PATRON") + '\n' + i18n::localize("INCIDENT_LOGGED"));
                        break;
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
    else if (auto fetch = Fetch::init("https://api.github.com/repos/FlagBrew/PKSM/releases/latest", false, true, &retString, nullptr, ""))
    {
        Gui::waitFrame(i18n::localize("UPDATE_CHECKING"));
        CURLcode res = fetch->perform();
        if (res != CURLE_OK)
        {
            Gui::error(i18n::localize("CURL_ERROR"), abs(res));
        }
        else
        {
            long status_code;
            fetch->getinfo(CURLINFO_RESPONSE_CODE, &status_code);
            switch (status_code)
            {
                case 200:
                {
                    nlohmann::json retJson = nlohmann::json::parse(retString, nullptr, false);
                    if (retJson.is_discarded())
                    {
                        Gui::warn(i18n::localize("UPDATE_CHECK_ERROR_BAD_JSON_1") + '\n' + i18n::localize("UPDATE_CHECK_ERROR_BAD_JSON_2"));
                    }
                    else if (retJson["tag_name"].get<std::string>() > StringUtils::format("%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO))
                    {
                        url = "https://github.com/FlagBrew/PKSM/releases/download/" + retJson["tag_name"].get<std::string>() + "/PKSM";
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
    if (!url.empty())
    {
        Gui::waitFrame(i18n::localize("UPDATE_FOUND"));
        std::string fileName = path.substr(path.find_last_of('/') + 1);
        Result res =
            Fetch::download(url, path, Configuration::getInstance().alphaChannel() ? "code=" + patronCode : "", progress_callback, &fileName);
        if (R_FAILED(res))
        {
            Gui::error(i18n::localize("UPDATE_FOUND_BUT_FAILED_DOWNLOAD"), res);
            Archive::deleteFile(Archive::sd(), path);
            return false;
        }

        Gui::waitFrame(i18n::localize("UPDATE_INSTALLING"));
        if (execPath != "")
        {
            Archive::deleteFile(Archive::sd(), execPath);
            Archive::moveFile(Archive::sd(), path, Archive::sd(), execPath);
            return true;
        }
        else
        {
            // Adapted from https://github.com/joel16/3DShell/blob/master/source/cia.c
            AM_TitleEntry title;
            Handle dstHandle;
            FSStream ciaFile(Archive::sd(), path, FS_OPEN_READ);
            if (ciaFile.good())
            {
                if (R_FAILED(res = AM_GetCiaFileInfo(MEDIATYPE_SD, &title, ciaFile.getRawHandle())))
                {
                    Gui::error(i18n::localize("BAD_CIA_FILE"), res);
                    ciaFile.close();
                    return false;
                }

                if (R_FAILED(res = AM_StartCiaInstall(MEDIATYPE_SD, &dstHandle)))
                {
                    Gui::error(i18n::localize("CIA_INSTALL_START_FAIL"), res);
                    ciaFile.close();
                    return false;
                }

                u8 buf[0x1000];
                u32 bytesWritten, bytesRead;
                u64 offset          = 0;
                bool ciaInstallGood = true;
                do
                {
                    memset(buf, 0, 0x1000);

                    bytesRead = ciaFile.read(buf, 0x1000);
                    if (R_FAILED(ciaFile.result()))
                    {
                        Gui::error(i18n::localize("CIA_UPDATE_READ_FAIL"), ciaFile.result());
                        ciaFile.close();
                        FSFILE_Close(dstHandle);
                        return false;
                    }

                    if (R_FAILED(res = FSFILE_Write(dstHandle, &bytesWritten, offset, buf, bytesRead, FS_WRITE_FLUSH)))
                    {
                        Gui::error(i18n::localize("CIA_UPDATE_WRITE_FAIL"), res);
                        ciaFile.close();
                        FSFILE_Close(dstHandle);
                        return false;
                    }

                    if (bytesWritten != bytesRead)
                    {
                        ciaInstallGood = false;
                    }

                    offset += bytesRead;
                } while (offset < ciaFile.size() && ciaInstallGood);

                if (!ciaInstallGood)
                {
                    AM_CancelCIAInstall(dstHandle);
                    ciaFile.close();
                    Gui::warn("Bytes written doesn't match bytes read:\n" + std::to_string(bytesWritten) + " vs " + std::to_string(bytesRead));
                    return false;
                }

                if (R_FAILED(res = AM_FinishCiaInstall(dstHandle)))
                {
                    Gui::error(i18n::localize("CIA_INSTALL_FINISH_FAIL"), res);
                    ciaFile.close();
                    return false;
                }

                ciaFile.close();

                Archive::deleteFile(Archive::sd(), path);

                return true;
            }
        }
    }
    return false;
}

Result App::init(const std::string& execPath)
{
    Result res;

    hidInit();
    gfxInitDefault();

    int x = 176;
    int y = 96;
    u16 w, h;
    for (auto& line : bootSplash)
    {
        std::copy(line.begin(), line.end(), gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &w, &h) + x++ * 3 * 240 + y * 3);
    }
    gfxSwapBuffersGpu();

#if !CITRA_DEBUG
    if (R_FAILED(res = svcConnectToPort(&hbldrHandle, "hb:ldr")))
        return consoleDisplayError("Rosalina sysmodule has not been found.\n\nMake sure you're running latest Luma3DS.", res);
#endif
    APT_GetAppCpuTimeLimit(&old_time_limit);
    APT_SetAppCpuTimeLimit(30);

    if (R_FAILED(res = cfguInit()))
        return consoleDisplayError("cfguInit failed.", res);
    if (R_FAILED(res = romfsInit()))
        return consoleDisplayError("romfsInit failed.", res);
    if (R_FAILED(res = Archive::init(execPath)))
        return consoleDisplayError("Archive::init failed.", res);
    if (R_FAILED(res = pxiDevInit()))
        return consoleDisplayError("pxiDevInit failed.", res);
    if (R_FAILED(res = amInit()))
        return consoleDisplayError("amInit failed.", res);
    if (R_FAILED(res = acInit()))
        return consoleDisplayError("acInit failed.", res);

    u32* socketBuffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);
    if (socketBuffer == NULL)
    {
        return consoleDisplayError("Failed to create socket buffer.", -1);
    }
    if (socInit(socketBuffer, SOC_BUFFERSIZE))
    {
        return consoleDisplayError("socInit failed.", -1);
    }

    if (R_FAILED(res = downloadAdditionalAssets()))
        return consoleDisplayError(
            "Additional assets download failed.\n\nAlways make sure you're connected to the internet and on the lastest version.", res);
    if (R_FAILED(res = Gui::init()))
        return consoleDisplayError("Gui::init failed.", res);

    i18n::init();
    Configuration::getInstance();

    if (Configuration::getInstance().autoUpdate() && update(execPath))
    {
        Result res = -1;
        if (execPath.empty())
        {
            u8 param[0x300];
            u8 hmac[0x20];

            if (R_SUCCEEDED(res = APT_PrepareToDoApplicationJump(0, 0x000400000EC10000, MEDIATYPE_SD)))
            {
                res = APT_DoApplicationJump(param, sizeof(param), hmac);
            }
        }
        else
        {
#if !CITRA_DEBUG
            std::string path = execPath.substr(execPath.find('/'));
            res              = HBLDR_SetTarget(path.c_str());
#endif
        }
        if (R_FAILED(res))
        {
            Gui::warn(i18n::localize("UPDATE_SUCCESS_1") + '\n' + i18n::localize("UPDATE_SUCCESS_2"));
        }
        return -1;
    }

    if (R_FAILED(res = Banks::init()))
        return consoleDisplayError("Banks::init failed.", res);

    Threads::create((ThreadFunc)TitleLoader::scanTitles);
    TitleLoader::scanSaves();

    randomNumbers.seed(osGetTime());

    Gui::setScreen(std::make_unique<TitleLoadScreen>());
    // uncomment when needing to debug with GDB

    return 0;
}

Result App::exit(void)
{
    svcCloseHandle(hbldrHandle);
    TitleLoader::exit();
    Gui::exit();
    socExit();
    acExit();
    Threads::destroy();
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
