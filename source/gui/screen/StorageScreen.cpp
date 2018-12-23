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
#include "AccelButton.hpp"
#include "ClickButton.hpp"
#include "SavLGPE.hpp"

static bool backHeld = false;

static u8 type1(Generation generation, u16 species)
{
    switch (generation)
    {
        case Generation::FOUR:
            return PersonalDPPtHGSS::type1(species);
        case Generation::FIVE:
            return PersonalBWB2W2::type1(species);
        case Generation::SIX:
            return PersonalXYORAS::type1(species);
        case Generation::SEVEN:
            return PersonalSMUSUM::type1(species);
        case Generation::LGPE:
        case Generation::UNUSED:
            return 0; //PersonalLGPE::type1(species);
    }
    return 0;
}

static u8 type2(Generation generation, u16 species)
{
    switch (generation)
    {
        case Generation::FOUR:
            return PersonalDPPtHGSS::type2(species);
        case Generation::FIVE:
            return PersonalBWB2W2::type2(species);
        case Generation::SIX:
            return PersonalXYORAS::type2(species);
        case Generation::SEVEN:
            return PersonalSMUSUM::type2(species);
        case Generation::LGPE:
        case Generation::UNUSED:
            return 0; //PersonalLGPE::type2(species);
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
        swkbdSetHintText(&state, i18n::localize("BANK_BOX_NAME").c_str());
        swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
        char input[41] = {0};
        SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
        input[40] = '\0';
        if (ret == SWKBD_BUTTON_CONFIRM)
        {
            bank.boxName(input, storageBox);
        }
    }
    else
    {
        switch (TitleLoader::save->generation())
        {
            case Generation::FOUR:
            case Generation::FIVE:
            {
                static SwkbdState state;
                static bool first = true;
                if (first)
                {
                    swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, 8);
                    first = false;
                }
                swkbdSetHintText(&state, i18n::localize("BOX_NAME").c_str());
                swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
                char input[18] = {0};
                SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
                input[16] = '\0';
                input[17] = '\0';
                if (ret == SWKBD_BUTTON_CONFIRM)
                {
                    TitleLoader::save->boxName(boxBox, input);
                }
            }
            break;
            case Generation::SIX:
            case Generation::SEVEN:
            {
                static SwkbdState state;
                static bool first = true;
                if (first)
                {
                    swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, 16);
                    first = false;
                }
                swkbdSetHintText(&state, i18n::localize("BOX_NAME").c_str());
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
            case Generation::LGPE:
            case Generation::UNUSED:
            // Do nothing
            break;
        }
    }
}

