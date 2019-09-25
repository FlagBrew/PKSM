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

#include "ViewOverlay.hpp"
#include "Configuration.hpp"
#include "PB7.hpp"
#include "gui.hpp"

static constexpr std::string_view displayKeys[] = {"TYPE", "NICKNAME", "OT", "NATURE", "ABILITY", "ITEM", "ESV_TSV", "TID_SID", "CTOT_FSHIP",
    "HIDDEN_POWER", "HP", "ATTACK", "DEFENSE", "SPATK.", "SPDEF.", "SPEED"};

void ViewOverlay::drawTop() const
{
    Gui::sprite(green ? ui_sheet_emulated_bg_top_green : ui_sheet_emulated_bg_top_blue, 0, 0);
    Gui::sprite(ui_sheet_bg_style_top_idx, 0, 0);
    Gui::backgroundAnimatedTop();
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
        Gui::text(i18n::localize(std::string(displayKeys[i])), 10, 36 + i * 20, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    }
    for (int i = 0; i < 6; i++)
    {
        Gui::text(i18n::localize(std::string(displayKeys[i + 10])), 238, 16 + i * 20, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    }
    Gui::text(i18n::localize("MOVES"), 252, 136, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);

    if (pkm)
    {
        Gui::text(
            i18n::species(Configuration::getInstance().language(), pkm->species()), 25, 7, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::ball(pkm->ball(), 4, 6);
        Gui::generation(*pkm, 115, 11);
        if (pkm->gender() == 0)
        {
            Gui::sprite(ui_sheet_icon_male_idx, 127, 10);
        }
        else if (pkm->gender() == 1)
        {
            Gui::sprite(ui_sheet_icon_female_idx, 129, 10);
        }
        else if (pkm->gender() == 2)
        {
            Gui::sprite(ui_sheet_icon_genderless_idx, 129, 10);
        }
        Gui::text(StringUtils::format(i18n::localize("LVL"), pkm->level()), 143, 10, FONT_SIZE_9, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        if (pkm->shiny())
        {
            Gui::sprite(ui_sheet_icon_shiny_idx, 191, 5);
        }
        if (pkm->pkrsDays() > 0)
        {
            switch (Configuration::getInstance().language())
            {
                case Language::TW:
                case Language::ZH:
                    Gui::sprite(ui_sheet_pkrs_chn_idx, 192, 15);
                    break;
                case Language::JP:
                    Gui::sprite(ui_sheet_pkrs_jpn_idx, 192, 15);
                    break;
                case Language::RU:
                    Gui::sprite(ui_sheet_pkrs_rus_idx, 192, 15);
                    break;
                default:
                    Gui::sprite(ui_sheet_pkrs_eng_idx, 192, 15);
                    break;
            }
        }
        else if (pkm->pkrs())
        {
            Gui::sprite(ui_sheet_pkrs_cured_idx, 201, 7);
        }

        u8 firstType  = pkm->type1();
        u8 secondType = pkm->type2();
        if (pkm->generation() == Generation::FOUR)
        {
            if (firstType > 8)
                firstType--;
            if (secondType > 8)
                secondType--;
        }
        if (firstType != secondType)
        {
            Gui::type(Configuration::getInstance().language(), firstType, 59, 35);
            Gui::type(Configuration::getInstance().language(), secondType, 108, 35);
        }
        else
        {
            Gui::type(Configuration::getInstance().language(), firstType, 83, 35);
        }

        Gui::text(pkm->nickname(), 87, 56, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(pkm->otName(), 87, 76, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(
            i18n::nature(Configuration::getInstance().language(), pkm->nature()), 87, 96, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::ability(Configuration::getInstance().language(), pkm->ability()), 87, 116, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
            TextPosY::TOP);
        Gui::text(
            i18n::item(Configuration::getInstance().language(), pkm->heldItem()), 87, 136, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(StringUtils::format("%i/%i", pkm->PSV(), pkm->TSV()), 87, 156, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(
            StringUtils::format("%u/%u", pkm->versionTID(), pkm->versionSID()), 87, 176, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(StringUtils::format("%i/%i", (int)pkm->currentFriendship(), (int)pkm->otFriendship()), 122, 196, FONT_SIZE_12, COLOR_BLACK,
            TextPosX::LEFT, TextPosY::TOP);
        Gui::text(
            i18n::hp(Configuration::getInstance().language(), pkm->hpType()), 122, 216, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);

        static constexpr int statValues[] = {0, 1, 2, 4, 5, 3};
        for (int i = 0; i < 6; i++)
        {
            Gui::text(std::to_string((int)pkm->iv(statValues[i])), 317, 16 + i * 20, FONT_SIZE_12, COLOR_BLACK, TextPosX::RIGHT, TextPosY::TOP);
            if (pkm->generation() == Generation::LGPE)
            {
                Gui::text(std::to_string((int)((PB7*)pkm.get())->awakened(statValues[i])), 342, 16 + i * 20, FONT_SIZE_12, COLOR_BLACK,
                    TextPosX::CENTER, TextPosY::TOP);
            }
            else
            {
                Gui::text(std::to_string((int)pkm->ev(statValues[i])), 342, 16 + i * 20, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
            }
            Gui::text(std::to_string((int)pkm->stat(statValues[i])), 367, 16 + i * 20, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        }

        for (int i = 0; i < 4; i++)
        {
            Gui::text(i18n::move(Configuration::getInstance().language(), pkm->move(i)), 252, 156 + i * 20, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
                TextPosY::TOP);
        }
    }
    else
    {
        Gui::ball(0, 4, 6);
        for (int i = 0; i < 4; i++)
        {
            Gui::text(
                i18n::move(Configuration::getInstance().language(), 0), 252, 156 + i * 20, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        }
    }
}
