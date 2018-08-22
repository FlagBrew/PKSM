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

#include "TitleLoadScreen.hpp"

static constexpr char langIds[] = {
    'E', //USA
    'S', //Spain
    'K', //Korea
    'J', //Japan
    'I', //Italy
    'D', //Germany
    'F', //France
    'O'  //Europe?
};

static constexpr char* dsIds[] = {
    "ADA", //Diamond
    "APA", //Pearl
    "CPU", //Platinum
    "IPK", //HeartGold
    "IPG", //SoulSilver
    "IRB", //Black
    "IRA", //White
    "IRE", //Black 2
    "IRD"  //White 2
};

static bool loadSave() { return true; }
static bool wirelessSave() { return true; }

TitleLoadScreen::TitleLoadScreen()
{
    Threads::create((ThreadFunc)TitleLoader::scan);
    for (int i = 0; i < 6; i++)
    {
        buttons.push_back(new Button(24, 96, 175, 16, std::bind(&TitleLoadScreen::setSelectedSave, this, i), ui_sheet_res_null_idx, "", 0.0f, 0));
    }
    buttons.push_back(new Button(200, 95, 96, 51, &loadSave, ui_sheet_res_null_idx, "", 0.0f, 0));
    buttons.push_back(new Button(200, 147, 96, 51, &wirelessSave, ui_sheet_res_null_idx, "", 0.0f, 0));
}

void TitleLoadScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    C2D_DrawRectSolid(0, 0, 0.5f, 400.0f, 240.0f, C2D_Color32(15, 22, 89, 255));

    Gui::sprite(ui_sheet_gameselector_bg_idx, 4, 29);
    Gui::sprite(ui_sheet_gameselector_cart_idx, 35, 93);

    if (TitleLoader::cardTitle != nullptr)
    {
        if (titleFromIndex(selectedTitle) == TitleLoader::cardTitle)
        {
            C2D_DrawRectSolid(38, 96, 0.5f, 52, 52, C2D_Color32(165, 244, 66, 255));
        }

        C2D_DrawImageAt(TitleLoader::cardTitle->icon(), 40, 98, 0.5f, NULL, 1.0f, 1.0f);
    }

    for (size_t i = 0; i < TitleLoader::nandTitles.size(); i++)
    {
        int y = TitleLoader::nandTitles.size() > 4 ? (i / 4) * 60 + 68 : 98;
        int x = 150 + (4 - (TitleLoader::nandTitles.size() % 4 == 0 ? 4 : TitleLoader::nandTitles.size() % 4)) * 30 + (i > 3 ? i - 4 : i) * 60;

        if (titleFromIndex(selectedTitle) == TitleLoader::nandTitles[i])
        {
            C2D_DrawRectSolid(x - 2, y - 2, 0.5f, 52, 52, C2D_Color32(165, 244, 66, 255));
        }

        C2D_DrawImageAt(TitleLoader::nandTitles[i]->icon(), x, y, 0.5f, NULL, 1.0f, 1.0f);
    }

    Gui::dynamicText(GFX_TOP, 8, "Select the game you want to edit.", FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
    Gui::dynamicText(4, 197, 120.0f, "Game Card", FONT_SIZE_14, FONT_SIZE_14, C2D_Color32(15, 22, 89, 255));
    Gui::dynamicText(128, 197, 268.0f, "Installed Games", FONT_SIZE_14, FONT_SIZE_14, C2D_Color32(15, 22, 89, 255));

    C2D_SceneBegin(g_renderTargetBottom);

    Gui::sprite(ui_sheet_gameselector_savebox_idx, 22, 94);

    int nextIdPart = ceilf(27 + textWidth("ID: ", FONT_SIZE_11));
    int nextMediaPart = ceilf(27 + textWidth("Media Type: ", FONT_SIZE_11));

    Gui::dynamicText("ID:", 27, 42, FONT_SIZE_11, FONT_SIZE_11, COLOR_LIGHTBLUE);
    Gui::dynamicText("Media Type:", 27, 54, FONT_SIZE_11, FONT_SIZE_11, COLOR_LIGHTBLUE);
    if (selectedTitle != -2)
    {
        C2D_DrawImageAt(titleFromIndex(selectedTitle)->icon(), 40, 98, 0.5f, NULL, 1.0f, 1.0f);
        Gui::dynamicText(titleFromIndex(selectedTitle)->name(), 27, 26, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::dynamicText(StringUtils::format("%X", titleFromIndex(selectedTitle)->lowId()), nextIdPart, 42, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
        
        std::string mediaType = selectedTitle == -1 ? "Cartridge" : "SD Installed";
        Gui::dynamicText(mediaType, nextMediaPart, 54, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
    }
    else
    {
        Gui::dynamicText("None", 27, 26, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
    }

    int y = 98;
    for (int i = firstSave; i < firstSave + 6; i++)
    {
        if (i == -1)
        {
            Gui::dynamicText("Game Save File", 29, y, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
        }
        else if (i < availableCheckpointSaves.size())
        {
            Gui::dynamicText(availableCheckpointSaves[i], 29, y, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
        }
        else
        {
            break;
        }
        y += 17;
    }

    if (selectedSave > 0)
    {
        C2D_DrawRectSolid(191, 102, 0.5f, 4, 5, C2D_Color32(0x0f, 0x16, 0x59, 255));
        C2D_DrawTriangle(189, 102, C2D_Color32(0x0f, 0x16, 0x59, 255),
                        197, 102, C2D_Color32(0x0f, 0x16, 0x59, 255),
                        193, 107, C2D_Color32(0x0f, 0x16, 0x59, 255), 0.5f);
    }

    C2D_DrawRectSolid(191, 186, 0.5f, 4, 5, C2D_Color32(0x0f, 0x16, 0x59, 255));
    C2D_DrawTriangle(189, 191, C2D_Color32(0x0f, 0x16, 0x59, 255),
                     197, 191, C2D_Color32(0x0f, 0x16, 0x59, 255),
                     193, 196, C2D_Color32(0x0f, 0x16, 0x59, 255), 0.5f);

    Gui::dynamicText(200, 113, 96, "Load \uE000", FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
    Gui::dynamicText(200, 163, 96, "Wireless \uE002", FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);

    Gui::dynamicText(GFX_BOTTOM, 225, "Press HOME or START to exit. Press \uE001 to go back.", FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
}

void TitleLoadScreen::update(touchPosition* touch)
{
    if (selectedTitle == -2)
    {
        if (TitleLoader::cardTitle == nullptr && !TitleLoader::nandTitles.empty())
        {
            selectedTitle = 0;
        }
        else if (TitleLoader::cardTitle != nullptr)
        {
            selectedTitle = -1;
        }
        else
        {
            return;
        }
    }

    u32 buttonsDown = hidKeysDown();

    if (selectedGame)
    {
        if (buttonsDown & KEY_B)
        {
            selectedGame = false;
            selectedSave = -1;
        }
        if (buttonsDown & KEY_A)
        {
            loadSave();
            return;
        }
        if (buttonsDown & KEY_X)
        {
            wirelessSave();
            return;
        }
        if (buttonsDown & KEY_DOWN)
        {
            if (selectedSave == 4)
            {
                if (firstSave + 5 < availableCheckpointSaves.size())
                {
                    firstSave++;
                }
            }
            else
            {
                selectedSave++;
            }
        }
        if (buttonsDown & KEY_UP)
        {
            if (selectedSave == 0)
            {
                if (firstSave > -1)
                {
                    firstSave--;
                }
            }
            else
            {
                selectedSave--;
            }
        }
        for (Button* button : buttons)
        {
            button->update(touch);
        }
    }
    else
    {
        if (buttonsDown & KEY_DOWN)
        {
            if (titleFromIndex(selectedTitle) == TitleLoader::cardTitle)
            {
                if (TitleLoader::nandTitles.size() > 4)
                {
                    selectedTitle = 4;
                }
                else if (!TitleLoader::nandTitles.empty())
                {
                    selectedTitle = 0;
                }
            }
            else
            {
                if (TitleLoader::nandTitles.size() > 4)
                {
                    if (selectedTitle < 4)
                    {
                        selectedTitle = selectedTitle + 4 > TitleLoader::nandTitles.size() - 1 ? TitleLoader::nandTitles.size() - 1 : selectedTitle + 4;
                    }
                    else
                    {
                        selectedTitle -= 4;
                    }
                }
                else if (TitleLoader::nandTitles.size() < 5 && selectedTitle < 4)
                {
                    selectedTitle++;
                }
            }
        }
        if (buttonsDown & KEY_UP)
        {
            if (titleFromIndex(selectedTitle) == TitleLoader::cardTitle)
            {
                if (TitleLoader::nandTitles.size() > 4)
                {
                    selectedTitle = 0;
                }
            }
            else
            {
                if (TitleLoader::nandTitles.size() > 4)
                {
                    if (selectedTitle > 4)
                    {
                        selectedTitle = selectedTitle - 4 < 0 ? 0 : selectedTitle - 4;
                    }
                    else
                    {
                        selectedTitle = selectedTitle + 4 > TitleLoader::nandTitles.size() - 1 ? TitleLoader::nandTitles.size() - 1 : selectedTitle + 4;
                    }
                }
                else if (TitleLoader::nandTitles.size() < 5 && selectedTitle > -2)
                {
                    selectedTitle--;
                }
            }
        }
        if (buttonsDown & KEY_RIGHT)
        {
            if (selectedTitle == -1 || selectedTitle % 4 < 2)
            {
                selectedTitle++;
            }
            else if (selectedTitle % 4 == 3)
            {
                selectedTitle = -1;
            }
        }
        if (buttonsDown & KEY_LEFT)
        {
            if (selectedTitle == -1)
            {
                selectedTitle = TitleLoader::nandTitles.size() < 4 ? TitleLoader::nandTitles.size() - 1 : 3;
            }
            else if (selectedTitle == 4)
            {
                selectedTitle = -1;
            }
            else
            {
                selectedTitle--;
            }
        }
        if (buttonsDown & KEY_A)
        {
            selectedGame = true;
            selectedSave = 0;
        }
    }
}