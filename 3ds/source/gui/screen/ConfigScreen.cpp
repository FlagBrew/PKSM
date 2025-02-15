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

#include "ConfigScreen.hpp"
#include "AccelButton.hpp"
#include "banks.hpp"
#include "ClickButton.hpp"
#include "Configuration.hpp"
#include "EditorScreen.hpp"
#include "ExtraSavesScreen.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "loader.hpp"
#include "PkmUtils.hpp"
#include "pkx/PKX.hpp"
#include "QRScanner.hpp"
#include "revision.h"
#include "thread.hpp"
#include "TitleIdOverlay.hpp"
#include "ToggleButton.hpp"
#include "utils/format.hpp"
#include <format>

namespace
{
    // TODO: change these to use i18n
    constexpr std::array<std::string_view, 14> credits = {"GitHub: github.com/FlagBrew/PKSM",
        "Credits:", "piepie62 and Admiral-Fish for their dedication",
        "dsoldier for the gorgeous graphic work", "SpiredMoth, trainboy2019 and all the scripters",
        "The whole FlagBrew team for collaborating with us",
        "Kaphotics and SciresM for PKHeX documentation",
        "fincs and WinterMute for citro2d and devkitARM",
        "kamronbatman and ProjectPokemon for EventsGallery", "All of the translators",
        "Subject21_J and all the submitters for PKSM's icon",
        "Allen (FMCore/FM1337/Sigkill) for the GPSS/Local GPSS", "Bernardo for creating PKSM"};

