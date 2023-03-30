/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2022 Bernardo Giordano, Admiral Fish, piepie62
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
#include "Species.hpp"
#include "thread.hpp"

ExtraSavesScreen::ExtraSavesScreen()
    : Screen(i18n::localize("A_CHOOSE_GROUP") + '\n' + i18n::localize("B_BACK") + "\n\uE004: " +
             i18n::localize("PREVIOUS_SYSTEMS") + "\n\uE005: " + i18n::localize("NEXT_SYSTEMS"))
{
    tabs.push_back(std::make_unique<ToggleButton>(
        1, 2, 104, 17,
        [&]()
        {
            systemGroup = SystemGroup::GB_GBC_GBA;
            saveGroup   = 0;
            return false;
        },
        ui_sheet_res_null_idx, i18n::localize("GB_GBC_GBA"), FONT_SIZE_11, COLOR_WHITE,
        ui_sheet_emulated_button_tabs_3_unselected_idx, i18n::localize("GB_GBC_GBA"), FONT_SIZE_11,
        COLOR_BLACK, &tabs, false));
    tabs.push_back(std::make_unique<ToggleButton>(
        108, 2, 104, 17,
        [&]()
        {
            systemGroup = SystemGroup::DS_3DS;
            saveGroup   = 0;
            return false;
        },
        ui_sheet_res_null_idx, i18n::localize("DS_3DS"), FONT_SIZE_11, COLOR_WHITE,
        ui_sheet_emulated_button_tabs_3_unselected_idx, i18n::localize("DS_3DS"), FONT_SIZE_11,
        COLOR_BLACK, &tabs, false));
    tabs.push_back(std::make_unique<ToggleButton>(
        215, 2, 104, 17,
        [&]()
        {
            systemGroup = SystemGroup::SWITCH;
            saveGroup   = 0;
            return false;
        },
        ui_sheet_res_null_idx, i18n::localize("SWITCH_CONSOLE"), FONT_SIZE_11, COLOR_WHITE,
        ui_sheet_emulated_button_tabs_3_unselected_idx, i18n::localize("SWITCH_CONSOLE"),
        FONT_SIZE_11, COLOR_BLACK, &tabs, false));
    tabs[0]->setState(true);
}

void ExtraSavesScreen::drawBottom() const
{
    Gui::sprite(ui_sheet_part_info_bottom_idx, 0, 0);

    for (const auto& tabButton : tabs)
    {
        tabButton->draw();
    }
}

