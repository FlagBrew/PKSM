/*
*   This file is part of PKSM
*   Copyright (C) 2016-2018 Bernardo Giordano, Admiral Fish, piepie62
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
#include <bitset>

#define TIMER(thingToDo) static int timer = 3; \
                         if (timer <= 0) \
                         { \
                            thingToDo; \
                            timer = 3; \
                         } \
                         else \
                            --timer

static bool inputNumber() { return false; }
static bool inputText() { return false; }
static bool inputDate() { return false; }
static bool countryChoice() { return false; }

ConfigScreen::ConfigScreen()
{
    tabs[0] = new Button(1, 2, 104, 17, [&](){ currentTab = 0; return false; }, ui_sheet_res_null_idx, "", 0.0f, 0);
    tabs[1] = new Button(108, 2, 104, 17, [&](){ currentTab = 1; return false; }, ui_sheet_res_null_idx, "", 0.0f, 0);
    tabs[2] = new Button(215, 2, 104, 17, [&](){ currentTab = 2; return false; }, ui_sheet_res_null_idx, "", 0.0f, 0);

    // First column of language buttons
    tabButtons[0].push_back(new Button(37, 52, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::JP); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));
    tabButtons[0].push_back(new Button(37, 74, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::EN); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));
    tabButtons[0].push_back(new Button(37, 96, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::FR); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));
    tabButtons[0].push_back(new Button(37, 118, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::DE); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));
    tabButtons[0].push_back(new Button(37, 140, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::IT); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));
    tabButtons[0].push_back(new Button(37, 162, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::ES); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));

    // Second column of language buttons
    tabButtons[0].push_back(new Button(177, 52, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::ZH); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));
    tabButtons[0].push_back(new Button(177, 74, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::KO); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));
    tabButtons[0].push_back(new Button(177, 96, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::NL); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));
    tabButtons[0].push_back(new Button(177, 118, 8, 8, [](){ Gui::clearStaticText(); Configuration::getInstance().language(Language::PT); return false; }, ui_sheet_res_null_idx, "", 0.0f, 0));

    // Defaults buttons; don't know how to do swkbd stuff yet
    tabButtons[1].push_back(new Button(112, 38, 15, 12, &inputNumber, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
    tabButtons[1].push_back(new Button(112, 62, 15, 12, &inputNumber, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
    tabButtons[1].push_back(new Button(112, 86, 15, 12, &inputText, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
    tabButtons[1].push_back(new Button(112, 110, 15, 12, &countryChoice, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
    tabButtons[1].push_back(new Button(112, 134, 15, 12, &inputDate, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));

    // Miscellaneous buttons
    tabButtons[2].push_back(new Button(237, 39, 15, 12, [](){ Gui::clearStaticText(); Configuration::getInstance().autoBackup(!Configuration::getInstance().autoBackup()); return true; }, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
    tabButtons[2].push_back(new Button(239, 62, 13, 13, [](){ TIMER(Configuration::getInstance().storageSize(Configuration::getInstance().storageSize() - 1)); return false; }, ui_sheet_button_minus_small_idx, "", 0.0f, 0));
    tabButtons[2].push_back(new Button(240, 62, 47, 13, &inputNumber, ui_sheet_res_null_idx, "", 0.0f, 0));
    tabButtons[2].push_back(new Button(288, 62, 13, 13, [](){ TIMER(Configuration::getInstance().storageSize(Configuration::getInstance().storageSize() + 1)); return false; }, ui_sheet_button_plus_small_idx, "", 0.0f, 0));
    tabButtons[2].push_back(new Button(237, 87, 15, 12, [](){ Gui::clearStaticText(); Configuration::getInstance().fixSectors(!Configuration::getInstance().fixSectors()); return true; }, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
    tabButtons[2].push_back(new Button(237, 111, 15, 12, [](){ Gui::clearStaticText(); Configuration::getInstance().transferEdit(!Configuration::getInstance().transferEdit()); return true; }, ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
}

void ConfigScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetBottom);
    Gui::backgroundBottom(false);

    if (currentTab != 0)
    {
        C2D_DrawRectSolid(1, 2, 0.5f, 104, 17, COLOR_DARKBLUE);
        Gui::staticText(1, 2, 104, "Language", FONT_SIZE_11, FONT_SIZE_11, COLOR_BLACK);
    }
    if (currentTab != 1)
    {
        C2D_DrawRectSolid(108, 2, 0.5f, 104, 17, COLOR_DARKBLUE);
        Gui::staticText(108, 2, 104, "Defaults", FONT_SIZE_11, FONT_SIZE_11, COLOR_BLACK);
    }
    if (currentTab != 2)
    {
        C2D_DrawRectSolid(215, 2, 0.5f, 104, 17, COLOR_DARKBLUE);
        Gui::staticText(215, 2, 104, "Misc.", FONT_SIZE_11, FONT_SIZE_11, COLOR_BLACK);
    }

    if (currentTab == 0)
    {
        Gui::dynamicText(1, 2, 104, "Language", FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);

        Gui::staticText("日本語", 59, 47, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::staticText("English", 59, 69, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::staticText("Français", 59, 91, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::staticText("Deutsche", 59, 113, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::staticText("Italiano", 59, 135, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::staticText("Español", 59, 157, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);

        Gui::staticText("中文", 199, 47, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::staticText("한국어", 199, 69, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::staticText("Nederlands", 199, 91, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::staticText("Português", 199, 113, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);

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
        }
    }
    else if (currentTab == 1)
    {
        Gui::staticText(108, 2, 104, "Defaults", FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);

        Gui::staticText("PID", 19, 36, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::staticText("SID", 19, 60, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::staticText("OT", 19, 84, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::staticText("Nationality", 19, 108, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::staticText("Date", 19, 132, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);

        Gui::dynamicText(std::to_string(Configuration::getInstance().defaultPID()), 140, 36, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, false);
        Gui::dynamicText(std::to_string(Configuration::getInstance().defaultSID()), 140, 60, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, false);
        Gui::dynamicText(Configuration::getInstance().defaultOT(), 140, 84, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, false);
        std::string data;
        switch (Configuration::getInstance().nationality())
        {
            // I have no clue what these values are lol
            default:
                data = "United States";
        }
        Gui::dynamicText(data, 140, 108, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, false);
        data = StringUtils::format("%02i-%02i-%i", Configuration::getInstance().month(), Configuration::getInstance().day(), Configuration::getInstance().year());
        Gui::dynamicText(data, 140, 132, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, false);

        for (Button* button : tabButtons[currentTab])
        {
            button->draw();
        }
    }
    else if (currentTab == 2)
    {
        Gui::staticText(215, 2, 104, "Misc.", FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);

        Gui::staticText("Automatically Back Up on Load", 19, 36, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::staticText("Storage Size", 19, 60, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::staticText("Fix Bad Sectors on Exit", 19, 84, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::staticText("Edit During Transfer", 19, 108, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);

        for (Button* button : tabButtons[currentTab])
        {
            button->draw();
        }

        Gui::staticText(Configuration::getInstance().autoBackup() ? "Yes" : "No", 260, 36, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::dynamicText(252, 60, 36, std::to_string(Configuration::getInstance().storageSize()), FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::staticText(Configuration::getInstance().fixSectors() ? "Yes" : "No", 260, 84, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::staticText(Configuration::getInstance().transferEdit() ? "Yes" : "No", 260, 108, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
    }
}

void ConfigScreen::update(touchPosition* touch)
{
    Screen::update();
    if (hidKeysDown() & KEY_B)
    {
        Gui::screenBack();
        return;
    }

    for (Button* button : tabs)
    {
        button->update(touch);
    }

    if (currentTab != 2)
    {
        for (Button* button : tabButtons[currentTab])
        {
            button->update(touch);
        }
    }
    else
    {
        // NOTE: if any other buttons are added, this number will need to be changed, as well
        static std::bitset<6> dirtyButtons;
        for (int i = 0; i < tabButtons[2].size(); i++)
        {
            if (!dirtyButtons[i])
                dirtyButtons[i] = tabButtons[2][i]->update(touch);
            else
                dirtyButtons[i] = tabButtons[2][i]->clicked(touch);
        }
    }
}