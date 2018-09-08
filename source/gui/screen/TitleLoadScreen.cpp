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
#include "MainMenu.hpp"
#include "FSStream.hpp"

bool TitleLoadScreen::loadSave() const
{
    if (selectedSave == -1)
    {
        TitleLoader::load(titleFromIndex(selectedTitle));
    }
    else
    {
        TitleLoader::load(availableCheckpointSaves[selectedSave + firstSave]);
    }
    Gui::setScreen(std::unique_ptr<Screen>(new MainMenu));
    return true;
}

static bool wirelessSave() { return true; }

TitleLoadScreen::TitleLoadScreen()
{
    Threads::create((ThreadFunc)TitleLoader::scan);
    for (int i = 0; i < 6; i++)
    {
        buttons.push_back(new Button(24, 96, 175, 16, std::bind(&TitleLoadScreen::setSelectedSave, this, i), ui_sheet_res_null_idx, "", 0.0f, 0));
    }
    buttons.push_back(new Button(200, 95, 96, 51, std::bind(&TitleLoadScreen::loadSave, this), ui_sheet_res_null_idx, "", 0.0f, 0));
    buttons.push_back(new Button(200, 147, 96, 51, &wirelessSave, ui_sheet_res_null_idx, "", 0.0f, 0));
}

void TitleLoadScreen::drawSelector(int x, int y) const
{
    static const int w = 2;
    float highlight_multiplier = fmax(0.0, fabs(fmod(Screen::timer(), 1.0) - 0.5) / 0.5);
    u8 r = COLOR_SELECTOR & 0xFF;
    u8 g = (COLOR_SELECTOR >> 8) & 0xFF;
    u8 b = (COLOR_SELECTOR >> 16) & 0xFF;
    u32 color = C2D_Color32(r + (255 - r) * highlight_multiplier, g + (255 - g) * highlight_multiplier, b + (255 - b) * highlight_multiplier, 255);

    C2D_DrawRectSolid(         x,          y, 0.5f, 50,       50, C2D_Color32(255, 255, 255, 100)); 
    C2D_DrawRectSolid(         x,          y, 0.5f, 50,        w, color); // top
    C2D_DrawRectSolid(         x,      y + w, 0.5f,  w, 50 - 2*w, color); // left
    C2D_DrawRectSolid(x + 50 - w,      y + w, 0.5f,  w, 50 - 2*w, color); // right
    C2D_DrawRectSolid(         x, y + 50 - w, 0.5f, 50,        w, color); // bottom
}

