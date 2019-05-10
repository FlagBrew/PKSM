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

#include "MainMenu.hpp"
#include "BagScreen.hpp"
#include "ConfigScreen.hpp"
#include "EditSelectorScreen.hpp"
#include "InjectSelectorScreen.hpp"
#include "ScriptScreen.hpp"
#include "StorageScreen.hpp"
#include "gui.hpp"
#include "revision.h"

static bool goToScreen(int buttonNum)
{
    switch (buttonNum)
    {
        case 0:
            if (TitleLoader::save->generation() == Generation::LGPE)
            {
                Gui::warn(i18n::localize("STORAGE_IMPLEMENTATION"), i18n::localize("STORAGE_CHECKBACK"));
                return false;
            }
            Gui::setScreen(std::make_unique<StorageScreen>());
            return true;
        case 1:
            Gui::setScreen(std::make_unique<EditSelectorScreen>());
            return true;
        case 2:
            if (TitleLoader::save->generation() == Generation::LGPE)
            {
                Gui::warn(i18n::localize("NO_WONDERCARDS"));
                return false;
            }
            Gui::setScreen(std::make_unique<InjectSelectorScreen>());
            return true;
        case 3:
            Gui::setScreen(std::make_unique<ScriptScreen>());
            return true;
        case 4:
            Gui::setScreen(std::make_unique<BagScreen>());
            return true;
        case 5:
            Gui::setScreen(std::make_unique<ConfigScreen>());
            return true;
    }
    return true;
}

MainMenu::MainMenu()
{
    buttons[0] =
        new MainMenuButton(15, 20, 140, 53, []() { return goToScreen(0); }, ui_sheet_icon_storage_idx, "STORAGE", FONT_SIZE_15, COLOR_WHITE, 27);
    buttons[1] =
        new MainMenuButton(165, 20, 140, 53, []() { return goToScreen(1); }, ui_sheet_icon_editor_idx, "EDITOR", FONT_SIZE_15, COLOR_WHITE, 28);
    buttons[2] =
        new MainMenuButton(15, 83, 140, 53, []() { return goToScreen(2); }, ui_sheet_icon_events_idx, "EVENTS", FONT_SIZE_15, COLOR_WHITE, 93);
    buttons[3] =
        new MainMenuButton(165, 83, 140, 53, []() { return goToScreen(3); }, ui_sheet_icon_scripts_idx, "SCRIPTS", FONT_SIZE_15, COLOR_WHITE, 91);
    buttons[4] = new MainMenuButton(15, 146, 140, 53, []() { return goToScreen(4); }, ui_sheet_icon_bag_idx, "BAG", FONT_SIZE_15, COLOR_WHITE, 157);
    buttons[5] =
        new MainMenuButton(165, 146, 140, 53, []() { return goToScreen(5); }, ui_sheet_icon_settings_idx, "SETTINGS", FONT_SIZE_15, COLOR_WHITE, 160);
}

MainMenu::~MainMenu()
{
    for (auto button : buttons)
    {
        delete button;
    }

    if (isLoadedSaveFromBridge())
    {
        if (Gui::showChoiceMessage(i18n::localize("BRIDGE_SHOULD_SEND_1"), i18n::localize("BRIDGE_SHOULD_SEND_2")))
        {
            bool sent = sendSaveToBridge();
            if (!sent)
            {
                // save a copy of the modified save to the SD card
                backupBridgeChanges();
            }
        }
        else
        {
            setLoadedSaveFromBridge(false);
        }
    }
}

static void menuTop()
{
    Gui::backgroundTop(false);
    Gui::staticText("PKSM", 200, 4, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLUE, TextPosX::CENTER, TextPosY::TOP);
    Gui::staticText(i18n::localize("SAVE_INFO"), 200, 26, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
    Gui::dynamicText(StringUtils::format(i18n::localize("GENERATION"), genToCstring(TitleLoader::save->generation())), 30, 40, FONT_SIZE_12,
        FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    Gui::dynamicText(StringUtils::format(i18n::localize("TRAINER_NAME"), TitleLoader::save->otName().c_str()), 30, 54, FONT_SIZE_12, FONT_SIZE_12,
        COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    Gui::dynamicText(
        i18n::localize("TID_SID") + ": " + std::to_string(TitleLoader::save->displayTID()) + "/" + std::to_string(TitleLoader::save->displaySID()),
        30, 68, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    Gui::dynamicText(StringUtils::format(TitleLoader::save->generation() == Generation::SEVEN ? i18n::localize("STAMPS") : i18n::localize("BADGES"),
                         TitleLoader::save->badges()),
        30, 82, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    Gui::dynamicText(StringUtils::format(i18n::localize("WC_NUM"), TitleLoader::save->currentGifts().size()), 30, 96, FONT_SIZE_12, FONT_SIZE_12,
        COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    Gui::dynamicText(StringUtils::format(i18n::localize("DEX_SEEN"), TitleLoader::save->dexSeen()), 30, 110, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE,
        TextPosX::LEFT, TextPosY::TOP);
    Gui::dynamicText(StringUtils::format(i18n::localize("DEX_CAUGHT"), TitleLoader::save->dexCaught()), 30, 124, FONT_SIZE_12, FONT_SIZE_12,
        COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
}

void MainMenu::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    menuTop();
    C2D_SceneBegin(g_renderTargetBottom);
    Gui::backgroundBottom(false);
    for (MainMenuButton* button : buttons)
    {
        button->draw();
    }
    static const std::string version = StringUtils::format("v%d.%d.%d-%s", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO, GIT_REV);
    Gui::staticText(version, 316, 223, FONT_SIZE_11, FONT_SIZE_11, COLOR_LIGHTBLUE, TextPosX::RIGHT, TextPosY::TOP);
}

void MainMenu::update(touchPosition* touch)
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
    for (MainMenuButton* button : buttons)
    {
        if (button->update(touch))
            return;
    }
    if (keysDown() & KEY_B)
    {
        if (Gui::showChoiceMessage(i18n::localize("SAVE_CHANGES_1"), i18n::localize("SAVE_CHANGES_2"), doTimer ? 250000000 : 0)) // Half second
        {
            TitleLoader::saveChanges();
        }
        Gui::screenBack();
        return;
    }
}
