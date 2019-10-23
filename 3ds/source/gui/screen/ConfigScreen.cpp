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

#include "ConfigScreen.hpp"
#include "AccelButton.hpp"
#include "ClickButton.hpp"
#include "ConfigCountryOverlay.hpp"
#include "ConfigSubRegionOverlay.hpp"
#include "Configuration.hpp"
#include "ExtraSavesScreen.hpp"
#include "QRScanner.hpp"
#include "ToggleButton.hpp"
#include "banks.hpp"
#include "gui.hpp"
#include "i18n.hpp"
#include "loader.hpp"

static constexpr std::array<std::string_view, 12> credits = {"piepie62 and Admiral-Fish for their dedication",
    "dsoldier for the gorgeous graphic work", "SpiredMoth, trainboy2019 and all the scripters", "The whole FlagBrew team for collaborating with us",
    "Kaphotics and SciresM for PKHeX documentation", "fincs and WinterMute for citro2d and devkitARM",
    "kamronbatman and ProjectPokemon for EventsGallery", "All of the translators", "Subject21_J and all the submitters for PKSM's icon",
    "Allen (FMCore/FM1337) for the GPSS backend", "Bernardo for creating PKSM"};

static void inputNumber(std::function<void(int)> callback, int digits, int maxValue)
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

static void inputOT()
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, 12);
    swkbdSetHintText(&state, i18n::localize("OT_NAME").c_str());
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    char input[25]  = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[24]       = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        Configuration::getInstance().defaultOT(input);
    }
}

static void inputPatronCode()
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_QWERTY, 3, 22);
    swkbdSetHintText(&state, i18n::localize("PATRON_CODE").c_str());
    std::string patronCode = Configuration::getInstance().patronCode();
    swkbdSetInitialText(&state, patronCode.c_str());
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    swkbdSetButton(&state, SwkbdButton::SWKBD_BUTTON_MIDDLE, i18n::localize("QR_SCANNER").c_str(), false);
    char input[45]  = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[44]       = '\0';
    if (ret == SWKBD_BUTTON_MIDDLE)
    {
        std::vector<u8> data = QRScanner::scan(QRMode::TEXT);
        if (!data.empty() && data.size() == 22)
        {
            std::copy(data.begin(), data.end(), input);
            input[21] = '\0';
            ret       = SWKBD_BUTTON_CONFIRM;
        }
    }
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        Configuration::getInstance().patronCode(input);
    }
}

