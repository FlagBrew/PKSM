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
#include "gui.hpp"
#include "AccelButton.hpp"
#include "ClickButton.hpp"
#include "banks.hpp"

#define STORAGE_BOX_LIMIT 2000

#define LIMITSTORAGE(number) number > STORAGE_BOX_LIMIT ? STORAGE_BOX_LIMIT : number < 0 ? 0 : number

static constexpr std::array<std::string_view, 11> credits = {
    "piepie62 and Admiral-Fish for their dedication",
    "dsoldier for the gorgeous graphic work",
    "SpiredMoth, trainboy2019 and all the scripters",
    "The whole FlagBrew team for collaborating with us",
    "Kaphotics and SciresM for PKHeX documentation",
    "fincs and WinterMute for citro2d and devkitARM",
    "kamronbatman and ProjectPokemon for EventsGallery",
    "All of the translators",
    "Subject21_J and all the submitters for PKSM's icon",
    "Mewmore for the default background music",
    "Bernardo for creating PKSM"
};

static void inputNumber(std::function<void(int)> callback, int digits, int maxValue)
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NUMPAD, 2, digits);
	swkbdSetFeatures(&state, SWKBD_FIXED_WIDTH);
    swkbdSetValidation(&state, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
    char input[digits + 1] = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[digits] = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        int tid = std::stoi(input);
        callback(std::min(maxValue, tid));
    }
}

static void inputOT()
{
    static SwkbdState state;
    static bool first = true;
    if (first)
    {
        swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, 12);
        first = false;
    }
    swkbdSetHintText(&state, i18n::localize("OT_NAME").c_str());
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    char input[25] = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[24] = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        Configuration::getInstance().defaultOT(input);
    }
}

static bool countryChoice()
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

