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

#include "SaveLoadScreen.hpp"
#include "AccelButton.hpp"
#include "ClickButton.hpp"
#include "ConfigScreen.hpp"
#include "Configuration.hpp"
#include "enums/Species.hpp"
#include "ExtraSavesSubScreen.hpp"
#include "gui.hpp"
#include "loader.hpp"
#include "MainMenu.hpp"
#include "Title.hpp"
#include "TitleLoadScreen.hpp"

namespace
{
    constexpr std::array<std::string_view, 9> dsIds = {
        "ADA", // Diamond
        "APA", // Pearl
        "CPU", // Platinum
        "IPK", // HeartGold
        "IPG", // SoulSilver
        "IRB", // Black
        "IRA", // White
        "IRE", // Black 2
        "IRD"  // White 2
    };
    constexpr std::array<std::string_view, 9> dsPrefixes = {
        "D: ", "P: ", "Pt: ", "HG: ", "SS: ", "B: ", "W: ", "B2: ", "W2: "};
    constexpr std::array<int, 9> dsGroups = {7, 7, 9, 8, 8, 10, 10, 11, 11};

    constexpr size_t CTR_TITLES = 24;

    std::array<std::string, CTR_TITLES> ctrIds;
    // Used to get ctrIds
    constexpr std::array<pksm::GameVersion, CTR_TITLES> ctrVersions = {pksm::GameVersion::R,
        pksm::GameVersion::S, pksm::GameVersion::E, pksm::GameVersion::FR, pksm::GameVersion::LG,
        pksm::GameVersion::X, pksm::GameVersion::Y, pksm::GameVersion::OR, pksm::GameVersion::AS,
        pksm::GameVersion::SN, pksm::GameVersion::MN, pksm::GameVersion::US, pksm::GameVersion::UM,
        pksm::GameVersion::RD, pksm::GameVersion::GN, pksm::GameVersion::BU, pksm::GameVersion::YW,
        pksm::GameVersion::GD, pksm::GameVersion::SV, pksm::GameVersion::C, pksm::GameVersion::GP,
        pksm::GameVersion::GE, pksm::GameVersion::SW, pksm::GameVersion::SH};
    constexpr std::array<std::string_view, CTR_TITLES> ctrPrefixes  = {
        "R: ", "S: ", "E: ", "FR: ", "LG: ", "X: ", "Y: ", "OR: ", "AS: ", "SU: ", "MO: ", "US: ",
        "UM: ", "R: ", "G: ", "B: ", "Y: ", "G: ", "S: ", "C: ", "LGP: ", "LGE: ", "Sw: ", "Sh: "};
    constexpr std::array<int, CTR_TITLES> ctrGroups = {
        4, 4, 5, 6, 6, 12, 12, 13, 13, 14, 14, 15, 15, 0, 0, 0, 1, 2, 2, 3, 16, 16, 17, 17};

    constexpr std::array<std::string_view, 18> names = {"RGB", "Y", "GS", "C", "RS", "E", "FRLG",
        "DP", "HGSS", "Pt", "BW", "B2W2", "XY", "ORAS", "SUMO", "USUM", "LGPE", "SwSh"};
}

SaveLoadScreen::SaveLoadScreen()
    : Screen(i18n::localize("A_SELECT") + '\n' + i18n::localize("X_SETTINGS") + '\n' +
             i18n::localize("Y_PRESENT") + '\n' + i18n::localize("START_EXIT") + "\n\uE004: " +
             i18n::localize("PREVIOUS_SYSTEMS") + "\n\uE005: " + i18n::localize("NEXT_SYSTEMS"))
{
    oldLang = Configuration::getInstance().language();
    buttons.push_back(std::make_unique<Button>(
        200, 147, 96, 51,
        [this]()
        {
            mustUpdateTitles = true;
            Gui::setScreen(std::make_unique<ExtraSavesSubScreen>(
                calcSaveGroupFromSystem(systemGroup, saveGroup)));
            return true;
        },
        ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK));
    buttons.push_back(std::make_unique<AccelButton>(
        24, 96, 175, 16, [this]() { return this->setSelectedSave(0); }, ui_sheet_res_null_idx, "",
        0.0f, COLOR_BLACK, 10, 10));
    for (int i = 1; i < 5; i++)
    {
        buttons.push_back(std::make_unique<ClickButton>(
            24, 96 + 17 * i, 175, 16, [this, i]() { return this->setSelectedSave(i); },
            ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK));
    }
    buttons.push_back(std::make_unique<AccelButton>(
        24, 181, 175, 16, [this]() { return this->setSelectedSave(5); }, ui_sheet_res_null_idx, "",
        0.0f, COLOR_BLACK, 10, 10));
    buttons.push_back(std::make_unique<Button>(
        200, 95, 96, 51, [this]() { return this->loadSave(); }, ui_sheet_res_null_idx, "", 0.0f,
        COLOR_BLACK));

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
    tabs[1]->setState(true);

    updateTitles();
}