    void inputNumber(std::function<void(int)> callback, int digits, int maxValue)
    {
        SwkbdState state;
        swkbdInit(&state, SWKBD_TYPE_NUMPAD, 2, digits);
        swkbdSetFeatures(&state, SWKBD_FIXED_WIDTH);
        swkbdSetValidation(&state, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
        char input[digits + 1] = {0};
        SwkbdButton ret        = swkbdInputText(&state, input, sizeof(input));
        input[digits]          = '\0';
        if (ret == SWKBD_BUTTON_CONFIRM)
        {
            int tid = std::stoi(input);
            callback(std::min(maxValue, tid));
        }
    }

    void inputPatronCode()
    {
        SwkbdState state;
        swkbdInit(&state, SWKBD_TYPE_QWERTY, 3, 22);
        swkbdSetHintText(&state, i18n::localize("PATRON_CODE").c_str());
        std::string patronCode = Configuration::getInstance().patronCode();
        swkbdSetInitialText(&state, patronCode.c_str());
        swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
        swkbdSetButton(
            &state, SwkbdButton::SWKBD_BUTTON_MIDDLE, i18n::localize("QR_SCANNER").c_str(), false);
        char input[45]  = {0};
        SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
        input[44]       = '\0';
        if (ret == SWKBD_BUTTON_MIDDLE)
        {
            std::string data = QRScanner<std::string>::scan();
            if (data.length() == 22)
            {
                std::copy(data.begin(), data.end(), input);
                input[22] = '\0';
                ret       = SWKBD_BUTTON_CONFIRM;
            }
            else if (!data.empty())
            {
                Gui::warn(i18n::localize("QR_WRONG_FORMAT"));
            }
        }
        if (ret == SWKBD_BUTTON_CONFIRM)
        {
            Configuration::getInstance().patronCode(input);
        }
    }

    void inputApiUrl()
    {
        SwkbdState state;
        swkbdInit(&state, SWKBD_TYPE_QWERTY, 3, 29);
        swkbdSetHintText(&state, i18n::localize("API_HINT").c_str());
        std::string apiUri = Configuration::getInstance().apiUrl();
        swkbdSetInitialText(&state, apiUri.c_str());
        swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
        char input[88]  = {0};
        SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
        input[87]       = '\0';
        if (ret == SWKBD_BUTTON_CONFIRM)
        {
            Configuration::getInstance().apiUrl(input);
        }
    }

    u8 getNextAlpha(int off)
    {
        static u8 retVals[14] = {
            190, 195, 200, 205, 210, 215, 220, 225, 230, 235, 240, 245, 250, 255};
        static bool up[14] = {false, false, false, false, false, false, false, false, false, false,
            false, false, false};
        static u8 timers[14] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
        if (timers[off] == 0)
        {
            if (retVals[off] < 105 && !up[off])
            {
                up[off] = true;
            }
            else if (retVals[off] == 255 && up[off])
            {
                up[off] = false;
            }
            else if (up[off])
            {
                retVals[off] += 5;
            }
            else
            {
                retVals[off] -= 5;
            }
            timers[off] = 3;
        }
        timers[off]--;
        return retVals[off];
    }
}

ConfigScreen::ConfigScreen()
{
    initButtons();
}

void ConfigScreen::initButtons()
{
    tabs.clear();
    for (auto& tab : tabButtons)
    {
        tab.clear();
    }

    tabs.push_back(std::make_unique<ToggleButton>(
        1, 2, 104, 17,
        [&]()
        {
            currentTab = 0;
            return false;
        },
        ui_sheet_res_null_idx, i18n::localize("LANGUAGE"), FONT_SIZE_11, COLOR_WHITE,
        ui_sheet_emulated_button_tabs_3_unselected_idx, i18n::localize("LANGUAGE"), FONT_SIZE_11,
        COLOR_BLACK, &tabs, false));
    tabs.push_back(std::make_unique<ToggleButton>(
        108, 2, 104, 17,
        [&]()
        {
            currentTab = 1;
            return false;
        },
        ui_sheet_res_null_idx, i18n::localize("DEFAULTS"), FONT_SIZE_11, COLOR_WHITE,
        ui_sheet_emulated_button_tabs_3_unselected_idx, i18n::localize("DEFAULTS"), FONT_SIZE_11,
        COLOR_BLACK, &tabs, false));
    tabs.push_back(std::make_unique<ToggleButton>(
        215, 2, 52, 17,
        [&]()
        {
            currentTab = 2;
            return false;
        },
        ui_sheet_res_null_idx, i18n::localize("MISC"), FONT_SIZE_11, COLOR_WHITE,
        ui_sheet_emulated_button_tabs_3_unselected_idx, i18n::localize("MISC"), FONT_SIZE_11,
        COLOR_BLACK, &tabs, false));
    tabs.push_back(std::make_unique<ToggleButton>(
        267, 2, 30, 17,
        [&]()
        {
            currentTab = 4;
            return false;
        },
        ui_sheet_res_null_idx, "API", FONT_SIZE_11, COLOR_WHITE,
        ui_sheet_emulated_button_tabs_3_unselected_idx, "API", FONT_SIZE_11,
        COLOR_BLACK, &tabs, false));
    tabs[0]->setState(true);

    // First column of language buttons
    tabButtons[0].push_back(std::make_unique<ToggleButton>(
        37, 52, 8, 8,
        [this]()
        {
            Gui::clearText();
            Configuration::getInstance().language(pksm::Language::JPN);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK,
        ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));
    tabButtons[0].push_back(std::make_unique<ToggleButton>(
        37, 74, 8, 8,
        [this]()
        {
            Gui::clearText();
            Configuration::getInstance().language(pksm::Language::ENG);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK,
        ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));
    tabButtons[0].push_back(std::make_unique<ToggleButton>(
        37, 96, 8, 8,
        [this]()
        {
            Gui::clearText();
            Configuration::getInstance().language(pksm::Language::FRE);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK,
        ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));
    tabButtons[0].push_back(std::make_unique<ToggleButton>(
        37, 118, 8, 8,
        [this]()
        {
            Gui::clearText();
            Configuration::getInstance().language(pksm::Language::GER);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK,
        ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));
    tabButtons[0].push_back(std::make_unique<ToggleButton>(
        37, 140, 8, 8,
        [this]()
        {
            Gui::clearText();
            Configuration::getInstance().language(pksm::Language::ITA);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK,
        ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));
    tabButtons[0].push_back(std::make_unique<ToggleButton>(
        37, 162, 8, 8,
        [this]()
        {
            Gui::clearText();
            Configuration::getInstance().language(pksm::Language::SPA);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK,
        ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));

    // Second column of language buttons
    tabButtons[0].push_back(std::make_unique<ToggleButton>(
        177, 52, 8, 8,
        [this]()
        {
            Gui::clearText();
            Configuration::getInstance().language(pksm::Language::CHS);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK,
        ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));
    tabButtons[0].push_back(std::make_unique<ToggleButton>(
        177, 74, 8, 8,
        [this]()
        {
            Gui::clearText();
            Configuration::getInstance().language(pksm::Language::KOR);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK,
        ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));
    tabButtons[0].push_back(std::make_unique<ToggleButton>(
        177, 96, 8, 8,
        [this]()
        {
            Gui::clearText();
            Configuration::getInstance().language(pksm::Language::NL);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK,
        ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));
    tabButtons[0].push_back(std::make_unique<ToggleButton>(
        177, 118, 8, 8,
        [this]()
        {
            Gui::clearText();
            Configuration::getInstance().language(pksm::Language::PT);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK,
        ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));
    tabButtons[0].push_back(std::make_unique<ToggleButton>(
        177, 140, 8, 8,
        [this]()
        {
            Gui::clearText();
            Configuration::getInstance().language(pksm::Language::RO);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK,
        ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));

    switch (Configuration::getInstance().language())
    {
        case pksm::Language::JPN:
            ((ToggleButton*)tabButtons[0][0].get())->setState(true);
            break;
        case pksm::Language::ENG:
            ((ToggleButton*)tabButtons[0][1].get())->setState(true);
            break;
        case pksm::Language::FRE:
            ((ToggleButton*)tabButtons[0][2].get())->setState(true);
            break;
        case pksm::Language::GER:
            ((ToggleButton*)tabButtons[0][3].get())->setState(true);
            break;
        case pksm::Language::ITA:
            ((ToggleButton*)tabButtons[0][4].get())->setState(true);
            break;
        case pksm::Language::SPA:
            ((ToggleButton*)tabButtons[0][5].get())->setState(true);
            break;
        case pksm::Language::CHS:
            ((ToggleButton*)tabButtons[0][6].get())->setState(true);
            break;
        case pksm::Language::KOR:
            ((ToggleButton*)tabButtons[0][7].get())->setState(true);
            break;
        case pksm::Language::NL:
            ((ToggleButton*)tabButtons[0][8].get())->setState(true);
            break;
        case pksm::Language::PT:
            ((ToggleButton*)tabButtons[0][9].get())->setState(true);
            break;
        case pksm::Language::RO:
            ((ToggleButton*)tabButtons[0][10].get())->setState(true);
            break;
        default:
            // Default to English
            ((ToggleButton*)tabButtons[0][1].get())->setState(true);
            break;
    }

    // Defaults buttons
    tabButtons[1].push_back(std::make_unique<Button>(
        140, 32, 15, 12,
        []()
        {
            Gui::setScreen(
                std::make_unique<EditorScreen>(PkmUtils::getDefault(pksm::Generation::ONE),
                    EditorScreen::PARTY_MAGIC_NUM, 0, true));
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[1].push_back(std::make_unique<Button>(
        140, 48, 15, 12,
        []()
        {
            Gui::setScreen(
                std::make_unique<EditorScreen>(PkmUtils::getDefault(pksm::Generation::TWO),
                    EditorScreen::PARTY_MAGIC_NUM, 0, true));
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[1].push_back(std::make_unique<Button>(
        140, 64, 15, 12,
        []()
        {
            Gui::setScreen(
                std::make_unique<EditorScreen>(PkmUtils::getDefault(pksm::Generation::THREE),
                    EditorScreen::PARTY_MAGIC_NUM, 0, true));
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[1].push_back(std::make_unique<Button>(
        140, 80, 15, 12,
        []()
        {
            Gui::setScreen(
                std::make_unique<EditorScreen>(PkmUtils::getDefault(pksm::Generation::FOUR),
                    EditorScreen::PARTY_MAGIC_NUM, 0, true));
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[1].push_back(std::make_unique<Button>(
        140, 96, 15, 12,
        []()
        {
            Gui::setScreen(
                std::make_unique<EditorScreen>(PkmUtils::getDefault(pksm::Generation::FIVE),
                    EditorScreen::PARTY_MAGIC_NUM, 0, true));
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[1].push_back(std::make_unique<Button>(
        140, 112, 15, 12,
        []()
        {
            Gui::setScreen(
                std::make_unique<EditorScreen>(PkmUtils::getDefault(pksm::Generation::SIX),
                    EditorScreen::PARTY_MAGIC_NUM, 0, true));
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[1].push_back(std::make_unique<ClickButton>(
        140, 128, 15, 12,
        []()
        {
            Gui::setScreen(
                std::make_unique<EditorScreen>(PkmUtils::getDefault(pksm::Generation::SEVEN),
                    EditorScreen::PARTY_MAGIC_NUM, 0, true));
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[1].push_back(std::make_unique<ClickButton>(
        140, 144, 15, 12,
        [this]()
        {
            Gui::setScreen(
                std::make_unique<EditorScreen>(PkmUtils::getDefault(pksm::Generation::LGPE),
                    EditorScreen::PARTY_MAGIC_NUM, 0, true));
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[1].push_back(std::make_unique<ClickButton>(
        140, 160, 15, 12,
        [this]()
        {
            Gui::setScreen(
                std::make_unique<EditorScreen>(PkmUtils::getDefault(pksm::Generation::EIGHT),
                    EditorScreen::PARTY_MAGIC_NUM, 0, true));
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[1].push_back(std::make_unique<Button>(
        140, 176, 15, 12,
        []()
        {
            inputNumber([](u16 a) { Configuration::getInstance().day(a); }, 2, 31);
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[1].push_back(std::make_unique<Button>(
        140, 192, 15, 12,
        []()
        {
            inputNumber([](u16 a) { Configuration::getInstance().month(a); }, 2, 12);
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[1].push_back(std::make_unique<Button>(
        140, 208, 15, 12,
        []()
        {
            inputNumber([](u16 a) { Configuration::getInstance().year(a); }, 4, 9999);
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));

    // Miscellaneous buttons
    tabButtons[2].push_back(std::make_unique<ClickButton>(
        247, 32, 15, 12,
        []()
        {
            Configuration::getInstance().autoBackup(!Configuration::getInstance().autoBackup());
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[2].push_back(std::make_unique<ClickButton>(
        247, 52, 15, 12,
        []()
        {
            Configuration::getInstance().transferEdit(!Configuration::getInstance().transferEdit());
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[2].push_back(std::make_unique<ClickButton>(
        247, 72, 15, 12,
        []()
        {
            Configuration::getInstance().writeFileSave(
                !Configuration::getInstance().writeFileSave());
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[2].push_back(std::make_unique<ClickButton>(
        247, 92, 15, 12,
        []()
        {
            Configuration::getInstance().useSaveInfo(!Configuration::getInstance().useSaveInfo());
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[2].push_back(std::make_unique<ClickButton>(
        247, 112, 15, 12,
        [this]()
        {
            Configuration::getInstance().useExtData(!Configuration::getInstance().useExtData());
            useExtDataChanged = !useExtDataChanged;
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[2].push_back(std::make_unique<ClickButton>(
        247, 132, 15, 12,
        []()
        {
            Configuration::getInstance().randomMusic(!Configuration::getInstance().randomMusic());
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[2].push_back(std::make_unique<ClickButton>(
        247, 152, 15, 12,
        [this]()
        {
            Configuration::getInstance().showBackups(!Configuration::getInstance().showBackups());
            showBackupsChanged = !showBackupsChanged;
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[2].push_back(std::make_unique<ClickButton>(
        247, 172, 15, 12,
        [this]()
        {
            Configuration::getInstance().autoUpdate(!Configuration::getInstance().autoUpdate());
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[2].push_back(std::make_unique<ClickButton>(
        247, 192, 15, 12,
        [this]()
        {
            Gui::setScreen(std::make_unique<ExtraSavesScreen>());
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[2].push_back(std::make_unique<ClickButton>(
        247, 212, 15, 12,
        [this]()
        {
            addOverlay<TitleIdOverlay>();
            titleIdsChanged = true;
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));

    tabButtons[3].push_back(std::make_unique<ClickButton>(
        247, 87, 15, 12,
        [this]()
        {
            inputPatronCode();
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[3].push_back(std::make_unique<ClickButton>(
        247, 111, 15, 12,
        [this]()
        {
            Configuration::getInstance().alphaChannel(!Configuration::getInstance().alphaChannel());
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[4].push_back(std::make_unique<ClickButton>(
        247, 87, 15, 12,
        [this]()
        {
            inputApiUrl();
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
}

void ConfigScreen::drawBottom() const
{
    // Color entire screen the nice medium blue without bars
    Gui::drawSolidRect(0, 0, 320, 240, PKSM_Color(40, 53, 147, 255));

    for (const auto& button : tabs)
    {
        button->draw();
    }

    if (currentTab == 0)
    {
        Gui::text(i18n::language(pksm::Language::JPN), 59, 47, FONT_SIZE_14, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::language(pksm::Language::ENG), 59, 69, FONT_SIZE_14, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::language(pksm::Language::FRE), 59, 91, FONT_SIZE_14, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::language(pksm::Language::GER), 59, 113, FONT_SIZE_14, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::language(pksm::Language::ITA), 59, 135, FONT_SIZE_14, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::language(pksm::Language::SPA), 59, 157, FONT_SIZE_14, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);

        Gui::text(i18n::language(pksm::Language::CHS), 199, 47, FONT_SIZE_14, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::language(pksm::Language::KOR), 199, 69, FONT_SIZE_14, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::language(pksm::Language::NL), 199, 91, FONT_SIZE_14, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::language(pksm::Language::PT), 199, 113, FONT_SIZE_14, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::language(pksm::Language::RO), 199, 135, FONT_SIZE_14, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);

        for (const auto& button : tabButtons[currentTab])
        {
            button->draw();
        }
    }
    else if (currentTab == 1)
    {
        Gui::text(pksm::format(i18n::localize("GENERATION"), (std::string)pksm::Generation::ONE),
            19, 30, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(pksm::format(i18n::localize("GENERATION"), (std::string)pksm::Generation::TWO),
            19, 46, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(pksm::format(i18n::localize("GENERATION"), (std::string)pksm::Generation::THREE),
            19, 62, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(pksm::format(i18n::localize("GENERATION"), (std::string)pksm::Generation::FOUR),
            19, 78, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(pksm::format(i18n::localize("GENERATION"), (std::string)pksm::Generation::FIVE),
            19, 94, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(pksm::format(i18n::localize("GENERATION"), (std::string)pksm::Generation::SIX),
            19, 110, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(pksm::format(i18n::localize("GENERATION"), (std::string)pksm::Generation::SEVEN),
            19, 126, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(pksm::format(i18n::localize("GENERATION"), (std::string)pksm::Generation::LGPE),
            19, 142, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(pksm::format(i18n::localize("GENERATION"), (std::string)pksm::Generation::EIGHT),
            19, 158, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::localize("DAY"), 19, 174, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT,
            TextPosY::TOP);
        Gui::text(i18n::localize("MONTH"), 19, 190, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT,
            TextPosY::TOP);
        Gui::text(i18n::localize("YEAR"), 19, 206, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT,
            TextPosY::TOP);

        // Gui::text(std::to_string(Configuration::getInstance().defaultTID()), 150, 36,
        // FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        // Gui::text(std::to_string(Configuration::getInstance().defaultSID()), 150, 57,
        // FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        // Gui::text(Configuration::getInstance().defaultOT(), 150, 78, FONT_SIZE_12, COLOR_WHITE,
        // TextPosX::LEFT, TextPosY::TOP); std::string data; switch
        // (Configuration::getInstance().nationality())
        // {
        //     case 0:
        //         data = "JPN";
        //         break;
        //     case 1:
        //         data = "USA";
        //         break;
        //     case 2:
        //         data = "EUR";
        //         break;
        //     case 3:
        //         data = "AUS";
        //         break;
        //     case 4:
        //         data = "CHN";
        //         break;
        //     case 5:
        //         data = "KOR";
        //         break;
        //     case 6:
        //         data = "TWN";
        //         break;
        //     default:
        //         data = "USA";
        //         break;
        // }
        // Gui::text(i18n::localize(data), 150, 99, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT,
        // TextPosY::TOP); Gui::text(i18n::country(Configuration::getInstance().language(),
        // Configuration::getInstance().defaultCountry()), 150, 120, FONT_SIZE_12,
        //     COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        // Gui::text(i18n::subregion(Configuration::getInstance().language(),
        // Configuration::getInstance().defaultCountry(),
        //               Configuration::getInstance().defaultRegion()),
        //     150, 141, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(std::to_string(Configuration::getInstance().day()), 168, 174, FONT_SIZE_12,
            COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(std::to_string(Configuration::getInstance().month()), 168, 190, FONT_SIZE_12,
            COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(std::to_string(Configuration::getInstance().year()), 168, 206, FONT_SIZE_12,
            COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);

        for (const auto& button : tabButtons[currentTab])
        {
            button->draw();
        }
    }
    else if (currentTab == 2)
    {
        Gui::text(i18n::localize("CONFIG_BACKUP_SAVE"), 19, 30, FONT_SIZE_12, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 223);
        Gui::text(i18n::localize("CONFIG_EDIT_TRANSFERS"), 19, 50, FONT_SIZE_12, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 223);
        Gui::text(i18n::localize("CONFIG_BACKUP_INJECTION"), 19, 70, FONT_SIZE_12, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 223);
        Gui::text(i18n::localize("CONFIG_SAVE_INFO"), 19, 90, FONT_SIZE_12, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 223);
        Gui::text(i18n::localize("CONFIG_USE_EXTDATA"), 19, 110, FONT_SIZE_12, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 223);
        Gui::text(i18n::localize("CONFIG_RANDOM_MUSIC"), 19, 130, FONT_SIZE_12, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 223);
        Gui::text(i18n::localize("CONFIG_SHOW_BACKUPS"), 19, 150, FONT_SIZE_12, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 223);
        Gui::text(i18n::localize("CONFIG_AUTO_UPDATE"), 19, 170, FONT_SIZE_12, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 223);
        Gui::text(i18n::localize("EXTRA_SAVES"), 19, 190, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT,
            TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 223);
        Gui::text(i18n::localize("TITLE_IDS"), 19, 210, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT,
            TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 223);

        for (const auto& button : tabButtons[currentTab])
        {
            button->draw();
        }

        Gui::text(Configuration::getInstance().autoBackup() ? i18n::localize("YES")
                                                            : i18n::localize("NO"),
            270, 30, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(Configuration::getInstance().transferEdit() ? i18n::localize("YES")
                                                              : i18n::localize("NO"),
            270, 50, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(Configuration::getInstance().writeFileSave() ? i18n::localize("YES")
                                                               : i18n::localize("NO"),
            270, 70, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(Configuration::getInstance().useSaveInfo() ? i18n::localize("YES")
                                                             : i18n::localize("NO"),
            270, 90, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(Configuration::getInstance().useExtData() ? i18n::localize("YES")
                                                            : i18n::localize("NO"),
            270, 110, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(Configuration::getInstance().randomMusic() ? i18n::localize("YES")
                                                             : i18n::localize("NO"),
            270, 130, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(Configuration::getInstance().showBackups() ? i18n::localize("YES")
                                                             : i18n::localize("NO"),
            270, 150, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(Configuration::getInstance().autoUpdate() ? i18n::localize("YES")
                                                            : i18n::localize("NO"),
            270, 170, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    }
    else if (currentTab == 3)
    {
        Gui::text("Patrons", 160, 24, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

        Gui::text(i18n::localize("PATRON_CODE"), 19, 84, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT,
            TextPosY::TOP);
        Gui::text(i18n::localize("ALPHA_UPDATES"), 19, 108, FONT_SIZE_14, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);

        for (const auto& button : tabButtons[currentTab])
        {
            button->draw();
        }

        Gui::text(Configuration::getInstance().alphaChannel() ? i18n::localize("YES")
                                                              : i18n::localize("NO"),
            270, 108, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    }
    else if (currentTab == 4)
    {
        Gui::text(i18n::localize("API_OPTIONS"), 160, 24, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

        Gui::text(i18n::localize("API_URL"), 19, 84, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        // Gui::text("Enabled", 19, 108, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);

        for (const auto& button : tabButtons[currentTab])
        {
            button->draw();
        }

        // Gui::text(
        //     Configuration::getInstance().useApiUrl() ? i18n::localize("YES") : i18n::localize("NO"),
        //     270, 108, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    }
}

void ConfigScreen::update(touchPosition* touch)
{
    u32 kDown = hidKeysDown();
    if (justSwitched)
    {
        if (keysHeld() & KEY_TOUCH)
        {
            return;
        }
        else
        {
            justSwitched = false;
        }
    }
    if (kDown & KEY_B)
    {
        back();
        return;
    }
    else if (kDown & KEY_UP)
    {
        if (debugMenu[0])
        {
            debugMenu.set(1);
        }
        else if (!debugMenu[1])
        {
            debugMenu.set(0);
        }
        else
        {
            debugMenu.reset();
        }
    }
    else if (kDown & KEY_DOWN)
    {
        if (debugMenu[2])
        {
            debugMenu.set(3);
        }
        else if (!debugMenu[3] && debugMenu[1])
        {
            debugMenu.set(2);
        }
        else
        {
            debugMenu.reset();
        }
    }
    else if (kDown & KEY_LEFT)
    {
        if (debugMenu[5])
        {
            debugMenu.set(6);
        }
        else if (!debugMenu[5] && debugMenu[3])
        {
            debugMenu.set(4);
        }
        else
        {
            debugMenu.reset();
        }
    }
    else if (kDown & KEY_RIGHT)
    {
        if (debugMenu[6])
        {
            debugMenu.set(7);
        }
        else if (!debugMenu[6] && debugMenu[4])
        {
            debugMenu.set(5);
        }
        else
        {
            debugMenu.reset();
        }
    }
    else if (kDown & KEY_A)
    {
        if (debugMenu[7])
        {
            debugMenu.set(8);
        }
        else
        {
            debugMenu.reset();
        }
    }

    if (debugMenu[8])
    {
        debugMenu.reset();
        currentTab = 4;
        return;
    }

    for (auto& button : tabs)
    {
        button->update(touch);
    }

    for (auto& button : tabButtons[currentTab])
    {
        button->update(touch);
    }

    if (hidKeysDown() & KEY_TOUCH)
    {
        if (touch->px > 0 && touch->px < 30 && touch->py > 0 && touch->py < 30)
        {
            patronMenu[0]        = true;
            countPatronMenuTimer = true;
            patronMenuTimer      = 600;
        }
        else if (touch->px > 290 && touch->px < 320 && touch->py < 30 && touch->py > 0)
        {
            patronMenu[1]        = true;
            countPatronMenuTimer = true;
            patronMenuTimer      = 600;
        }
        else if (touch->px > 0 && touch->px < 30 && touch->py > 210 && touch->py < 240)
        {
            patronMenu[2]        = true;
            countPatronMenuTimer = true;
            patronMenuTimer      = 600;
        }
        else if (touch->px > 290 && touch->px < 320 && touch->py > 210 && touch->py < 240)
        {
            patronMenu[3]        = true;
            countPatronMenuTimer = true;
            patronMenuTimer      = 600;
        }
        if (patronMenu[0] && patronMenu[1] && patronMenu[2] && patronMenu[3])
        {
            currentTab      = 3;
            patronMenuTimer = 0;
        }
    }

    if (countPatronMenuTimer)
    {
        if (patronMenuTimer > 0)
        {
            patronMenuTimer--;
        }
        else
        {
            countPatronMenuTimer = false;
            for (int i = 0; i < 8; i++)
            {
                patronMenu[i] = false;
            }
        }
    }
}

void ConfigScreen::back()
{
    Configuration::getInstance().save();
    if (useExtDataChanged)
    {
        Banks::swapSD(!Configuration::getInstance().useExtData());
    }
    if (showBackupsChanged || titleIdsChanged)
    {
        TitleLoader::scanSaves();
    }
    PkmUtils::saveDefaults();
    Gui::screenBack();
}

void ConfigScreen::drawTop() const
{
    Gui::backgroundTop(false);
    const std::string pksm = std::format(
        "PKSM v{:d}.{:d}.{:d}-{:s}", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO, GIT_REV);
    Gui::text(pksm, 200, 12.5f, FONT_SIZE_12, COLOR_BLUE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::text(std::string(credits[0]), 200, 30, FONT_SIZE_14,
        PKSM_Color(0xFF, 0xFF, 0xFF, getNextAlpha(0)), TextPosX::CENTER, TextPosY::TOP);
    int y = 35;
    for (size_t i = 1; i < credits.size(); i++)
    {
        Gui::text(std::string(credits[i]), 200, y += 15, FONT_SIZE_14,
            PKSM_Color(0xFF, 0xFF, 0xFF, getNextAlpha(i)), TextPosX::CENTER, TextPosY::TOP);
    }
}
