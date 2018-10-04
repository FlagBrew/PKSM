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
#include "FSStream.hpp"

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

void StorageScreen::setBoxName(bool storage)
{
    if (storage)
    {
        static SwkbdState state;
        static bool first = true;
        if (first)
        {
            swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, 20);
            first = false;
        }
        swkbdSetHintText(&state, "Bank Box Name");
        swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
        char input[41] = {0};
        SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
        input[40] = '\0';
        if (ret == SWKBD_BUTTON_CONFIRM)
        {
            //storage->setBoxName(storageBox);
        }
    }
    else
    {
        switch (TitleLoader::save->generation())
        {
            case 4:
            case 5:
            {
                static SwkbdState state;
                static bool first = true;
                if (first)
                {
                    swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, 19);
                    first = false;
                }
                swkbdSetHintText(&state, "Box Name");
                swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
                char input[40] = {0};
                SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
                input[38] = '\0';
                input[39] = '\0';
                if (ret == SWKBD_BUTTON_CONFIRM)
                {
                    TitleLoader::save->boxName(boxBox, input);
                }
            }
            break;
            case 6:
            case 7:
            {
                static SwkbdState state;
                static bool first = true;
                if (first)
                {
                    swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, 16);
                    first = false;
                }
                swkbdSetHintText(&state, "Box Name");
                swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
                char input[34] = {0};
                SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
                input[32] = '\0';
                input[33] = '\0';
                if (ret == SWKBD_BUTTON_CONFIRM)
                {
                    TitleLoader::save->boxName(boxBox, input);
                }
            }
            break;
        }
    }
}

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
    mainButtons[7] = new Button(4, 212, 33, 28, &wirelessStuff, ui_sheet_button_wireless_idx, "", 0.0f, 0);
    mainButtons[8] = new Button(283, 211, 34, 28, std::bind(&StorageScreen::backButton, this), ui_sheet_button_back_idx, "", 0.0f, 0);

    // Pokemon buttons
    u16 y = 45;
    for (u8 row = 0; row < 5; row++)
    {
        u16 x = 4;
        for (u8 column = 0; column < 6; column++)
        {
            clickButtons[row*6 + column] = new Button(x, y, 34, 30, std::bind(&StorageScreen::clickBottomIndex, this, row*6 + column + 1), ui_sheet_res_null_idx, "", 0.0f, 0);
            x += 34;
        }
        y += 30;
    }
    clickButtons[30] = new Button(32, 15, 164, 24, std::bind(&StorageScreen::clickBottomIndex, this, 0), ui_sheet_res_null_idx, "", 0.0f, 0);
    TitleLoader::save->cryptBoxData(true);
}

