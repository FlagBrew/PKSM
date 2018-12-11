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

#include "MainMenu.hpp"
#include "gui.hpp"
#include "StorageScreen.hpp"
#include "ConfigScreen.hpp"
#include "InjectSelectorScreen.hpp"
#include "EditSelectorScreen.hpp"
#include "ScriptScreen.hpp"
#include "BagScreen.hpp"

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
    buttons[0] = new MainMenuButton(15, 20, 140, 53, [](){ return goToScreen(0); }, ui_sheet_icon_storage_idx, i18n::localize("STORAGE"), FONT_SIZE_15, COLOR_WHITE, 27);
    buttons[1] = new MainMenuButton(165, 20, 140, 53, [](){ return goToScreen(1); }, ui_sheet_icon_editor_idx, i18n::localize("EDITOR"), FONT_SIZE_15, COLOR_WHITE, 28);
    buttons[2] = new MainMenuButton(15, 83, 140, 53, [](){ return goToScreen(2); }, ui_sheet_icon_events_idx, i18n::localize("EVENTS"), FONT_SIZE_15, COLOR_WHITE, 93);
    buttons[3] = new MainMenuButton(165, 83, 140, 53, [](){ return goToScreen(3); }, ui_sheet_icon_scripts_idx, i18n::localize("SCRIPTS"), FONT_SIZE_15, COLOR_WHITE, 91);    
    buttons[4] = new MainMenuButton(15, 146, 140, 53, [](){ return goToScreen(4); }, ui_sheet_icon_bag_idx, i18n::localize("BAG"), FONT_SIZE_15, COLOR_WHITE, 157);
    buttons[5] = new MainMenuButton(165, 146, 140, 53, [](){ return goToScreen(5); }, ui_sheet_icon_settings_idx, i18n::localize("SETTINGS"), FONT_SIZE_15, COLOR_WHITE, 160);    
}

MainMenu::~MainMenu()
{
    for (int i = 0; i < 6; i++)
    {
        delete buttons[i];
    }
}

static void menuTop()
{
    static const std::string version = StringUtils::format("v%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);
    Gui::backgroundTop(false);
    Gui::staticText(GFX_TOP, 4, "PKSM", FONT_SIZE_14, FONT_SIZE_14, COLOR_BLUE);
    Gui::staticText(version, 398, 229, FONT_SIZE_9, FONT_SIZE_9, COLOR_LIGHTBLUE, true);
}

void MainMenu::draw() const
{
    Gui::clearTextBufs();
    C2D_SceneBegin(g_renderTargetTop);
    menuTop();
    C2D_SceneBegin(g_renderTargetBottom);
    Gui::backgroundBottom(false);
    for (MainMenuButton* button : buttons)
    {
        button->draw();
    }
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
        if (Gui::showChoiceMessage(i18n::localize("SAVE_CHANGES_1"), i18n::localize("SAVE_CHANGES_2")))
        {
            TitleLoader::saveChanges();
        }
        Gui::screenBack();
        return;
    }
}