void ExtraSavesScreen::drawTop() const
{
    Gui::drawSolidRect(0, 0, 400.0f, 240.0f, PKSM_Color(15, 22, 89, 255));

    Gui::sprite(ui_sheet_emulated_gameselector_bg_solid_idx, 4, 29);
    // Gui::sprite(ui_sheet_gameselector_cart_idx, 35, 93);

    if (systemGroup == SystemGroup::GB_GBC_GBA)
    {
        int x = 86, y = 79;
        // draw GB[C] game boxes
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::text(
            "R", x + 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
        Gui::text(
            "G", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
        Gui::pkm(pksm::Species::Squirtle, 0, pksm::Generation::ONE, pksm::Gender::Genderless,
            x + 17, y + 20);
        Gui::text(
            "B", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

        x += 60;
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::pkm(
            pksm::Species::Pikachu, 0, pksm::Generation::ONE, pksm::Gender::Genderless, x + 8, y);
        Gui::text("Y", x + 24, y + 30, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

        x += 60;
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::pkm(
            pksm::Species::HoOh, 0, pksm::Generation::TWO, pksm::Gender::Genderless, x - 5, y - 2);
        Gui::text(
            "G", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
        Gui::pkm(pksm::Species::Lugia, 0, pksm::Generation::TWO, pksm::Gender::Genderless, x + 17,
            y + 20);
        Gui::text(
            "S", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

        x += 60;
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::pkm(
            pksm::Species::Suicune, 0, pksm::Generation::TWO, pksm::Gender::Genderless, x + 8, y);
        Gui::text("C", x + 24, y + 30, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

        x = 116, y += 60;
        // draw GBA game boxes
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::pkm(pksm::Species::Groudon, 0, pksm::Generation::THREE, pksm::Gender::Genderless,
            x - 2, y - 2);
        Gui::text(
            "R", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
        Gui::pkm(pksm::Species::Kyogre, 0, pksm::Generation::THREE, pksm::Gender::Genderless,
            x + 16, y + 20);
        Gui::text(
            "S", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

        x += 60;
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::pkm(pksm::Species::Rayquaza, 0, pksm::Generation::THREE, pksm::Gender::Genderless,
            x + 8, y);
        Gui::text("E", x + 24, y + 30, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

        x += 60;
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::pkm(pksm::Species::Charizard, 0, pksm::Generation::THREE, pksm::Gender::Genderless,
            x - 3, y - 2);
        Gui::text("FR", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER,
            TextPosY::CENTER);
        Gui::pkm(pksm::Species::Venusaur, 0, pksm::Generation::THREE, pksm::Gender::Genderless,
            x + 17, y + 20);
        Gui::text(
            "LG", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

        if (saveGroup < 4)
        {
            Gui::drawSelector(85 + saveGroup * 60, 78);
        }
        else
        {
            Gui::drawSelector(115 + (saveGroup - 4) * 60, 138);
        }
    }
    else if (systemGroup == SystemGroup::DS_3DS)
    {
        int x = 56, y = 79;
        // draw DS game boxes
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::pkm(pksm::Species::Dialga, 0, pksm::Generation::FOUR, pksm::Gender::Genderless, x - 4,
            y - 2);
        Gui::text(
            "D", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
        Gui::pkm(pksm::Species::Palkia, 0, pksm::Generation::FOUR, pksm::Gender::Genderless, x + 18,
            y + 20);
        Gui::text(
            "P", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

        x += 60;
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::pkm(
            pksm::Species::HoOh, 0, pksm::Generation::FOUR, pksm::Gender::Genderless, x - 5, y - 2);
        Gui::text("HG", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER,
            TextPosY::CENTER);
        Gui::pkm(pksm::Species::Lugia, 0, pksm::Generation::FOUR, pksm::Gender::Genderless, x + 17,
            y + 20);
        Gui::text(
            "SS", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

        x += 60;
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::pkm(
            pksm::Species::Giratina, 1, pksm::Generation::FOUR, pksm::Gender::Genderless, x + 8, y);
        Gui::text("Pt", x + 24, y + 30, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

        x += 60;
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::pkm(pksm::Species::Reshiram, 0, pksm::Generation::FIVE, pksm::Gender::Genderless,
            x - 3, y - 2);
        Gui::text(
            "B", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
        Gui::pkm(pksm::Species::Zekrom, 0, pksm::Generation::FIVE, pksm::Gender::Genderless, x + 18,
            y + 20);
        Gui::text(
            "W", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

        x += 60;
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::pkm(pksm::Species::Kyurem, 2, pksm::Generation::FIVE, pksm::Gender::Genderless, x - 6,
            y - 2);
        Gui::text("B2", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER,
            TextPosY::CENTER);
        Gui::pkm(pksm::Species::Kyurem, 1, pksm::Generation::FIVE, pksm::Gender::Genderless, x + 16,
            y + 20);
        Gui::text(
            "W2", x + 11, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

        x = 86, y += 60;
        // draw 3DS game boxes
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::pkm(pksm::Species::Xerneas, 0, pksm::Generation::SIX, pksm::Gender::Genderless, x - 3,
            y - 2);
        Gui::text(
            "X", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
        Gui::pkm(pksm::Species::Yveltal, 0, pksm::Generation::SIX, pksm::Gender::Genderless, x + 16,
            y + 20);
        Gui::text(
            "Y", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

        x += 60;
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::pkm(pksm::Species::Groudon, 1, pksm::Generation::SIX, pksm::Gender::Genderless, x - 2,
            y - 2);
        Gui::text("OR", x + 48 - 7, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER,
            TextPosY::CENTER);
        Gui::pkm(pksm::Species::Kyogre, 1, pksm::Generation::SIX, pksm::Gender::Genderless, x + 16,
            y + 20);
        Gui::text(
            "AS", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

        x += 60;
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::pkm(pksm::Species::Solgaleo, 0, pksm::Generation::SEVEN, pksm::Gender::Genderless,
            x - 4, y - 2);
        Gui::text(
            "S", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
        Gui::pkm(pksm::Species::Lunala, 0, pksm::Generation::SEVEN, pksm::Gender::Genderless,
            x + 18, y + 20);
        Gui::text(
            "M", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

        x += 60;
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::pkm(pksm::Species::Necrozma, 1, pksm::Generation::SEVEN, pksm::Gender::Genderless,
            x - 3, y - 2);
        Gui::text("US", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER,
            TextPosY::CENTER);
        Gui::pkm(pksm::Species::Necrozma, 2, pksm::Generation::SEVEN, pksm::Gender::Genderless,
            x + 19, y + 20);
        Gui::text(
            "UM", x + 11, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

        if (saveGroup < 5)
        {
            Gui::drawSelector(55 + saveGroup * 60, 78);
        }
        else
        {
            Gui::drawSelector(85 + (saveGroup - 5) * 60, 138);
        }
    }
    else
    {
        int x = 86, y = 79;
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::pkm(pksm::Species::Pikachu, 0, pksm::Generation::LGPE, pksm::Gender::Genderless, x - 5,
            y - 2);
        Gui::text("LG", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER,
            TextPosY::CENTER);
        Gui::pkm(pksm::Species::Eevee, 0, pksm::Generation::LGPE, pksm::Gender::Genderless, x + 17,
            y + 20);
        Gui::text(
            "PE", x + 9, y + 37, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);

        /*
        x += 60;
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::pkm(
            pksm::Species::Zacian, 1, pksm::Generation::EIGHT, pksm::Gender::Genderless, x - 5, y -
        2); Gui::text( "Sw", x + 48 - 9, y + 12, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER,
        TextPosY::CENTER); Gui::pkm( pksm::Species::Zamazenta, 1, pksm::Generation::EIGHT,
        pksm::Gender::Genderless, x + 17, y + 20); Gui::text("Sh", x + 9, y + 37, FONT_SIZE_11,
        COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
        */

        Gui::drawSelector(85 + saveGroup * 60, 78);
    }

    Gui::text(i18n::localize("GROUP_CHOICE_INSTR"), 200, 8, FONT_SIZE_11, COLOR_WHITE,
        TextPosX::CENTER, TextPosY::TOP);
}

void ExtraSavesScreen::update(touchPosition* touch)
{
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_DOWN)
    {
        switch (systemGroup)
        {
            case SystemGroup::GB_GBC_GBA:
                if (saveGroup == 3)
                {
                    saveGroup = 6;
                }
                else if (saveGroup <= 2)
                {
                    saveGroup += 4;
                }
                else
                {
                    saveGroup -= 4;
                }
                break;
            case SystemGroup::DS_3DS:
                if (saveGroup == 4)
                {
                    saveGroup = 8;
                }
                else if (saveGroup <= 3)
                {
                    saveGroup += 5;
                }
                else
                {
                    saveGroup -= 5;
                }
                break;
            case SystemGroup::SWITCH:
                // no-op because currently only one row
                break;
            default:
                break;
        }
    }
    else if (downKeys & KEY_UP)
    {
        // i've seen this one i've seen this one! it's a classic
        switch (systemGroup)
        {
            case SystemGroup::GB_GBC_GBA:
                if (saveGroup == 3)
                {
                    saveGroup = 6;
                }
                else if (saveGroup < 3)
                {
                    saveGroup += 4;
                }
                else
                {
                    saveGroup -= 4;
                }
                break;
            case SystemGroup::DS_3DS:
                if (saveGroup == 4)
                {
                    saveGroup = 8;
                }
                else if (saveGroup < 4)
                {
                    saveGroup += 5;
                }
                else
                {
                    saveGroup -= 5;
                }
                break;
            case SystemGroup::SWITCH:
                // no-op because currently only one row
                break;
            default:
                break;
        }
    }
    else if (downKeys & KEY_RIGHT)
    {
        switch (systemGroup)
        {
            case SystemGroup::GB_GBC_GBA:
                if (saveGroup == 3)
                {
                    saveGroup = 0;
                }
                else if (saveGroup == 6)
                {
                    saveGroup = 4;
                }
                else
                {
                    saveGroup++;
                }
                break;
            case SystemGroup::DS_3DS:
                if (saveGroup == 4)
                {
                    saveGroup = 0;
                }
                else if (saveGroup == 8)
                {
                    saveGroup = 5;
                }
                else
                {
                    saveGroup++;
                }
                break;
            case SystemGroup::SWITCH:
                /*
                if (saveGroup == 1)
                {
                    saveGroup = 0;
                }
                else
                {
                    saveGroup++;
                }
                */
                // no-op because currently only one game
                break;
            default:
                break;
        }
    }
    else if (downKeys & KEY_LEFT)
    {
        switch (systemGroup)
        {
            case SystemGroup::GB_GBC_GBA:
                if (saveGroup == 0)
                {
                    saveGroup = 3;
                }
                else if (saveGroup == 4)
                {
                    saveGroup = 6;
                }
                else
                {
                    saveGroup--;
                }
                break;
            case SystemGroup::DS_3DS:
                if (saveGroup == 0)
                {
                    saveGroup = 4;
                }
                else if (saveGroup == 5)
                {
                    saveGroup = 8;
                }
                else
                {
                    saveGroup--;
                }
                break;
            case SystemGroup::SWITCH:
                /*
                if (saveGroup == 0)
                {
                    saveGroup = 1;
                }
                else
                {
                    saveGroup--;
                }
                */
                // no-op because currently only one game
                break;
            default:
                break;
        }
    }

    if (downKeys & KEY_A)
    {
        Gui::setScreen(
            std::make_unique<ExtraSavesSubScreen>(calcSaveGroupFromSystem(systemGroup, saveGroup)));
    }
    else if (downKeys & KEY_B)
    {
        Threads::executeTask(TitleLoader::scanSaves);
        Gui::screenBack();
        return;
    }

    if (downKeys & KEY_L)
    {
        systemGroup--;
        saveGroup = 0;
        tabs[u8(systemGroup)]->setState(true);
    }
    else if (downKeys & KEY_R)
    {
        systemGroup++;
        saveGroup = 0;
        tabs[u8(systemGroup)]->setState(true);
    }

    for (auto& button : tabs)
    {
        button->update(touch);
    }
}
