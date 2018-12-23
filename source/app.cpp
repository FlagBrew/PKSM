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
#include "Configuration.hpp"
#include "TitleLoadScreen.hpp"
#include <stdio.h>

extern "C" {
#include "download.h"
}

#define SOC_ALIGN      0x1000
#define SOC_BUFFERSIZE 0x100000

// increase the stack in order to allow quirc to decode large qrs
int __stacksize__ = 64 * 1024;

static u32 old_time_limit;

struct asset {
    std::string url;
    std::string path;
};

static Result downloadAdditionalAssets(void) {
    Result res = 0;
    asset assets[2] = {
        {"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/pkm_spritesheet.t3x", "/3ds/PKSM/assets/pkm_spritesheet.t3x"},
        {"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/types_spritesheet.t3x", "/3ds/PKSM/assets/types_spritesheet.t3x"}
    };
    if (!io::exists(assets[0].path) || !io::exists(assets[1].path)) {
        Result res1 = download(assets[0].url.c_str(), assets[0].path.c_str());
        Result res2 = download(assets[1].url.c_str(), assets[1].path.c_str());
        res = !res1 && !res2 ? 0 : -1;
    }
    return res;
}

Result App::init(std::string execPath)
{
#ifdef PICOC_DEBUG
    dup2(2, 1); // Redirects stdout to stderr for GDB to capture
#endif
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
    if (R_FAILED(res = downloadAdditionalAssets())) return res;
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
    Threads::create((ThreadFunc)TitleLoader::scanSaves);

    Gui::setScreen(std::make_unique<TitleLoadScreen>());

#ifdef PICOC_DEBUG
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
    Threads::destroy();
    Configuration::getInstance().save();
    i18n::exit();
    Gui::exit();
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