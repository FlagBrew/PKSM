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

#include "SaveLoadScreen.hpp"
#include "TitleLoadScreen.hpp"
#include "MainMenu.hpp"
#include "ConfigScreen.hpp"
#include "AccelButton.hpp"
#include "ClickButton.hpp"

static constexpr std::string_view dsIds[9] = {
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

static constexpr std::string_view ctrIds[] = {
    "0x0055D",
    "0x0055E",
    "0x011C4",
    "0x011C5",
    "0x01648",
    "0x0175E",
    "0x01B50",
    "0x01B51"
};

static constexpr std::string_view ctrNames[] = {
    "XY",
    "ORAS",
    "SUMO",
    "USUM"
};

static constexpr std::string_view dsNames[] = {
    "Pt",
    "DP",
    "HGSS",
    "BW",
    "B2W2"
};

static bool wirelessSave() { return true; }

SaveLoadScreen::SaveLoadScreen()
{
    buttons.push_back(new AccelButton(24, 96, 175, 16, [this](){ return this->setSelectedSave(0); }, ui_sheet_res_null_idx, "", 0.0f, 0, 10, 10));
    for (int i = 1; i < 5; i++)
    {
        buttons.push_back(new ClickButton(24, 96 + 17 * i, 175, 16, [this, i](){ return this->setSelectedSave(i); }, ui_sheet_res_null_idx, "", 0.0f, 0));
    }
    buttons.push_back(new AccelButton(24, 181, 175, 16, [this](){ return this->setSelectedSave(5); }, ui_sheet_res_null_idx, "", 0.0f, 0, 10, 10));
    buttons.push_back(new Button(200, 95, 96, 51, [this](){ return this->loadSave(); }, ui_sheet_res_null_idx, "", 0.0f, 0));
    buttons.push_back(new Button(200, 147, 96, 51, &wirelessSave, ui_sheet_res_null_idx, "", 0.0f, 0));

    for (auto i = TitleLoader::sdSaves.begin(); i != TitleLoader::sdSaves.end(); i++)
    {
        std::string key = i->first;
        if (key.size() == 4)
        {
            if (key.substr(0,3) == dsIds[0])
            {
                for (size_t j = 0; j < i->second.size(); j++)
                {
                    saves[0].push_back({"D: ", i->second[j]});
                }
            }
            else if (key.substr(0,3) == dsIds[1])
            {
                for (size_t j = 0; j < i->second.size(); j++)
                {
                    saves[0].push_back({"P: ", i->second[j]});
                }
            }
            else if (key.substr(0,3) == dsIds[2])
            {
                for (size_t j = 0; j < i->second.size(); j++)
                {
                    saves[-1].push_back({"Pt: ", i->second[j]});
                }
            }
            else if (key.substr(0,3) == dsIds[3])
            {
                for (size_t j = 0; j < i->second.size(); j++)
                {
                    saves[1].push_back({"HG: ", i->second[j]});
                }
            }
            else if (key.substr(0,3) == dsIds[4])
            {
                for (size_t j = 0; j < i->second.size(); j++)
                {
                    saves[1].push_back({"SS: ", i->second[j]});
                }
            }
            else if (key.substr(0,3) == dsIds[5])
            {
                for (size_t j = 0; j < i->second.size(); j++)
                {
                    saves[2].push_back({"B: ", i->second[j]});
                }
            }
            else if (key.substr(0,3) == dsIds[6])
            {
                for (size_t j = 0; j < i->second.size(); j++)
                {
                    saves[2].push_back({"W: ", i->second[j]});
                }
            }
            else if (key.substr(0,3) == dsIds[7])
            {
                for (size_t j = 0; j < i->second.size(); j++)
                {
                    saves[3].push_back({"B2: ", i->second[j]});
                }
            }
            else if (key.substr(0,3) == dsIds[8])
            {
                for (size_t j = 0; j < i->second.size(); j++)
                {
                    saves[3].push_back({"W2: ", i->second[j]});
                }
            }
        }
        else
        {
            if (key == ctrIds[0])
            {
                for (size_t j = 0; j < i->second.size(); j++)
                {
                    saves[4].push_back({"X: ", i->second[j]});
                }
            }
            else if (key == ctrIds[1])
            {
                for (size_t j = 0; j < i->second.size(); j++)
                {
                    saves[4].push_back({"Y: ", i->second[j]});
                }
            }
            else if (key == ctrIds[2])
            {
                for (size_t j = 0; j < i->second.size(); j++)
                {
                    saves[5].push_back({"OR: ", i->second[j]});
                }
            }
            else if (key == ctrIds[3])
            {
                for (size_t j = 0; j < i->second.size(); j++)
                {
                    saves[5].push_back({"AS: ", i->second[j]});
                }
            }
            else if (key == ctrIds[4])
            {
                for (size_t j = 0; j < i->second.size(); j++)
                {
                    saves[6].push_back({"S: ", i->second[j]});
                }
            }
            else if (key == ctrIds[5])
            {
                for (size_t j = 0; j < i->second.size(); j++)
                {
                    saves[6].push_back({"M: ", i->second[j]});
                }
            }
            else if (key == ctrIds[6])
            {
                for (size_t j = 0; j < i->second.size(); j++)
                {
                    saves[7].push_back({"US: ", i->second[j]});
                }
            }
            else if (key == ctrIds[7])
            {
                for (size_t j = 0; j < i->second.size(); j++)
                {
                    saves[7].push_back({"UM: ", i->second[j]});
                }
            }
        }
    }
}

void SaveLoadScreen::drawSelector(int x, int y) const
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

void SaveLoadScreen::draw(void) const
{
    C2D_SceneBegin(g_renderTargetTop);
    C2D_DrawRectSolid(0, 0, 0.5f, 400.0f, 240.0f, C2D_Color32(15, 22, 89, 255));

    Gui::sprite(ui_sheet_emulated_gameselector_bg_idx, 4, 29);
    Gui::sprite(ui_sheet_gameselector_cart_idx, 35, 93);

    int x = 90;
    int y = 68;
    
    // draw DS game boxes
    C2D_DrawRectSolid(x += 60, y, 0.5f, 48, 48, COLOR_HIGHBLUE);
    Gui::staticText(x, y + 18, 48, "DP", FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
    C2D_DrawRectSolid(x += 60, y, 0.5f, 48, 48, COLOR_HIGHBLUE);
    Gui::staticText(x, y + 18, 48, "HGSS", FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
    C2D_DrawRectSolid(x += 60, y, 0.5f, 48, 48, COLOR_HIGHBLUE);
    Gui::staticText(x, y + 18, 48, "BW", FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
    C2D_DrawRectSolid(x += 60, y, 0.5f, 48, 48, COLOR_HIGHBLUE);
    Gui::staticText(x, y + 18, 48, "B2W2", FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
    C2D_DrawRectSolid(40, 98, 0.5f, 48, 48, COLOR_HIGHBLUE);
    Gui::staticText(40, 116, 48, "Pt", FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);

    x = 90, y = 128;
    // draw 3DS game boxes
    C2D_DrawRectSolid(x += 60, y, 0.5f, 48, 48, COLOR_HIGHBLUE);
    Gui::staticText(x, y + 18, 48, "XY", FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
    C2D_DrawRectSolid(x += 60, y, 0.5f, 48, 48, COLOR_HIGHBLUE);
    Gui::staticText(x, y + 18, 48, "ORAS", FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
    C2D_DrawRectSolid(x += 60, y, 0.5f, 48, 48, COLOR_HIGHBLUE);
    Gui::staticText(x, y + 18, 48, "SUMO", FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
    C2D_DrawRectSolid(x += 60, y, 0.5f, 48, 48, COLOR_HIGHBLUE);
    Gui::staticText(x, y + 18, 48, "USUM", FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);

    if (saveGroup == -1)
    {
        drawSelector(39, 97);
    }
    else if (saveGroup < 4)
    {
        drawSelector(149 + saveGroup * 60, 67);
    }
    else
    {
        drawSelector(149 + (saveGroup - 4) * 60, 127);
    }

    Gui::staticText(GFX_TOP, 8, i18n::localize("LOADER_INSTRUCTIONS_TOP_PRESENT"), FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);

    C2D_SceneBegin(g_renderTargetBottom);
    Gui::backgroundBottom(true);
    Gui::sprite(ui_sheet_gameselector_savebox_idx, 22, 94);

    Gui::staticText(std::string(titleName(saveGroup)), 27, 26, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);

    if (selectedSave > -1)
    {
        C2D_DrawRectSolid(24, 96 + 17 * selectedSave, 0.5f, 174, 16, C2D_Color32(0x0f, 0x16, 0x59, 255));
    }

    y = 97;
    for (int i = firstSave; i < firstSave + 6; i++)
    {
        if (i < (int) saves[saveGroup].size())
        {
            std::string save = saves[saveGroup][i].second.substr(0, saves[saveGroup][i].second.find_last_of('/'));
            save = save.substr(save.find_last_of('/') + 1);
            save = saves[saveGroup][i].first + save;
            Gui::dynamicText(save, 29, y, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
        }
        else
        {
            break;
        }
        y += 17;
    }

    if (selectedSave > 0 && firstSave > 0)
    {
        C2D_DrawRectSolid(191, 102, 0.5f, 4, 5, C2D_Color32(0x0f, 0x16, 0x59, 255));
        C2D_DrawTriangle(189, 102, C2D_Color32(0x0f, 0x16, 0x59, 255),
                         197, 102, C2D_Color32(0x0f, 0x16, 0x59, 255),
                         193, 97, C2D_Color32(0x0f, 0x16, 0x59, 255), 0.5f);
    }

    if (selectedSave < 5 && (size_t)firstSave + 5 < saves[saveGroup].size() - 1)
    {
        C2D_DrawRectSolid(191, 186, 0.5f, 4, 5, C2D_Color32(0x0f, 0x16, 0x59, 255));
        C2D_DrawTriangle(189, 191, C2D_Color32(0x0f, 0x16, 0x59, 255),
                        197, 191, C2D_Color32(0x0f, 0x16, 0x59, 255),
                        193, 196, C2D_Color32(0x0f, 0x16, 0x59, 255), 0.5f);
    }

    Gui::staticText(200, 113, 96, i18n::localize("LOADER_LOAD"), FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
    Gui::staticText(200, 163, 96, i18n::localize("LOADER_WIRELESS"), FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);

    Gui::staticText(GFX_BOTTOM, 223, i18n::localize("LOADER_INSTRUCTIONS_BOTTOM"), FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);

    C2D_DrawRectSolid(245, 23, 0.5f, 48, 48, COLOR_BLACK);
    C2D_DrawRectSolid(243, 21, 0.5f, 52, 52, C2D_Color32(15, 22, 89, 255));
    Gui::sprite(ui_sheet_gameselector_unknown_idx, 245, 23);
}

void SaveLoadScreen::update(touchPosition* touch)
{
    Screen::update();
    u32 downKeys = hidKeysDown();
    if (selectedGroup)
    {
        if (downKeys & KEY_B)
        {
            selectedGroup = false;
            selectedSave = -1;
            firstSave = 0;
        }
        if (downKeys & KEY_A)
        {
            loadSave();
            return;
        }
        if (downKeys & KEY_X)
        {
            wirelessSave();
            return;
        }
        if (downKeys & KEY_DOWN)
        {
            if (selectedSave == 4)
            {
                if (firstSave + 5 < (int) saves[saveGroup].size() - 1)
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
                if (firstSave + selectedSave < (int) saves[saveGroup].size() - 1)
                {
                    selectedSave++;
                }
            }
        }
        if (downKeys & KEY_UP)
        {
            if (selectedSave == 1)
            {
                if (firstSave > 0)
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
        if (downKeys & KEY_DOWN)
        {
            if (saveGroup == -1)
            {
                saveGroup = 4;
            }
            else if (saveGroup < 4)
            {
                saveGroup += 4;
            }
            else
            {
                saveGroup -= 4;
            }
        }
        else if (downKeys & KEY_UP)
        {
            if (saveGroup == -1)
            {
                saveGroup = 0;
            }
            else if (saveGroup < 4)
            {
                saveGroup += 4;
            }
            else
            {
                saveGroup -= 4;
            }
        }
        else if (downKeys & KEY_RIGHT)
        {
            if (saveGroup == -1)
            {
                saveGroup = 0;
            }
            else if (saveGroup % 4 == 3)
            {
                saveGroup = -1;
            }
            else
            {
                saveGroup++;
            }
        }
        else if (downKeys & KEY_LEFT)
        {
            if (saveGroup == -1)
            {
                saveGroup = 3;
            }
            else if (saveGroup % 4 == 0)
            {
                saveGroup = -1;
            }
            else
            {
                saveGroup--;
            }
        }
        else if (downKeys & KEY_Y)
        {
            Gui::screenBack();
            Gui::setScreen(std::make_unique<TitleLoadScreen>());
            return;
        }
        if (downKeys & KEY_A)
        {
            if (saves[saveGroup].size() != 0)
            {
                selectedGroup = true;
                selectedSave = 0;
            }
        }
    }

    if (downKeys & KEY_SELECT)
    {
        Gui::setScreen(std::make_unique<ConfigScreen>());
    }
}

bool SaveLoadScreen::loadSave()
{
    if (TitleLoader::load(nullptr, saves[saveGroup][selectedSave + firstSave].second))
    {
        Gui::setScreen(std::make_unique<MainMenu>());
        return true;
    }
    return false;
}

constexpr std::string_view SaveLoadScreen::titleName(int index)
{
    if (index < 4)
    {
        return dsNames[index + 1];
    }
    else
    {
        return ctrNames[index - 4];
    }
}

bool SaveLoadScreen::setSelectedSave(int i)
{
    if (i == 5)
    {
        if (firstSave + 5 < (int) saves[saveGroup].size() - 1)
        {
            firstSave++;
            selectedSave = 4;
        }
        else if (firstSave + 5 < (int) saves[saveGroup].size())
        {
            selectedSave = 5;
        }
    }
    else if (i == 0 && firstSave != 0)
    {
        firstSave--;
        selectedSave = 1;
    }
    else if (firstSave + i < (int) saves[saveGroup].size())
    {
        selectedSave = i;
    }
    return false;
}