/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2025 Bernardo Giordano, Admiral Fish, piepie62
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

#include "PkmSummary.hpp"
#include "Configuration.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "pkx/PKX.hpp"
#include <format>
#include <string>

namespace
{
    // Separators and bullets that split the panel into its four rows.
    void drawFrame()
    {
        Gui::sprite(ui_sheet_stripe_separator_idx, 274, 97);
        Gui::sprite(ui_sheet_stripe_separator_idx, 274, 137);
        Gui::sprite(ui_sheet_stripe_separator_idx, 274, 177);
        Gui::sprite(ui_sheet_point_big_idx, 265, 66);
        Gui::sprite(ui_sheet_point_big_idx, 265, 103);
        Gui::sprite(ui_sheet_point_big_idx, 265, 146);
        Gui::sprite(ui_sheet_point_big_idx, 265, 186);
    }

    // Nickname, dex number, and the level row: the gender and shiny marks hang off the left
    // edge of the level text, so they share its measured width.
    void drawIdentity(const pksm::PKX& pkm)
    {
        Gui::text(
            pkm.nickname(), 276, 61, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        std::string info = "#" + std::to_string(int(pkm.species()));
        Gui::text(info, 273, 77, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        info      = i18n::localize("LV") + std::to_string(pkm.level());
        auto text = Gui::parseText(info, FONT_SIZE_12, 0.0f);
        int width = text->maxWidth(FONT_SIZE_12);
        Gui::text(text, 375 - width, 77, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
            TextPosY::TOP);
        switch (pkm.gender())
        {
            case pksm::Gender::Male:
                Gui::sprite(ui_sheet_icon_male_idx, 362 - width, 80);
                break;
            case pksm::Gender::Female:
                Gui::sprite(ui_sheet_icon_female_idx, 364 - width, 80);
                break;
            case pksm::Gender::Genderless:
                Gui::sprite(ui_sheet_icon_genderless_idx, 364 - width, 80);
                break;
            case pksm::Gender::INVALID:
                break;
        }
        if (pkm.shiny())
        {
            Gui::sprite(ui_sheet_icon_shiny_idx, 352 - width, 81);
        }
    }

    // Species name and typing; a single type is centered instead of paired.
    void drawSpeciesAndTypes(const pksm::PKX& pkm)
    {
        Gui::text(pkm.species().localize(Configuration::getInstance().language()), 276, 98,
            FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        pksm::Type firstType  = pkm.type1();
        pksm::Type secondType = pkm.type2();
        if (firstType != secondType)
        {
            Gui::type(Configuration::getInstance().language(), firstType, 276, 115);
            Gui::type(Configuration::getInstance().language(), secondType, 325, 115);
        }
        else
        {
            Gui::type(Configuration::getInstance().language(), firstType, 300, 115);
        }
    }

    // Original trainer, shown with the trainer ID as the Pokemon's own game displays it.
    void drawTrainer(const pksm::PKX& pkm)
    {
        std::string info =
            pkm.otName() + '\n' + i18n::localize("LOADER_ID") + std::to_string(pkm.versionTID());
        Gui::text(info, 276, 141, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    }

    // Nature, which generations one and two do not have, and the two IV rows.
    void drawNatureAndIVs(const pksm::PKX& pkm)
    {
        Gui::text(pkm.generation() >= pksm::Generation::THREE
                      ? pkm.nature().localize(Configuration::getInstance().language())
                      : "—",
            276, 181, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        std::string info = i18n::localize("IV") + ": ";
        auto text        = Gui::parseText(info, FONT_SIZE_12, 0.0f);
        int width        = text->maxWidth(FONT_SIZE_12);
        Gui::text(
            text, 276, 197, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        info = std::format("{:2d}/{:2d}/{:2d}", pkm.iv(pksm::Stat::HP), pkm.iv(pksm::Stat::ATK),
            pkm.iv(pksm::Stat::DEF));
        Gui::text(info, 276 + width + 70 / 2, 197, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER,
            TextPosY::TOP);
        info = std::format("{:2d}/{:2d}/{:2d}", pkm.iv(pksm::Stat::SPATK),
            pkm.iv(pksm::Stat::SPDEF), pkm.iv(pksm::Stat::SPD));
        Gui::text(info, 276 + width + 70 / 2, 209, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER,
            TextPosY::TOP);
    }
}

void PkmSummary::draw(const pksm::PKX* pkm)
{
    drawFrame();
    if (!pkm)
    {
        return;
    }

    drawIdentity(*pkm);
    drawSpeciesAndTypes(*pkm);
    drawTrainer(*pkm);
    drawNatureAndIVs(*pkm);
    Gui::format(*pkm, 276, 213);
}
