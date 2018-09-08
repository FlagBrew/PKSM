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
#include "Configuration.hpp"
#include "TitleLoadScreen.hpp"

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

StorageScreen::StorageScreen()
{
    mainButtons[0] = new Button(242, 9, 47, 22, std::bind(&StorageScreen::swapBoxWithStorage, this), ui_sheet_button_swap_boxes_idx, "", 0.0f, 0);
    mainButtons[1] = new Button(212, 40, 108, 28, std::bind(&StorageScreen::showViewer, this), ui_sheet_button_editor_idx,
                                    "StorageButtonView", FONT_SIZE_12, COLOR_BLACK);
    mainButtons[2] = new Button(212, 71, 108, 28, std::bind(&StorageScreen::clearBox, this), ui_sheet_button_editor_idx,
                                    "StorageButtonClear", FONT_SIZE_12, COLOR_BLACK);
    mainButtons[3] = new Button(212, 102, 108, 28, std::bind(&StorageScreen::releasePkm, this), ui_sheet_button_editor_idx,
                                    "StorageButtonRelease", FONT_SIZE_12, COLOR_BLACK);
    mainButtons[4] = new Button(212, 133, 108, 28, std::bind(&StorageScreen::dumpPkm, this), ui_sheet_button_editor_idx,
                                    "StorageButtonDump", FONT_SIZE_12, COLOR_BLACK);

    mainButtons[5] = new Button(8, 15, 17, 24, std::bind(&StorageScreen::lastBox, this, true), ui_sheet_res_null_idx, "", 0.0f, 0);
    mainButtons[6] = new Button(189, 15, 17, 24, std::bind(&StorageScreen::nextBox, this, true), ui_sheet_res_null_idx, "", 0.0f, 0);
    //mainButtons[5] = new Button(7, 17, 16, 20, std::bind(&StorageScreen::lastBox, this, true), ui_sheet_res_button_arrow_idx, "", 0.0f, 0);
    //mainButtons[6] = new Button(185, 17, 16, 20, std::bind(&StorageScreen::nextBox, this, true), ui_sheet_res_emulated_button_arrow_right_idx, "", 0.0f, 0);
    mainButtons[7] = new Button(32, 15, 164, 24, &changeBoxName, ui_sheet_res_null_idx, "", 0.0f, 0);
    mainButtons[8] = new Button(4, 212, 33, 28, &wirelessStuff, ui_sheet_button_wireless_idx, "", 0.0f, 0);
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
    TitleLoader::save->cryptBoxData(true);
}

