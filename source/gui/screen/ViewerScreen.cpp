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

#include "ViewerScreen.hpp"
#include "gui.hpp"
#include "Configuration.hpp"

static constexpr std::string_view displayKeys[] = {
    "Nickname",
    "OT",
    "Pok\u00E9rus",
    "Nature",
    "Ability",
    "Item",
    "ESV/TSV",
    "TID/SID",
    "CT/OT F.ship",
    "Hidden Power",
    "HP",
    "Attack",
    "Defense",
    "Sp. Atk",
    "Sp. Def",
    "Speed"
};

void ViewerScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    Gui::sprite(green ? ui_sheet_emulated_bg_top_green : ui_sheet_emulated_bg_top_blue, 0, 0);
    Gui::sprite(ui_sheet_bg_style_top_idx, 0, 0);
    Gui::backgroundAnimated(GFX_TOP);
    Gui::sprite(ui_sheet_bar_arc_top_blue_idx, 0, 0);
    
    Gui::sprite(ui_sheet_textbox_name_top_idx, 0, 3);

    for (int y = 34; y < 196; y += 40)
    {
        Gui::sprite(ui_sheet_stripe_info_row_idx, 0, y);
    }

    for (int y = 40; y < 240; y += 20)
    {
        Gui::sprite(ui_sheet_point_big_idx, 1, y);
    }

    for (int y = 14; y < 108; y += 40)
    {
        Gui::sprite(ui_sheet_stripe_stats_editor_idx, 232, y);
    }

    for (int y = 134; y < 240; y += 40)
    {
        Gui::sprite(ui_sheet_stripe_move_editor_row_idx, 232, y);
    }

    Gui::sprite(ui_sheet_point_big_idx, 237, 140);
    for (int y = 161; y < 240; y += 20)
    {
        Gui::sprite(ui_sheet_point_small_idx, 238, y);
    }

    for (int x = 321; x < 360; x += 38)
    {
        for (int y = 22; y < 123; y += 20)
        {
            Gui::sprite(ui_sheet_point_small_idx, x, y);
        }
    }

    for (int i = 0; i < 10; i++)
    {
        Gui::dynamicText(std::string(displayKeys[i]), 10, 36 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
    }
    for (int i = 0; i < 6; i++)
    {
        Gui::dynamicText(std::string(displayKeys[i + 10]), 238, 16 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
    }
    Gui::dynamicText("Moves", 252, 136, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);

    if (pkm)
    {
        Gui::dynamicText(i18n::species(Configuration::getInstance().language(), pkm->species()), 25, 7, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE, false);
        Gui::ball(pkm->ball(), 4, 6);
        Gui::generation(pkm.get(), 115, 11);
        if (pkm->gender() == 0)
        {
            Gui::sprite(ui_sheet_icon_male_idx, 127, 10);
        }
        else if (pkm->gender() == 1)
        {
            Gui::sprite(ui_sheet_icon_female_idx, 129, 10);
        }
        Gui::dynamicText(StringUtils::format("Lvl. %i", pkm->level()), 143, 10, FONT_SIZE_9, FONT_SIZE_9, COLOR_WHITE, false);
        if (pkm->shiny())
        {
            Gui::sprite(ui_sheet_icon_shiny_idx, 191, 8);
        }

        Gui::dynamicText(pkm->nickname(), 87, 36, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
        Gui::dynamicText(pkm->otName(), 87, 56, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
        Gui::dynamicText(pkm->pkrsDays() > 0 ? "Yes" : "No", 87, 76, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
        Gui::dynamicText(i18n::nature(Configuration::getInstance().language(), pkm->nature()), 87, 96, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
        Gui::dynamicText(i18n::ability(Configuration::getInstance().language(), pkm->ability()), 87, 116, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
        Gui::dynamicText(i18n::item(Configuration::getInstance().language(), pkm->heldItem()), 87, 136, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
        Gui::dynamicText(StringUtils::format("%i/%i", pkm->PSV(), pkm->TSV()), 87, 156, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
        Gui::dynamicText(StringUtils::format("%i/%i", pkm->TID(), pkm->SID()), 87, 176, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
        Gui::dynamicText(StringUtils::format("%i/%i", (int)pkm->currentFriendship(), (int)pkm->otFriendship()), 122, 196, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
        Gui::dynamicText(i18n::hp(Configuration::getInstance().language(), pkm->hpType()), 122, 216, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);

        static constexpr int statValues[] = { 0, 1, 2, 4, 5, 3 };
        for (int i = 0; i < 6; i++)
        {
            Gui::dynamicText(std::to_string((int)pkm->iv(statValues[i])), 317, 16 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, true);
            Gui::dynamicText(326, 16 + i * 20, 33, std::to_string((int)pkm->ev(statValues[i])), FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
            Gui::dynamicText(std::to_string((int)pkm->stat(statValues[i])), 367, 16 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
        }

        for (int i = 0; i < 4; i++)
        {
            Gui::dynamicText(i18n::move(Configuration::getInstance().language(), pkm->move(i)), 252, 156 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
        }
    }
    else
    {
        Gui::ball(0, 4, 6);
        for (int i = 0; i < 4; i++)
        {
            Gui::dynamicText(i18n::move(Configuration::getInstance().language(), 0), 252, 156 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
        }
    }
}