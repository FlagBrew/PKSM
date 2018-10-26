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

// increase the stack in order to allow quirc to decode large qrs
int __stacksize__ = 64 * 1024;

static u32 old_time_limit;

Result App::init(void)
{
#ifdef PICOC_DEBUG
    dup2(2, 1); // Redirects stdout to stderr for GDB to capture
#endif
    Result res;

    APT_GetAppCpuTimeLimit(&old_time_limit);
    APT_SetAppCpuTimeLimit(30);
    
    if (R_FAILED(res = cfguInit())) return res;
    if (R_FAILED(res = romfsInit())) return res;
    if (R_FAILED(res = Archive::init())) return res;
    if (R_FAILED(res = pxiDevInit())) return res;
    if (R_FAILED(res = amInit())) return res;
    if (R_FAILED(res = Gui::init())) return res;
    i18n::init();
    Configuration::getInstance();

    Threads::create((ThreadFunc)TitleLoader::scanTitles);
    Threads::create((ThreadFunc)TitleLoader::scanSaves);

    Gui::setScreen(std::make_unique<TitleLoadScreen>());

    // uncomment when needing to debug with GDB
#ifdef PICOC_DEBUG
    consoleDebugInit(debugDevice_SVC);
#endif
    // while(aptMainLoop() && !(hidKeysDown() & KEY_START)) { hidScanInput(); }

    return 0;
}

Result App::exit(void)
{
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