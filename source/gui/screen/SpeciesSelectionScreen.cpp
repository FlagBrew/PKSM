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

#include "SpeciesSelectionScreen.hpp"
#include "gui.hpp"
#include "loader.hpp"
#include "Configuration.hpp"

void SpeciesSelectionScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    Gui::sprite(ui_sheet_part_mtx_5x8_idx, 0, 0);

    int x = (hid.index() % 8) * 50;
    int y = (hid.index() / 8) * 48;
    // Selector
    C2D_DrawRectSolid(x, y, 0.5f, 49, 47, COLOR_MASKBLACK);
    C2D_DrawRectSolid(x, y, 0.5f, 49, 1, COLOR_YELLOW);
    C2D_DrawRectSolid(x, y, 0.5f, 1, 47, COLOR_YELLOW);
    C2D_DrawRectSolid(x + 48, y, 0.5f, 1, 47, COLOR_YELLOW);
    C2D_DrawRectSolid(x, y + 46, 0.5f, 49, 1, COLOR_YELLOW);

    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            if (hid.page() * hid.maxVisibleEntries() + x + y * 8 + 1 > (size_t) TitleLoader::save->maxSpecies())
            {
                break;
            }
            Gui::pkm(hid.page() * hid.maxVisibleEntries() + x + y * 8 + 1, 0, TitleLoader::save->generation(), x * 50 + 7, y * 48 + 2);
            Gui::dynamicText(x * 50, y * 48 + 34, 50, std::to_string(hid.page() * hid.maxVisibleEntries() + x + y * 8 + 1), FONT_SIZE_9, FONT_SIZE_9, COLOR_WHITE);
        }
    }
}

void SpeciesSelectionScreen::update(touchPosition* touch)
{
    hid.update(TitleLoader::save->maxSpecies());
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_A)
    {
        if (pkm->species() == 0 || !pkm->nicknamed())
        {
            pkm->nickname(i18n::species(Configuration::getInstance().language(), hid.fullIndex() + 1).c_str());
        }
        pkm->species((u16) hid.fullIndex() + 1);
        pkm->alternativeForm(0);
        if (pkm->generation() != 4)
        {
            pkm->ability(0);
        }
        else
        {
            pkm->ability(PersonalDPPtHGSS::ability(pkm->species(), pkm->abilityNumber()));
        }
        done = true;
        return;
    }
    else if (downKeys & KEY_B)
    {
        if (pkm->species() != 0)
        {
            done = true;
        }
        else
        {
            Gui::warn("Please select a species");
        }
        return;
    }
}