void StorageScreen::draw() const
{
    std::shared_ptr<PKX> infoMon = storageChosen ? /*storage->pkm(storageBox, cursorIndex)*/ testPkm : TitleLoader::save->pkm(boxBox, cursorIndex);
    if (infoMon->species() == 0)
    {
        infoMon = nullptr;
    }
    C2D_SceneBegin(g_renderTargetBottom);
    Gui::sprite(ui_sheet_emulated_bg_bottom_green, 0, 0);
    Gui::sprite(ui_sheet_bg_style_bottom_idx, 0, 0);
    Gui::sprite(ui_sheet_bar_arc_bottom_green_idx, 0, 206);

    Gui::sprite(ui_sheet_bar_boxname_with_arrows_idx, 7, 15);
    Gui::sprite(ui_sheet_storage_box_corner_idx, 2, 44);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_horizontal_idx, 202, 44);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_vertical_idx, 2, 193);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_both_idx, 202, 193);
    for (Button* b : mainButtons)
    {
        b->draw();
    }

    u16 y = 45;
    for (u8 row = 0; row < 5; row++)
    {
        u16 x = 4;
        for (u8 column = 0; column < 6; column++)
        {
            std::unique_ptr<PKX> pokemon = TitleLoader::save->pkm(boxBox, row * 6 + column);
            if (pokemon->species() > 0)
            {
                Gui::pkm(pokemon.get(), x, y);
            }
            x += 34;
        }
        y += 30;
    }

    Gui::dynamicText(25, 18, 164, TitleLoader::save->boxName(boxBox), FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK);

    if (!storageChosen)
    {
        Gui::sprite(ui_sheet_pointer_arrow_idx, 21 + (cursorIndex % 6) * 34, 30 + (cursorIndex / 6) * 30 + bobPointer());
    }

    if (viewer)
    {
        C2D_DrawRectSolid(0, 0, 0.5f, 320, 240, C2D_Color32(0, 0, 0, 120));
        mainButtons[9]->draw();
        viewer->draw();
    }
    else
    {
        C2D_SceneBegin(g_renderTargetTop);
        Gui::sprite(ui_sheet_emulated_bg_top_green, 0, 0);
        Gui::sprite(ui_sheet_bg_style_top_idx, 0, 0);
        Gui::backgroundAnimated(GFX_TOP);
        Gui::sprite(ui_sheet_bar_arc_top_green_idx, 0, 0);

        Gui::sprite(ui_sheet_textbox_pksm_idx, 261, 3);
        Gui::staticText("PKSM", 394, 7, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, true);

        Gui::sprite(ui_sheet_bar_boxname_empty_idx, 44, 21);
        Gui::staticText(45, 24, 24, "\uE004", FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK);
        Gui::staticText(225, 24, 24, "\uE005", FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK);
        Gui::dynamicText(69, 24, 156, StringUtils::format("Bank %i", storageBox + 1), FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK);

        Gui::sprite(ui_sheet_storagemenu_cross_idx, 36, 50);
        Gui::sprite(ui_sheet_storagemenu_cross_idx, 246, 50);
        Gui::sprite(ui_sheet_storagemenu_cross_idx, 36, 220);
        Gui::sprite(ui_sheet_storagemenu_cross_idx, 246, 220);

        u16 y = 66;
        for (u8 row = 0; row < 5; row++)
        {
            u16 x = 45;
            for (u8 column = 0; column < 6; column++)
            {
                //Gui::pkm(storage->pkm(storageBox, row * 6 + column).get(), x, y);
                Gui::pkm(testPkm.get(), x, y);
                x += 34;
            }
            y += 30;
        }

        Gui::sprite(ui_sheet_stripe_separator_idx, 274, 97);
        Gui::sprite(ui_sheet_stripe_separator_idx, 274, 137);
        Gui::sprite(ui_sheet_stripe_separator_idx, 274, 177);
        Gui::sprite(ui_sheet_point_big_idx, 265, 66);
        Gui::sprite(ui_sheet_point_big_idx, 265, 103);
        Gui::sprite(ui_sheet_point_big_idx, 265, 146);
        Gui::sprite(ui_sheet_point_big_idx, 265, 186);

        if (storageChosen)
        {
            Gui::sprite(ui_sheet_pointer_arrow_idx, 62 + (cursorIndex % 6) * 34, 51 + (cursorIndex / 6) * 30 + bobPointer());
        }

        if (infoMon)
        {
            Gui::dynamicText(infoMon->nickname(), 276, 61, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            std::string info = "#" + std::to_string(infoMon->species());
            Gui::dynamicText(info, 276, 77, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            info = "Lv." + std::to_string(infoMon->level());
            float width = textWidth(info, FONT_SIZE_12);
            Gui::dynamicText(info, 375 - (int) width, 77, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            if (infoMon->gender() == 0)
            {
                Gui::sprite(ui_sheet_icon_male_idx, 358 - (int) width, 80);
            }
            else if (infoMon->gender() == 1)
            {
                Gui::sprite(ui_sheet_icon_female_idx, 360 - (int) width, 80);
            }
            if (infoMon->shiny())
            {
                Gui::sprite(ui_sheet_icon_shiny_idx, 346 - (int) width, 81);
            }

            info = i18n::species(Configuration::getInstance().language(), infoMon->species());
            Gui::dynamicText(info, 276, 98, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            u8 firstType = type1(infoMon->generation(), infoMon->formSpecies());
            u8 secondType = type2(infoMon->generation(), infoMon->formSpecies());
            if (firstType != secondType)
            {
                Gui::type(Configuration::getInstance().language(), firstType, 276, 115);
                Gui::type(Configuration::getInstance().language(), secondType, 325, 115);
            }
            else
            {
                Gui::type(Configuration::getInstance().language(), firstType, 300, 115);
            }

            info = infoMon->otName() + "\nID. " + std::to_string(infoMon->TID());
            Gui::dynamicText(info, 276, 141, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);

            info = i18n::nature(Configuration::getInstance().language(), infoMon->nature());
            Gui::dynamicText(info, 276, 181, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            info = "IV: ";
            width = textWidth(info, FONT_SIZE_12);
            Gui::dynamicText(info, 276, 197, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            info = StringUtils::format("%2i/%2i/%2i", infoMon->iv(0), infoMon->iv(1), infoMon->iv(2));
            Gui::dynamicText(276 + (int) width, 197, 70, info, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
            info = StringUtils::format("%2i/%2i/%2i", infoMon->iv(4), infoMon->iv(5), infoMon->iv(3));
            Gui::dynamicText(276 + (int) width, 209, 70, info, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
        }
    }

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

        if (kDown & KEY_B)
        {
            backButton();
            return;
        }
        else if (kDown & KEY_X)
        {
            showViewer();
            return;
        }
        else if (kDown & KEY_Y)
        {
            Configuration::getInstance().autoBackup(!Configuration::getInstance().autoBackup());
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
            else if (kHeld & KEY_R)
            {
                nextBox();
                sleep = true;
            }
            else if (kHeld & KEY_L)
            {
                lastBox();
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
            storageBox = Configuration::getInstance().storageSize() - 1;
        }
    }
    else
    {
        boxBox--;
        if (boxBox == -1)
        {
            boxBox = TitleLoader::save->maxBoxes() - 1;
        }
    }
    return false;
}

bool StorageScreen::nextBox(bool forceBottom)
{
    if (storageChosen && !forceBottom)
    {
        storageBox++;
        if (storageBox == Configuration::getInstance().storageSize())
        {
            storageBox = 0;
        }
    }
    else
    {
        boxBox++;
        if (boxBox == TitleLoader::save->maxBoxes())
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
    std::shared_ptr<PKX> view = storageChosen ? testPkm : TitleLoader::save->pkm(boxBox, cursorIndex);
    if (view->species() != 0)
    {
        viewer = std::unique_ptr<ViewerScreen>(new ViewerScreen(view, true));
    }
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
                TitleLoader::save->pkm(*TitleLoader::save->emptyPkm(), boxBox, i);
        }
    }
    return false;
}

bool StorageScreen::releasePkm()
{
    backHeld = true;
    if (Gui::showChoiceMessage("Release the selected Pok\xE9mon?"))
    {
        if (storageChosen) { // Storage set all slots in box to PK7()s (or however it happens)
        }
        else
            TitleLoader::save->pkm(*TitleLoader::save->emptyPkm(), boxBox, cursorIndex);
    }
    return false;
}