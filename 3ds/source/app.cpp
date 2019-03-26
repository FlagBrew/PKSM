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
#include "random.hpp"

// increase the stack in order to allow quirc to decode large qrs
int __stacksize__ = 64 * 1024;

static u32 old_time_limit;

struct asset {
    std::string url;
    std::string path;
    unsigned char hash[SHA256_BLOCK_SIZE];
};

static bool matchSha256HashFromFile(const std::string& path, unsigned char* sha)
{
    bool match = false;
    FILE* in = fopen(path.c_str(), "rb");
    if (!ferror(in))
    {
        fseek(in, 0, SEEK_END);
        size_t size = ftell(in);
        fseek(in, 0, SEEK_SET);
        char* data = new char[size];
        fread(data, 1, size, in);
        char hash[SHA256_BLOCK_SIZE];
        sha256((unsigned char*)hash, (unsigned char*)data, size);
        delete[] data;
        match = memcmp(sha, hash, SHA256_BLOCK_SIZE) == 0;
    }
    fclose(in);
    return match;
}

static Result downloadAdditionalAssets(void) {
    Result res = 0;
    asset assets[2] = {
        {
            "https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/pkm_spritesheet.t3x",
            "/3ds/PKSM/assets/pkm_spritesheet.t3x",
            {
                0xa5, 0x0e, 0x59, 0x75, 0x00, 0xf0, 0xe1, 0x6a,
                0x6e, 0xe9, 0xd4, 0x5b, 0xb3, 0x3b, 0x9c, 0x08,
                0xe8, 0x69, 0xc0, 0x1d, 0x10, 0x53, 0x3f, 0xe0,
                0xbe, 0x7e, 0x2c, 0xa4, 0xe7, 0x6d, 0xcc, 0x48
            }
        },
        {
            "https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/types_spritesheet.t3x",
            "/3ds/PKSM/assets/types_spritesheet.t3x",
            {
                0xea, 0x7f, 0x92, 0x86, 0x0a, 0x9b, 0x4d, 0x50,
                0x3a, 0x0c, 0x2a, 0x6e, 0x48, 0x60, 0xfb, 0x93,
                0x1f, 0xd3, 0xd7, 0x7d, 0x6a, 0xbb, 0x1d, 0xdb,
                0xac, 0x59, 0xeb, 0xf1, 0x66, 0x34, 0xa4, 0x91
            }
        }
    };

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
            u32 status;
            ACU_GetWifiStatus(&status);
            if (status == 0) return -1;
            Result res1 = download(item.url.c_str(), item.path.c_str());
            if (R_FAILED(res1)) return res1;
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
    while (aptMainLoop() && !(hidKeysDown() & KEY_START)) { hidScanInput(); }
    return res;
}

Result App::init(std::string execPath)
{
    Result res;

    hidInit();
    gfxInitDefault();

#if !CITRA_DEBUG
    Handle hbldrHandle;
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
        return consoleDisplayError("Additional assets download failed.\n\nAlways make sure you're connected to the internet and on the lastest version.", res);
    if (R_FAILED(res = Gui::init()))
        return consoleDisplayError("Gui::init failed.", res);
    
    Configuration::getInstance();
    i18n::init();

    Threads::create((ThreadFunc)TitleLoader::scanTitles);
    TitleLoader::scanSaves();
    TitleLoader::bank = std::make_shared<Bank>();

    randomNumbers.seed(osGetTime());

    Gui::setScreen(std::make_unique<TitleLoadScreen>());
    // uncomment when needing to debug with GDB
    // while(aptMainLoop() && !(hidKeysDown() & KEY_START)) { hidScanInput(); }

    return 0;
}

Result App::exit(void)
{
    TitleLoader::bank->save();
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