void SaveLoadScreen::makeInstructions()
{
    instructions =
        Instructions(i18n::localize("A_SELECT") + '\n' + i18n::localize("X_SETTINGS") + '\n' +
                     i18n::localize("Y_PRESENT") + '\n' + i18n::localize("START_EXIT") +
                     "\n\uE004: " + i18n::localize("PREVIOUS_SYSTEMS") +
                     "\n\uE005: " + i18n::localize("NEXT_SYSTEMS"));
}

void SaveLoadScreen::drawTop(void) const
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

    Gui::text(i18n::localize("LOADER_INSTRUCTIONS_TOP_PRESENT"), 200, 8, FONT_SIZE_11, COLOR_WHITE,
        TextPosX::CENTER, TextPosY::TOP);
}

void SaveLoadScreen::drawBottom() const
{
    SaveGroup fullSaveGroup = calcSaveGroupFromSystem(systemGroup, saveGroup);

    Gui::backgroundBottom(true);
    Gui::sprite(ui_sheet_gameselector_savebox_idx, 22, 94);

    for (const auto& tabButton : tabs)
    {
        tabButton->draw();
    }

    Gui::text(std::string(names[u8(fullSaveGroup)]), 27, 26, FONT_SIZE_14, COLOR_WHITE,
        TextPosX::LEFT, TextPosY::TOP);

    if (selectedSave > -1)
    {
        Gui::drawSolidRect(24, 96 + 17 * selectedSave, 174, 16, PKSM_Color(0x0f, 0x16, 0x59, 255));
    }

    int y = 97;
    for (int i = firstSave; i < firstSave + 6; i++)
    {
        if (i < (int)saves[u8(fullSaveGroup)].size())
        {
            std::string save = saves[u8(fullSaveGroup)][i].second.substr(
                0, saves[u8(fullSaveGroup)][i].second.find_last_of('/'));
            save = save.substr(save.find_last_of('/') + 1);
            save = saves[u8(fullSaveGroup)][i].first + save;
            if (i - firstSave == selectedSave)
            {
                Gui::text(save, 29, y, FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP,
                    TextWidthAction::SCROLL, 169.0f);
            }
            else
            {
                Gui::text(save, 29, y, FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP,
                    TextWidthAction::SLICE, 169.0f);
            }
        }
        else
        {
            break;
        }
        y += 17;
    }

    if (selectedSave > 0 && firstSave > 0)
    {
        Gui::drawSolidRect(191, 102, 4, 5, PKSM_Color(0x0f, 0x16, 0x59, 255));
        Gui::drawSolidTriangle(189, 102, 197, 102, 193, 97, PKSM_Color(0x0f, 0x16, 0x59, 255));
    }

    if (selectedSave < 5 && saves[u8(fullSaveGroup)].size() != 0 &&
        (size_t)firstSave + 5 < saves[u8(fullSaveGroup)].size() - 1)
    {
        Gui::drawSolidRect(191, 186, 4, 5, PKSM_Color(0x0f, 0x16, 0x59, 255));
        Gui::drawSolidTriangle(189, 191, 197, 191, 193, 196, PKSM_Color(0x0f, 0x16, 0x59, 255));
    }

    Gui::text(i18n::localize("LOADER_LOAD"), 248, 113, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER,
        TextPosY::TOP);
    Gui::text(i18n::localize("ADD_SAVE"), 248, 163, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER,
        TextPosY::TOP);

    Gui::text(i18n::localize("LOADER_INSTRUCTIONS_BOTTOM"), 160, 223, FONT_SIZE_11, COLOR_WHITE,
        TextPosX::CENTER, TextPosY::TOP);

    Gui::drawSolidRect(245, 23, 48, 48, COLOR_BLACK);
    Gui::drawSolidRect(243, 21, 52, 52, PKSM_Color(15, 22, 89, 255));
    Gui::sprite(ui_sheet_gameselector_unknown_idx, 245, 23);
}