ConfigScreen::ConfigScreen() : oldStorage(Configuration::getInstance().storageSize())
{
    tabs[0] = new Button(1, 2, 104, 17, [&](){ currentTab = 0; return false; }, ui_sheet_res_null_idx, "", 0.0f, 0);
    tabs[1] = new Button(108, 2, 104, 17, [&](){ currentTab = 1; return false; }, ui_sheet_res_null_idx, "", 0.0f, 0);
    tabs[2] = new Button(215, 2, 104, 17, [&](){ currentTab = 2; return false; }, ui_sheet_res_null_idx, "", 0.0f, 0);

    // First column of language buttons
    tabButtons[0].push_back(new ClickButton(37, 52, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::JP); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));
    tabButtons[0].push_back(new ClickButton(37, 74, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::EN); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));
    tabButtons[0].push_back(new ClickButton(37, 96, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::FR); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));
    tabButtons[0].push_back(new ClickButton(37, 118, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::DE); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));
    tabButtons[0].push_back(new ClickButton(37, 140, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::IT); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));
    tabButtons[0].push_back(new ClickButton(37, 162, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::ES); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));

    // Second column of language buttons
    tabButtons[0].push_back(new ClickButton(177, 52, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::ZH); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));
    tabButtons[0].push_back(new ClickButton(177, 74, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::KO); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));
    tabButtons[0].push_back(new ClickButton(177, 96, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::NL); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));
    tabButtons[0].push_back(new ClickButton(177, 118, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::PT); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));
    tabButtons[0].push_back(new ClickButton(177, 140, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::RU); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));

    // Defaults buttons
    tabButtons[1].push_back(new Button(122, 38, 15, 12, [](){ Gui::setNextKeyboardFunc([](){ inputNumber([](u16 a){ Configuration::getInstance().defaultTID(a); }, 5, 0xFFFF); }); return false; }, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
    tabButtons[1].push_back(new Button(122, 62, 15, 12, [](){ Gui::setNextKeyboardFunc([](){ inputNumber([](u16 a){ Configuration::getInstance().defaultSID(a); }, 5, 0xFFFF); }); return false; }, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
    tabButtons[1].push_back(new Button(122, 86, 15, 12, [](){ Gui::setNextKeyboardFunc(&inputOT); return false; }, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
    tabButtons[1].push_back(new ClickButton(122, 110, 15, 12, &countryChoice, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
    tabButtons[1].push_back(new Button(122, 134, 15, 12, [](){ Gui::setNextKeyboardFunc([](){ inputNumber([](u16 a){ Configuration::getInstance().day(a); }, 2, 31); }); return false; }, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
    tabButtons[1].push_back(new Button(122, 158, 15, 12, [](){ Gui::setNextKeyboardFunc([](){ inputNumber([](u16 a){ Configuration::getInstance().month(a); }, 2, 12); }); return false; }, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
    tabButtons[1].push_back(new Button(122, 182, 15, 12, [](){ Gui::setNextKeyboardFunc([](){ inputNumber([](u16 a){ Configuration::getInstance().year(a); }, 4, 9999); }); return false; }, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));

    // Miscellaneous buttons
    tabButtons[2].push_back(new ClickButton(247, 39, 15, 12, [](){ Configuration::getInstance().autoBackup(!Configuration::getInstance().autoBackup()); return true; }, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
    tabButtons[2].push_back(new AccelButton(231, 62, 13, 13, [this](){ Configuration::getInstance().storageSize(LIMITSTORAGE(Configuration::getInstance().storageSize() - 1)); storageSizeChanged = true; return false; }, ui_sheet_button_minus_small_idx, "", 0.0f, 0));
    tabButtons[2].push_back(new Button(245, 62, 50, 13, [this](){ Gui::setNextKeyboardFunc([](){ inputNumber([](u16 a){ Configuration::getInstance().storageSize(a); }, 4, STORAGE_BOX_LIMIT); }); storageSizeChanged = true; return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));
    tabButtons[2].push_back(new AccelButton(296, 62, 13, 13, [this](){ Configuration::getInstance().storageSize(LIMITSTORAGE(Configuration::getInstance().storageSize() + 1)); storageSizeChanged = true; return false; }, ui_sheet_button_plus_small_idx, "", 0.0f, 0));
    tabButtons[2].push_back(new ClickButton(247, 87, 15, 12, [](){ Configuration::getInstance().transferEdit(!Configuration::getInstance().transferEdit()); return true; }, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
    tabButtons[2].push_back(new ClickButton(247, 111, 15, 12, [](){ Configuration::getInstance().writeFileSave(!Configuration::getInstance().writeFileSave()); return true; }, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
    tabButtons[2].push_back(new ClickButton(247, 135, 15, 12, [](){ Configuration::getInstance().useSaveInfo(!Configuration::getInstance().useSaveInfo()); return true; }, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
    tabButtons[2].push_back(new ClickButton(247, 159, 15, 12, [this](){ Configuration::getInstance().useExtData(!Configuration::getInstance().useExtData()); useExtDataChanged = !useExtDataChanged; return true; }, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
    tabButtons[2].push_back(new ClickButton(247, 183, 15, 12, [](){ Configuration::getInstance().randomMusic(!Configuration::getInstance().randomMusic()); return true; }, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
}

void ConfigScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    drawTop();

    C2D_SceneBegin(g_renderTargetBottom);
    Gui::backgroundBottom(false);

    if (currentTab != 0)
    {
        C2D_DrawRectSolid(1, 2, 0.5f, 104, 17, COLOR_DARKBLUE);
        Gui::staticText(i18n::localize("LANGUAGE"), 53, 2, FONT_SIZE_11, FONT_SIZE_11, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    }
    if (currentTab != 1)
    {
        C2D_DrawRectSolid(108, 2, 0.5f, 104, 17, COLOR_DARKBLUE);
        Gui::staticText(i18n::localize("DEFAULTS"), 160, 2, FONT_SIZE_11, FONT_SIZE_11, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    }
    if (currentTab != 2)
    {
        C2D_DrawRectSolid(215, 2, 0.5f, 104, 17, COLOR_DARKBLUE);
        Gui::staticText(i18n::localize("MISC"), 267, 2, FONT_SIZE_11, FONT_SIZE_11, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    }

    if (currentTab == 0)
    {
        Gui::staticText(i18n::localize("LANGUAGE"), 53, 2, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

        Gui::staticText("日本語", 59, 47, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText("English", 59, 69, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText("Français", 59, 91, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText("Deutsche", 59, 113, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText("Italiano", 59, 135, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText("Español", 59, 157, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);

        Gui::staticText("中文", 199, 47, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText("한국어", 199, 69, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText("Nederlands", 199, 91, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText("Português", 199, 113, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText("русский", 199, 135, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);

        C2D_DrawRectSolid(37, 52, 0.5f, 8, 8, COLOR_MENUBLUE);
        C2D_DrawRectSolid(37, 74, 0.5f, 8, 8, COLOR_MENUBLUE);
        C2D_DrawRectSolid(37, 96, 0.5f, 8, 8, COLOR_MENUBLUE);
        C2D_DrawRectSolid(37, 118, 0.5f, 8, 8, COLOR_MENUBLUE);
        C2D_DrawRectSolid(37, 140, 0.5f, 8, 8, COLOR_MENUBLUE);
        C2D_DrawRectSolid(37, 162, 0.5f, 8, 8, COLOR_MENUBLUE);

        C2D_DrawRectSolid(177, 52, 0.5f, 8, 8, COLOR_MENUBLUE);
        C2D_DrawRectSolid(177, 74, 0.5f, 8, 8, COLOR_MENUBLUE);
        C2D_DrawRectSolid(177, 96, 0.5f, 8, 8, COLOR_MENUBLUE);
        C2D_DrawRectSolid(177, 118, 0.5f, 8, 8, COLOR_MENUBLUE);
        C2D_DrawRectSolid(177, 140, 0.5f, 8, 8, COLOR_MENUBLUE);

        switch (Configuration::getInstance().language())
        {
            case Language::JP:
                C2D_DrawRectSolid(36, 51, 0.5f, 10, 10, COLOR_HIGHBLUE);
                break;
            case Language::EN:
            case Language::UNUSED:
                C2D_DrawRectSolid(36, 73, 0.5f, 10, 10, COLOR_HIGHBLUE);
                break;
            case Language::FR:
                C2D_DrawRectSolid(36, 95, 0.5f, 10, 10, COLOR_HIGHBLUE);
                break;
            case Language::DE:
                C2D_DrawRectSolid(36, 117, 0.5f, 10, 10, COLOR_HIGHBLUE);
                break;
            case Language::IT:
                C2D_DrawRectSolid(36, 139, 0.5f, 10, 10, COLOR_HIGHBLUE);
                break;
            case Language::ES:
                C2D_DrawRectSolid(36, 161, 0.5f, 10, 10, COLOR_HIGHBLUE);
                break;
            case Language::ZH:
            case Language::TW:
                C2D_DrawRectSolid(176, 51, 0.5f, 10, 10, COLOR_HIGHBLUE);
                break;
            case Language::KO:
                C2D_DrawRectSolid(176, 73, 0.5f, 10, 10, COLOR_HIGHBLUE);
                break;
            case Language::NL:
                C2D_DrawRectSolid(176, 95, 0.5f, 10, 10, COLOR_HIGHBLUE);
                break;
            case Language::PT:
                C2D_DrawRectSolid(176, 117, 0.5f, 10, 10, COLOR_HIGHBLUE);
                break;
            case Language::RU:
                C2D_DrawRectSolid(176, 139, 0.5f, 10, 10, COLOR_HIGHBLUE);
                break;
        }
    }
    else if (currentTab == 1)
    {
        Gui::staticText(i18n::localize("DEFAULTS"), 160, 2, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

        Gui::staticText(i18n::localize("TID"), 19, 36, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(i18n::localize("SID"), 19, 60, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(i18n::localize("OT"), 19, 84, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(i18n::localize("NATIONALITY"), 19, 108, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(i18n::localize("DAY"), 19, 132, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(i18n::localize("MONTH"), 19, 156, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(i18n::localize("YEAR"), 19, 180, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);

        Gui::dynamicText(std::to_string(Configuration::getInstance().defaultTID()), 150, 36, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::dynamicText(std::to_string(Configuration::getInstance().defaultSID()), 150, 60, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::dynamicText(Configuration::getInstance().defaultOT(), 150, 84, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
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
        Gui::dynamicText(i18n::localize(data), 150, 108, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::dynamicText(std::to_string(Configuration::getInstance().day()), 150, 132, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::dynamicText(std::to_string(Configuration::getInstance().month()), 150, 156, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::dynamicText(std::to_string(Configuration::getInstance().year()), 150, 180, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);

        for (Button* button : tabButtons[currentTab])
        {
            button->draw();
        }
    }
    else if (currentTab == 2)
    {
        Gui::staticText(i18n::localize("MISC"), 215 + 104 / 2, 2, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

        Gui::staticText(i18n::localize("CONFIG_BACKUP_SAVE"), 19, 36, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(i18n::localize("CONFIG_STORAGE_SIZE"), 19, 60, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(i18n::localize("CONFIG_EDIT_TRANSFERS"), 19, 84, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(i18n::localize("CONFIG_BACKUP_INJECTION"), 19, 108, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(i18n::localize("CONFIG_SAVE_INFO"), 19, 132, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(i18n::localize("CONFIG_USE_EXTDATA"), 19, 156, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(i18n::localize("CONFIG_RANDOM_MUSIC"), 19, 180, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);

        for (Button* button : tabButtons[currentTab])
        {
            button->draw();
        }

        Gui::staticText(Configuration::getInstance().autoBackup() ? i18n::localize("YES") : i18n::localize("NO"), 270, 36, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::dynamicText(std::to_string(Configuration::getInstance().storageSize()), 245 + 50 / 2, 60, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
        Gui::staticText(Configuration::getInstance().transferEdit() ? i18n::localize("YES") : i18n::localize("NO"), 270, 84, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(Configuration::getInstance().writeFileSave() ? i18n::localize("YES") : i18n::localize("NO"), 270, 108, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(Configuration::getInstance().useSaveInfo() ? i18n::localize("YES") : i18n::localize("NO"), 270, 132, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(Configuration::getInstance().useExtData() ? i18n::localize("YES") : i18n::localize("NO"), 270, 156, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(Configuration::getInstance().randomMusic() ? i18n::localize("YES") : i18n::localize("NO"), 270, 180, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
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
    Screen::update();
    if (hidKeysDown() & KEY_B)
    {
        back();
        return;
    }

    for (Button* button : tabs)
    {
        button->update(touch);
    }

    for (Button* button : tabButtons[currentTab])
    {
        button->update(touch);
    }
}

void ConfigScreen::back()
{
    Configuration::getInstance().save();
    if (storageSizeChanged)
    {
        Banks::bank->resize(Configuration::getInstance().storageSize());
    }
    else if (useExtDataChanged)
    {
        Banks::bank->save();
    }
    Gui::screenBack();
}

static u8 getNextAlpha(int off)
{
    static u8 retVals[11] = { 205, 210, 215, 220, 225, 230, 235, 240, 245, 250, 255 };
    static bool up[11] = { false, false, false, false, false, false, false, false, false, false, false };
    static u8 timers[11] = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };
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
    Gui::staticText("PKSM", 200, 4, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLUE, TextPosX::CENTER, TextPosY::TOP);
    int y = 25;
    for (size_t i = 0; i < credits.size(); i++)
    {
        Gui::dynamicText(std::string(credits[i]), 200, y += 16, FONT_SIZE_15, FONT_SIZE_15, C2D_Color32(0xFF, 0xFF, 0xFF, getNextAlpha(i)), TextPosX::CENTER, TextPosY::TOP);
    }
}
