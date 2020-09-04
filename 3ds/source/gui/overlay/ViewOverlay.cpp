/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2020 Bernardo Giordano, Admiral Fish, piepie62
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
#include "format.h"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "pkx/PB7.hpp"

namespace
{
    constexpr const char* displayKeys[] = {"TYPE", "NICKNAME", "OT", "NATURE", "ABILITY", "ITEM",
        "ESV_TSV", "TID_SID", "CTOT_FSHIP", "HIDDEN_POWER", "HP", "ATTACK", "DEFENSE", "SPATK.",
        "SPDEF.", "SPEED"};
    constexpr std::array<float, 16> displayWidths = {
        75, 75, 75, 75, 75, 75, 75, 75, 110, 110, 60, 60, 60, 60, 60, 60};
}

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
        Gui::text(i18n::localize(std::string(displayKeys[i])), 10, 36 + i * 20, FONT_SIZE_12,
            COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SQUISH, displayWidths[i]);
    }

    constexpr PKSM_Color statDisplayColors[3] = {
        COLOR_UNSELECTBLUE, COLOR_BLACK, COLOR_UNSELECTRED};
    int statColorIndex[6] = {1, 1, 1, 1, 1, 1};
    if (pokemon.index() != 0 || std::get<0>(pokemon).get())
    {
        // Subtract from the ones that are affected negatively
        switch (getPKM().nature())
        {
            case pksm::Nature::Hardy:
            case pksm::Nature::Bold:
            case pksm::Nature::Modest:
            case pksm::Nature::Calm:
            case pksm::Nature::Timid:
                statColorIndex[u8(pksm::Stat::ATK)]--;
                break;
            case pksm::Nature::Lonely:
            case pksm::Nature::Docile:
            case pksm::Nature::Mild:
            case pksm::Nature::Gentle:
            case pksm::Nature::Hasty:
                statColorIndex[u8(pksm::Stat::DEF)]--;
                break;
            case pksm::Nature::Adamant:
            case pksm::Nature::Impish:
            case pksm::Nature::Bashful:
            case pksm::Nature::Careful:
            case pksm::Nature::Jolly:
                statColorIndex[u8(pksm::Stat::SPATK)]--;
                break;
            case pksm::Nature::Naughty:
            case pksm::Nature::Lax:
            case pksm::Nature::Rash:
            case pksm::Nature::Quirky:
            case pksm::Nature::Naive:
                statColorIndex[u8(pksm::Stat::SPDEF)]--;
                break;
            case pksm::Nature::Brave:
            case pksm::Nature::Relaxed:
            case pksm::Nature::Quiet:
            case pksm::Nature::Sassy:
            case pksm::Nature::Serious:
                statColorIndex[u8(pksm::Stat::SPD)]--;
                break;
        }
        // Add to the ones that are affected positively
        switch (getPKM().nature())
        {
            case pksm::Nature::Hardy:
            case pksm::Nature::Lonely:
            case pksm::Nature::Adamant:
            case pksm::Nature::Naughty:
            case pksm::Nature::Brave:
                statColorIndex[u8(pksm::Stat::ATK)]++;
                break;
            case pksm::Nature::Bold:
            case pksm::Nature::Docile:
            case pksm::Nature::Impish:
            case pksm::Nature::Lax:
            case pksm::Nature::Relaxed:
                statColorIndex[u8(pksm::Stat::DEF)]++;
                break;
            case pksm::Nature::Modest:
            case pksm::Nature::Mild:
            case pksm::Nature::Bashful:
            case pksm::Nature::Rash:
            case pksm::Nature::Quiet:
                statColorIndex[u8(pksm::Stat::SPATK)]++;
                break;
            case pksm::Nature::Calm:
            case pksm::Nature::Gentle:
            case pksm::Nature::Careful:
            case pksm::Nature::Quirky:
            case pksm::Nature::Sassy:
                statColorIndex[u8(pksm::Stat::SPDEF)]++;
                break;
            case pksm::Nature::Timid:
            case pksm::Nature::Hasty:
            case pksm::Nature::Jolly:
            case pksm::Nature::Naive:
            case pksm::Nature::Serious:
                statColorIndex[u8(pksm::Stat::SPD)]++;
                break;
        }
    }

    int statIndex = 0;
    for (const auto& stat : {pksm::Stat::HP, pksm::Stat::ATK, pksm::Stat::DEF, pksm::Stat::SPATK,
             pksm::Stat::SPDEF, pksm::Stat::SPD})
    {
        Gui::text(i18n::localize(std::string(displayKeys[statIndex + 10])), 238,
            16 + statIndex * 20, FONT_SIZE_12, statDisplayColors[statColorIndex[u8(stat)]],
            TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SQUISH, displayWidths[statIndex + 10]);
        statIndex++;
    }
    Gui::text(i18n::localize("MOVES"), 252, 136, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
        TextPosY::TOP);

    if (pokemon.index() == 0 && !std::get<0>(pokemon).get())
    {
        drawEmpty();
    }
    else
    {
        drawPkm(getPKM());
    }
}

