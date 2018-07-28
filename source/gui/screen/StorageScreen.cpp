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

#include "StorageScreen.hpp"
#include "gui.hpp"
#include "MainMenu.hpp"
#include "PK4.hpp"

// TODO: remove
static u8 test[] = {0x0B,0xEB,0x64,0x89,0x00,0x00,0xC8,0xA5,0x12,0x00,0x00,0x00,0x2A,0x8A,0x42,0x73,0x47,0x9C,0x00,0x00,0xA0,0x91,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x21,0x00,0x62,0x00,0xEF,0x00,0x22,0x01,0x23,0x1E,0x14,0x14,0x00,0x00,0x00,0x00,0xD1,0xA1,0x33,0x3C,0x00,0x00,0x00,0x00,0x02,0x13,0x01,0x00,0x00,0x00,0x00,0x00,0x50,0x00,0x69,0x00,0x64,0x00,0x67,0x00,0x65,0x00,0x6F,0x00,0x74,0x00,0xFF,0xFF,0x6F,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x17,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x52,0x00,0x6F,0x00,0x43,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x01,0x0D,0x00,0x00,0x4B,0x00,0x00,0x19,0x0A,0x00,0x00,0x00};
static std::shared_ptr<PKX> testPkm(new PK5(test));

static bool backHeld = false;

static u8 type1(int generation, u16 species)
{
    switch (generation)
    {
        case 4:
            return PersonalDPPtHGSS::type1(species);
        case 5:
            return PersonalBWB2W2::type1(species);
        case 6:
            return PersonalXYORAS::type1(species);
        case 7:
            return PersonalSMUSUM::type1(species);
    }
    return 0;
}

static u8 type2(int generation, u16 species)
{
    switch (generation)
    {
        case 4:
            return PersonalDPPtHGSS::type2(species);
        case 5:
            return PersonalBWB2W2::type2(species);
        case 6:
            return PersonalXYORAS::type2(species);
        case 7:
            return PersonalSMUSUM::type2(species);
    }
    return 0;
}

int bobPointer()
{
    static int currentBob = 0;
    static bool up = true;
    if (up)
    {
        currentBob++;
        if (currentBob >= 12)
        {
            up = false;
        }
    }
    else
    {
        currentBob--;
        if (currentBob <= 0)
        {
            up = true;
        }
    }
    return currentBob / 4;
}

bool changeBoxName() { return false; }

bool wirelessStuff() { return false; }

StorageScreen::StorageScreen(Sav* save)
{
    this->save = save;
    mainButtons[0] = new Button(242, 5, 45, 20, std::bind(&StorageScreen::swapBoxWithStorage, this), ui_sheet_button_swap_boxes_idx, "", 0.0f, 0);
    mainButtons[1] = new Button(208, 43, 108, 31, std::bind(&StorageScreen::showViewer, this), ui_sheet_button_editor_idx,
                                    "StorageButtonView", FONT_SIZE_12, COLOR_BLACK);
    mainButtons[2] = new Button(208, 72, 108, 31, std::bind(&StorageScreen::clearBox, this), ui_sheet_button_editor_idx,
                                    "StorageButtonClear", FONT_SIZE_12, COLOR_BLACK);
    mainButtons[3] = new Button(208, 101, 108, 31, std::bind(&StorageScreen::releasePkm, this), ui_sheet_button_editor_idx,
                                    "StorageButtonRelease", FONT_SIZE_12, COLOR_BLACK);
    mainButtons[4] = new Button(208, 130, 108, 31, std::bind(&StorageScreen::dumpPkm, this), ui_sheet_button_editor_idx,
                                    "StorageButtonDump", FONT_SIZE_12, COLOR_BLACK);

    //mainButtons[5] = new Button(7, 17, 16, 20, std::bind(&StorageScreen::lastBox, this, true), ui_sheet_res_button_arrow_idx, "", 0.0f, 0);
    //mainButtons[6] = new Button(185, 17, 16, 20, std::bind(&StorageScreen::nextBox, this, true), ui_sheet_res_emulated_button_arrow_right_idx, "", 0.0f, 0);
    mainButtons[7] = new Button(32, 14, 144, 26, &changeBoxName, ui_sheet_res_null_idx, "", 0.0f, 0);
    mainButtons[8] = new Button(240, 211, 33, 28, &wirelessStuff, ui_sheet_button_wireless_idx, "", 0.0f, 0);
    mainButtons[9] = new Button(283, 211, 34, 28, std::bind(&StorageScreen::backButton, this), ui_sheet_button_back_idx, "", 0.0f, 0);

    // Pokemon buttons
    u16 y = 45;
    for (u8 row = 0; row < 5; row++)
    {
        u16 x = 4;
        for (u8 column = 0; column < 6; column++)
        {
            pkmButtons[row*6 + column] = new Button(x, y, 34, 30, std::bind(&StorageScreen::setBottomIndex, this, row*6 + column), ui_sheet_res_null_idx, "", 0.0f, 0);
            x += 34;
        }
        y += 30;
    }
}