StorageScreen::StorageScreen()
{
    mainButtons[0] = new ClickButton(242, 9, 47, 22, [this](){ return this->swapBoxWithStorage(); }, ui_sheet_button_swap_boxes_idx, "", 0.0f, 0);
    mainButtons[1] = new Button(212, 40, 108, 28, [this](){ return this->showViewer(); }, ui_sheet_button_editor_idx,
                                    i18n::localize("VIEW"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[2] = new Button(212, 71, 108, 28, [this](){ return this->clearBox(); }, ui_sheet_button_editor_idx,
                                    i18n::localize("CLEAR"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[3] = new Button(212, 102, 108, 28, [this](){ return this->releasePkm(); }, ui_sheet_button_editor_idx,
                                    i18n::localize("RELEASE"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[4] = new Button(212, 133, 108, 28, [this](){ return this->dumpPkm(); }, ui_sheet_button_editor_idx,
                                    i18n::localize("DUMP"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[5] = new Button(212, 164, 108, 28, [this](){ return this->duplicate(); }, ui_sheet_button_editor_idx,
                                    i18n::localize("CLONE"), FONT_SIZE_12, COLOR_BLACK);
    //mainButtons[6] = new Button(4, 212, 33, 28, &wirelessStuff, ui_sheet_button_wireless_idx, "", 0.0f, 0);
    mainButtons[6] = new Button(4, 212, 33, 28, [this](){ return false; }, ui_sheet_res_null_idx, "", 0.0f, 0);
    mainButtons[7] = new Button(283, 211, 34, 28, [this](){ return this->backButton(); }, ui_sheet_button_back_idx, "", 0.0f, 0);
    mainButtons[8] = new AccelButton(8, 15, 17, 24, [this](){ return this->lastBox(true); }, ui_sheet_res_null_idx, "", 0.0f, 0, 10, 5);
    mainButtons[9] = new AccelButton(189, 15, 17, 24, [this](){ return this->nextBox(true); }, ui_sheet_res_null_idx, "", 0.0f, 0, 10, 5);

    // Pokemon buttons
    u16 y = 45;
    for (u8 row = 0; row < 5; row++)
    {
        u16 x = 4;
        for (u8 column = 0; column < 6; column++)
        {
            clickButtons[row*6 + column] = new ClickButton(x, y, 34, 30, [this, row, column](){ return this->clickBottomIndex(row*6 + column + 1); }, ui_sheet_res_null_idx, "", 0.0f, 0);
            x += 34;
        }
        y += 30;
    }
    clickButtons[30] = new ClickButton(32, 15, 164, 24, [this](){ return this->clickBottomIndex(0); }, ui_sheet_res_null_idx, "", 0.0f, 0);
    TitleLoader::save->cryptBoxData(true);
}

StorageScreen::~StorageScreen()
{
    for (auto button : mainButtons)
    {
        delete button;
    }
    for (auto button : clickButtons)
    {
        delete button;
    }

    if (TitleLoader::save->generation() == Generation::LGPE)
    {
        ((SavLGPE*)TitleLoader::save.get())->compressBox();
        int occupiedSlots = 0;
        for (int i = 0; i < TitleLoader::save->maxSlot(); i++)
        {
            if (TitleLoader::save->pkm(0, i)->encryptionConstant())
            {
                occupiedSlots++;
            }
            else
            {
                break;
            }
        }
        ((SavLGPE*)TitleLoader::save.get())->boxedPkm(occupiedSlots);
    }
    TitleLoader::save->cryptBoxData(false);
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
            infoMon = storageChosen ? bank.pkm(storageBox, cursorIndex - 1) : TitleLoader::save->pkm(boxBox, cursorIndex - 1);
        }
    }
    if (infoMon && infoMon->encryptionConstant() == 0)
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
            if (TitleLoader::save->generation() == Generation::LGPE && row * 6 + column + boxBox * 30 >= TitleLoader::save->maxSlot())
            {
                C2D_DrawRectSolid(x, y, 0.5f, 34, 30, C2D_Color32(128, 128, 128, 128));
            }
            else
            {
                std::unique_ptr<PKX> pokemon = TitleLoader::save->pkm(boxBox, row * 6 + column);
                if (pokemon->species() > 0)
                {
                    Gui::pkm(pokemon.get(), x, y);
                }
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
        C2D_DrawRectSolid(0, 0, 0.5f, 320, 240, COLOR_MASKBLACK);
        if (!moveMon)
        {
            Gui::staticText(GFX_BOTTOM, 110, i18n::localize("PRESS_TO_CLONE"), FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
        }
        mainButtons[6]->draw();
        viewer->draw();
    }
    else
    {
        C2D_SceneBegin(g_renderTargetTop);
        Gui::sprite(ui_sheet_emulated_bg_top_green, 0, 0);
        Gui::sprite(ui_sheet_bg_style_top_idx, 0, 0);
        Gui::backgroundAnimatedTop();
        Gui::sprite(ui_sheet_bar_arc_top_green_idx, 0, 0);

        Gui::sprite(ui_sheet_textbox_pksm_idx, 261, 3);
        Gui::staticText("PKSM", 394, 7, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, true);

        Gui::sprite(ui_sheet_bar_boxname_empty_idx, 44, 21);
        Gui::staticText(45, 24, 24, "\uE004", FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK);
        Gui::staticText(225, 24, 24, "\uE005", FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK);
        Gui::dynamicText(69, 24, 156, bank.boxName(storageBox), FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK);

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
                auto pkm = bank.pkm(storageBox, row * 6 + column);
                if (pkm->species() > 0)
                {
                    Gui::pkm(pkm.get(), x, y);
                }
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
            info = i18n::localize("LV") + std::to_string(infoMon->level());
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
            if (infoMon->generation() == Generation::FOUR)
            {
                if (firstType > 8)
                    firstType--;
                if (secondType > 8)
                    secondType--;
            }
            if (firstType != secondType)
            {
                Gui::type(Configuration::getInstance().language(), firstType, 276, 115);
                Gui::type(Configuration::getInstance().language(), secondType, 325, 115);
            }
            else
            {
                Gui::type(Configuration::getInstance().language(), firstType, 300, 115);
            }

            info = infoMon->otName() + '\n' + i18n::localize("LOADER_ID") + std::to_string(infoMon->TID());
            Gui::dynamicText(info, 276, 141, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);

            info = i18n::nature(Configuration::getInstance().language(), infoMon->nature());
            Gui::dynamicText(info, 276, 181, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            info = i18n::localize("IV") + ": ";
            width = textWidth(info, FONT_SIZE_12);
            Gui::dynamicText(info, 276, 197, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            info = StringUtils::format("%2i/%2i/%2i", infoMon->iv(0), infoMon->iv(1), infoMon->iv(2));
            Gui::dynamicText(276 + (int) width, 197, 70, info, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
            info = StringUtils::format("%2i/%2i/%2i", infoMon->iv(4), infoMon->iv(5), infoMon->iv(3));
            Gui::dynamicText(276 + (int) width, 209, 70, info, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
            Gui::format(infoMon.get(), 276, 210);
        }
    }
}

void StorageScreen::update(touchPosition* touch)
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
    static bool sleep = true;
    u32 kDown = hidKeysDown();
    u32 kHeld = hidKeysHeld();
    if (!viewer)
    {
        for (size_t i = 0; i < mainButtons.size(); i++)
        {
            if (mainButtons[i]->update(touch))
                return;
        }
        backHeld = false;
        for (Button* button : clickButtons)
        {
            if (button->update(touch))
                return;
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
            //wirelessStuff();
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
        if (kDown & KEY_B || mainButtons[6]->update(touch))
        {
            backButton();
        }
        else if (!moveMon && kDown & KEY_X)
        {
            moveMon = viewer->getPkm()->clone();
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
            Gui::warn(i18n::localize("BANK_FAILED_EXIT"));
        }
        else
        {
            if (Gui::showChoiceMessage(i18n::localize("BANK_SAVE_CHANGES")))
            {
                bank.save();
            }
            Gui::screenBack();
        }
    }
    return true;
}

bool StorageScreen::showViewer()
{
    if (cursorIndex == 0 || (!storageChosen && boxBox * 30 + cursorIndex - 1 >= TitleLoader::save->maxSlot()))
    {
        return false;
    }
    std::shared_ptr<PKX> view = storageChosen ? bank.pkm(storageBox, cursorIndex - 1) : TitleLoader::save->pkm(boxBox, cursorIndex - 1);
    if (view->species() != 0)
    {
        viewer = std::make_unique<ViewerScreen>(view, true);
    }
    return true;
}

bool StorageScreen::clearBox()
{
    backHeld = true;
    if (Gui::showChoiceMessage(i18n::localize("BANK_CONFIRM_CLEAR")))
    {
        for (int i = 0; i < 30; i++)
        {
            if (storageChosen)
            {
                bank.pkm(*TitleLoader::save->emptyPkm(), storageBox, i);
            }
            else if (boxBox * 30 + cursorIndex - 1 < TitleLoader::save->maxSlot())
            {
                TitleLoader::save->pkm(*TitleLoader::save->emptyPkm(), boxBox, i);
            }
        }
    }
    return false;
}

bool StorageScreen::releasePkm()
{
    backHeld = true;
    if (Gui::showChoiceMessage(i18n::localize("BANK_CONFIRM_RELEASE")))
    {
        if (storageChosen)
        {
            bank.pkm(*TitleLoader::save->emptyPkm(), storageBox, cursorIndex - 1);
        }
        else if (boxBox * 30 + cursorIndex - 1 < TitleLoader::save->maxSlot())
        {
            TitleLoader::save->pkm(*TitleLoader::save->emptyPkm(), boxBox, cursorIndex - 1);
            if (TitleLoader::save->generation() == Generation::LGPE)
            {
                SavLGPE* sav = (SavLGPE*)TitleLoader::save.get();
                for (int i = 0; i < sav->partyCount(); i++)
                {
                    if (sav->partyBoxSlot(i) == boxBox * 30 + cursorIndex - 1)
                    {
                        sav->partyBoxSlot(i, 1001);
                        sav->fixParty();
                    }
                }
            }
        }
    }
    return false;
}

static void regionChange(PK6* pkm)
{
    if (pkm->htName() != "" && (TitleLoader::save->country() != pkm->geoCountry(0) || TitleLoader::save->subRegion() != pkm->geoRegion(0)))
    {
        for (u8 i = 4; i > 0; i--)
        {
            pkm->geoCountry(i, pkm->geoCountry(i-1));
            pkm->geoRegion(i, pkm->geoRegion(i-1));
        }
        pkm->geoCountry(0, TitleLoader::save->country());
        pkm->geoRegion(0, TitleLoader::save->subRegion());
    }
}

static void regionChange(PK7* pkm)
{
    if (pkm->htName() != "" && (TitleLoader::save->country() != pkm->geoCountry(0) || TitleLoader::save->subRegion() != pkm->geoRegion(0)))
    {
        for (u8 i = 4; i > 0; i--)
        {
            pkm->geoCountry(i, pkm->geoCountry(i-1));
            pkm->geoRegion(i, pkm->geoRegion(i-1));
        }
        pkm->geoCountry(0, TitleLoader::save->country());
        pkm->geoRegion(0, TitleLoader::save->subRegion());
    }
}

static void memoryChange(PK6* pkm)
{
    static const int allowedFeelings = 0x04CBFD;
    pkm->htMemory(4);
    pkm->htTextVar(0);
    pkm->htIntensity(1);
    int feel = 20; // Too high of a value, the next condition will always be false with it
    while ((allowedFeelings & (1 << feel)) == 0)
    {
        feel = rand() % 10;
    }
    pkm->htFeeling(feel);
}

static void memoryChange(PK7* pkm)
{
    static const int allowedFeelings = 0x04CBFD;
    pkm->htMemory(4);
    pkm->htTextVar(0);
    pkm->htIntensity(1);
    int feel = 20; // Too high of a value, the next condition will always be false with it
    while ((allowedFeelings & (1 << feel)) == 0)
    {
        feel = rand() % 10;
    }
    pkm->htFeeling(feel);
}

void StorageScreen::pickup()
{
    if (!moveMon)
    {
        if (storageChosen)
        {
            moveMon = bank.pkm(storageBox, cursorIndex - 1);
        }
        else if (boxBox * 30 + cursorIndex - 1 < TitleLoader::save->maxSlot())
        {
            if (TitleLoader::save->generation() == Generation::LGPE)
            {
                SavLGPE* sav = (SavLGPE*)TitleLoader::save.get();
                partyNum = -1;
                for (int i = 0; i < TitleLoader::save->partyCount(); i++)
                {
                    if (sav->partyBoxSlot(i) == boxBox * 30 + cursorIndex - 1)
                    {
                        partyNum = i;
                        break;
                    }
                }
            }
            moveMon = TitleLoader::save->pkm(boxBox, cursorIndex - 1);
        }
        else
        {
            return;
        }

        if (storageChosen)
        {
            bank.pkm(*TitleLoader::save->emptyPkm(), storageBox, cursorIndex - 1);
            fromStorage = true;
        }
        else
        {
            TitleLoader::save->pkm(*TitleLoader::save->emptyPkm(), boxBox, cursorIndex - 1);
            fromStorage = false;
        }

        if (moveMon->encryptionConstant() == 0)
        {
            moveMon = nullptr;
            partyNum = -1;
        }
    }
    else
    {
        if (storageChosen)
        {
            std::shared_ptr<PKX> temPkm = bank.pkm(storageBox, cursorIndex - 1);
            bank.pkm(*moveMon, storageBox, cursorIndex - 1);
            moveMon = temPkm;
            
            if (moveMon && moveMon->species() > 0)
            {
                fromStorage = true;
            }
            else
            {
                moveMon = nullptr;
                fromStorage = false;
            }
        }
        else if (boxBox * 30 + cursorIndex - 1 < TitleLoader::save->maxSlot())
        {
            u8 (*formCounter)(u16);
            switch (TitleLoader::save->generation())
            {
                case Generation::FOUR:
                    formCounter = PersonalDPPtHGSS::formCount;
                    break;
                case Generation::FIVE:
                    formCounter = PersonalBWB2W2::formCount;
                    break;
                case Generation::SIX:
                    formCounter = PersonalXYORAS::formCount;
                    break;
                case Generation::SEVEN:
                default:
                    formCounter = PersonalSMUSUM::formCount;
                    break;
            }
            bool moveBad = false;
            for (int i = 0; i < 4; i++)
            {
                if (moveMon->move(i) > TitleLoader::save->maxMove())
                {
                    moveBad = true;
                    break;
                }
                if (moveMon->generation() == Generation::SIX)
                {
                    PK6* pk6 = (PK6*) moveMon.get();
                    if (pk6->relearnMove(i) > TitleLoader::save->maxMove())
                    {
                        moveBad = true;
                        break;
                    }
                }
                else if (moveMon->generation() == Generation::SEVEN)
                {
                    PK7* pk7 = (PK7*) moveMon.get();
                    if (pk7->relearnMove(i) > TitleLoader::save->maxMove())
                    {
                        moveBad = true;
                        break;
                    }
                }
            }
            if (moveMon->species() > TitleLoader::save->maxSpecies())
            {
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_SPECIES"));
                return;
            }
            else if (moveMon->alternativeForm() > formCounter(moveMon->species()))
            {
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_FORM"));
                return;
            }
            else if (moveMon->ability() > TitleLoader::save->maxAbility())
            {
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_ABILITY"));
                return;
            }
            else if (moveMon->heldItem() > TitleLoader::save->maxItem())
            {
                
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_ITEM"));
                return;
            }
            else if (moveMon->ball() > TitleLoader::save->maxBall())
            {
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_BALL"));
                return;
            }
            else if (moveBad)
            {
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_MOVE"));
                return;
            }
            std::shared_ptr<PKX> temPkm = TitleLoader::save->pkm(boxBox, cursorIndex - 1);
            if ((Configuration::getInstance().transferEdit() || moveMon->generation() == TitleLoader::save->generation()) || Gui::showChoiceMessage(StringUtils::format(i18n::localize("GEN_CHANGE_1"), genToString(moveMon->generation()).c_str(), genToString(TitleLoader::save->generation()).c_str()), i18n::localize("GEN_CHANGE_2")))
            {
                while (moveMon->generation() != TitleLoader::save->generation())
                {
                    if (moveMon->generation() > TitleLoader::save->generation())
                    {
                        moveMon = moveMon->previous();
                    }
                    else
                    {
                        moveMon = moveMon->next();
                    }
                }
                if (Configuration::getInstance().transferEdit() && fromStorage)
                {
                    if (TitleLoader::save->otName() == moveMon->otName() && TitleLoader::save->TID() == moveMon->TID() && TitleLoader::save->SID() == moveMon->SID() && TitleLoader::save->gender() == moveMon->otGender())
                    {
                        if (moveMon->generation() == Generation::SIX)
                        {
                            PK6* movePkm = (PK6*)moveMon.get();
                            movePkm->currentHandler(0);
                            regionChange(movePkm);
                        }
                        else if (moveMon->generation() == Generation::SEVEN)
                        {
                            PK7* movePkm = (PK7*)moveMon.get();
                            movePkm->currentHandler(0);
                            regionChange(movePkm);
                        }
                    }
                    else
                    {
                        if (moveMon->generation() == Generation::SIX)
                        {
                            PK6* movePkm = (PK6*)moveMon.get();
                            movePkm->currentHandler(1);
                            regionChange(movePkm);
                            movePkm->htName(TitleLoader::save->otName().c_str());
                            movePkm->htGender(TitleLoader::save->gender());
                            if (movePkm->htMemory() == 0)
                            {
                                memoryChange(movePkm);
                            }
                        }
                        else if (moveMon->generation() == Generation::SEVEN)
                        {
                            PK7* movePkm = (PK7*)moveMon.get();
                            movePkm->currentHandler(1);
                            regionChange(movePkm);
                            movePkm->htName(TitleLoader::save->otName().c_str());
                            movePkm->htGender(TitleLoader::save->gender());
                            if (movePkm->htMemory() == 0)
                            {
                                memoryChange(movePkm);
                            }
                        }
                    }
                }
                TitleLoader::save->pkm(*moveMon, boxBox, cursorIndex - 1);
                TitleLoader::save->dex(*moveMon);
                if (partyNum != -1)
                {
                    ((SavLGPE*)TitleLoader::save.get())->partyBoxSlot(partyNum, boxBox * 30 + cursorIndex - 1);
                }
                if (temPkm->encryptionConstant() == 0)
                {
                    moveMon = nullptr;
                }
                else
                {
                    moveMon = temPkm;
                }
            }
            fromStorage = false;
        }
    }
}

bool StorageScreen::dumpPkm()
{
    if (Gui::showChoiceMessage(i18n::localize("BANK_CONFIRM_DUMP")))
    {
        char stringDate[11] = {0};
        char stringTime[10] = {0};
        time_t unixTime = time(NULL);
        struct tm* timeStruct = gmtime((const time_t *)&unixTime);
        std::strftime(stringDate, 10,"%Y-%m-%d", timeStruct);
        std::strftime(stringTime, 9,"/%H-%M-%S", timeStruct);
        std::string path = std::string("/3ds/PKSM/dumps/") + stringDate;
        mkdir(path.c_str(), 777);
        path += stringTime;
        if (moveMon)
        {
            path += moveMon->generation() != Generation::LGPE ? ".pk" + genToString(moveMon->generation()) : ".pb7";
            FSStream out(Archive::sd(), StringUtils::UTF8toUTF16(path), FS_OPEN_CREATE | FS_OPEN_WRITE, moveMon->length);
            if (out.good())
            {
                out.write(moveMon->rawData(), moveMon->length);
            }
            else
            {
                Gui::error(i18n::localize("FAILED_OPEN_DUMP"), out.result());
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
                    if (boxBox * 30 + cursorIndex - 1 >= TitleLoader::save->maxSlot())
                    {
                        return false;
                    }
                    dumpMon = TitleLoader::save->pkm(boxBox, cursorIndex - 1);
                    path += dumpMon->generation() != Generation::LGPE ? ".pk" + genToString(dumpMon->generation()) : ".pb7";
                    FSStream out(Archive::sd(), StringUtils::UTF8toUTF16(path), FS_OPEN_CREATE | FS_OPEN_WRITE, dumpMon->length);
                    if (out.good())
                    {
                        out.write(dumpMon->rawData(), dumpMon->length);
                    }
                    else
                    {
                        Gui::error(i18n::localize("FAILED_OPEN_DUMP"), out.result());
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
                    dumpMon = bank.pkm(storageBox, cursorIndex - 1);
                    path += dumpMon->generation() != Generation::LGPE ? ".pk" + genToString(dumpMon->generation()) : ".pb7";
                    FSStream out(Archive::sd(), StringUtils::UTF8toUTF16(path), FS_OPEN_CREATE | FS_OPEN_WRITE, dumpMon->length);
                    if (out.good())
                    {
                        out.write(dumpMon->rawData(), dumpMon->length);
                    }
                    else
                    {
                        Gui::error(i18n::localize("FAILED_OPEN_DUMP"), out.result());
                    }
                    out.close();
                }
            }
        }
        return true;
    }
    return false;
}

bool StorageScreen::duplicate()
{
    if (!moveMon && cursorIndex > 0)
    {
        if (storageChosen)
        {
            moveMon = bank.pkm(storageBox, cursorIndex - 1);
        }
        else
        {
            if (boxBox * 30 + cursorIndex - 1 >= TitleLoader::save->maxSlot())
            {
                return false;
            }
            moveMon = TitleLoader::save->pkm(boxBox, cursorIndex - 1)->clone();
        }
        if (moveMon->encryptionConstant() == 0)
        {
            moveMon = nullptr;
        }
    }
    return false;
}

bool StorageScreen::swapBoxWithStorage()
{
    for (int i = 0; i < 30; i++)
    {
        if (boxBox * 30 + i >= TitleLoader::save->maxSlot())
        {
            break;
        }
        std::shared_ptr<PKX> temPkm = bank.pkm(storageBox, i);
        if (temPkm->encryptionConstant() == 0)
        {
            temPkm = TitleLoader::save->emptyPkm();
        }
        if ((Configuration::getInstance().transferEdit() || temPkm->generation() == TitleLoader::save->generation()) || Gui::showChoiceMessage(StringUtils::format(i18n::localize("GEN_CHANGE_1"), genToString(moveMon->generation()).c_str(), genToString(TitleLoader::save->generation()).c_str()), i18n::localize("GEN_CHANGE_2")))
        {
            while (temPkm->generation() != TitleLoader::save->generation())
            {
                if (temPkm->generation() > TitleLoader::save->generation())
                {
                    temPkm = temPkm->previous();
                }
                else
                {
                    temPkm = temPkm->next();
                }
            }
            auto otherTemPkm = TitleLoader::save->pkm(boxBox, i);
            TitleLoader::save->pkm(*temPkm, boxBox, i);
            bank.pkm(*otherTemPkm, storageBox, i);
        }
    }
    return false;
}