void ViewOverlay::drawPkm(pksm::PKX& pkm) const
{
    Gui::text(pkm.species().localize(Configuration::getInstance().language()), 25, 7, FONT_SIZE_12,
        COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    Gui::ball(pkm.ball(), 4, 6);
    Gui::generation(pkm, 115, 11);
    switch (pkm.gender())
    {
        case pksm::Gender::Male:
            Gui::sprite(ui_sheet_icon_male_idx, 127, 10);
            break;
        case pksm::Gender::Female:
            Gui::sprite(ui_sheet_icon_female_idx, 129, 10);
            break;
        case pksm::Gender::Genderless:
            Gui::sprite(ui_sheet_icon_genderless_idx, 129, 10);
            break;
    }
    Gui::text(fmt::format(i18n::localize("LVL"), pkm.level()), 143, 10, FONT_SIZE_9, COLOR_WHITE,
        TextPosX::LEFT, TextPosY::TOP);
    if (pkm.shiny())
    {
        Gui::sprite(ui_sheet_icon_shiny_idx, 191, 5);
    }
    if (pkm.pkrsDays() > 0)
    {
        switch (Configuration::getInstance().language())
        {
            case pksm::Language::CHT:
            case pksm::Language::CHS:
                Gui::sprite(ui_sheet_pkrs_chn_idx, 192, 15);
                break;
            case pksm::Language::JPN:
                Gui::sprite(ui_sheet_pkrs_jpn_idx, 192, 15);
                break;
            case pksm::Language::RU:
                Gui::sprite(ui_sheet_pkrs_rus_idx, 192, 15);
                break;
            default:
                Gui::sprite(ui_sheet_pkrs_eng_idx, 192, 15);
                break;
        }
    }
    else if (pkm.pkrs())
    {
        Gui::sprite(ui_sheet_pkrs_cured_idx, 201, 7);
    }

    pksm::Type firstType  = pkm.type1();
    pksm::Type secondType = pkm.type2();
    if (firstType != secondType)
    {
        Gui::type(Configuration::getInstance().language(), firstType, 59, 35);
        Gui::type(Configuration::getInstance().language(), secondType, 108, 35);
    }
    else
    {
        Gui::type(Configuration::getInstance().language(), firstType, 83, 35);
    }

    Gui::text(pkm.nickname(), 87, 56, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(pkm.otName(), 87, 76, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(pkm.nature().localize(Configuration::getInstance().language()), 87, 96, FONT_SIZE_12,
        COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(pkm.ability().localize(Configuration::getInstance().language()), 87, 116,
        FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(i18n::item(Configuration::getInstance().language(), pkm.heldItem()), 87, 136,
        FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(fmt::format(FMT_STRING("{:d}/{:d}"), pkm.PSV(), pkm.TSV()), 87, 156, FONT_SIZE_12,
        COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(fmt::format(FMT_STRING("{:d}/{:d}"), pkm.versionTID(), pkm.versionSID()), 87, 176,
        FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(fmt::format(FMT_STRING("{:d}/{:d}"), pkm.currentFriendship(), pkm.otFriendship()),
        122, 196, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(i18n::type(Configuration::getInstance().language(), pkm.hpType()), 122, 216,
        FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);

    static constexpr pksm::Stat statValues[] = {pksm::Stat::HP, pksm::Stat::ATK, pksm::Stat::DEF,
        pksm::Stat::SPATK, pksm::Stat::SPDEF, pksm::Stat::SPD};
    for (int i = 0; i < 6; i++)
    {
        Gui::text(std::to_string((int)pkm.iv(statValues[i])), 317, 16 + i * 20, FONT_SIZE_12,
            COLOR_BLACK, TextPosX::RIGHT, TextPosY::TOP);
        if (pkm.generation() == pksm::Generation::LGPE)
        {
            Gui::text(
                std::to_string((int)reinterpret_cast<pksm::PB7&>(pkm).awakened(statValues[i])), 342,
                16 + i * 20, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
        }
        else
        {
            Gui::text(std::to_string((int)pkm.ev(statValues[i])), 342, 16 + i * 20, FONT_SIZE_12,
                COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
        }
        Gui::text(std::to_string((int)pkm.stat(statValues[i])), 367, 16 + i * 20, FONT_SIZE_12,
            COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    }

    for (int i = 0; i < 4; i++)
    {
        Gui::text(i18n::move(Configuration::getInstance().language(), pkm.move(i)), 252,
            156 + i * 20, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP,
            TextWidthAction::SQUISH, 144.0f);
    }
}

void ViewOverlay::drawEmpty() const
{
    Gui::ball(pksm::Ball::None, 4, 6);
    for (int i = 0; i < 4; i++)
    {
        Gui::text(i18n::move(Configuration::getInstance().language(), pksm::Move::None), 252,
            156 + i * 20, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    }
}

pksm::PKX& ViewOverlay::getPKM() const
{
    if (pokemon.index() == 1)
    {
        return std::get<1>(pokemon).get();
    }
    return *std::get<0>(pokemon).get();
}
