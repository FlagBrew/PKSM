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

#include "StorageViewOverlay.hpp"
#include "EditorScreen.hpp"
#include "StorageScreen.hpp"
#include "gui.hpp"

void StorageViewOverlay::drawBottom() const
{
    dim();
    if (clone.empty())
    {
        Gui::text(i18n::localize("PRESS_TO_CLONE"), 160, 110, FONT_SIZE_18, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
    }
}

void StorageViewOverlay::update(touchPosition* touch)
{
    u32 kDown = hidKeysDown();
    if (clone.empty() && kDown & KEY_X)
    {
        clone.emplace_back(pkm->clone());
        partyNum.push_back(-1);
        cloneDims          = {1, 1};
        currentlySelecting = false;
        screen.removeOverlay();
    }
    else if (kDown & KEY_UP)
    {
        if (emergencyMode[0])
        {
            emergencyMode.set(1);
        }
        else if (!emergencyMode[1])
        {
            emergencyMode.set(0);
        }
        else
        {
            emergencyMode.reset();
        }
    }
    else if (kDown & KEY_DOWN)
    {
        if (emergencyMode[2])
        {
            emergencyMode.set(3);
        }
        else if (!emergencyMode[3] && emergencyMode[1])
        {
            emergencyMode.set(2);
        }
        else
        {
            emergencyMode.reset();
        }
    }
    else if (kDown & KEY_LEFT)
    {
        if (emergencyMode[5])
        {
            emergencyMode.set(6);
        }
        else if (!emergencyMode[5] && emergencyMode[3])
        {
            emergencyMode.set(4);
        }
        else
        {
            emergencyMode.reset();
        }
    }
    else if (kDown & KEY_RIGHT)
    {
        if (emergencyMode[6])
        {
            emergencyMode.set(7);
        }
        else if (!emergencyMode[6] && emergencyMode[4])
        {
            emergencyMode.set(5);
        }
        else
        {
            emergencyMode.reset();
        }
    }
    else if (kDown & KEY_A)
    {
        if (emergencyMode[7])
        {
            emergencyMode.set(8);
        }
        else
        {
            emergencyMode.reset();
        }
    }
    else if (kDown & KEY_B)
    {
        if (emergencyMode[8] && emergencyInfo.first != -1)
        {
            Gui::setScreen(std::make_unique<EditorScreen>(pkm, emergencyInfo.first, emergencyInfo.second, true));
        }
        screen.removeOverlay();
    }
}