void StorageScreen::draw() const
{
    std::shared_ptr<PKX> infoMon = nullptr;
    if (moveMon)
    {
        infoMon = moveMon;
    }
    else
    {
        if (cursorIndex != 0)
        {
            infoMon = storageChosen ? /*storage->pkm(storageBox, cursorIndex)*/ testPkm : TitleLoader::save->pkm(boxBox, cursorIndex - 1);
        }
    }
    if (infoMon && infoMon->species() == 0)
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
        if (cursorIndex == 0)
        {
            int dy = bobPointer();
            if (moveMon)
            {
                Gui::pkm(moveMon.get(), 94, 5 + dy);
            }
            Gui::sprite(ui_sheet_pointer_arrow_idx, 106, -4 + dy);
        }
        else
        {
            int tempIndex = cursorIndex - 1;
            int yMod = (tempIndex / 6) * 30 + bobPointer();
            if (moveMon)
            {
                Gui::pkm(moveMon.get(), 9 + (tempIndex % 6) * 34, 39 + yMod);
            }
            Gui::sprite(ui_sheet_pointer_arrow_idx, 21 + (tempIndex % 6) * 34, 30 + yMod);
        }
    }

    if (viewer)
    {
        C2D_DrawRectSolid(0, 0, 0.5f, 320, 240, C2D_Color32(0, 0, 0, 120));
        mainButtons[8]->draw();
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
            if (cursorIndex == 0)
            {
                int dy = bobPointer();
                if (moveMon)
                {
                    Gui::pkm(moveMon.get(), 135, 11 + dy);
                }
                Gui::sprite(ui_sheet_pointer_arrow_idx, 147, 2 + dy);
            }
            else
            {
                int tempIndex = cursorIndex - 1;
                int yMod = (tempIndex / 6) * 30 + bobPointer();
                if (moveMon)
                {
                    Gui::pkm(moveMon.get(), 50 + (tempIndex % 6) * 34, 60 + yMod);
                }
                Gui::sprite(ui_sheet_pointer_arrow_idx, 62 + (tempIndex % 6) * 34, 51 + yMod);
            }
        }

        if (infoMon)
        {
            Gui::dynamicText(infoMon->nickname(), 276, 61, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            std::string info = "#" + std::to_string(infoMon->species());
            Gui::dynamicText(info, 273, 77, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            info = "Lv." + std::to_string(infoMon->level());
            float width = textWidth(info, FONT_SIZE_12);
            Gui::dynamicText(info, 375 - (int) width, 77, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            if (infoMon->gender() == 0)
            {
                Gui::sprite(ui_sheet_icon_male_idx, 362 - (int) width, 80);
            }
            else if (infoMon->gender() == 1)
            {
                Gui::sprite(ui_sheet_icon_female_idx, 364 - (int) width, 80);
            }
            if (infoMon->shiny())
            {
                Gui::sprite(ui_sheet_icon_shiny_idx, 352 - (int) width, 81);
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
}

void StorageScreen::update(touchPosition* touch)
{
    Screen::update();
    static bool sleep = true;
    if (!viewer)
    {
        u32 kDown = hidKeysDown();
        u32 kHeld = hidKeysHeld();

        for (Button* button : mainButtons)
        {
            if (button->update(touch))
                return;
        }
        backHeld = false;
        // prevents double pressing
        if (kDown & KEY_TOUCH)
        {
            for (Button* button : clickButtons)
            {
                if (button->update(touch))
                    return;
            }
        }

        static int buttonCooldown = 10;

        if (kDown & KEY_A)
        {
            if (cursorIndex == 0)
            {
                if (!moveMon)
                {
                    Gui::setNextKeyboardFunc(std::bind(&StorageScreen::setBoxName, this, storageChosen));
                }
            }
            else
            {
                pickup();
            }
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
        else if (kDown & KEY_Y)
        {
            wirelessStuff();
            return;
        }
        else if (buttonCooldown <= 0)
        {
            sleep = false;
            if (kHeld & KEY_LEFT)
            {
                if (cursorIndex == 0)
                {
                    lastBox();
                }
                else if (cursorIndex > 1) 
                {
                    cursorIndex--;
                }
                else if (cursorIndex == 1)
                {
                    lastBox();
                    cursorIndex = 30;
                }
                sleep = true;
            }
            else if (kHeld & KEY_RIGHT)
            {
                if (cursorIndex == 0)
                {
                    nextBox();
                }
                else if (cursorIndex < 30)
                {
                    cursorIndex++;
                }
                else if (cursorIndex == 30)
                {
                    nextBox();
                    cursorIndex = 1;
                }
                sleep = true;
            }
            else if (kHeld & KEY_UP)
            {
                if (cursorIndex == 0 && !storageChosen)
                {
                    storageChosen = true;
                    cursorIndex = 27;
                }
                else if (cursorIndex > 0 && cursorIndex <= 6)
                {
                    cursorIndex = 0;
                }
                else if (cursorIndex > 6)
                {			
                    cursorIndex -= 6;
                }
                sleep = true;
            }
            else if (kHeld & KEY_DOWN)
            {
                if (cursorIndex >= 25 && storageChosen)
                {
                    storageChosen = false;
                    cursorIndex = 0;
                }
                else if (cursorIndex == 0)
                {
                    cursorIndex = 3;
                }
                else if (cursorIndex < 25)
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
        if (hidKeysDown() & KEY_B || mainButtons[8]->update(touch))
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

bool StorageScreen::clickBottomIndex(int index)
{
    if (cursorIndex == index && !storageChosen)
    {
        if (cursorIndex == 0)
        {
            if (!moveMon)
            {
                Gui::setNextKeyboardFunc(std::bind(&StorageScreen::setBoxName, this, false));
                return true;
            }
        }
        else
        {
            pickup();
        }
    }
    else
    {
        cursorIndex = index;
        storageChosen = false;
    }
    return false;
}

bool StorageScreen::backButton()
{
    if (!backHeld)
    {
        backHeld = true;
        if (viewer)
        {
            viewer = nullptr;
        }
        else if (moveMon)
        {
            Gui::warn("Exiting is not allowed when a Pok\u00E9mon is held!");
        }
        else
        {
            Gui::screenBack();
        }
    }
    return true;
}

bool StorageScreen::showViewer()
{
    std::shared_ptr<PKX> view = cursorIndex == 0 ? TitleLoader::save->emptyPkm() : storageChosen ? testPkm : TitleLoader::save->pkm(boxBox, cursorIndex - 1);
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
    if (Gui::showChoiceMessage("Release the selected Pok\u00E9mon?"))
    {
        if (storageChosen) { // Storage set all slots in box to PK7()s (or however it happens)
        }
        else
            TitleLoader::save->pkm(*TitleLoader::save->emptyPkm(), boxBox, cursorIndex);
    }
    return false;
}

void StorageScreen::pickup()
{
    if (!moveMon)
    {
        moveMon = storageChosen ? /*storage->pkm(storageBox, cursorIndex)*/ testPkm : TitleLoader::save->pkm(boxBox, cursorIndex - 1);
        if (storageChosen)
        {
            // set storage pkm to emptyPKM
        }
        else
        {
            TitleLoader::save->pkm(*TitleLoader::save->emptyPkm(), boxBox, cursorIndex - 1);
        }
    }
    else
    {
        if (storageChosen)
        {
            // storage implementation->pkm(storageBox, cursorIndex, moveMon);
            // moveMon = nullptr;
        }
        else
        {
            std::shared_ptr<PKX> temPkm = TitleLoader::save->pkm(boxBox, cursorIndex - 1);
            TitleLoader::save->pkm(*moveMon, boxBox, cursorIndex - 1);
            if (temPkm->species() == 0)
            {
                moveMon = nullptr;
            }
            else
            {
                moveMon = temPkm;
            }
        }
    }
}

bool StorageScreen::dumpPkm()
{
    if (Gui::showChoiceMessage("Dump selected Pok\u00E9mon?"))
    {
        char stringDate[9] = {0};
        char stringTime[8] = {0};
        time_t unixTime = time(NULL);
        struct tm* timeStruct = gmtime((const time_t *)&unixTime);
        std::strftime(stringDate, 8,"%Y%m%d", timeStruct);
        std::strftime(stringTime, 7,"/%H%M%S", timeStruct);
        std::string path = std::string("/3ds/PKSM/dumps/") + stringDate;
        mkdir(path.c_str(), 777);
        path += stringTime;
        if (moveMon)
        {
            path += StringUtils::format(".pk%d", moveMon->generation());
            FSStream out(Archive::sd(), StringUtils::UTF8toUTF16(path), FS_OPEN_CREATE | FS_OPEN_WRITE, moveMon->length);
            if (out.good())
            {
                out.write(moveMon->rawData(), moveMon->length);
            }
            else
            {
                Gui::warn("Could not open file for dump!");
            }
            out.close();
        }
        else
        {
            std::shared_ptr<PKX> dumpMon;
            if (!storageChosen)
            {
                if (cursorIndex == 0)
                {
                    return false;
                }
                else
                {
                    dumpMon = TitleLoader::save->pkm(boxBox, cursorIndex - 1);
                    path += StringUtils::format(".pk%d", dumpMon->generation());
                    FSStream out(Archive::sd(), StringUtils::UTF8toUTF16(path), FS_OPEN_CREATE | FS_OPEN_WRITE, TitleLoader::save->emptyPkm()->length);
                    if (out.good())
                    {
                        out.write(dumpMon->rawData(), dumpMon->length);
                    }
                    else
                    {
                        Gui::warn("Could not open file for dump!");
                    }
                    out.close();
                }
            }
            else
            {
                if (cursorIndex == 0)
                {
                    return false;
                }
                else
                {
                    dumpMon = testPkm; // Storage implementation thingy
                    path += StringUtils::format(".pk%d", dumpMon->generation());
                    FSStream out(Archive::sd(), StringUtils::UTF8toUTF16(path), FS_OPEN_CREATE | FS_OPEN_WRITE, TitleLoader::save->emptyPkm()->length);
                    if (out.good())
                    {
                        out.write(dumpMon->rawData(), dumpMon->length);
                    }
                    else
                    {
                        Gui::warn("Could not open file for dump!");
                    }
                    out.close();
                }
            }
        }
        return true;
    }
    return false;
}