void StorageScreen::draw() const
{
    // static const std::shared_ptr<PKX>& infoMon = testPkm; // storageChosen ? storage->pkm(storageBox, cursorIndex) : save->pkm(storageBox, cursorIndex)
    // C2D_SceneBegin(g_renderTargetBottom);
    // Gui::sprite(ui_sheet_res_box_bottom_idx, 0, 0);
    // Gui::sprite(ui_sheet_res_bar_editor_idx, 0, 210);
    // for (Button* button : mainButtons)
    // {
    //     button->draw();
    // }

    // //Gui::dynamicText(32, 19, 144, save->boxName(hid.page()), FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
    // Gui::dynamicText(32, 19, 144, "This is a box", FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);

    // // actual Pokemon
    // u16 y = 45;
    // for (u8 row = 0; row < 5; row++)
    // {
    //     u16 x = 4;
    //     for (u8 column = 0; column < 6; column++)
    //     {
    //         //Gui::pkm(save->pkm(boxBox, row * 6 + column).get(), x, y);
    //         Gui::pkm(70, x, y);
    //         x += 34;
    //     }
    //     y += 30;
    // }
    // if (!storageChosen)
    // {
    //     Gui::sprite(ui_sheet_res_selector_box_idx, 22 + (cursorIndex % 6) * 34, 37 + (cursorIndex / 6) * 30 + bobPointer());
    // }

    // if (viewer)
    // {
    //     C2D_DrawRectSolid(0, 0, 0.5f, 320, 210, C2D_Color32(0, 0, 0, 120));
    //     viewer->draw();
    // }
    // else
    // {
    //     C2D_SceneBegin(g_renderTargetTop);
    //     Gui::backgroundAnimated(GFX_TOP);
    //     Gui::sprite(ui_sheet_res_storage_top_idx, 34, 5);
    //     Gui::dynamicText(80, 9, 124, StringUtils::format("Bank %i", storageBox), FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);

    //     // actual Pokemon
    //     u16 y = 45;
    //     for (u8 row = 0; row < 5; row++)
    //     {
    //         u16 x = 44;
    //         for (u8 column = 0; column < 6; column++)
    //         {
    //             //Gui::pkm(storage->pkm(storageBox, row * 6 + column).get(), x, y); Don't know what the call will be?
    //             Gui::pkm(99, x, y);
    //             x += 34;
    //         }
    //         y += 30;
    //     }
        
    //     // Mini viewer
    //     Gui::dynamicText(infoMon->nickname(), 273, 50, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
    //     Gui::dynamicText(StringUtils::format("#%i", infoMon->species()), 273, 67, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        
    //     std::string text = StringUtils::format("Lv.%i", infoMon->level());
    //     float width = textWidth(text, FONT_SIZE_12);
    //     Gui::dynamicText(text, 374 - (int) width, 67, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
    //     if (infoMon->gender() == 0)
    //     {
    //         Gui::sprite(ui_sheet_res_male_idx, 358 - (int) width, 67);
    //     }
    //     else if (infoMon->gender() == 1)
    //     {
    //         Gui::sprite(ui_sheet_res_female_idx, 360 - (int) width, 67);
    //     }
    //     if (infoMon->shiny())
    //     {
    //         Gui::sprite(ui_sheet_res_shiny_idx, 346 - (int) width, 69);
    //     }

    //     Gui::dynamicText(i18n::species(/*Settings::language()*/Language::EN, infoMon->species()), 273, 86, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
    //     u8 firstType = type1(infoMon->generation(), infoMon->formSpecies());
    //     u8 secondType = type2(infoMon->generation(), infoMon->formSpecies());
    //     Gui::type(/*Settings::language()*/Language::EN, firstType, 273, 103);
    //     if (firstType != secondType)
    //         Gui::type(/*Settings::language()*/Language::EN, secondType, 325, 103);

    //     Gui::dynamicText(infoMon->otName(), 273, 126, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
    //     Gui::dynamicText(StringUtils::format("ID. %i", infoMon->TID()), 374, 143, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE, true);

    //     Gui::dynamicText(i18n::nature(/*Settings::language()*/Language::EN, infoMon->nature()), 273, 167, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);

    //     Gui::dynamicText(StringUtils::format("%i/%i/%i", infoMon->iv(0), infoMon->iv(1), infoMon->iv(2)), 374, 167, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE, true);
    //     Gui::dynamicText(StringUtils::format("%i/%i/%i", infoMon->iv(4), infoMon->iv(5), infoMon->iv(3)), 374, 183, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE, true);

    //     // Cursor

    //     if (storageChosen)
    //     {
    //         Gui::sprite(ui_sheet_res_selector_box_idx, 62 + (cursorIndex % 6) * 34, 37 + (cursorIndex / 6) * 30 + bobPointer());
    //     }
    // }
}

