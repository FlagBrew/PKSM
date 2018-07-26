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
#include "CreditsScreen.hpp"
#include "StorageScreen.hpp"

static constexpr int icons[6] = {
    ui_spritesheet_res_icon_storage_idx,
    ui_spritesheet_res_icon_editor_idx,
    ui_spritesheet_res_icon_events_idx,
    ui_spritesheet_res_icon_scripts_idx,
    ui_spritesheet_res_icon_settings_idx,
    ui_spritesheet_res_icon_credits_idx
};

static constexpr int yLevels[6] = {
    27,
    28,
    93,
    91,
    157,
    160
};

static bool goToScreen(int buttonNum)
{
    switch (buttonNum)
    {
        case 0:
            Gui::setScreen(std::unique_ptr<Screen>(new StorageScreen(NULL)));
            return true;
        case 1:
            //Gui::setScreen(new EditorScreen);
            return true;
        case 2:
            //Gui::setScreen(new EventsScreen);
            return true;
        case 3:
            //Gui::setScreen(new ScriptsScreen);
            return true;
        case 4:
            //Gui::setScreen(new OptionsScreen);
            return true;
        case 5:
            Gui::setScreen(std::unique_ptr<Screen>(new CreditsScreen));
            return true;
    }
    return true;
}

MainMenu::MainMenu()
{
    for (u8 i = 0; i < 3; i++)
    {
        for (u8 j = 0; j < 2; j++)
        {
            buttons[i*2 + j] = new MainMenuButton(
                15 + 150*j, 20 + 63*i, 140, 53,
                std::bind(&goToScreen, i*2 + j),
                icons[i*2 + j],
                StringUtils::format("MainMenuBtn%i", i*2 + j),
                FONT_SIZE_15,
                COLOR_WHITE,
                yLevels[i*2 + j]);
        }
    }
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
    Gui::clearTextBufs();
    Gui::backgroundTop();
    Gui::staticText(GFX_TOP, 4, "PKSM", FONT_SIZE_14, FONT_SIZE_14, COLOR_BLUE);
    Gui::staticText(version, 398, 229, FONT_SIZE_9, FONT_SIZE_9, COLOR_LIGHTBLUE, true);
}

void MainMenu::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    menuTop();

    C2D_SceneBegin(g_renderTargetBottom);
    Gui::backgroundBottom();
    for (MainMenuButton* button : buttons)
    {
        button->draw();
    }
}

void MainMenu::update(touchPosition* touch)
{
    for (MainMenuButton* button : buttons)
    {
        if (button->update(touch))
            return;
    }
}