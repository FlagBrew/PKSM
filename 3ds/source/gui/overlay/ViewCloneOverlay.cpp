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

#include "ViewCloneOverlay.hpp"
#include "gui.hpp"

void ViewCloneOverlay::draw() const
{
    ViewOverlay::draw();

    C2D_SceneBegin(g_renderTargetBottom);
    dim();
    if (clone.empty())
    {
        Gui::staticText(i18n::localize("PRESS_TO_CLONE"), 160, 110, FONT_SIZE_18, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
    }
}

void ViewCloneOverlay::update(touchPosition* touch)
{
    u32 kDown = hidKeysDown();
    if (kDown & KEY_B)
    {
        screen.removeOverlay();
    }
    else if (clone.empty() && kDown & KEY_X)
    {
        clone.emplace_back(pkm->clone());
        partyNum.push_back(-1);
        cloneDims          = {1, 1};
        currentlySelecting = false;
        screen.removeOverlay();
    }
}
