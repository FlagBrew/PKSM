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

#include "TitleLoadScreen.hpp"

bool TitleLoadScreen::loadSave() const
{
    bool status = false;
    if (selectedSave + firstSave == -1)
    {
        status = TitleLoader::load(titleFromIndex(selectedTitle));
    }
    else
    {
        status = TitleLoader::load(titleFromIndex(selectedTitle), availableCheckpointSaves[selectedSave + firstSave]);
    }
    if (status)
    {
        Gui::setScreen(std::make_unique<MainMenu>());
    }

    return status;
}

TitleLoadScreen::TitleLoadScreen()
{
    buttons.push_back(new AccelButton(24, 96, 175, 16, [this](){ return this->setSelectedSave(0); }, ui_sheet_res_null_idx, "", 0.0f, 0, 10, 10));
    for (int i = 1; i < 5; i++)
    {
        buttons.push_back(new ClickButton(24, 96 + 17 * i, 175, 16, [this, i](){ return this->setSelectedSave(i); }, ui_sheet_res_null_idx, "", 0.0f, 0));
    }
    buttons.push_back(new AccelButton(24, 181, 175, 16, [this](){ return this->setSelectedSave(5); }, ui_sheet_res_null_idx, "", 0.0f, 0, 10, 10));
    buttons.push_back(new Button(200, 95, 96, 51, [this](){ return this->loadSave(); }, ui_sheet_res_null_idx, "", 0.0f, 0));
    buttons.push_back(new Button(200, 147, 96, 51, &receiveSaveFromBridge, ui_sheet_res_null_idx, "", 0.0f, 0));
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

void TitleLoadScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    C2D_DrawRectSolid(0, 0, 0.5f, 400.0f, 240.0f, C2D_Color32(15, 22, 89, 255));

    Gui::sprite(ui_sheet_emulated_gameselector_bg_idx, 4, 29);
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

    Gui::staticText(i18n::localize("LOADER_INSTRUCTIONS_TOP_ABSENT"), 200, 8, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
    Gui::staticText(i18n::localize("LOADER_GAME_CARD"), 4 + 120 / 2, 197, FONT_SIZE_14, FONT_SIZE_14, C2D_Color32(15, 22, 89, 255), TextPosX::CENTER, TextPosY::TOP);
    Gui::staticText(i18n::localize("LOADER_INSTALLED_GAMES"), 128 + 268 / 2, 197, FONT_SIZE_14, FONT_SIZE_14, C2D_Color32(15, 22, 89, 255), TextPosX::CENTER, TextPosY::TOP);

    C2D_SceneBegin(g_renderTargetBottom);
    Gui::backgroundBottom(true);
    Gui::sprite(ui_sheet_gameselector_savebox_idx, 22, 94);

    int nextIdPart = ceilf(27 + StringUtils::textWidth(i18n::localize("LOADER_ID"), FONT_SIZE_11));
    int nextMediaPart = ceilf(27 + StringUtils::textWidth(i18n::localize("LOADER_MEDIA_TYPE"), FONT_SIZE_11));

    Gui::staticText(i18n::localize("LOADER_ID"), 27, 46, FONT_SIZE_11, FONT_SIZE_11, COLOR_LIGHTBLUE, TextPosX::LEFT, TextPosY::TOP);
    Gui::staticText(i18n::localize("LOADER_MEDIA_TYPE"), 27, 58, FONT_SIZE_11, FONT_SIZE_11, COLOR_LIGHTBLUE, TextPosX::LEFT, TextPosY::TOP);
    if (selectedTitle != -2)
    {
        C2D_DrawRectSolid(243, 21, 0.5f, 52, 52, C2D_Color32(15, 22, 89, 255));
        C2D_DrawImageAt(titleFromIndex(selectedTitle)->icon(), 245, 23, 0.5f, NULL, 1.0f, 1.0f);
        Gui::dynamicText(titleFromIndex(selectedTitle)->name(), 27, 26, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::dynamicText(StringUtils::format("%08X", titleFromIndex(selectedTitle)->lowId()), nextIdPart, 46, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        
        std::string mediaType = selectedTitle == -1 ? i18n::localize("LOADER_CARTRIDGE") : i18n::localize("LOADER_SD");
        Gui::staticText(mediaType, nextMediaPart, 58, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    }
    else
    {
        Gui::staticText(i18n::localize("NONE"), 27, 26, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    }

    if (selectedSave > -1)
    {
        C2D_DrawRectSolid(24, 96 + 17 * selectedSave, 0.5f, 174, 16, C2D_Color32(0x0f, 0x16, 0x59, 255));
    }

    int y = 97;
    for (int i = firstSave; i < firstSave + 6; i++)
    {
        if (i == -1)
        {
            Gui::staticText(i18n::localize("LOADER_GAME_SAVE"), 29, y, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        }
        else if (i < (int) availableCheckpointSaves.size())
        {
            std::string save = availableCheckpointSaves[i].substr(0, availableCheckpointSaves[i].find_last_of('/'));
            save = save.substr(save.find_last_of('/') + 1);
            Gui::dynamicText(save, 29, y, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        }
        else
        {
            break;
        }
        y += 17;
    }

    if (selectedSave > 0 && firstSave > -1)
    {
        C2D_DrawRectSolid(191, 102, 0.5f, 4, 5, C2D_Color32(0x0f, 0x16, 0x59, 255));
        C2D_DrawTriangle(189, 102, C2D_Color32(0x0f, 0x16, 0x59, 255),
                         197, 102, C2D_Color32(0x0f, 0x16, 0x59, 255),
                         193, 97, C2D_Color32(0x0f, 0x16, 0x59, 255), 0.5f);
    }

    if (selectedSave < 5 && (size_t)firstSave + 5 < availableCheckpointSaves.size() - 1)
    {
        C2D_DrawRectSolid(191, 186, 0.5f, 4, 5, C2D_Color32(0x0f, 0x16, 0x59, 255));
        C2D_DrawTriangle(189, 191, C2D_Color32(0x0f, 0x16, 0x59, 255),
                        197, 191, C2D_Color32(0x0f, 0x16, 0x59, 255),
                        193, 196, C2D_Color32(0x0f, 0x16, 0x59, 255), 0.5f);
    }

    Gui::staticText(i18n::localize("LOADER_LOAD"), 200 + 96 / 2, 113, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
    Gui::staticText(i18n::localize("LOADER_WIRELESS"), 200 + 96 / 2, 163, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

    Gui::staticText(i18n::localize("LOADER_INSTRUCTIONS_BOTTOM"), 160, 223, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
}

void TitleLoadScreen::update(touchPosition* touch)
{
    Screen::update();
    u32 buttonsDown = hidKeysDown();
    if (TitleLoader::cardUpdate())
    {
        selectedGame = false;
        selectedSave = -1;
        firstSave = -1;
        selectedTitle = -2;
    }
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
            if (buttonsDown & KEY_Y)
            {
                Gui::screenBack();
                Gui::setScreen(std::make_unique<SaveLoadScreen>());
            }
            return;
        }
    }

    if (selectedGame)
    {
        if (buttonsDown & KEY_B)
        {
            selectedGame = false;
            selectedSave = -1;
            firstSave = -1;
        }
        if (buttonsDown & KEY_A)
        {
            loadSave();
            return;
        }
        if (buttonsDown & KEY_X)
        {
            receiveSaveFromBridge();
            return;
        }
        if (buttonsDown & KEY_DOWN)
        {
            if (selectedSave == 4)
            {
                if (firstSave + 5 < (int) availableCheckpointSaves.size() - 1)
                {
                    firstSave++;
                }
                else
                {
                    selectedSave++;
                }
            }
            else
            {
                if (firstSave + selectedSave < (int) availableCheckpointSaves.size() - 1)
                {
                    selectedSave++;
                }
            }
        }
        if (buttonsDown & KEY_UP)
        {
            if (selectedSave == 1)
            {
                if (firstSave > -1)
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
                        selectedTitle = selectedTitle + 4 > (int) TitleLoader::nandTitles.size() - 1 ? TitleLoader::nandTitles.size() - 1 : selectedTitle + 4;
                    }
                    else
                    {
                        selectedTitle -= 4;
                    }
                }
                else if (TitleLoader::nandTitles.size() < 5)
                {
                    if (selectedTitle < (int) TitleLoader::nandTitles.size() - 2)
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
        else if (buttonsDown & KEY_UP)
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
                        selectedTitle = selectedTitle + 4 > (int) TitleLoader::nandTitles.size() - 1 ? TitleLoader::nandTitles.size() - 1 : selectedTitle + 4;
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
        else if (buttonsDown & KEY_RIGHT)
        {
            if (selectedTitle == (int) TitleLoader::nandTitles.size() - 1 || selectedTitle == 3)
            {
                if (TitleLoader::cardTitle)
                {
                    selectedTitle = -1;
                }
                else
                {
                    if (TitleLoader::nandTitles.size() > 4 && selectedTitle > 3)
                    {
                        if (selectedTitle > 3)
                        {
                            selectedTitle = 4;
                        }
                        else if (selectedTitle == 3)
                        {
                            selectedTitle = 0;
                        }
                    }
                    else
                    {
                        selectedTitle = 0;
                    }
                }
            }
            else
            {
                selectedTitle++;
            }
        }
        else if (buttonsDown & KEY_LEFT)
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
                    selectedTitle = (int) TitleLoader::nandTitles.size() - 1;
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
                    selectedTitle = TitleLoader::nandTitles.size() > 4 ? 3 : (int) TitleLoader::nandTitles.size() - 1;
                }
            }
            else
            {
                selectedTitle--;
            }
        }
        else if (buttonsDown & KEY_Y)
        {
            Gui::screenBack();
            Gui::setScreen(std::make_unique<SaveLoadScreen>());
            return;
        }
        if (buttonsDown & KEY_A)
        {
            selectedGame = true;
            selectedSave = 0;
        }
    }
    availableCheckpointSaves = TitleLoader::sdSaves[titleFromIndex(selectedTitle)->checkpointPrefix()];

    if (buttonsDown & KEY_SELECT)
    {
        Gui::setScreen(std::make_unique<ConfigScreen>());
    }
}

bool TitleLoadScreen::setSelectedSave(int i)
{
    if (i == 5)
    {
        if (firstSave + 5 < (int) availableCheckpointSaves.size() - 1)
        {
            firstSave++;
            selectedSave = 4;
        }
        else if (firstSave + 5 < (int) availableCheckpointSaves.size())
        {
            selectedSave = 5;
        }
    }
    else if (i == 0 && firstSave != -1)
    {
        firstSave--;
        selectedSave = 1;
    }
    else if (firstSave + i < (int) availableCheckpointSaves.size())
    {
        selectedSave = i;
    }
    return false;
}