void SaveLoadScreen::update(touchPosition* touch)
{
    SaveGroup fullSaveGroup = calcSaveGroupFromSystem(systemGroup, saveGroup);

    if (mustUpdateTitles)
    {
        mustUpdateTitles = false;
        updateTitles();
        return;
    }
    if (oldLang != Configuration::getInstance().language())
    {
        oldLang = Configuration::getInstance().language();
        makeInstructions();
    }
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_START)
    {
        Gui::exitMainLoop();
        return;
    }
    if (selectedGroup)
    {
        if (downKeys & KEY_B)
        {
            selectedGroup = false;
            selectedSave  = -1;
            firstSave     = 0;
        }
        if (downKeys & KEY_A)
        {
            loadSave();
            return;
        }
        if (downKeys & KEY_DOWN)
        {
            if (selectedSave == 4)
            {
                if (firstSave + 5 < (int)saves[u8(fullSaveGroup)].size() - 1)
                {
                    firstSave++;
                }
                else if (firstSave + selectedSave < (int)saves[u8(fullSaveGroup)].size() - 1)
                {
                    selectedSave++;
                }
            }
            else
            {
                if (firstSave + selectedSave < (int)saves[u8(fullSaveGroup)].size() - 1)
                {
                    selectedSave++;
                }
            }
        }
        if (downKeys & KEY_UP)
        {
            if (selectedSave == 1)
            {
                if (firstSave > 0)
                {
                    firstSave--;
                }
                else
                {
                    selectedSave--;
                }
            }
            else if (selectedSave != 0)
            {
                selectedSave--;
            }
        }
        for (auto& button : buttons)
        {
            button->update(touch);
        }
    }
    else
    {
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
                    break;
                default:
                    break;
            }
        }
        else if (downKeys & KEY_Y)
        {
            Gui::screenBack();
            Gui::setScreen(std::make_unique<TitleLoadScreen>());
            return;
        }
        if (downKeys & KEY_A)
        {
            if (saves[u8(fullSaveGroup)].size() != 0)
            {
                selectedGroup = true;
                selectedSave  = 0;
            }
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
        if (buttons[0]->update(touch))
        {
            return;
        }
        for (auto& button : tabs)
        {
            button->update(touch);
        }
    }
    if (downKeys & KEY_X)
    {
        mustUpdateTitles = true;
        Gui::setScreen(std::make_unique<ConfigScreen>());
    }
}

bool SaveLoadScreen::loadSave()
{
    SaveGroup fullSaveGroup = calcSaveGroupFromSystem(systemGroup, saveGroup);

    if (TitleLoader::load(nullptr, saves[u8(fullSaveGroup)][selectedSave + firstSave].second))
    {
        Gui::setScreen(std::make_unique<MainMenu>());
        return true;
    }
    return false;
}

bool SaveLoadScreen::setSelectedSave(int i)
{
    SaveGroup fullSaveGroup = calcSaveGroupFromSystem(systemGroup, saveGroup);

    if (i == 5)
    {
        if (firstSave + 5 < (int)saves[u8(fullSaveGroup)].size() - 1)
        {
            firstSave++;
            selectedSave = 4;
        }
        else if (firstSave + 5 < (int)saves[u8(fullSaveGroup)].size())
        {
            selectedSave = 5;
        }
    }
    else if (i == 0 && firstSave != 0)
    {
        firstSave--;
        selectedSave = 1;
    }
    else if (firstSave + i < (int)saves[u8(fullSaveGroup)].size())
    {
        selectedSave = i;
    }
    return false;
}

void SaveLoadScreen::updateTitles()
{
    for (size_t i = 0; i < ctrVersions.size(); i++)
    {
        std::string tidStr = Configuration::getInstance().titleId(ctrVersions[i]);
        u64 tid            = strtoull(tidStr.c_str(), nullptr, 16);
        ctrIds[i]          = Title::tidToCheckpointPrefix(tid);
    }

    for (auto& i : saves)
    {
        i.clear();
    }

    auto allSaves = TitleLoader::sdSaves.lock();
    for (auto i = allSaves->begin(); i != allSaves->end(); i++)
    {
        if (i->first.size() == 4)
        {
            std::string key = i->first.substr(0, 3);
            auto found      = std::find(dsIds.begin(), dsIds.end(), key);
            if (found != dsIds.end())
            {
                auto num = std::distance(dsIds.begin(), found);
                for (size_t j = 0; j < i->second.size(); j++)
                {
                    saves[dsGroups[num]].emplace_back(std::string(dsPrefixes[num]), i->second[j]);
                }
            }
        }
        else
        {
            auto found = std::find(ctrIds.begin(), ctrIds.end(), i->first);
            if (found != ctrIds.end())
            {
                auto num = std::distance(ctrIds.begin(), found);
                for (size_t j = 0; j < i->second.size(); j++)
                {
                    saves[ctrGroups[num]].emplace_back(std::string(ctrPrefixes[num]), i->second[j]);
                }
            }
        }
    }
}