void StorageScreen::update(touchPosition* touch)
{
    static bool sleep = true;
    if (!viewer)
    {
        for (Button* button : mainButtons)
        {
            if (button->update(touch))
                return;
        }
        backHeld = false;
        for (Button* button : pkmButtons)
        {
            button->update(touch);
        }

        static int buttonCooldown = 10;

        u64 kDown = hidKeysDown();
        u64 kHeld = hidKeysHeld();

        if (kDown & KEY_L)
        {
            lastBox();
        }
        else if (kDown & KEY_R)
        {
            nextBox();
        }
        else if (kDown & KEY_B)
        {
            backButton();
            return;
        }
        else if (kDown & KEY_X)
        {
            showViewer();
            return;
        }
        else if (buttonCooldown <= 0)
        {
            sleep = false;
            if (kHeld & KEY_LEFT)
            {
                if (cursorIndex > 0) 
                {
                    cursorIndex--;
                }
                else if (cursorIndex == 0)
                {
                    lastBox();
                    cursorIndex = 29;
                }
                sleep = true;
            }
            else if (kHeld & KEY_RIGHT)
            {
                if (cursorIndex < 29)
                {
                    cursorIndex++;
                }
                else if (cursorIndex == 29)
                {
                    nextBox();
                    cursorIndex = 0;
                }
                sleep = true;
            }
            else if (kHeld & KEY_UP)
            {
                if (cursorIndex <= 5 && !storageChosen)
                {
                    storageChosen = true;
                    cursorIndex += 24;
                }
                else if (cursorIndex > 5)
                {			
                    cursorIndex -= 6;
                }
                sleep = true;
            }
            else if (kHeld & KEY_DOWN)
            {
                if (cursorIndex >= 24 && storageChosen)
                {
                    storageChosen = false;
                    cursorIndex -= 24;
                }
                else if (cursorIndex < 24)
                {
                    cursorIndex += 6;
                }
                sleep = true;
            }
            if (sleep)
                buttonCooldown = 10;
        }
        if (sleep)
            buttonCooldown--;
    }
    else
    {
        if (hidKeysDown() & KEY_B || mainButtons[9]->update(touch))
        {
            backButton();
        }
    }
}

bool StorageScreen::lastBox(bool forceBottom)
{
    if (storageChosen && !forceBottom)
    {
        storageBox--;
        if (storageBox == -1)
        {
            storageBox = 31; // Storage_implementation->maxBoxNum;
        }
    }
    else
    {
        boxBox--;
        if (boxBox == -1)
        {
            boxBox = 31;
            //boxBox = save->boxes - 1;
        }
    }
    return false;
}

bool StorageScreen::nextBox(bool forceBottom)
{
    if (storageChosen && !forceBottom)
    {
        storageBox++;
        if (storageBox == 32) // save->boxes)
        {
            storageBox = 0;
        }
    }
    else
    {
        boxBox++;
        if (boxBox == 32) // save->boxes)
        {
            boxBox = 0;
        }
    }
    return false;
}

bool StorageScreen::setBottomIndex(int index)
{
    cursorIndex = index;
    storageChosen = false;
    return false;
}

bool StorageScreen::backButton()
{
    if (!backHeld)
    {
        backHeld = true;
        if (!viewer)
            Gui::screenBack();
        else
            viewer.reset(NULL);
    }
    return true;
}

bool StorageScreen::showViewer()
{
    static const std::shared_ptr<PKX>& view = testPkm;
    viewer = std::unique_ptr<ViewerScreen>(new ViewerScreen(view));
    return true;
}

bool StorageScreen::clearBox()
{
    backHeld = true;
    if (Gui::showChoiceMessage("Erase the selected box?"))
    {
        for (int i = 0; i < 30; i++)
        {
            if (storageChosen) { // Storage set all slots in box to PK7()s (or however it happens)
            }
            else
                save->pkm(*save->emptyPkm(), boxBox, i);
        }
    }
    return false;
}

bool StorageScreen::releasePkm()
{
    backHeld = true;
    if (Gui::showChoiceMessage("Release the selected Pokémon?"))
    {
        if (storageChosen) { // Storage set all slots in box to PK7()s (or however it happens)
        }
        else
            save->pkm(*save->emptyPkm(), boxBox, cursorIndex);
    }
    return false;
}
