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

#include "StorageScreen.hpp"
#include "AccelButton.hpp"
#include "BankSelectionScreen.hpp"
#include "BoxOverlay.hpp"
#include "ClickButton.hpp"
#include "Configuration.hpp"
#include "FSStream.hpp"
#include "MainMenu.hpp"
#include "PK4.hpp"
#include "QRScanner.hpp"
#include "SavLGPE.hpp"
#include "SortOverlay.hpp"
#include "StorageOverlay.hpp"
#include "TitleLoadScreen.hpp"
#include "ViewCloneOverlay.hpp"
#include "banks.hpp"
#include "fetch.hpp"
#include <PB7.hpp>
#include <variant>

extern "C" {
#include "base64.h"
}

extern std::stack<std::unique_ptr<Screen>> screens;

static bool backHeld = false;

int bobPointer()
{
    static int currentBob = 0;
    static bool up        = true;
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
        char input[41]  = {0};
        SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
        input[40]       = '\0';
        if (ret == SWKBD_BUTTON_CONFIRM)
        {
            Banks::bank->boxName(input, storageBox);
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
                char input[18]  = {0};
                SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
                input[16]       = '\0';
                input[17]       = '\0';
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
                char input[34]  = {0};
                SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
                input[32]       = '\0';
                input[33]       = '\0';
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
    : Screen(i18n::localize("A_PICKUP") + '\n' + i18n::localize("X_SHARE") + '\n' + i18n::localize("Y_CURSOR_MODE") + '\n' +
             i18n::localize("L_BOX_PREV") + '\n' + i18n::localize("R_BOX_NEXT") + '\n' + i18n::localize("START_EXTRA_FUNC") + '\n' +
             i18n::localize("B_BACK"))
{
    instructions.addBox(true, 69, 21, 156, 24, COLOR_GREY, i18n::localize("A_BOX_NAME"), COLOR_WHITE);
    instructions.addCircle(false, 266, 23, 11, COLOR_GREY);
    instructions.addBox(false, 264, 23, 4, 50, COLOR_GREY);
    instructions.addBox(false, 148, 57, 120, 16, COLOR_GREY, i18n::localize("BOX_SWAP"), COLOR_WHITE);
    mainButtons[0] =
        std::make_unique<ClickButton>(242, 12, 47, 22, [this]() { return this->swapBoxWithStorage(); }, ui_sheet_button_swap_boxes_idx, "", 0.0f, 0);
    mainButtons[1] = std::make_unique<Button>(
        212, 47, 108, 28, [this]() { return this->showViewer(); }, ui_sheet_button_editor_idx, i18n::localize("VIEW"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[2] = std::make_unique<Button>(
        212, 78, 108, 28, [this]() { return this->clearBox(); }, ui_sheet_button_editor_idx, i18n::localize("CLEAR"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[3] = std::make_unique<Button>(
        212, 109, 108, 28, [this]() { return this->releasePkm(); }, ui_sheet_button_editor_idx, i18n::localize("RELEASE"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[4] = std::make_unique<Button>(
        212, 140, 108, 28, [this]() { return this->dumpPkm(); }, ui_sheet_button_editor_idx, i18n::localize("DUMP"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[5] = std::make_unique<Button>(
        212, 171, 108, 28, [this]() { return this->duplicate(); }, ui_sheet_button_editor_idx, i18n::localize("CLONE"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[6] = std::make_unique<Button>(283, 211, 34, 28, [this]() { return this->backButton(); }, ui_sheet_button_back_idx, "", 0.0f, 0);
    mainButtons[7] =
        std::make_unique<AccelButton>(8, 15, 17, 24, [this]() { return this->prevBox(true); }, ui_sheet_res_null_idx, "", 0.0f, 0, 10, 5);
    mainButtons[8] =
        std::make_unique<AccelButton>(189, 15, 17, 24, [this]() { return this->nextBox(true); }, ui_sheet_res_null_idx, "", 0.0f, 0, 10, 5);

    instructions.addCircle(false, 17, 225, 8, COLOR_GREY);
    instructions.addBox(false, 15, 175, 4, 50, COLOR_GREY);
    instructions.addBox(false, 15, 175, 120, 18, COLOR_GREY, i18n::localize("SHARE_HINT"), COLOR_WHITE);
    mainButtons[9] = std::make_unique<ClickButton>(3, 211, 28, 28,
        [this]() {
            if (!infoMon)
            {
                if (!Gui::showChoiceMessage(i18n::localize("SHARE_CODE_ENTER_PROMPT")))
                {
                    return false;
                }
                Gui::setNextKeyboardFunc([this]() { this->shareReceive(); });
            }
            else
            {
                if (!Gui::showChoiceMessage(i18n::localize("SHARE_SEND_CONFIRM")))
                {
                    return false;
                }
                shareSend();
            }
            return true;
        },
        ui_sheet_button_wireless_no_y_idx, "", 0.0f, 0);

    // Pokemon buttons
    u16 y = 45;
    for (u8 row = 0; row < 5; row++)
    {
        u16 x = 4;
        for (u8 column = 0; column < 6; column++)
        {
            clickButtons[row * 6 + column] = std::make_unique<ClickButton>(
                x, y, 34, 30, [this, row, column]() { return this->clickBottomIndex(row * 6 + column + 1); }, ui_sheet_res_null_idx, "", 0.0f, 0);
            x += 34;
        }
        y += 30;
    }
    instructions.addBox(false, 25, 15, 164, 24, COLOR_GREY, i18n::localize("A_BOX_NAME"), COLOR_WHITE);
    clickButtons[30] =
        std::make_unique<ClickButton>(25, 15, 164, 24, [this]() { return this->clickBottomIndex(0); }, ui_sheet_res_null_idx, "", 0.0f, 0);
    TitleLoader::save->cryptBoxData(true);

    boxBox = TitleLoader::save->currentBox();
}

StorageScreen::~StorageScreen()
{
    if (TitleLoader::save->generation() == Generation::LGPE)
    {
        ((SavLGPE*)TitleLoader::save.get())->compressBox();
        int occupiedSlots = 0;
        for (int i = 0; i < TitleLoader::save->maxSlot(); i++)
        {
            if (TitleLoader::save->pkm(u8(0), i)->encryptionConstant())
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
    TitleLoader::save->currentBox((u8)boxBox);
}

void StorageScreen::drawBottom() const
{
    Gui::sprite(ui_sheet_emulated_bg_bottom_green, 0, 0);
    Gui::sprite(ui_sheet_bg_style_bottom_idx, 0, 0);
    Gui::sprite(ui_sheet_bar_arc_bottom_green_idx, 0, 206);

    Gui::sprite(ui_sheet_bar_boxname_with_arrows_idx, 7, 15);
    Gui::sprite(ui_sheet_storage_box_corner_idx, 2, 44);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_horizontal_idx, 202, 44);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_vertical_idx, 2, 193);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_both_idx, 202, 193);
    for (auto& b : mainButtons)
    {
        b->draw();
    }

    u16 partyPkm[6];
    std::fill_n(partyPkm, 6, 1001);
    if (TitleLoader::save->generation() == Generation::LGPE)
    {
        for (int i = 0; i < 6; i++)
        {
            partyPkm[i] = ((SavLGPE*)TitleLoader::save.get())->partyBoxSlot(i);
        }
    }

    u16 y = 45;
    for (u8 row = 0; row < 5; row++)
    {
        u16 x = 4;
        for (u8 column = 0; column < 6; column++)
        {
            // C2D_Color32(0x50, 0xF0, 0x40, 0x80);
            if (currentlySelecting && !storageChosen && column <= std::max((cursorIndex - 1) % 6, selectDimensions.first) &&
                column >= std::min((cursorIndex - 1) % 6, selectDimensions.first) &&
                row <= std::max((cursorIndex - 1) / 6, selectDimensions.second) && row >= std::min((cursorIndex - 1) / 6, selectDimensions.second))
            {
                C2D_DrawRectSolid(x, y, 0.5f, 34, 30, C2D_Color32(0x50, 0xC0, 0x40, 0xC0));
            }
            if (TitleLoader::save->generation() == Generation::LGPE && row * 6 + column + boxBox * 30 >= TitleLoader::save->maxSlot())
            {
                C2D_DrawRectSolid(x, y, 0.5f, 34, 30, C2D_Color32(128, 128, 128, 128));
            }
            else
            {
                std::shared_ptr<PKX> pokemon = TitleLoader::save->pkm(boxBox, row * 6 + column);
                if (pokemon->species() > 0)
                {
                    Gui::pkm(*pokemon, x, y);
                }
                if (TitleLoader::save->generation() == Generation::LGPE)
                {
                    int partySlot = std::distance(partyPkm, std::find(partyPkm, partyPkm + 6, boxBox * 30 + row * 6 + column));
                    if (partySlot < 6)
                    {
                        Gui::sprite(ui_sheet_emulated_party_indicator_1_idx + partySlot, x + 26, y + 24);
                    }
                }
            }
            x += 34;
        }
        y += 30;
    }

    Gui::text(TitleLoader::save->boxName(boxBox), 25 + 164 / 2, 18, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);

    if (!storageChosen)
    {
        if (cursorIndex == 0)
        {
            int dy = bobPointer();
            for (size_t i = 0; i < moveMon.size(); i++)
            {
                int x = 97 + (i % selectDimensions.first) * 34;
                int y = 10 + dy + (i / selectDimensions.first) * 30;
                if (selectDimensions.first > 1 || selectDimensions.second > 1)
                {
                    C2D_DrawRectSolid(x, y, 0.5f, 34, 30, C2D_Color32(0x50, 0xC0, 0x40, 0xC0));
                }
                if (moveMon[i])
                {
                    Gui::pkm(*moveMon[i], x, y, 1.0f, COLOR_GREY_BLEND, 1.0f);
                    Gui::pkm(*moveMon[i], x - 3, y - 5);
                }
            }
            switch (pickupMode)
            {
                case SINGLE:
                default:
                    Gui::sprite(ui_sheet_pointer_arrow_idx, 106, -4 + dy);
                    break;
                case SWAP:
                    Gui::sprite(ui_sheet_pointer_arrow3_idx, 106, -4 + dy);
                    break;
                case MULTI:
                    Gui::sprite(ui_sheet_pointer_arrow2_idx, 106, -4 + dy);
                    break;
            }
        }
        else
        {
            int tempIndex = cursorIndex - 1;
            int yMod      = (tempIndex / 6) * 30 + bobPointer();
            for (size_t i = 0; i < moveMon.size(); i++)
            {
                int x = 12 + (tempIndex % 6) * 34 + (i % selectDimensions.first) * 34;
                int y = 44 + yMod + (i / selectDimensions.first) * 30;
                if (selectDimensions.first > 1 || selectDimensions.second > 1)
                {
                    C2D_DrawRectSolid(x, y, 0.5f, 34, 30, C2D_Color32(0x50, 0xC0, 0x40, 0xC0));
                }
                if (moveMon[i])
                {
                    Gui::pkm(*moveMon[i], x, y, 1.0f, COLOR_GREY_BLEND, 1.0f);
                    Gui::pkm(*moveMon[i], x - 3, y - 5);
                }
            }
            switch (pickupMode)
            {
                case SINGLE:
                default:
                    Gui::sprite(ui_sheet_pointer_arrow_idx, 21 + (tempIndex % 6) * 34, 30 + yMod);
                    break;
                case SWAP:
                    Gui::sprite(ui_sheet_pointer_arrow3_idx, 21 + (tempIndex % 6) * 34, 30 + yMod);
                    break;
                case MULTI:
                    Gui::sprite(ui_sheet_pointer_arrow2_idx, 21 + (tempIndex % 6) * 34, 30 + yMod);
                    break;
            }
        }
    }
}

void StorageScreen::drawTop() const
{
    Gui::sprite(ui_sheet_emulated_bg_top_green, 0, 0);
    Gui::sprite(ui_sheet_bg_style_top_idx, 0, 0);
    Gui::backgroundAnimatedTop();
    Gui::sprite(ui_sheet_bar_arc_top_green_idx, 0, 0);

    Gui::sprite(ui_sheet_textbox_pksm_idx, 261, 3);
    Gui::text(Banks::bank->name(), 394, 7, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::RIGHT, TextPosY::TOP);

    Gui::sprite(ui_sheet_bar_boxname_empty_idx, 44, 21);
    Gui::text("\uE004", 45 + 24 / 2, 24, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    Gui::text("\uE005", 225 + 24 / 2, 24, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    Gui::text(Banks::bank->boxName(storageBox), 69 + 156 / 2, 24, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);

    Gui::sprite(ui_sheet_storagemenu_cross_idx, 36, 50);
    Gui::sprite(ui_sheet_storagemenu_cross_idx, 246, 50);
    Gui::sprite(ui_sheet_storagemenu_cross_idx, 36, 220);
    Gui::sprite(ui_sheet_storagemenu_cross_idx, 246, 220);

    int y = 66;
    for (u8 row = 0; row < 5; row++)
    {
        u16 x = 45;
        for (u8 column = 0; column < 6; column++)
        {
            // C2D_Color32(0x50, 0xF0, 0x40, 0x80);
            if (currentlySelecting && storageChosen && column <= std::max((cursorIndex - 1) % 6, selectDimensions.first) &&
                column >= std::min((cursorIndex - 1) % 6, selectDimensions.first) &&
                row <= std::max((cursorIndex - 1) / 6, selectDimensions.second) && row >= std::min((cursorIndex - 1) / 6, selectDimensions.second))
            {
                C2D_DrawRectSolid(x, y, 0.5f, 34, 30, C2D_Color32(0x50, 0xC0, 0x40, 0xC0));
            }
            auto pkm = Banks::bank->pkm(storageBox, row * 6 + column);
            if (pkm->species() > 0)
            {
                Gui::pkm(*pkm, x, y);
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
            for (size_t i = 0; i < moveMon.size(); i++)
            {
                int x = 138 + (i % selectDimensions.first) * 34;
                int y = 16 + dy + (i / selectDimensions.first) * 30;
                if (selectDimensions.first > 1 || selectDimensions.second > 1)
                {
                    C2D_DrawRectSolid(x, y, 0.5f, 34, 30, C2D_Color32(0x50, 0xC0, 0x40, 0xC0));
                }
                if (moveMon[i])
                {
                    Gui::pkm(*moveMon[i], x, y, 1.0f, COLOR_GREY_BLEND, 1.0f);
                    Gui::pkm(*moveMon[i], x - 3, y - 5);
                }
            }
            switch (pickupMode)
            {
                case SINGLE:
                default:
                    Gui::sprite(ui_sheet_pointer_arrow_idx, 147, 2 + dy);
                    break;
                case SWAP:
                    Gui::sprite(ui_sheet_pointer_arrow3_idx, 147, 2 + dy);
                    break;
                case MULTI:
                    Gui::sprite(ui_sheet_pointer_arrow2_idx, 147, 2 + dy);
                    break;
            }
        }
        else
        {
            int tempIndex = cursorIndex - 1;
            int yMod      = (tempIndex / 6) * 30 + bobPointer();
            for (size_t i = 0; i < moveMon.size(); i++)
            {
                int x = 53 + (tempIndex % 6) * 34 + (i % selectDimensions.first) * 34;
                int y = 65 + yMod + (i / selectDimensions.first) * 30;
                if (selectDimensions.first > 1 || selectDimensions.second > 1)
                {
                    C2D_DrawRectSolid(x, y, 0.5f, 34, 30, C2D_Color32(0x50, 0xC0, 0x40, 0xC0));
                }
                if (moveMon[i])
                {
                    Gui::pkm(*moveMon[i], x, y, 1.0f, COLOR_GREY_BLEND, 1.0f);
                    Gui::pkm(*moveMon[i], x - 3, y - 5);
                }
            }
            switch (pickupMode)
            {
                case SINGLE:
                default:
                    Gui::sprite(ui_sheet_pointer_arrow_idx, 62 + (tempIndex % 6) * 34, 51 + yMod);
                    break;
                case SWAP:
                    Gui::sprite(ui_sheet_pointer_arrow3_idx, 62 + (tempIndex % 6) * 34, 51 + yMod);
                    break;
                case MULTI:
                    Gui::sprite(ui_sheet_pointer_arrow2_idx, 62 + (tempIndex % 6) * 34, 51 + yMod);
                    break;
            }
        }
    }

    if (infoMon)
    {
        Gui::text(infoMon->nickname(), 276, 61, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        std::string info = "#" + std::to_string(infoMon->species());
        Gui::text(info, 273, 77, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        info        = i18n::localize("LV") + std::to_string(infoMon->level());
        float width = StringUtils::textWidth(info, FONT_SIZE_12);
        Gui::text(info, 375 - (int)width, 77, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        if (infoMon->gender() == 0)
        {
            Gui::sprite(ui_sheet_icon_male_idx, 362 - (int)width, 80);
        }
        else if (infoMon->gender() == 1)
        {
            Gui::sprite(ui_sheet_icon_female_idx, 364 - (int)width, 80);
        }
        else if (infoMon->gender() == 2)
        {
            Gui::sprite(ui_sheet_icon_genderless_idx, 364 - (int)width, 80);
        }
        if (infoMon->shiny())
        {
            Gui::sprite(ui_sheet_icon_shiny_idx, 352 - (int)width, 81);
        }

        Gui::text(i18n::species(Configuration::getInstance().language(), infoMon->species()), 276, 98, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK,
            TextPosX::LEFT, TextPosY::TOP);
        u8 firstType  = infoMon->type1();
        u8 secondType = infoMon->type2();
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

        info = infoMon->otName() + '\n' + i18n::localize("LOADER_ID") + std::to_string(infoMon->versionTID());
        Gui::text(info, 276, 141, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);

        Gui::text(i18n::nature(Configuration::getInstance().language(), infoMon->nature()), 276, 181, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK,
            TextPosX::LEFT, TextPosY::TOP);
        info  = i18n::localize("IV") + ": ";
        width = StringUtils::textWidth(info, FONT_SIZE_12);
        Gui::text(info, 276, 197, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        info = StringUtils::format("%2i/%2i/%2i", infoMon->iv(0), infoMon->iv(1), infoMon->iv(2));
        Gui::text(info, 276 + (int)width + 70 / 2, 197, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
        info = StringUtils::format("%2i/%2i/%2i", infoMon->iv(4), infoMon->iv(5), infoMon->iv(3));
        Gui::text(info, 276 + (int)width + 70 / 2, 209, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
        Gui::format(*infoMon, 276, 213);
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
    u32 kDown         = hidKeysDown();
    u32 kHeld         = hidKeysHeld();

    if (kDown & KEY_B)
    {
        backButton();
        return;
    }

    for (auto& button : mainButtons)
    {
        if (button->update(touch))
            return;
    }
    backHeld = false;
    for (auto& button : clickButtons)
    {
        if (button->update(touch))
            return;
    }

    static int buttonCooldown = 10;

    if (kDown & KEY_A)
    {
        if (cursorIndex == 0)
        {
            if (moveMon.empty())
            {
                Gui::setNextKeyboardFunc(std::bind(&StorageScreen::setBoxName, this, storageChosen));
            }
        }
        else
        {
            pickup();
        }
    }
    else if (kDown & KEY_START)
    {
        currentOverlay = std::make_unique<StorageOverlay>(*this, storageChosen, boxBox, storageBox);
        justSwitched   = true;
    }
    else if (buttonCooldown <= 0)
    {
        if (kDown & KEY_X)
        {
            if (!infoMon)
            {
                if (!Gui::showChoiceMessage(i18n::localize("SHARE_CODE_ENTER_PROMPT")))
                {
                    return;
                }
                Gui::setNextKeyboardFunc([this]() { this->shareReceive(); });
            }
            else
            {
                if (!Gui::showChoiceMessage(i18n::localize("SHARE_SEND_CONFIRM")))
                {
                    return;
                }
                shareSend();
            }
            return;
        }
        sleep = false;
        if (kHeld & KEY_LEFT)
        {
            if (cursorIndex == 0)
            {
                prevBox();
            }
            else if (cursorIndex > 1)
            {
                cursorIndex--;
            }
            else if (cursorIndex == 1)
            {
                prevBox();
                cursorIndex        = 30;
                currentlySelecting = false;
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
                cursorIndex        = 1;
                currentlySelecting = false;
            }
            sleep = true;
        }
        else if (kHeld & KEY_UP)
        {
            if (cursorIndex == 0 && !storageChosen)
            {
                storageChosen = true;
                cursorIndex   = 27;
            }
            else if (cursorIndex > 0 && cursorIndex <= 6)
            {
                cursorIndex        = 0;
                currentlySelecting = false;
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
                storageChosen      = false;
                cursorIndex        = 0;
                currentlySelecting = false;
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
            prevBox();
            sleep = true;
        }
        else if (kHeld & KEY_ZR)
        {
            nextBoxTop();
            sleep = true;
        }
        else if (kHeld & KEY_ZL)
        {
            prevBoxTop();
            sleep = true;
        }
        if (kDown & KEY_Y)
        {
            sleep = true;
            if (moveMon.empty() && !currentlySelecting)
            {
                pickupMode = PickupMode((pickupMode + 1) % 3);
                return;
            }
        }

        if (sleep)
            buttonCooldown = 10;
    }
    if (sleep)
        buttonCooldown--;

    if (cursorIndex != 0)
    {
        infoMon = storageChosen ? Banks::bank->pkm(storageBox, cursorIndex - 1) : TitleLoader::save->pkm(boxBox, cursorIndex - 1);
    }
    else
    {
        infoMon = nullptr;
    }
    if (infoMon && (infoMon->encryptionConstant() == 0 && infoMon->species() == 0))
    {
        infoMon = nullptr;
    }
}

bool StorageScreen::prevBox(bool forceBottom)
{
    if (storageChosen && !forceBottom)
    {
        storageBox--;
        if (storageBox == -1)
        {
            storageBox = Banks::bank->boxes() - 1;
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

bool StorageScreen::prevBoxTop()
{
    storageBox--;
    if (storageBox == -1)
    {
        storageBox = Banks::bank->boxes() - 1;
    }
    return false;
}

bool StorageScreen::nextBox(bool forceBottom)
{
    if (storageChosen && !forceBottom)
    {
        storageBox++;
        if (storageBox == Banks::bank->boxes())
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

bool StorageScreen::nextBoxTop()
{
    storageBox++;
    if (storageBox == Banks::bank->boxes())
    {
        storageBox = 0;
    }
    return false;
}

bool StorageScreen::clickBottomIndex(int index)
{
    if (cursorIndex == index && !storageChosen)
    {
        if (cursorIndex == 0)
        {
            if (moveMon.empty())
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
        if (storageChosen && currentlySelecting)
        {
            currentlySelecting = false;
            selectDimensions   = std::pair{0, 0};
        }
        storageChosen = false;
    }
    return false;
}

bool StorageScreen::backButton()
{
    if (!backHeld)
    {
        backHeld = true;
        if (currentlySelecting)
        {
            currentlySelecting = false;
        }
        else if (!moveMon.empty())
        {
            Gui::warn(i18n::localize("BANK_FAILED_EXIT"));
        }
        else
        {
            bool timer = false;
            if (Banks::bank->hasChanged())
            {
                if (Gui::showChoiceMessage(i18n::localize("BANK_SAVE_CHANGES")))
                {
                    Banks::bank->save();
                    timer = true;
                }
                else
                {
                    Banks::bank->load(Banks::bank->boxes());
                }
            }
            Gui::screenBack();
            ((MainMenu*)screens.top().get())->setTimer(timer);
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

    if (infoMon && infoMon->species() != 0)
    {
        currentOverlay = std::make_unique<ViewCloneOverlay>(*this, infoMon, moveMon, partyNum, selectDimensions, currentlySelecting);
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
                Banks::bank->pkm(TitleLoader::save->emptyPkm(), storageBox, i);
            }
            else if (boxBox * 30 + cursorIndex - 1 < TitleLoader::save->maxSlot())
            {
                TitleLoader::save->pkm(TitleLoader::save->emptyPkm(), boxBox, i, false);
            }
        }
    }
    return false;
}

bool StorageScreen::releasePkm()
{
    backHeld = true;
    if (cursorIndex != 0 && Gui::showChoiceMessage(i18n::localize("BANK_CONFIRM_RELEASE")))
    {
        if (storageChosen)
        {
            Banks::bank->pkm(TitleLoader::save->emptyPkm(), storageBox, cursorIndex - 1);
        }
        else if (boxBox * 30 + cursorIndex - 1 < TitleLoader::save->maxSlot())
        {
            TitleLoader::save->pkm(TitleLoader::save->emptyPkm(), boxBox, cursorIndex - 1, false);
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

bool StorageScreen::isValidTransfer(std::shared_ptr<PKX> moveMon, bool bulkTransfer)
{
    if (!moveMon)
    {
        return false;
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
            PK6* pk6 = (PK6*)moveMon.get();
            if (pk6->relearnMove(i) > TitleLoader::save->maxMove())
            {
                moveBad = true;
                break;
            }
        }
        else if (moveMon->generation() == Generation::SEVEN)
        {
            PK7* pk7 = (PK7*)moveMon.get();
            if (pk7->relearnMove(i) > TitleLoader::save->maxMove())
            {
                moveBad = true;
                break;
            }
        }
    }
    if (moveBad)
    {
        if (!bulkTransfer)
            Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_MOVE"));
        return false;
    }
    else if (moveMon->species() > TitleLoader::save->maxSpecies())
    {
        if (!bulkTransfer)
            Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_SPECIES"));
        return false;
    }
    else if (moveMon->alternativeForm() > TitleLoader::save->formCount(moveMon->species()) &&
             !((moveMon->species() == 664 || moveMon->species() == 665) && moveMon->alternativeForm() <= TitleLoader::save->formCount(666)))
    {
        if (!bulkTransfer)
            Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_FORM"));
        return false;
    }
    else if (moveMon->ability() > TitleLoader::save->maxAbility())
    {
        if (!bulkTransfer)
            Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_ABILITY"));
        return false;
    }
    else if (moveMon->heldItem() > TitleLoader::save->maxItem())
    {
        if (!bulkTransfer)
            Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_ITEM"));
        return false;
    }
    else if (moveMon->ball() > TitleLoader::save->maxBall())
    {
        if (!bulkTransfer)
            Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_BALL"));
        return false;
    }
    return true;
}

void StorageScreen::pickup()
{
    bool acceptGenChange = Configuration::getInstance().transferEdit();
    bool checkedWithUser = Configuration::getInstance().transferEdit();
    if (moveMon.empty())
    {
        if (pickupMode == MULTI)
        {
            if (currentlySelecting)
            {
                grabSelection(true);
            }
            else
            {
                selectDimensions.first  = (cursorIndex - 1) % 6;
                selectDimensions.second = (cursorIndex - 1) / 6;
                currentlySelecting      = true;
            }
        }
        else
        {
            if (storageChosen)
            {
                moveMon.emplace_back(Banks::bank->pkm(storageBox, cursorIndex - 1));
                partyNum.push_back(-1);
                Banks::bank->pkm(TitleLoader::save->emptyPkm(), storageBox, cursorIndex - 1);
                fromStorage = true;
            }
            else if (boxBox * 30 + cursorIndex - 1 < TitleLoader::save->maxSlot())
            {
                partyNum.push_back(-1);
                if (TitleLoader::save->generation() == Generation::LGPE)
                {
                    SavLGPE* sav = (SavLGPE*)TitleLoader::save.get();
                    for (int i = 0; i < TitleLoader::save->partyCount(); i++)
                    {
                        if (sav->partyBoxSlot(i) == boxBox * 30 + cursorIndex - 1)
                        {
                            partyNum[0] = i;
                            break;
                        }
                    }
                }
                moveMon.push_back(TitleLoader::save->pkm(boxBox, cursorIndex - 1));
                TitleLoader::save->pkm(TitleLoader::save->emptyPkm(), boxBox, cursorIndex - 1, false);
                fromStorage = false;
            }
            else
            {
                return;
            }
            if (pickupMode == SINGLE)
            {
                selectDimensions = std::pair{1, 1};
            }
            else
            {
                selectDimensions = std::pair{storageChosen ? storageBox : boxBox, cursorIndex - 1};
            }

            if ((moveMon.back()->encryptionConstant() == 0 && moveMon.back()->species() == 0))
            {
                moveMon.clear();
                partyNum.clear();
                selectDimensions = std::pair{0, 0};
            }
        }
    }
    else
    {
        if (pickupMode != SWAP && storageChosen &&
            cursorIndex + (selectDimensions.first - 1) + (selectDimensions.second - 1) * 6 <= 30 // Checks Y bounds
            && (cursorIndex - 1) % 6 + selectDimensions.first <= 6)                              // Checks X bounds
        {
            fromStorage = false;
            for (int y = 0; y < selectDimensions.second; y++)
            {
                for (int x = 0; x < selectDimensions.first; x++)
                {
                    int index                   = x + y * selectDimensions.first;
                    std::shared_ptr<PKX> temPkm = Banks::bank->pkm(storageBox, cursorIndex - 1 + x + y * 6);
                    if (moveMon[index])
                    {
                        Banks::bank->pkm(moveMon[index], storageBox, cursorIndex - 1 + x + y * 6);
                    }
                    else
                    {
                        Banks::bank->pkm(TitleLoader::save->emptyPkm(), storageBox, cursorIndex - 1 + x + y * 6);
                    }
                    moveMon[index] = temPkm;

                    if (moveMon[index] && moveMon[index]->species() > 0)
                    {
                        fromStorage = true;
                    }
                    else
                    {
                        moveMon[index] = nullptr;
                    }
                }
            }
        }
        else if (pickupMode != SWAP && !storageChosen &&
                 boxBox * 30 + cursorIndex + (selectDimensions.first - 1) + (selectDimensions.second - 1) * 6 <=
                     TitleLoader::save->maxSlot()                                                        // Checks full bounds
                 && cursorIndex + (selectDimensions.first - 1) + (selectDimensions.second - 1) * 6 <= 30 // Checks Y bounds
                 && (cursorIndex - 1) % 6 + selectDimensions.first <= 6)                                 // Checks X bounds
        {
            for (int y = 0; y < selectDimensions.second; y++)
            {
                for (int x = 0; x < selectDimensions.first; x++)
                {
                    int index = x + y * selectDimensions.first;
                    if (!isValidTransfer(moveMon[index]))
                    {
                        continue;
                    }
                    if (!checkedWithUser && moveMon[index]->generation() != TitleLoader::save->generation())
                    {
                        checkedWithUser = true;
                        acceptGenChange = Gui::showChoiceMessage(i18n::localize("GEN_CHANGE_1"), i18n::localize("GEN_CHANGE_2"));
                    }
                    std::shared_ptr<PKX> temPkm = TitleLoader::save->pkm(boxBox, cursorIndex - 1 + x + y * 6);
                    if (moveMon[index]->generation() == TitleLoader::save->generation() || acceptGenChange)
                    {
                        TitleLoader::save->pkm(
                            moveMon[index], boxBox, cursorIndex - 1 + x + y * 6, Configuration::getInstance().transferEdit() && fromStorage);
                        TitleLoader::save->dex(moveMon[index]);
                        if (partyNum[index] != -1)
                        {
                            ((SavLGPE*)TitleLoader::save.get())->partyBoxSlot(partyNum[index], boxBox * 30 + cursorIndex - 1 + x + y * 6);
                        }
                        if ((temPkm->encryptionConstant() == 0 && temPkm->species() == 0))
                        {
                            moveMon[index] = nullptr;
                        }
                        else
                        {
                            moveMon[index] = temPkm;
                        }
                    }
                }
            }
            fromStorage = false;
        }
        else if (pickupMode == SWAP && (storageChosen || boxBox * 30 + cursorIndex <= TitleLoader::save->maxSlot()))
        {
            if (storageChosen && fromStorage)
            {
                Banks::bank->pkm(Banks::bank->pkm(storageBox, cursorIndex - 1), selectDimensions.first, selectDimensions.second);
                Banks::bank->pkm(moveMon[0], storageBox, cursorIndex - 1);
                moveMon.clear();
                partyNum.clear();
            }
            else if (!storageChosen && !fromStorage)
            {
                TitleLoader::save->pkm(TitleLoader::save->pkm(boxBox, cursorIndex - 1), selectDimensions.first, selectDimensions.second, false);
                TitleLoader::save->pkm(moveMon[0], boxBox, cursorIndex - 1, false);
                if (TitleLoader::save->generation() == Generation::LGPE)
                {
                    SavLGPE* save = (SavLGPE*)TitleLoader::save.get();
                    if (partyNum[0] != -1)
                    {
                        save->partyBoxSlot(partyNum[0], boxBox * 30 + cursorIndex - 1);
                    }
                    for (int i = 0; i < 6; i++)
                    {
                        if (save->partyBoxSlot(i) == boxBox * 30 + cursorIndex - 1 && i != partyNum[0])
                        {
                            save->partyBoxSlot(i, selectDimensions.first * 30 + selectDimensions.second);
                            break;
                        }
                    }
                }
                moveMon.clear();
                partyNum.clear();
            }
            else
            {
                std::shared_ptr<PKX> bankMon = storageChosen ? Banks::bank->pkm(storageBox, cursorIndex - 1) : moveMon[0];
                std::shared_ptr<PKX> saveMon = storageChosen ? moveMon[0] : TitleLoader::save->pkm(boxBox, cursorIndex - 1);
                if (!isValidTransfer(bankMon))
                {
                    return;
                }
                if ((Configuration::getInstance().transferEdit() || bankMon->generation() == TitleLoader::save->generation()) ||
                    Gui::showChoiceMessage(i18n::localize("GEN_CHANGE_1"), i18n::localize("GEN_CHANGE_2")))
                {
                    if (storageChosen)
                    {
                        if (partyNum[0] != -1)
                        {
                            ((SavLGPE*)TitleLoader::save.get())->partyBoxSlot(partyNum[0], 1001);
                            TitleLoader::save->fixParty();
                        }
                        TitleLoader::save->pkm(
                            bankMon, selectDimensions.first, selectDimensions.second, Configuration::getInstance().transferEdit() && fromStorage);
                        Banks::bank->pkm(saveMon, storageBox, cursorIndex - 1);
                    }
                    else
                    {
                        SavLGPE* save = (SavLGPE*)TitleLoader::save.get();
                        for (int i = 0; i < 6; i++)
                        {
                            if (save->partyBoxSlot(i) == boxBox * 30 + cursorIndex - 1 && i != partyNum[0])
                            {
                                save->partyBoxSlot(i, selectDimensions.first * 30 + selectDimensions.second);
                                break;
                            }
                        }
                        TitleLoader::save->pkm(bankMon, boxBox, cursorIndex - 1, Configuration::getInstance().transferEdit() && fromStorage);
                        Banks::bank->pkm(saveMon, selectDimensions.first, selectDimensions.second);
                    }
                    moveMon.clear();
                    partyNum.clear();
                    fromStorage = !fromStorage;
                }
            }
        }
        if (std::find_if(moveMon.begin(), moveMon.end(), [](const std::shared_ptr<PKX>& pkm) { return (bool)pkm; }) == moveMon.end())
        {
            moveMon.clear();
            partyNum.clear();
            selectDimensions   = std::pair{0, 0};
            currentlySelecting = false;
        }
        scrunchSelection();
    }
}

bool StorageScreen::dumpPkm()
{
    if (cursorIndex != 0 && Gui::showChoiceMessage(i18n::localize("BANK_CONFIRM_DUMP")))
    {
        char stringDate[11]   = {0};
        char stringTime[10]   = {0};
        time_t unixTime       = time(NULL);
        struct tm* timeStruct = gmtime((const time_t*)&unixTime);
        std::strftime(stringDate, 10, "%Y-%m-%d", timeStruct);
        std::strftime(stringTime, 9, "/%H-%M-%S", timeStruct);
        std::string path = std::string("/3ds/PKSM/dumps/") + stringDate;
        mkdir(path.c_str(), 777);
        path += stringTime;
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
                path += " - " + std::to_string(dumpMon->species()) + " - " + dumpMon->nickname() + " - " + StringUtils::format("%08X") +
                        (dumpMon->generation() != Generation::LGPE ? ".pk" + genToString(dumpMon->generation()) : ".pb7");
                FSStream out(Archive::sd(), StringUtils::UTF8toUTF16(path), FS_OPEN_CREATE | FS_OPEN_WRITE, dumpMon->getLength());
                if (out.good())
                {
                    out.write(dumpMon->rawData(), dumpMon->getLength());
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
                dumpMon = Banks::bank->pkm(storageBox, cursorIndex - 1);
                path += " - " + std::to_string(dumpMon->species()) + " - " + dumpMon->nickname() + " - " + StringUtils::format("%08X") +
                        (dumpMon->generation() != Generation::LGPE ? ".pk" + genToString(dumpMon->generation()) : ".pb7");
                FSStream out(Archive::sd(), StringUtils::UTF8toUTF16(path), FS_OPEN_CREATE | FS_OPEN_WRITE, dumpMon->getLength());
                if (out.good())
                {
                    out.write(dumpMon->rawData(), dumpMon->getLength());
                }
                else
                {
                    Gui::error(i18n::localize("FAILED_OPEN_DUMP"), out.result());
                }
                out.close();
            }
        }
        return true;
    }
    return false;
}

bool StorageScreen::duplicate()
{
    if (moveMon.empty() && cursorIndex > 0)
    {
        if (storageChosen)
        {
            moveMon.emplace_back(Banks::bank->pkm(storageBox, cursorIndex - 1));
        }
        else
        {
            if (boxBox * 30 + cursorIndex - 1 >= TitleLoader::save->maxSlot())
            {
                return false;
            }
            moveMon.emplace_back(TitleLoader::save->pkm(boxBox, cursorIndex - 1)->clone());
        }
        if ((moveMon.back()->encryptionConstant() == 0 && moveMon.back()->species() == 0))
        {
            moveMon.clear();
        }
        else
        {
            partyNum.push_back(-1);
            selectDimensions = std::pair{1, 1};
        }
    }
    return false;
}

bool StorageScreen::swapBoxWithStorage()
{
    std::vector<int> notGenMatch;
    bool acceptGenChange = Configuration::getInstance().transferEdit();
    bool checkedWithUser = Configuration::getInstance().transferEdit();
    for (int i = 0; i < 30; i++)
    {
        if (boxBox * 30 + i >= TitleLoader::save->maxSlot())
        {
            break;
        }
        std::shared_ptr<PKX> temPkm = Banks::bank->pkm(storageBox, i);
        if ((temPkm->encryptionConstant() == 0 && temPkm->species() == 0))
        {
            temPkm = TitleLoader::save->emptyPkm();
        }
        if (!checkedWithUser && temPkm->generation() != TitleLoader::save->generation())
        {
            checkedWithUser = true;
            acceptGenChange = Gui::showChoiceMessage(i18n::localize("GEN_CHANGE_1"), i18n::localize("GEN_CHANGE_2"));
        }
        if (acceptGenChange || temPkm->generation() == TitleLoader::save->generation())
        {
            TitleLoader::save->transfer(temPkm);
            if (isValidTransfer(temPkm, true))
            {
                auto otherTemPkm = TitleLoader::save->pkm(boxBox, i);
                TitleLoader::save->pkm(temPkm, boxBox, i, Configuration::getInstance().transferEdit());
                TitleLoader::save->dex(temPkm);
                Banks::bank->pkm(otherTemPkm, storageBox, i);
            }
        }
        else
        {
            notGenMatch.push_back(i + 1);
        }
    }
    if (!notGenMatch.empty())
    {
        std::string unswapped;
        for (int i : notGenMatch)
        {
            unswapped += std::to_string(i) + ",";
        }
        unswapped.pop_back();
        Gui::warn(i18n::localize("NO_SWAP_BULK"), unswapped);
    }
    return false;
}

void StorageScreen::scrunchSelection()
{
    if (selectDimensions.first <= 1 && selectDimensions.second <= 1)
    {
        return;
    }
    std::vector<int> removableRows, removableColumns;

    bool colDone = false, rowDone = false;
    int firstCol = 0, lastCol = selectDimensions.first - 1, firstRow = 0, lastRow = selectDimensions.second - 1;
    while (!(colDone && rowDone))
    {
        if (!colDone)
        {
            colDone        = true;
            bool canRemove = true, doLast = firstCol < lastCol;
            for (int row = 0; row < selectDimensions.second; row++)
            {
                if (moveMon[row * selectDimensions.first + firstCol])
                {
                    canRemove = false;
                    break;
                }
            }
            if (canRemove)
            {
                removableColumns.push_back(firstCol++);
                colDone = false;
            }
            if (doLast)
            {
                canRemove = true;
                for (int row = 0; row < selectDimensions.second; row++)
                {
                    if (moveMon[row * selectDimensions.first + lastCol])
                    {
                        canRemove = false;
                        break;
                    }
                }
                if (canRemove)
                {
                    removableColumns.push_back(lastCol--);
                    colDone = false;
                }
            }
            else
            {
                colDone = true;
            }
        }

        if (!rowDone)
        {
            rowDone        = true;
            bool canRemove = true, doLast = firstRow < lastRow;
            for (int col = 0; col < selectDimensions.first; col++)
            {
                if (moveMon[firstRow * selectDimensions.first + col])
                {
                    canRemove = false;
                    break;
                }
            }
            if (canRemove)
            {
                removableRows.push_back(firstRow++);
                rowDone = false;
            }
            if (doLast)
            {
                canRemove = true;
                for (int col = 0; col < selectDimensions.first; col++)
                {
                    if (moveMon[lastRow * selectDimensions.first + col])
                    {
                        canRemove = false;
                        break;
                    }
                }
                if (canRemove)
                {
                    removableRows.push_back(lastRow--);
                    rowDone = false;
                }
            }
            else
            {
                rowDone = true;
            }
        }
    }
    for (size_t i = moveMon.size(); i != 0; i--)
    {
        int x = (i - 1) % selectDimensions.first;
        int y = (i - 1) / selectDimensions.first;
        if (std::find(removableColumns.begin(), removableColumns.end(), x) != removableColumns.end() ||
            std::find(removableRows.begin(), removableRows.end(), y) != removableRows.end())
        {
            moveMon.erase(moveMon.begin() + (i - 1));
        }
    }
    selectDimensions.first -= removableColumns.size();
    selectDimensions.second -= removableRows.size();
}

void StorageScreen::grabSelection(bool remove)
{
    int cursorX   = (cursorIndex - 1) % 6;
    int cursorY   = (cursorIndex - 1) / 6;
    int baseIndex = std::min(selectDimensions.first, cursorX) + std::min(selectDimensions.second, cursorY) * 6;
    // Convert to actual dimensions
    selectDimensions.first  = std::abs(selectDimensions.first - cursorX) + 1;
    selectDimensions.second = std::abs(selectDimensions.second - cursorY) + 1;

    for (int y = 0; y < selectDimensions.second; y++)
    {
        for (int x = 0; x < selectDimensions.first; x++)
        {
            int pickupIndex = baseIndex + x + y * 6;
            if (storageChosen)
            {
                fromStorage = true;
                moveMon.emplace_back(Banks::bank->pkm(storageBox, pickupIndex));
                partyNum.push_back(-1);
                if (moveMon.back()->encryptionConstant() == 0 && moveMon.back()->species() == 0)
                {
                    *moveMon.rbegin() = nullptr;
                }
                if (remove)
                {
                    Banks::bank->pkm(TitleLoader::save->emptyPkm(), storageBox, pickupIndex);
                }
            }
            else if (boxBox * 30 + pickupIndex - 1 < TitleLoader::save->maxSlot())
            {
                fromStorage = false;
                partyNum.push_back(-1);
                if (TitleLoader::save->generation() == Generation::LGPE)
                {
                    SavLGPE* sav = (SavLGPE*)TitleLoader::save.get();
                    for (int i = 0; i < TitleLoader::save->partyCount(); i++)
                    {
                        if (sav->partyBoxSlot(i) == boxBox * 30 + pickupIndex)
                        {
                            *partyNum.rbegin() = i;
                            break;
                        }
                    }
                }
                moveMon.emplace_back(TitleLoader::save->pkm(boxBox, pickupIndex));
                if (moveMon.back()->encryptionConstant() == 0 && moveMon.back()->species() == 0)
                {
                    *moveMon.rbegin() = nullptr;
                }
                if (remove)
                {
                    TitleLoader::save->pkm(TitleLoader::save->emptyPkm(), boxBox, pickupIndex, false);
                }
            }
            else
            {
                break;
            }
        }
    }
    currentlySelecting = false;
    if (std::find_if(moveMon.begin(), moveMon.end(), [](const std::shared_ptr<PKX>& pkm) { return (bool)pkm; }) == moveMon.end())
    {
        moveMon.clear();
        partyNum.clear();
        selectDimensions = std::pair{0, 0};
    }
    else
    {
        cursorIndex = baseIndex + 1;
    }
    scrunchSelection();
}

static bool transferFine(std::shared_ptr<PKX>& pkm)
{
    TitleLoader::save->transfer(pkm);
    for (int i = 0; i < 4; i++)
    {
        if (pkm->move(i) > TitleLoader::save->maxMove())
        {
            Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_MOVE"));
            return false;
        }
        if (pkm->generation() == Generation::SIX)
        {
            PK6* pk6 = (PK6*)pkm.get();
            if (pk6->relearnMove(i) > TitleLoader::save->maxMove())
            {
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_MOVE"));
                return false;
            }
        }
        else if (pkm->generation() == Generation::SEVEN)
        {
            PK7* pk7 = (PK7*)pkm.get();
            if (pk7->relearnMove(i) > TitleLoader::save->maxMove())
            {
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_MOVE"));
                return false;
            }
        }
    }
    if (pkm->species() > TitleLoader::save->maxSpecies())
    {
        Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_SPECIES"));
        return false;
    }
    else if (pkm->alternativeForm() > TitleLoader::save->formCount(pkm->species()))
    {
        Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_FORM"));
        return false;
    }
    else if (pkm->ability() > TitleLoader::save->maxAbility())
    {
        Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_ABILITY"));
        return false;
    }
    else if (pkm->heldItem() > TitleLoader::save->maxItem())
    {
        Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_ITEM"));
        return false;
    }
    else if (pkm->ball() > TitleLoader::save->maxBall())
    {
        Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_BALL"));
        return false;
    }
    return true;
}

static size_t header_callback(char* buffer, size_t size, size_t nitems, void* userdata)
{
    std::string tmp(buffer, size * nitems);
    if (tmp.find("Generation:") == 0)
    {
        tmp = tmp.substr(12);
        if (tmp.find("4") == 0)
        {
            *(Generation*)(userdata) = Generation::FOUR;
        }
        else if (tmp.find("5") == 0)
        {
            *(Generation*)(userdata) = Generation::FIVE;
        }
        else if (tmp.find("6") == 0)
        {
            *(Generation*)(userdata) = Generation::SIX;
        }
        else if (tmp.find("7") == 0)
        {
            *(Generation*)(userdata) = Generation::SEVEN;
        }
        else if (tmp.find("LGPE") == 0)
        {
            *(Generation*)(userdata) = Generation::LGPE;
        }
    }
    return nitems * size;
}

void StorageScreen::shareSend()
{
    const u8* rawData = infoMon->rawData();
    size_t outSize;
    long status_code     = 0;
    char* b64Data        = base64_encode((char*)rawData, infoMon->getLength(), &outSize);
    std::string postdata = b64Data;
    free(b64Data);

    std::string version = "Generation: " + genToString(infoMon->generation());
    std::string size    = "Size: " + std::to_string(infoMon->getLength());
    std::string info =
        "Info: " + infoMon->nickname() + "," + infoMon->otName() + "," + std::to_string((int)infoMon->level()) + "," +
        std::to_string(infoMon->species()) + "," + std::to_string(infoMon->move(0)) + "," + std::to_string(infoMon->move(1)) + "," +
        std::to_string(infoMon->move(2)) + "," + std::to_string(infoMon->move(3)) + "," + std::to_string((int)infoMon->nature()) + "," +
        std::to_string((int)infoMon->iv(0)) + "," + std::to_string((int)infoMon->iv(1)) + "," + std::to_string((int)infoMon->iv(2)) // HP, Atk, Def
        + "," + std::to_string((int)infoMon->iv(5)) + "," + std::to_string((int)infoMon->iv(3)) + "," +
        std::to_string((int)infoMon->iv(4)) // Sp. Atk, Sp. Def, Speed
        + "," + std::to_string((int)infoMon->gender()) + "," + std::to_string((bool)infoMon->shiny()) + "," +
        std::to_string((int)infoMon->ability()) + "," + std::to_string((int)infoMon->heldItem()) + "," + std::to_string((int)infoMon->TID());
    struct curl_slist* headers = NULL;
    headers                    = curl_slist_append(headers, "Content-Type: application/base64");
    headers                    = curl_slist_append(headers, version.c_str());
    headers                    = curl_slist_append(headers, size.c_str());
    headers                    = curl_slist_append(headers, info.c_str());

    std::string writeData = "";
    if (Fetch::init("https://flagbrew.org/gpss/share", true, true, &writeData, headers, postdata))
    {
        CURLcode res = Fetch::perform();
        if (res != CURLE_OK)
        {
            Gui::error(i18n::localize("CURL_ERROR"), abs(res));
        }
        else
        {
            Fetch::getinfo(CURLINFO_RESPONSE_CODE, &status_code);
            switch (status_code)
            {
                case 200:
                    Gui::warn(i18n::localize("SHARE_DOWNLOAD_CODE"), writeData);
                    break;
                case 400:
                    Gui::error(i18n::localize("SHARE_FAILED_CHECK"), status_code);
                    break;
                case 502:
                    Gui::error(i18n::localize("HTTP_OFFLINE"), status_code);
                    break;
                default:
                    Gui::error(i18n::localize("HTTP_UNKNOWN_ERROR"), status_code);
                    break;
            }
        }
        Fetch::exit();
    }
    curl_slist_free_all(headers);
}

void StorageScreen::shareReceive()
{
    static SwkbdState state;
    static bool first = true;
    if (first)
    {
        swkbdInit(&state, SWKBD_TYPE_NUMPAD, 3, 10);
        first = false;
    }
    swkbdSetFeatures(&state, SWKBD_FIXED_WIDTH);
    swkbdSetValidation(&state, SWKBD_FIXEDLEN, 0, 0);
    swkbdSetButton(&state, SwkbdButton::SWKBD_BUTTON_MIDDLE, i18n::localize("QR_SCANNER").c_str(), false);
    char input[11]  = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[10]       = '\0';
    CURLcode res;
    if (ret == SWKBD_BUTTON_MIDDLE)
    {
        std::vector<u8> data = QRScanner::scan(NUMBER);
        if (!data.empty() && data.size() < 12)
        {
            std::copy(data.begin(), data.end(), input);
            input[10] = '\0';
            ret = SWKBD_BUTTON_CONFIRM;
        }
    }
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        const std::string url  = "https://flagbrew.org/gpss/download/" + std::string(input);
        std::string retB64Data = "";
        if (Fetch::init(url, false, true, &retB64Data, nullptr, ""))
        {
            long status_code = 0;
            Generation gen   = Generation::UNUSED;
            Fetch::setopt(CURLOPT_HEADERDATA, &gen);
            Fetch::setopt(CURLOPT_HEADERFUNCTION, header_callback);
            res = Fetch::perform();
            if (res != CURLE_OK)
            {
                Gui::error(i18n::localize("CURL_ERROR"), abs(res));
            }
            else
            {
                Fetch::getinfo(CURLINFO_RESPONSE_CODE, &status_code);
                switch (status_code)
                {
                    case 200:
                        break;
                    case 400:
                    case 404:
                        Gui::error(i18n::localize("SHARE_INVALID_CODE"), status_code);
                        Fetch::exit();
                        return;
                    case 502:
                        Gui::error(i18n::localize("HTTP_OFFLINE"), status_code);
                        Fetch::exit();
                        return;
                    default:
                        Gui::error(i18n::localize("HTTP_UNKNOWN_ERROR"), status_code);
                        Fetch::exit();
                        return;
                }
                size_t outSize;
                u8* retData = base64_decode(retB64Data.data(), retB64Data.size(), &outSize);

                size_t targetLength = 0;
                switch (gen)
                {
                    case Generation::FOUR:
                    case Generation::FIVE:
                        targetLength = 138;
                        break;
                    case Generation::SIX:
                    case Generation::SEVEN:
                        targetLength = 234;
                        break;
                    case Generation::LGPE:
                        targetLength = 261;
                        break;
                    default:
                        break;
                }
                if (outSize != targetLength)
                {
                    Gui::error(i18n::localize("SHARE_ERROR_INCORRECT_VERSION"), outSize);
                    free(retData);
                    Fetch::exit();
                    return;
                }

                auto pkm = PKX::getPKM(gen, retData);

                if (storageChosen)
                {
                    Banks::bank->pkm(pkm, storageBox, cursorIndex - 1);
                }
                else
                {
                    if ((TitleLoader::save->generation() != Generation::LGPE && pkm->generation() != Generation::LGPE) ||
                        (TitleLoader::save->generation() == Generation::LGPE && pkm->generation() == Generation::LGPE))
                    {
                        if (transferFine(pkm))
                        {
                            TitleLoader::save->pkm(pkm, boxBox, cursorIndex - 1, true);
                        }
                    }
                    else
                    {
                        Gui::warn(i18n::localize("SHARE_GENERATION_TRANSFER_ERROR"));
                    }
                }
                free(retData);
            }
        }
        Fetch::exit();
    }
}
