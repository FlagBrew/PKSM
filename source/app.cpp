/*
*   This file is part of PKSM
*   Copyright (C) 2016-2018 Bernardo Giordano, Admiral Fish, piepie62
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
#include <random>

std::mt19937 randomNumbers;

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
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (file.good())
    {
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);
        char* data = new char[size];
        file.read(data, size);
        char hash[SHA256_BLOCK_SIZE];
        sha256((unsigned char*)hash, (unsigned char*)data, size);
        delete[] data;
        //std::ofstream out(path + ".bin", std::ios::binary);
        //out.write(hash, SHA256_BLOCK_SIZE);
        //out.close();
        match = memcmp(sha, hash, SHA256_BLOCK_SIZE) == 0;
    }
    file.close();
    return match;
}

static Result downloadAdditionalAssets(void) {
    Result res = 0;
    asset assets[2] = {
        {"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/pkm_spritesheet.t3x", "/3ds/PKSM/assets/pkm_spritesheet.t3x",
            {
                0xa5, 0x0e, 0x59, 0x75, 0x00, 0xf0, 0xe1, 0x6a,
                0x6e, 0xe9, 0xd4, 0x5b, 0xb3, 0x3b, 0x9c, 0x08,
                0xe8, 0x69, 0xc0, 0x1d, 0x10, 0x53, 0x3f, 0xe0,
                0xbe, 0x7e, 0x2c, 0xa4, 0xe7, 0x6d, 0xcc, 0x48
            }
        },
        {"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/types_spritesheet.t3x", "/3ds/PKSM/assets/types_spritesheet.t3x",
            {
                0x59, 0x31, 0xa2, 0x1e, 0x1d, 0x00, 0x0b, 0x82,
                0xe3, 0x1a, 0x07, 0x16, 0xd6, 0xc6, 0x96, 0xe1,
                0x61, 0x62, 0xb0, 0xf6, 0xc6, 0x29, 0xd4, 0x9a,
                0x9c, 0xa8, 0xf1, 0x8a, 0x3d, 0xd5, 0x2d, 0x0c
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
            Result res1 = download(item.url.c_str(), item.path.c_str());
            if (R_FAILED(res1)) return res1;
        }
    }
    return res;
}

static Result consoleDisplayError(Result res)
{
    consoleInit(GFX_TOP, nullptr);
    printf("Downloading assets failed!\nError code: %i", res);
    hidInit();
    while (aptMainLoop() && (hidKeysDown() & KEY_START)) hidScanInput();
    hidExit();
    return res;
}

Result App::init(std::string execPath)
{
    Result res;

    Handle hbldrHandle;
    if (R_FAILED(res = svcConnectToPort(&hbldrHandle, "hb:ldr"))) return res;

    APT_GetAppCpuTimeLimit(&old_time_limit);
    APT_SetAppCpuTimeLimit(30);
    
    if (R_FAILED(res = cfguInit())) return res;
    if (R_FAILED(res = romfsInit())) return res;
    if (R_FAILED(res = Archive::init(execPath))) return res;
    if (R_FAILED(res = pxiDevInit())) return res;
    if (R_FAILED(res = amInit())) return res;
    if (R_FAILED(res = hidInit())) return res;
    if (R_FAILED(res = downloadAdditionalAssets())) return consoleDisplayError(res);
    if (R_FAILED(res = Gui::init())) return res;
    Configuration::getInstance();
    i18n::init();

    u32* socketBuffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);
    if (socketBuffer == NULL)
    {
        return -1;
    }
    if (socInit(socketBuffer, SOC_BUFFERSIZE))
    {
        return -1;
    }

    Threads::create((ThreadFunc)TitleLoader::scanTitles);
    TitleLoader::scanSaves();

    randomNumbers.seed(osGetTime());

    Gui::setScreen(std::make_unique<TitleLoadScreen>());

#ifdef PICOC_DEBUG
    dup2(2, 1); // Redirects stdout to stderr for GDB to capture
    consoleDebugInit(debugDevice_SVC);
#endif
    // uncomment when needing to debug with GDB
    // while(aptMainLoop() && !(hidKeysDown() & KEY_START)) { hidScanInput(); }

    return 0;
}

Result App::exit(void)
{
    socExit();
    TitleLoader::exit();
    Gui::exit();
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
    
    return 0;
}