void TitleLoadScreen::draw()
{
    C2D_SceneBegin(g_renderTargetTop);
    C2D_DrawRectSolid(0, 0, 0.5f, 400.0f, 240.0f, C2D_Color32(15, 22, 89, 255));

    Gui::sprite(ui_sheet_gameselector_bg_idx, 4, 29);
    Gui::sprite(ui_sheet_gameselector_cart_idx, 35, 93);

    if (TitleLoader::cardTitle != nullptr)
    {
        C2D_DrawImageAt(TitleLoader::cardTitle->icon(), 40, 98, 0.5f, NULL, 1.0f, 1.0f);
        if (titleFromIndex(selectedTitle) == TitleLoader::cardTitle)
        {
            drawSelector(39, 97);
        }
    }

    for (size_t i = 0; i < TitleLoader::nandTitles.size(); i++)
    {
        int y = TitleLoader::nandTitles.size() > 4 ? (i / 4) * 60 + 68 : 98;
        int x = 150 + (4 - (TitleLoader::nandTitles.size() % 4 == 0 ? 4 : TitleLoader::nandTitles.size() % 4)) * 30 + (i > 3 ? i - 4 : i) * 60;;
        if (TitleLoader::nandTitles.size() > 4 && i < 4)
        {
            x = 150 + (i > 3 ? i - 4 : i) * 60;
        }

        C2D_DrawImageAt(TitleLoader::nandTitles[i]->icon(), x, y, 0.5f, NULL, 1.0f, 1.0f);
        if (titleFromIndex(selectedTitle) == TitleLoader::nandTitles[i])
        {
            drawSelector(x - 1, y - 1);
        }
    }

    Gui::staticText(GFX_TOP, 8, i18n::localize("LOADER_INSTRUCTIONS_TOP"), FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
    Gui::staticText(4, 197, 120.0f, i18n::localize("LOADER_GAME_CARD"), FONT_SIZE_14, FONT_SIZE_14, C2D_Color32(15, 22, 89, 255));
    Gui::staticText(128, 197, 268.0f, i18n::localize("LOADER_INSTALLED_GAMES"), FONT_SIZE_14, FONT_SIZE_14, C2D_Color32(15, 22, 89, 255));

    C2D_SceneBegin(g_renderTargetBottom);
    Gui::backgroundBottom(true);
    Gui::sprite(ui_sheet_gameselector_savebox_idx, 22, 94);

    int nextIdPart = ceilf(27 + textWidth(i18n::localize("LOADER_ID"), FONT_SIZE_11));
    int nextMediaPart = ceilf(27 + textWidth(i18n::localize("LOADER_MEDIA_TYPE"), FONT_SIZE_11));

    Gui::staticText(i18n::localize("LOADER_ID"), 27, 42, FONT_SIZE_11, FONT_SIZE_11, COLOR_LIGHTBLUE);
    Gui::staticText(i18n::localize("LOADER_MEDIA_TYPE"), 27, 54, FONT_SIZE_11, FONT_SIZE_11, COLOR_LIGHTBLUE);
    if (selectedTitle != -2)
    {
        C2D_DrawImageAt(titleFromIndex(selectedTitle)->icon(), 245, 23, 0.5f, NULL, 1.0f, 1.0f);
        Gui::dynamicText(titleFromIndex(selectedTitle)->name(), 27, 26, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        Gui::dynamicText(StringUtils::format("%08X", titleFromIndex(selectedTitle)->lowId()), nextIdPart, 42, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
        
        std::string mediaType = selectedTitle == -1 ? i18n::localize("LOADER_CARTRIDGE") : i18n::localize("LOADER_SD");
        Gui::staticText(mediaType, nextMediaPart, 54, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
    }
    else
    {
        Gui::staticText(i18n::localize("NONE"), 27, 26, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
    }

    if (selectedSave > -1)
    {
        C2D_DrawRectSolid(24, 96 + 17 * selectedSave, 0.5f, 174, 16, C2D_Color32(0x0f, 0x16, 0x59, 255));
    }

    int y = 98;
    for (int i = firstSave; i < firstSave + 6; i++)
    {
        if (i == -1)
        {
            Gui::staticText(i18n::localize("LOADER_GAME_SAVE"), 29, y, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
        }
        else if (i < availableCheckpointSaves.size())
        {
            std::string save;
            bool passedSlash = false;
            for (int j = availableCheckpointSaves[i].size() - 1; j > -1; j--)
            {
                if (availableCheckpointSaves[i][j] == '/')
                {
                    if (passedSlash)
                    {
                        break;
                    }
                    else
                    {
                        passedSlash = true;
                    }
                }
                else
                {
                    if (passedSlash)
                    {
                        save = availableCheckpointSaves[i][j] + save;
                    }
                }
            }
            Gui::dynamicText(save, 29, y, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
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
                        193, 97, C2D_Color32(0x0f, 0x16, 0x59, 255), 0.5f);
    }

    C2D_DrawRectSolid(191, 186, 0.5f, 4, 5, C2D_Color32(0x0f, 0x16, 0x59, 255));
    C2D_DrawTriangle(189, 191, C2D_Color32(0x0f, 0x16, 0x59, 255),
                     197, 191, C2D_Color32(0x0f, 0x16, 0x59, 255),
                     193, 196, C2D_Color32(0x0f, 0x16, 0x59, 255), 0.5f);

    Gui::staticText(200, 113, 96, i18n::localize("LOADER_LOAD"), FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
    Gui::staticText(200, 163, 96, i18n::localize("LOADER_WIRELESS"), FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);

    Gui::staticText(GFX_BOTTOM, 225, i18n::localize("LOADER_INSTRUCTIONS_BOTTOM"), FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);

    Screen::draw();
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
                else if (TitleLoader::nandTitles.size() < 5)
                {
                    if (selectedTitle < TitleLoader::nandTitles.size() - 1)
                    {
                        selectedTitle++;
                    }
                    else
                    {
                        if (TitleLoader::cardTitle)
                        {
                            selectedTitle = -1;
                        }
                        else
                        {
                            selectedTitle = 0;
                        }
                    }
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
                else
                {
                    selectedTitle = TitleLoader::nandTitles.size() - 1;
                }
            }
            else
            {
                if (TitleLoader::nandTitles.size() > 4)
                {
                    if (selectedTitle > 3)
                    {
                        selectedTitle = selectedTitle - 4;
                    }
                    else
                    {
                        selectedTitle = selectedTitle + 4 > TitleLoader::nandTitles.size() - 1 ? TitleLoader::nandTitles.size() - 1 : selectedTitle + 4;
                    }
                }
                else if (TitleLoader::nandTitles.size() < 5 && selectedTitle > -2)
                {
                    selectedTitle--;
                    if (selectedTitle == -1 && !TitleLoader::cardTitle)
                    {
                        selectedTitle = TitleLoader::nandTitles.size() - 1;
                    }
                }
            }
        }
        if (buttonsDown & KEY_RIGHT)
        {
            if (selectedTitle == TitleLoader::nandTitles.size() - 1 || selectedTitle == 3)
            {
                if (TitleLoader::cardTitle)
                {
                    selectedTitle = -1;
                }
                else
                {
                    if (selectedTitle == 3)
                    {
                        selectedTitle = 0;
                    }
                    else
                    {
                        selectedTitle = 4;
                    }
                }
            }
            else
            {
                selectedTitle++;
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
                if (TitleLoader::cardTitle)
                {
                    selectedTitle = -1;
                }
                else
                {
                    selectedTitle = TitleLoader::nandTitles.size() - 1;
                }
            }
            else if (selectedTitle == 0)
            {
                if (TitleLoader::cardTitle)
                {
                    selectedTitle = -1;
                }
                else
                {
                    selectedTitle = TitleLoader::nandTitles.size() > 4 ? 3 : TitleLoader::nandTitles.size() - 1;
                }
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
    availableCheckpointSaves = TitleLoader::sdSaves[StringUtils::format("0x%05X", titleFromIndex(selectedTitle)->lowId() >> 8)];
}