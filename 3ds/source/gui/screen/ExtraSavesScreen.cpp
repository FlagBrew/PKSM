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

#include "ExtraSavesScreen.hpp"
#include "Configuration.hpp"
#include "ExtraSavesSubScreen.hpp"
#include "gui.hpp"
#include "loader.hpp"
#include "thread.hpp"

ExtraSavesScreen::ExtraSavesScreen() : Screen(i18n::localize("A_CHOOSE_GROUP") + '\n' + i18n::localize("B_BACK")) {}

ExtraSavesScreen::~ExtraSavesScreen()
{
    Threads::create((ThreadFunc)TitleLoader::scanSaves);
}

void ExtraSavesScreen::drawBottom() const
{
    Gui::sprite(ui_sheet_part_info_bottom_idx, 0, 0);
}

void ExtraSavesScreen::drawTop() const
{
    Gui::drawSolidRect(0, 0, 400.0f, 240.0f, PKSM_Color(15, 22, 89, 255));

    Gui::sprite(ui_sheet_emulated_gameselector_bg_idx, 4, 29);
    Gui::sprite(ui_sheet_gameselector_cart_idx, 35, 93);

    int x = 90;
    int y = 68;

    // draw DS game boxes
    Gui::drawSolidRect(x += 60, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(483, 0, Generation::SEVEN, 2, x - 4, y - 2);
    Gui::text("D", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::pkm(484, 0, Generation::SEVEN, 2, x + 18, y + 20);
    Gui::text("P", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::drawSolidRect(x += 60, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(250, 0, Generation::SEVEN, 2, x - 5, y - 2);
    Gui::text("HG", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::pkm(249, 0, Generation::SEVEN, 2, x + 17, y + 20);
    Gui::text("SS", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::drawSolidRect(x += 60, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(643, 0, Generation::SEVEN, 2, x - 3, y - 2);
    Gui::text("B", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::pkm(644, 0, Generation::SEVEN, 2, x + 18, y + 20);
    Gui::text("W", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::drawSolidRect(x += 60, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(646, 2, Generation::SEVEN, 2, x - 6, y - 2);
    Gui::text("B2", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::pkm(646, 1, Generation::SEVEN, 2, x + 16, y + 20);
    Gui::text("W2", x + 11, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::drawSolidRect(40, 98, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(487, 1, Generation::SEVEN, 2, 48, 98);
    Gui::text("Pt", 40 + 24, 98 + 30, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

    x = 90, y = 128;
    // draw 3DS game boxes
    Gui::drawSolidRect(x += 60, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(716, 0, Generation::SEVEN, 2, x - 3, y - 2);
    Gui::text("X", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::pkm(717, 0, Generation::SEVEN, 2, x + 16, y + 20);
    Gui::text("Y", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::drawSolidRect(x += 60, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(383, 1, Generation::SEVEN, 2, x - 2, y - 2);
    Gui::text("OR", x + 48 - 7, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::pkm(382, 1, Generation::SEVEN, 2, x + 16, y + 20);
    Gui::text("AS", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::drawSolidRect(x += 60, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(791, 0, Generation::SEVEN, 2, x - 4, y - 2);
    Gui::text("S", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::pkm(792, 0, Generation::SEVEN, 2, x + 18, y + 20);
    Gui::text("M", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::drawSolidRect(x += 60, y, 48, 48, COLOR_HIGHBLUE);
    Gui::pkm(800, 1, Generation::SEVEN, 2, x - 3, y - 2);
    Gui::text("US", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::pkm(800, 2, Generation::SEVEN, 2, x + 19, y + 20);
    Gui::text("UM", x + 11, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

    if (saveGroup == -1)
    {
        Gui::drawSelector(39, 97);
    }
    else if (saveGroup < 4)
    {
        Gui::drawSelector(149 + saveGroup * 60, 67);
    }
    else
    {
        Gui::drawSelector(149 + (saveGroup - 4) * 60, 127);
    }

    Gui::text(i18n::localize("GROUP_CHOICE_INSTR"), 200, 8, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
}

void ExtraSavesScreen::update(touchPosition* touch)
{
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_DOWN)
    {
        if (saveGroup == -1)
        {
            saveGroup = 4;
        }
        else if (saveGroup < 4)
        {
            saveGroup += 4;
        }
        else
        {
            saveGroup -= 4;
        }
    }
    else if (downKeys & KEY_UP)
    {
        if (saveGroup == -1)
        {
            saveGroup = 0;
        }
        else if (saveGroup < 4)
        {
            saveGroup += 4;
        }
        else
        {
            saveGroup -= 4;
        }
    }
    else if (downKeys & KEY_RIGHT)
    {
        if (saveGroup == -1)
        {
            saveGroup = 0;
        }
        else if (saveGroup % 4 == 3)
        {
            saveGroup = -1;
        }
        else
        {
            saveGroup++;
        }
    }
    else if (downKeys & KEY_LEFT)
    {
        if (saveGroup == -1)
        {
            saveGroup = 3;
        }
        else if (saveGroup % 4 == 0)
        {
            saveGroup = -1;
        }
        else
        {
            saveGroup--;
        }
    }
    if (downKeys & KEY_A)
    {
        Gui::setScreen(std::make_unique<ExtraSavesSubScreen>(ExtraSavesSubScreen::Group(saveGroup)));
    }
    else if (downKeys & KEY_B)
    {
        Gui::screenBack();
        return;
    }
}