static bool nationalityChoice()
{
    switch (Configuration::getInstance().nationality())
    {
        case 0:
            Configuration::getInstance().nationality(1);
            break;
        case 1:
            Configuration::getInstance().nationality(2);
            break;
        case 2:
            Configuration::getInstance().nationality(3);
            break;
        case 3:
            Configuration::getInstance().nationality(4);
            break;
        case 4:
            Configuration::getInstance().nationality(5);
            break;
        case 5:
            Configuration::getInstance().nationality(6);
            break;
        case 6:
            Configuration::getInstance().nationality(0);
            break;
        default:
            Configuration::getInstance().nationality(0);
            break;
    }
    return false;
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

    tabs.push_back(std::make_unique<ToggleButton>(1, 2, 104, 17,
        [&]() {
            currentTab = 0;
            return false;
        },
        ui_sheet_res_null_idx, i18n::localize("LANGUAGE"), FONT_SIZE_11, COLOR_WHITE, ui_sheet_emulated_button_tab_unselected_idx,
        i18n::localize("LANGUAGE"), FONT_SIZE_11, COLOR_BLACK, &tabs, false));
    tabs.push_back(std::make_unique<ToggleButton>(108, 2, 104, 17,
        [&]() {
            currentTab = 1;
            return false;
        },
        ui_sheet_res_null_idx, i18n::localize("DEFAULTS"), FONT_SIZE_11, COLOR_WHITE, ui_sheet_emulated_button_tab_unselected_idx,
        i18n::localize("DEFAULTS"), FONT_SIZE_11, COLOR_BLACK, &tabs, false));
    tabs.push_back(std::make_unique<ToggleButton>(215, 2, 104, 17,
        [&]() {
            currentTab = 2;
            return false;
        },
        ui_sheet_res_null_idx, i18n::localize("MISC"), FONT_SIZE_11, COLOR_WHITE, ui_sheet_emulated_button_tab_unselected_idx, i18n::localize("MISC"),
        FONT_SIZE_11, COLOR_BLACK, &tabs, false));
    tabs[0]->setState(true);

    // First column of language buttons
    tabButtons[0].push_back(std::make_unique<ToggleButton>(37, 52, 8, 8,
        [this]() {
            Gui::clearText();
            Configuration::getInstance().language(Language::JP);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK, ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));
    tabButtons[0].push_back(std::make_unique<ToggleButton>(37, 74, 8, 8,
        [this]() {
            Gui::clearText();
            Configuration::getInstance().language(Language::EN);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK, ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));
    tabButtons[0].push_back(std::make_unique<ToggleButton>(37, 96, 8, 8,
        [this]() {
            Gui::clearText();
            Configuration::getInstance().language(Language::FR);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK, ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));
    tabButtons[0].push_back(std::make_unique<ToggleButton>(37, 118, 8, 8,
        [this]() {
            Gui::clearText();
            Configuration::getInstance().language(Language::DE);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK, ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));
    tabButtons[0].push_back(std::make_unique<ToggleButton>(37, 140, 8, 8,
        [this]() {
            Gui::clearText();
            Configuration::getInstance().language(Language::IT);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK, ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));
    tabButtons[0].push_back(std::make_unique<ToggleButton>(37, 162, 8, 8,
        [this]() {
            Gui::clearText();
            Configuration::getInstance().language(Language::ES);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK, ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));

    // Second column of language buttons
    tabButtons[0].push_back(std::make_unique<ToggleButton>(177, 52, 8, 8,
        [this]() {
            Gui::clearText();
            Configuration::getInstance().language(Language::ZH);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK, ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));
    tabButtons[0].push_back(std::make_unique<ToggleButton>(177, 74, 8, 8,
        [this]() {
            Gui::clearText();
            Configuration::getInstance().language(Language::KO);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK, ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));
    tabButtons[0].push_back(std::make_unique<ToggleButton>(177, 96, 8, 8,
        [this]() {
            Gui::clearText();
            Configuration::getInstance().language(Language::NL);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK, ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));
    tabButtons[0].push_back(std::make_unique<ToggleButton>(177, 118, 8, 8,
        [this]() {
            Gui::clearText();
            Configuration::getInstance().language(Language::PT);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK, ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));
    tabButtons[0].push_back(std::make_unique<ToggleButton>(177, 140, 8, 8,
        [this]() {
            Gui::clearText();
            Configuration::getInstance().language(Language::RU);
            initButtons();
            return false;
        },
        ui_sheet_emulated_button_lang_enabled_idx, "", 0.0f, COLOR_BLACK, ui_sheet_emulated_button_lang_disabled_idx, "", 0.0f, COLOR_BLACK,
        reinterpret_cast<std::vector<std::unique_ptr<ToggleButton>>*>(&tabButtons[0]), false));

    switch (Configuration::getInstance().language())
    {
        case Language::JP:
            ((ToggleButton*)tabButtons[0][0].get())->setState(true);
            break;
        case Language::EN:
            ((ToggleButton*)tabButtons[0][1].get())->setState(true);
            break;
        case Language::FR:
            ((ToggleButton*)tabButtons[0][2].get())->setState(true);
            break;
        case Language::DE:
            ((ToggleButton*)tabButtons[0][3].get())->setState(true);
            break;
        case Language::IT:
            ((ToggleButton*)tabButtons[0][4].get())->setState(true);
            break;
        case Language::ES:
            ((ToggleButton*)tabButtons[0][5].get())->setState(true);
            break;
        case Language::ZH:
            ((ToggleButton*)tabButtons[0][6].get())->setState(true);
            break;
        case Language::KO:
            ((ToggleButton*)tabButtons[0][7].get())->setState(true);
            break;
        case Language::NL:
            ((ToggleButton*)tabButtons[0][8].get())->setState(true);
            break;
        case Language::PT:
            ((ToggleButton*)tabButtons[0][9].get())->setState(true);
            break;
        case Language::RU:
            ((ToggleButton*)tabButtons[0][10].get())->setState(true);
            break;
        default:
            // Default to English
            ((ToggleButton*)tabButtons[0][1].get())->setState(true);
            break;
    }

    // Defaults buttons
    tabButtons[1].push_back(std::make_unique<Button>(122, 38, 15, 12,
        []() {
            inputNumber([](u16 a) { Configuration::getInstance().defaultTID(a); }, 5, 0xFFFF);
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[1].push_back(std::make_unique<Button>(122, 59, 15, 12,
        []() {
            inputNumber([](u16 a) { Configuration::getInstance().defaultSID(a); }, 5, 0xFFFF);
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[1].push_back(std::make_unique<Button>(122, 80, 15, 12,
        []() {
            inputOT();
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[1].push_back(
        std::make_unique<ClickButton>(122, 101, 15, 12, &nationalityChoice, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[1].push_back(std::make_unique<ClickButton>(122, 122, 15, 12,
        [this]() {
            addOverlay<ConfigCountryOverlay>();
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[1].push_back(std::make_unique<ClickButton>(122, 143, 15, 12,
        [this]() {
            addOverlay<ConfigSubRegionOverlay>();
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[1].push_back(std::make_unique<Button>(122, 164, 15, 12,
        []() {
            inputNumber([](u16 a) { Configuration::getInstance().day(a); }, 2, 31);
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[1].push_back(std::make_unique<Button>(122, 185, 15, 12,
        []() {
            inputNumber([](u16 a) { Configuration::getInstance().month(a); }, 2, 12);
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[1].push_back(std::make_unique<Button>(122, 206, 15, 12,
        []() {
            inputNumber([](u16 a) { Configuration::getInstance().year(a); }, 4, 9999);
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));

    // Miscellaneous buttons
    tabButtons[2].push_back(std::make_unique<ClickButton>(247, 39, 15, 12,
        []() {
            Configuration::getInstance().autoBackup(!Configuration::getInstance().autoBackup());
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[2].push_back(std::make_unique<ClickButton>(247, 60, 15, 12,
        []() {
            Configuration::getInstance().transferEdit(!Configuration::getInstance().transferEdit());
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[2].push_back(std::make_unique<ClickButton>(247, 81, 15, 12,
        []() {
            Configuration::getInstance().writeFileSave(!Configuration::getInstance().writeFileSave());
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[2].push_back(std::make_unique<ClickButton>(247, 102, 15, 12,
        []() {
            Configuration::getInstance().useSaveInfo(!Configuration::getInstance().useSaveInfo());
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[2].push_back(std::make_unique<ClickButton>(247, 123, 15, 12,
        [this]() {
            Configuration::getInstance().useExtData(!Configuration::getInstance().useExtData());
            useExtDataChanged = !useExtDataChanged;
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[2].push_back(std::make_unique<ClickButton>(247, 144, 15, 12,
        []() {
            Configuration::getInstance().randomMusic(!Configuration::getInstance().randomMusic());
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[2].push_back(std::make_unique<ClickButton>(247, 165, 15, 12,
        [this]() {
            Configuration::getInstance().showBackups(!Configuration::getInstance().showBackups());
            showBackupsChanged = !showBackupsChanged;
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[2].push_back(std::make_unique<ClickButton>(247, 186, 15, 12,
        [this]() {
            Configuration::getInstance().autoUpdate(!Configuration::getInstance().autoUpdate());
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[2].push_back(std::make_unique<ClickButton>(247, 207, 15, 12,
        [this]() {
            Gui::setScreen(std::make_unique<ExtraSavesScreen>());
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[3].push_back(std::make_unique<ClickButton>(247, 87, 15, 12,
        [this]() {
            inputPatronCode();
            return false;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    tabButtons[3].push_back(std::make_unique<ClickButton>(247, 111, 15, 12,
        [this]() {
            Configuration::getInstance().alphaChannel(!Configuration::getInstance().alphaChannel());
            return true;
        },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
}

void ConfigScreen::drawBottom() const
{
    Gui::backgroundBottom(false);

    for (auto& button : tabs)
    {
        button->draw();
    }

    if (currentTab == 0)
    {
        Gui::text("日本語", 59, 47, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text("English", 59, 69, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text("Français", 59, 91, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text("Deutsche", 59, 113, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text("Italiano", 59, 135, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text("Español", 59, 157, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);

        Gui::text("中文", 199, 47, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text("한국어", 199, 69, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text("Nederlands", 199, 91, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text("Português", 199, 113, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text("русский", 199, 135, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);

        for (auto& button : tabButtons[currentTab])
        {
            button->draw();
        }
    }
    else if (currentTab == 1)
    {
        Gui::text(i18n::localize("TID"), 19, 36, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::localize("SID"), 19, 57, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::localize("OT"), 19, 78, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::localize("3DS_REGION"), 19, 99, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::localize("COUNTRY"), 19, 120, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::localize("SUBREGION"), 19, 141, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::localize("DAY"), 19, 162, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::localize("MONTH"), 19, 183, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::localize("YEAR"), 19, 204, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);

        Gui::text(std::to_string(Configuration::getInstance().defaultTID()), 150, 36, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(std::to_string(Configuration::getInstance().defaultSID()), 150, 57, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(Configuration::getInstance().defaultOT(), 150, 78, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        std::string data;
        switch (Configuration::getInstance().nationality())
        {
            case 0:
                data = "JPN";
                break;
            case 1:
                data = "USA";
                break;
            case 2:
                data = "EUR";
                break;
            case 3:
                data = "AUS";
                break;
            case 4:
                data = "CHN";
                break;
            case 5:
                data = "KOR";
                break;
            case 6:
                data = "TWN";
                break;
            default:
                data = "USA";
                break;
        }
        Gui::text(i18n::localize(data), 150, 99, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::country(Configuration::getInstance().language(), Configuration::getInstance().defaultCountry()), 150, 120, FONT_SIZE_12,
            COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::subregion(Configuration::getInstance().language(), Configuration::getInstance().defaultCountry(),
                      Configuration::getInstance().defaultRegion()),
            150, 141, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(std::to_string(Configuration::getInstance().day()), 150, 162, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(std::to_string(Configuration::getInstance().month()), 150, 183, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(std::to_string(Configuration::getInstance().year()), 150, 204, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);

        for (auto& button : tabButtons[currentTab])
        {
            button->draw();
        }
    }
    else if (currentTab == 2)
    {
        Gui::text(i18n::localize("CONFIG_BACKUP_SAVE"), 19, 36, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP,
            TextWidthAction::SQUISH_OR_SCROLL, 223);
        Gui::text(i18n::localize("CONFIG_EDIT_TRANSFERS"), 19, 57, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP,
            TextWidthAction::SQUISH_OR_SCROLL, 223);
        Gui::text(i18n::localize("CONFIG_BACKUP_INJECTION"), 19, 78, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP,
            TextWidthAction::SQUISH_OR_SCROLL, 223);
        Gui::text(i18n::localize("CONFIG_SAVE_INFO"), 19, 99, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP,
            TextWidthAction::SQUISH_OR_SCROLL, 223);
        Gui::text(i18n::localize("CONFIG_USE_EXTDATA"), 19, 120, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP,
            TextWidthAction::SQUISH_OR_SCROLL, 223);
        Gui::text(i18n::localize("CONFIG_RANDOM_MUSIC"), 19, 141, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP,
            TextWidthAction::SQUISH_OR_SCROLL, 223);
        Gui::text(i18n::localize("CONFIG_SHOW_BACKUPS"), 19, 162, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP,
            TextWidthAction::SQUISH_OR_SCROLL, 223);
        Gui::text(i18n::localize("CONFIG_AUTO_UPDATE"), 19, 183, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP,
            TextWidthAction::SQUISH_OR_SCROLL, 223);
        Gui::text(
            i18n::localize("EXTRA_SAVES"), 19, 204, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 223);

        for (auto& button : tabButtons[currentTab])
        {
            button->draw();
        }

        Gui::text(Configuration::getInstance().autoBackup() ? i18n::localize("YES") : i18n::localize("NO"), 270, 36, FONT_SIZE_12, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);
        Gui::text(Configuration::getInstance().transferEdit() ? i18n::localize("YES") : i18n::localize("NO"), 270, 57, FONT_SIZE_12, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);
        Gui::text(Configuration::getInstance().writeFileSave() ? i18n::localize("YES") : i18n::localize("NO"), 270, 78, FONT_SIZE_12, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);
        Gui::text(Configuration::getInstance().useSaveInfo() ? i18n::localize("YES") : i18n::localize("NO"), 270, 99, FONT_SIZE_12, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);
        Gui::text(Configuration::getInstance().useExtData() ? i18n::localize("YES") : i18n::localize("NO"), 270, 120, FONT_SIZE_12, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);
        Gui::text(Configuration::getInstance().randomMusic() ? i18n::localize("YES") : i18n::localize("NO"), 270, 141, FONT_SIZE_12, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);
        Gui::text(Configuration::getInstance().showBackups() ? i18n::localize("YES") : i18n::localize("NO"), 270, 162, FONT_SIZE_12, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);
        Gui::text(Configuration::getInstance().autoUpdate() ? i18n::localize("YES") : i18n::localize("NO"), 270, 183, FONT_SIZE_12, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);
    }
    else if (currentTab == 3)
    {
        Gui::text("Patrons", 160, 24, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

        Gui::text(i18n::localize("PATRON_CODE"), 19, 84, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::localize("ALPHA_UPDATES"), 19, 108, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);

        for (auto& button : tabButtons[currentTab])
        {
            button->draw();
        }

        Gui::text(Configuration::getInstance().alphaChannel() ? i18n::localize("YES") : i18n::localize("NO"), 270, 108, FONT_SIZE_14, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);
    }
}

void ConfigScreen::update(touchPosition* touch)
{
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
    if (hidKeysDown() & KEY_B)
    {
        back();
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
    if (showBackupsChanged)
    {
        TitleLoader::scanSaves();
    }
    Gui::screenBack();
}

static u8 getNextAlpha(int off)
{
    static u8 retVals[12] = {200, 205, 210, 215, 220, 225, 230, 235, 240, 245, 250, 255};
    static bool up[12]    = {false, false, false, false, false, false, false, false, false, false, false};
    static u8 timers[12]  = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
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

void ConfigScreen::drawTop() const
{
    Gui::backgroundTop(false);
    Gui::text("PKSM", 200, 12.5f, FONT_SIZE_12, COLOR_BLUE, TextPosX::CENTER, TextPosY::CENTER);
    int y = 20;
    for (size_t i = 0; i < credits.size(); i++)
    {
        Gui::text(
            std::string(credits[i]), 200, y += 15, FONT_SIZE_14, PKSM_Color(0xFF, 0xFF, 0xFF, getNextAlpha(i)), TextPosX::CENTER, TextPosY::TOP);
    }
}
