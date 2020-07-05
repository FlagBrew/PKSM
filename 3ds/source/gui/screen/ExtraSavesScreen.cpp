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

#include "ExtraSavesScreen.hpp"
#include "Configuration.hpp"
#include "ExtraSavesSubScreen.hpp"
#include "Species.hpp"
#include "gui.hpp"
#include "loader.hpp"
#include "thread.hpp"

ExtraSavesScreen::ExtraSavesScreen()
    : Screen(i18n::localize("A_CHOOSE_GROUP") + '\n' + i18n::localize("B_BACK"))
{
}

ExtraSavesScreen::~ExtraSavesScreen()
{
    Threads::create([](void*) { TitleLoader::scanSaves(); }, nullptr, 16 * 1024);
}

void ExtraSavesScreen::drawBottom() const
{
    Gui::sprite(ui_sheet_part_info_bottom_idx, 0, 0);
}

void ExtraSavesScreen::drawTop() const
{
    Gui::drawSolidRect(0, 0, 400.0f, 240.0f, PKSM_Color(15, 22, 89, 255));

    Gui::sprite(ui_sheet_emulated_gameselector_bg_solid_idx, 4, 29);
    // Gui::sprite(ui_sheet_gameselector_cart_idx, 35, 93);

    int x = 116, y = 49;
    // draw GBA game boxes
    Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(
        pksm::Species::Groudon, 0, pksm::Generation::THREE, pksm::Gender::Genderless, x - 2, y - 2);
    Gui::text(
        "R", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::pkm(pksm::Species::Kyogre, 0, pksm::Generation::THREE, pksm::Gender::Genderless, x + 16,
        y + 20);
    Gui::text("S", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

    x += 60;
    Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(
        pksm::Species::Rayquaza, 0, pksm::Generation::THREE, pksm::Gender::Genderless, x + 8, y);
    Gui::text("E", x + 24, y + 30, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

    x += 60;
    Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(pksm::Species::Charizard, 0, pksm::Generation::THREE, pksm::Gender::Genderless, x - 3,
        y - 2);
    Gui::text(
        "FR", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::pkm(pksm::Species::Venusaur, 0, pksm::Generation::THREE, pksm::Gender::Genderless, x + 17,
        y + 20);
    Gui::text("LG", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

    x = 86, y += 60;
    // draw 3DS game boxes
    Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(
        pksm::Species::Xerneas, 0, pksm::Generation::SIX, pksm::Gender::Genderless, x - 3, y - 2);
    Gui::text(
        "X", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::pkm(
        pksm::Species::Yveltal, 0, pksm::Generation::SIX, pksm::Gender::Genderless, x + 16, y + 20);
    Gui::text("Y", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

    x += 60;
    Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(
        pksm::Species::Groudon, 1, pksm::Generation::SIX, pksm::Gender::Genderless, x - 2, y - 2);
    Gui::text(
        "OR", x + 48 - 7, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::pkm(
        pksm::Species::Kyogre, 1, pksm::Generation::SIX, pksm::Gender::Genderless, x + 16, y + 20);
    Gui::text("AS", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

    x += 60;
    Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(pksm::Species::Solgaleo, 0, pksm::Generation::SEVEN, pksm::Gender::Genderless, x - 4,
        y - 2);
    Gui::text(
        "S", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::pkm(pksm::Species::Lunala, 0, pksm::Generation::SEVEN, pksm::Gender::Genderless, x + 18,
        y + 20);
    Gui::text("M", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

    x += 60;
    Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(pksm::Species::Necrozma, 1, pksm::Generation::SEVEN, pksm::Gender::Genderless, x - 3,
        y - 2);
    Gui::text(
        "US", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::pkm(pksm::Species::Necrozma, 2, pksm::Generation::SEVEN, pksm::Gender::Genderless, x + 19,
        y + 20);
    Gui::text("UM", x + 11, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

    x = 56, y += 60;
    // draw DS game boxes
    Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(
        pksm::Species::Dialga, 0, pksm::Generation::FOUR, pksm::Gender::Genderless, x - 4, y - 2);
    Gui::text(
        "D", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::pkm(
        pksm::Species::Palkia, 0, pksm::Generation::FOUR, pksm::Gender::Genderless, x + 18, y + 20);
    Gui::text("P", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

    x += 60;
    Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(
        pksm::Species::HoOh, 0, pksm::Generation::FOUR, pksm::Gender::Genderless, x - 5, y - 2);
    Gui::text(
        "HG", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::pkm(
        pksm::Species::Lugia, 0, pksm::Generation::FOUR, pksm::Gender::Genderless, x + 17, y + 20);
    Gui::text("SS", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

    x += 60;
    Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(
        pksm::Species::Giratina, 1, pksm::Generation::FOUR, pksm::Gender::Genderless, x + 8, y);
    Gui::text("Pt", x + 24, y + 30, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

    x += 60;
    Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(
        pksm::Species::Reshiram, 0, pksm::Generation::FIVE, pksm::Gender::Genderless, x - 3, y - 2);
    Gui::text(
        "B", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::pkm(
        pksm::Species::Zekrom, 0, pksm::Generation::FIVE, pksm::Gender::Genderless, x + 18, y + 20);
    Gui::text("W", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

    x += 60;
    Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(
        pksm::Species::Kyurem, 2, pksm::Generation::FIVE, pksm::Gender::Genderless, x - 6, y - 2);
    Gui::text(
        "B2", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::pkm(
        pksm::Species::Kyurem, 1, pksm::Generation::FIVE, pksm::Gender::Genderless, x + 16, y + 20);
    Gui::text("W2", x + 11, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

    if (saveGroup < 3)
    {
        Gui::drawSelector(115 + saveGroup * 60, 48);
    }
    else if (saveGroup < 7)
    {
        Gui::drawSelector(85 + (saveGroup - 3) * 60, 108);
    }
    else
    {
        Gui::drawSelector(55 + (saveGroup - 7) * 60, 168);
    }

    Gui::text(i18n::localize("GROUP_CHOICE_INSTR"), 200, 8, FONT_SIZE_11, COLOR_WHITE,
        TextPosX::CENTER, TextPosY::TOP);
}

void ExtraSavesScreen::update(touchPosition* touch)
{
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_DOWN)
    {
        if (saveGroup < 3)
        {
            saveGroup += 3;
        }
        else if (saveGroup < 7)
        {
            saveGroup += 4;
        }
        else if (saveGroup == 7 || saveGroup == 8)
        {
            saveGroup = 0;
        }
        else if (saveGroup == 9)
        {
            saveGroup = 1;
        }
        else if (saveGroup == 10 || saveGroup == 11)
        {
            saveGroup = 2;
        }
    }
    else if (downKeys & KEY_UP)
    {
        if (saveGroup == 11)
        {
            saveGroup -= 5;
        }
        else if (saveGroup >= 7)
        {
            saveGroup -= 4;
        }
        else if (saveGroup == 6)
        {
            saveGroup -= 4;
        }
        else if (saveGroup >= 3)
        {
            saveGroup -= 3;
        }
        else
        {
            saveGroup += 8;
        }
    }
    else if (downKeys & KEY_RIGHT)
    {
        if (saveGroup == 2)
        {
            saveGroup = 0;
        }
        else if (saveGroup == 6)
        {
            saveGroup = 3;
        }
        else if (saveGroup == 11)
        {
            saveGroup = 7;
        }
        else
        {
            saveGroup++;
        }
    }
    else if (downKeys & KEY_LEFT)
    {
        if (saveGroup == 0)
        {
            saveGroup = 2;
        }
        else if (saveGroup == 3)
        {
            saveGroup = 6;
        }
        else if (saveGroup == 7)
        {
            saveGroup = 11;
        }
        else
        {
            saveGroup--;
        }
    }
    if (downKeys & KEY_A)
    {
        Gui::setScreen(
            std::make_unique<ExtraSavesSubScreen>(ExtraSavesSubScreen::Group(saveGroup)));
    }
    else if (downKeys & KEY_B)
    {
        Gui::screenBack();
        return;
    }
}
