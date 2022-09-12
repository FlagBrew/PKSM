/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2021 Bernardo Giordano, Admiral Fish, piepie62
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
#include "CloudScreen.hpp"
#include "Configuration.hpp"
#include "MainMenu.hpp"
#include "SortOverlay.hpp"
#include "StorageOverlay.hpp"
#include "StorageViewOverlay.hpp"
#include "TitleLoadScreen.hpp"
#include "app.hpp"
#include "banks.hpp"
#include "base64.hpp"
#include "fetch.hpp"
#include "format.h"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "io.hpp"
#include "loader.hpp"
#include "pkx/PB7.hpp"
#include "pkx/PK4.hpp"
#include "sav/SavLGPE.hpp"
#include <stack>
#include <sys/stat.h>
#include <variant>

void StorageScreen::setBoxName(bool storage)
{
    if (storage)
    {
        SwkbdState state;
        swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, 20);
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
            case pksm::Generation::TWO:
            {
                SwkbdState state;
                swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, 8);
                swkbdSetHintText(&state, i18n::localize("BOX_NAME").c_str());
                swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
                char input[18] = {0};
                size_t length;
                if (!(TitleLoader::save->language() == pksm::Language::JPN ||
                        TitleLoader::save->language() == pksm::Language::KOR))
                {
                    length = 18;
                }
                else
                {
                    length = 10;
                }
                SwkbdButton ret = swkbdInputText(&state, input, length);
                input[16]       = '\0';
                input[17]       = '\0';
                if (ret == SWKBD_BUTTON_CONFIRM)
                {
                    TitleLoader::save->boxName(boxBox, input);
                }
            }
            break;
            case pksm::Generation::THREE:
            {
                SwkbdState state;
                swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, 8);
                swkbdSetHintText(&state, i18n::localize("BOX_NAME").c_str());
                swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
                char input[18] = {
                    0}; // gotta make room for jpn characters going from two width to one
                SwkbdButton ret = swkbdInputText(
                    &state, input, TitleLoader::save->language() == pksm::Language::JPN ? 18 : 10);
                input[16] = '\0';
                input[17] = '\0';
                if (ret == SWKBD_BUTTON_CONFIRM)
                {
                    TitleLoader::save->boxName(boxBox, input);
                }
            }
            break;
            case pksm::Generation::FOUR:
            case pksm::Generation::FIVE:
            {
                SwkbdState state;
                swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, 8);
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
            case pksm::Generation::SIX:
            case pksm::Generation::SEVEN:
            case pksm::Generation::EIGHT:
            {
                SwkbdState state;
                swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, 16);
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
            case pksm::Generation::ONE:
            case pksm::Generation::LGPE:
            case pksm::Generation::UNUSED:
                // Do nothing
                break;
        }
    }
}

StorageScreen::StorageScreen()
    : Screen(i18n::localize("A_PICKUP") + '\n' + i18n::localize("X_CLONE") + '\n' +
             i18n::localize("Y_CURSOR_MODE") + '\n' + i18n::localize("L_BOX_PREV") + '\n' +
             i18n::localize("R_BOX_NEXT") + '\n' + i18n::localize("START_EXTRA_FUNC") + '\n' +
             i18n::localize("B_BACK"))
{
    instructions.addBox(
        true, 69, 21, 156, 24, COLOR_GREY, i18n::localize("A_BOX_NAME"), COLOR_WHITE);
    instructions.addCircle(false, 266, 23, 11, COLOR_GREY);
    instructions.addLine(false, 266, 23, 266, 73, 4, COLOR_GREY);
    instructions.addBox(
        false, 148, 57, 120, 16, COLOR_GREY, i18n::localize("BOX_SWAP"), COLOR_WHITE);
    mainButtons[0] = std::make_unique<ClickButton>(
        242, 12, 47, 22, [this]() { return this->swapBoxWithStorage(); },
        ui_sheet_button_swap_boxes_idx, "", 0.0f, COLOR_BLACK);
    mainButtons[1] = std::make_unique<Button>(
        212, 47, 108, 28, [this]() { return this->showViewer(); }, ui_sheet_button_editor_idx,
        i18n::localize("VIEW"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[2] = std::make_unique<Button>(
        212, 78, 108, 28, [this]() { return this->clearBox(); }, ui_sheet_button_editor_idx,
        i18n::localize("CLEAR"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[3] = std::make_unique<Button>(
        212, 109, 108, 28, [this]() { return this->releasePkm(); }, ui_sheet_button_editor_idx,
        i18n::localize("RELEASE"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[4] = std::make_unique<Button>(
        212, 140, 108, 28, [this]() { return this->dumpPkm(); }, ui_sheet_button_editor_idx,
        i18n::localize("DUMP"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[5] = std::make_unique<Button>(
        212, 171, 108, 28, [this]() { return this->duplicate(); }, ui_sheet_button_editor_idx,
        i18n::localize("CLONE"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[6] = std::make_unique<Button>(
        283, 211, 34, 28, [this]() { return this->backButton(); }, ui_sheet_button_back_idx, "",
        0.0f, COLOR_BLACK);
    mainButtons[7] = std::make_unique<AccelButton>(
        8, 15, 17, 24, [this]() { return this->prevBox(true); }, ui_sheet_res_null_idx, "", 0.0f,
        COLOR_BLACK, 10, 5);
    mainButtons[8] = std::make_unique<AccelButton>(
        189, 15, 17, 24, [this]() { return this->nextBox(true); }, ui_sheet_res_null_idx, "", 0.0f,
        COLOR_BLACK, 10, 5);

    instructions.addCircle(false, 17, 225, 8, COLOR_GREY);
    instructions.addLine(false, 17, 175, 17, 225, 4, COLOR_GREY);
    instructions.addBox(
        false, 15, 175, 120, 18, COLOR_GREY, i18n::localize("GPSS_BROWSE"), COLOR_WHITE);
    mainButtons[9] = std::make_unique<ClickButton>(
        3, 211, 28, 28,
        [this]()
        {
            Gui::setScreen(std::make_unique<CloudScreen>(storageBox, filter));
            justSwitched = true;
            return true;
        },
        ui_sheet_button_wireless_no_y_idx, "", 0.0f, COLOR_BLACK);

    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;

    // Pokemon buttons
    u16 y = 45;
    for (u8 row = 0; row < 5; row++)
    {
        u16 x = 4;
        if (maxPkmInBox == 20)
        {
            x += 34;
        }
        for (u8 column = 0; column < (maxPkmInBox / 5); column++)
        {
            clickButtons[row * (maxPkmInBox / 5) + column] = std::make_unique<ClickButton>(
                x, y, 34, 30,
                [this, row, column, maxPkmInBox]()
                { return this->clickBottomIndex(row * (maxPkmInBox / 5) + column + 1); },
                ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK);
            x += 34;
        }
        y += 30;
    }
    instructions.addBox(
        false, 25, 15, 164, 24, COLOR_GREY, i18n::localize("A_BOX_NAME"), COLOR_WHITE);
    clickButtons[maxPkmInBox] = std::make_unique<ClickButton>(
        25, 15, 164, 24, [this]() { return this->clickBottomIndex(0); }, ui_sheet_res_null_idx, "",
        0.0f, COLOR_BLACK);
    TitleLoader::save->cryptBoxData(true);

    boxBox = TitleLoader::save->currentBox() % TitleLoader::save->maxBoxes();
}

void StorageScreen::drawBottom() const
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;

    Gui::sprite(ui_sheet_emulated_bg_bottom_green, 0, 0);
    Gui::sprite(ui_sheet_bg_style_bottom_idx, 0, 0);
    Gui::sprite(ui_sheet_bar_arc_bottom_green_idx, 0, 206);

    Gui::sprite(ui_sheet_bar_boxname_with_arrows_idx, 7, 15);
    Gui::sprite(ui_sheet_storage_box_corner_idx, maxPkmInBox == 20 ? 36 : 2, 44);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_horizontal_idx,
        maxPkmInBox == 20 ? 168 : 202, 44);
    Gui::sprite(
        ui_sheet_emulated_storage_box_corner_flipped_vertical_idx, maxPkmInBox == 20 ? 36 : 2, 193);
    Gui::sprite(
        ui_sheet_emulated_storage_box_corner_flipped_both_idx, maxPkmInBox == 20 ? 168 : 202, 193);
    for (const auto& b : mainButtons)
    {
        b->draw();
    }

    u16 partyPkm[6];
    std::fill_n(partyPkm, 6, 1001);
    if (TitleLoader::save->generation() == pksm::Generation::LGPE)
    {
        for (int i = 0; i < 6; i++)
        {
            partyPkm[i] = ((pksm::SavLGPE*)TitleLoader::save.get())->partyBoxSlot(i);
        }
    }

    for (u8 row = 0; row < 5; row++)
    {
        u16 y = 45 + row * 30;
        for (u8 column = 0; column < (maxPkmInBox / 5); column++)
        {
            u16 x = 4 + column * 34;
            if (maxPkmInBox == 20)
            {
                x += 34;
            }
            if (currentlySelecting && !storageChosen &&
                column <= std::max((cursorIndex - 1) % (maxPkmInBox / 5), selectDimensions.first) &&
                column >= std::min((cursorIndex - 1) % (maxPkmInBox / 5), selectDimensions.first) &&
                row <= std::max((cursorIndex - 1) / (maxPkmInBox / 5), selectDimensions.second) &&
                row >= std::min((cursorIndex - 1) / (maxPkmInBox / 5), selectDimensions.second))
            {
                Gui::drawSolidRect(x, y, 34, 30, COLOR_GREEN_HIGHLIGHT);
            }
            if (TitleLoader::save->generation() == pksm::Generation::LGPE &&
                row * 6 + column + boxBox * 30 >= TitleLoader::save->maxSlot())
            {
                Gui::drawSolidRect(x, y, 34, 30, PKSM_Color(128, 128, 128, 128));
            }
            else
            {
                std::unique_ptr<pksm::PKX> pokemon =
                    TitleLoader::save->pkm(boxBox, row * (maxPkmInBox / 5) + column);
                if (pokemon->species() != pksm::Species::None)
                {
                    float blend = *pokemon == *filter ? 0.0f : 0.5f;
                    Gui::pkm(*pokemon, x, y, 1.0f, COLOR_BLACK, blend);
                }
                if (TitleLoader::save->generation() == pksm::Generation::LGPE)
                {
                    int partySlot = std::distance(partyPkm,
                        std::find(partyPkm, partyPkm + 6, boxBox * 30 + row * 6 + column));
                    if (partySlot < 6)
                    {
                        Gui::sprite(
                            ui_sheet_emulated_party_indicator_1_idx + partySlot, x + 26, y + 24);
                    }
                }
            }
        }
    }

    std::string show = TitleLoader::save->boxName(boxBox);
    if (show.empty())
    {
        show = i18n::localize("BOX") + std::to_string(boxBox);
    }
    Gui::text(show, 107, 18, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);

    if (!storageChosen)
    {
        if (cursorIndex == 0)
        {
            int dy = Gui::pointerBob();
            for (size_t i = 0; i < moveMon.size(); i++)
            {
                int x = 97 + (i % selectDimensions.first) * 34;
                int y = 10 + dy + (i / selectDimensions.first) * 30;
                if (pickupMode == MULTI &&
                    (selectDimensions.first > 1 || selectDimensions.second > 1))
                {
                    Gui::drawSolidRect(x, y, 34, 30, COLOR_GREEN_HIGHLIGHT);
                }
                if (moveMon[i])
                {
                    float blend = *moveMon[i] == *filter ? 0.0f : 0.5f;
                    Gui::pkm(*moveMon[i], x, y, 1.0f, COLOR_GREY_BLEND, 1.0f);
                    Gui::pkm(*moveMon[i], x - 3, y - 5, 1.0f, COLOR_BLACK, blend);
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
            int yMod      = (tempIndex / (maxPkmInBox / 5)) * 30 + Gui::pointerBob();
            for (size_t i = 0; i < moveMon.size(); i++)
            {
                int x = (maxPkmInBox == 20 ? 46 : 12) + (tempIndex % (maxPkmInBox / 5)) * 34 +
                        (i % selectDimensions.first) * 34;
                int y = 44 + yMod + (i / selectDimensions.first) * 30;
                if (pickupMode == MULTI &&
                    (selectDimensions.first > 1 || selectDimensions.second > 1))
                {
                    Gui::drawSolidRect(x, y, 34, 30, COLOR_GREEN_HIGHLIGHT);
                }
                if (moveMon[i])
                {
                    float blend = *moveMon[i] == *filter ? 0.0f : 0.5f;
                    Gui::pkm(*moveMon[i], x, y, 1.0f, COLOR_GREY_BLEND, 1.0f);
                    Gui::pkm(*moveMon[i], x - 3, y - 5, 1.0f, COLOR_BLACK, blend);
                }
            }
            switch (pickupMode)
            {
                case SINGLE:
                default:
                    Gui::sprite(ui_sheet_pointer_arrow_idx,
                        (maxPkmInBox == 20 ? 55 : 21) + (tempIndex % (maxPkmInBox / 5)) * 34,
                        30 + yMod);
                    break;
                case SWAP:
                    Gui::sprite(ui_sheet_pointer_arrow3_idx,
                        (maxPkmInBox == 20 ? 55 : 21) + (tempIndex % (maxPkmInBox / 5)) * 34,
                        30 + yMod);
                    break;
                case MULTI:
                    Gui::sprite(ui_sheet_pointer_arrow2_idx,
                        (maxPkmInBox == 20 ? 55 : 21) + (tempIndex % (maxPkmInBox / 5)) * 34,
                        30 + yMod);
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
    Gui::text(
        Banks::bank->name(), 394, 7, FONT_SIZE_14, COLOR_WHITE, TextPosX::RIGHT, TextPosY::TOP);

    Gui::sprite(ui_sheet_bar_boxname_empty_idx, 44, 21);
    Gui::text(
        "\uE004", 45 + 24 / 2, 24, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    Gui::text(
        "\uE005", 225 + 24 / 2, 24, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    Gui::text(Banks::bank->boxName(storageBox), 69 + 156 / 2, 24, FONT_SIZE_14, COLOR_BLACK,
        TextPosX::CENTER, TextPosY::TOP);

    Gui::sprite(ui_sheet_storagemenu_cross_idx, 36, 50);
    Gui::sprite(ui_sheet_storagemenu_cross_idx, 246, 50);
    Gui::sprite(ui_sheet_storagemenu_cross_idx, 36, 220);
    Gui::sprite(ui_sheet_storagemenu_cross_idx, 246, 220);

    for (u8 row = 0; row < 5; row++)
    {
        u16 y = 66 + row * 30;
        for (u8 column = 0; column < 6; column++)
        {
            u16 x = 45 + column * 34;
            if (currentlySelecting && storageChosen &&
                column <= std::max((cursorIndex - 1) % 6, selectDimensions.first) &&
                column >= std::min((cursorIndex - 1) % 6, selectDimensions.first) &&
                row <= std::max((cursorIndex - 1) / 6, selectDimensions.second) &&
                row >= std::min((cursorIndex - 1) / 6, selectDimensions.second))
            {
                Gui::drawSolidRect(x, y, 34, 30, COLOR_GREEN_HIGHLIGHT);
            }
            auto pkm = Banks::bank->pkm(storageBox, row * 6 + column);
            if (pkm->species() != pksm::Species::None)
            {
                float blend = *pkm == *filter ? 0.0f : 0.5f;
                Gui::pkm(*pkm, x, y, 1.0f, COLOR_BLACK, blend);
            }
        }
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
            int dy = Gui::pointerBob();
            for (size_t i = 0; i < moveMon.size(); i++)
            {
                int x = 138 + (i % selectDimensions.first) * 34;
                int y = 16 + dy + (i / selectDimensions.first) * 30;
                if (pickupMode == MULTI &&
                    (selectDimensions.first > 1 || selectDimensions.second > 1))
                {
                    Gui::drawSolidRect(x, y, 34, 30, COLOR_GREEN_HIGHLIGHT);
                }
                if (moveMon[i])
                {
                    float blend = *moveMon[i] == *filter ? 0.0f : 0.5f;
                    Gui::pkm(*moveMon[i], x, y, 1.0f, COLOR_GREY_BLEND, 1.0f);
                    Gui::pkm(*moveMon[i], x - 3, y - 5, 1.0f, COLOR_BLACK, blend);
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
            int yMod      = (tempIndex / 6) * 30 + Gui::pointerBob();
            for (size_t i = 0; i < moveMon.size(); i++)
            {
                int x = 53 + (tempIndex % 6) * 34 + (i % selectDimensions.first) * 34;
                int y = 65 + yMod + (i / selectDimensions.first) * 30;
                if (pickupMode == MULTI &&
                    (selectDimensions.first > 1 || selectDimensions.second > 1))
                {
                    Gui::drawSolidRect(x, y, 34, 30, COLOR_GREEN_HIGHLIGHT);
                }
                if (moveMon[i])
                {
                    float blend = *moveMon[i] == *filter ? 0.0f : 0.5f;
                    Gui::pkm(*moveMon[i], x, y, 1.0f, COLOR_GREY_BLEND, 1.0f);
                    Gui::pkm(*moveMon[i], x - 3, y - 5, 1.0f, COLOR_BLACK, blend);
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
        Gui::text(
            infoMon->nickname(), 276, 61, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        std::string info = "#" + std::to_string(int(infoMon->species()));
        Gui::text(info, 273, 77, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        info      = i18n::localize("LV") + std::to_string(infoMon->level());
        auto text = Gui::parseText(info, FONT_SIZE_12, 0.0f);
        int width = text->maxWidth(FONT_SIZE_12);
        Gui::text(text, 375 - width, 77, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
            TextPosY::TOP);
        switch (infoMon->gender())
        {
            case pksm::Gender::Male:
                Gui::sprite(ui_sheet_icon_male_idx, 362 - width, 80);
                break;
            case pksm::Gender::Female:
                Gui::sprite(ui_sheet_icon_female_idx, 364 - width, 80);
                break;
            case pksm::Gender::Genderless:
                Gui::sprite(ui_sheet_icon_genderless_idx, 364 - width, 80);
                break;
            case pksm::Gender::INVALID:
                break;
        }
        if (infoMon->shiny())
        {
            Gui::sprite(ui_sheet_icon_shiny_idx, 352 - width, 81);
        }

        Gui::text(infoMon->species().localize(Configuration::getInstance().language()), 276, 98,
            FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        pksm::Type firstType  = infoMon->type1();
        pksm::Type secondType = infoMon->type2();
        if (firstType != secondType)
        {
            Gui::type(Configuration::getInstance().language(), firstType, 276, 115);
            Gui::type(Configuration::getInstance().language(), secondType, 325, 115);
        }
        else
        {
            Gui::type(Configuration::getInstance().language(), firstType, 300, 115);
        }

        info = infoMon->otName() + '\n' + i18n::localize("LOADER_ID") +
               std::to_string(infoMon->versionTID());
        Gui::text(info, 276, 141, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);

        Gui::text(infoMon->generation() >= pksm::Generation::THREE
                      ? infoMon->nature().localize(Configuration::getInstance().language())
                      : "—",
            276, 181, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        info  = i18n::localize("IV") + ": ";
        text  = Gui::parseText(info, FONT_SIZE_12, 0.0f);
        width = text->maxWidth(FONT_SIZE_12);
        Gui::text(
            text, 276, 197, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        info = fmt::format(FMT_STRING("{:2d}/{:2d}/{:2d}"), infoMon->iv(pksm::Stat::HP),
            infoMon->iv(pksm::Stat::ATK), infoMon->iv(pksm::Stat::DEF));
        Gui::text(info, 276 + width + 70 / 2, 197, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER,
            TextPosY::TOP);
        info = fmt::format(FMT_STRING("{:2d}/{:2d}/{:2d}"), infoMon->iv(pksm::Stat::SPATK),
            infoMon->iv(pksm::Stat::SPDEF), infoMon->iv(pksm::Stat::SPD));
        Gui::text(info, 276 + width + 70 / 2, 209, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER,
            TextPosY::TOP);
        Gui::format(*infoMon, 276, 213);
    }
}

void StorageScreen::update(touchPosition* touch)
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;
    if (justSwitched)
    {
        if ((keysHeld() | keysDown()) & KEY_TOUCH)
        {
            return;
        }
        else
        {
            justSwitched = false;
        }
    }
    u32 kDown   = hidKeysDown();
    u32 kRepeat = hidKeysDownRepeat();

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
        if (button)
        {
            if (button->update(touch))
                return;
        }
    }

    if (kDown & KEY_A)
    {
        if (cursorIndex == 0)
        {
            if (moveMon.empty())
            {
                setBoxName(storageChosen);
            }
        }
        else
        {
            pickup();
        }
    }
    else if (kDown & KEY_START)
    {
        addOverlay<StorageOverlay>(storageChosen, boxBox, storageBox, filter);
        justSwitched = true;
    }
    else if (kDown & KEY_X)
    {
        if (currentlySelecting)
        {
            duplicate();
        }
        else
        {
            showViewer();
        }
    }
    else if (kDown & KEY_Y)
    {
        if (moveMon.empty() && !currentlySelecting)
        {
            pickupMode = PickupMode((pickupMode + 1) % 3);
            return;
        }
    }

    if (kRepeat & KEY_LEFT)
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
            cursorIndex        = (storageChosen ? 30 : maxPkmInBox);
            currentlySelecting = false;
        }
    }
    else if (kRepeat & KEY_RIGHT)
    {
        if (cursorIndex == 0)
        {
            nextBox();
        }
        else if (cursorIndex < (storageChosen ? 30 : maxPkmInBox))
        {
            cursorIndex++;
        }
        else if (cursorIndex == (storageChosen ? 30 : maxPkmInBox))
        {
            nextBox();
            cursorIndex        = 1;
            currentlySelecting = false;
        }
    }
    else if (kRepeat & KEY_UP)
    {
        if (cursorIndex == 0 && !storageChosen)
        {
            storageChosen = true;
            cursorIndex   = 27;
        }
        else if (storageChosen || maxPkmInBox == 30)
        {
            if (cursorIndex > 0 && cursorIndex <= 6)
            {
                cursorIndex        = 0;
                currentlySelecting = false;
            }
            else if (cursorIndex > 6)
            {
                cursorIndex -= 6;
            }
        }
        else
        {
            if (cursorIndex > 0 && cursorIndex <= 4)
            {
                cursorIndex        = 0;
                currentlySelecting = false;
            }
            else if (cursorIndex > 4)
            {
                cursorIndex -= 4;
            }
        }
    }
    else if (kRepeat & KEY_DOWN)
    {
        if (cursorIndex >= 25 && storageChosen)
        {
            storageChosen      = false;
            cursorIndex        = 0;
            currentlySelecting = false;
        }
        else if (storageChosen || maxPkmInBox == 30)
        {
            if (cursorIndex == 0)
            {
                cursorIndex = 3;
            }
            else if (cursorIndex < 25)
            {
                cursorIndex += 6;
            }
        }
        else
        {
            if (cursorIndex == 0)
            {
                cursorIndex = 2;
            }
            else if (cursorIndex < 17)
            {
                cursorIndex += 4;
            }
        }
    }
    else if (kRepeat & KEY_R)
    {
        nextBox();
    }
    else if (kRepeat & KEY_L)
    {
        prevBox();
    }
    else if (kRepeat & KEY_ZR)
    {
        nextBoxTop();
    }
    else if (kRepeat & KEY_ZL)
    {
        prevBoxTop();
    }

    if (cursorIndex != 0)
    {
        infoMon = storageChosen ? Banks::bank->pkm(storageBox, cursorIndex - 1)
                                : TitleLoader::save->pkm(boxBox, cursorIndex - 1);
    }
    else
    {
        infoMon = nullptr;
    }
    if (infoMon && infoMon->species() == pksm::Species::None)
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
    currentlySelecting = false;
    return false;
}

bool StorageScreen::prevBoxTop()
{
    storageBox--;
    if (storageBox == -1)
    {
        storageBox = Banks::bank->boxes() - 1;
    }
    currentlySelecting = false;
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
    currentlySelecting = false;
    return false;
}

bool StorageScreen::nextBoxTop()
{
    storageBox++;
    if (storageBox == Banks::bank->boxes())
    {
        storageBox = 0;
    }
    currentlySelecting = false;
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
                setBoxName(false);
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
            if (Banks::bank->hasChanged())
            {
                if (Gui::showChoiceMessage(i18n::localize("BANK_SAVE_CHANGES")))
                {
                    Banks::bank->save();
                }
                else
                {
                    Banks::bank->load(Banks::bank->boxes());
                }
            }

            if (TitleLoader::save->generation() == pksm::Generation::LGPE)
            {
                ((pksm::SavLGPE*)TitleLoader::save.get())->compressBox();
                int occupiedSlots = 0;
                for (int i = 0; i < TitleLoader::save->maxSlot(); i++)
                {
                    if (TitleLoader::save->pkm(u8(0), i)->species())
                    {
                        occupiedSlots++;
                    }
                    else
                    {
                        break;
                    }
                }
                ((pksm::SavLGPE*)TitleLoader::save.get())->boxedPkm(occupiedSlots);
            }
            TitleLoader::save->cryptBoxData(false);
            TitleLoader::save->currentBox((u8)boxBox);
            Gui::screenBack();
        }
    }
    return true;
}

bool StorageScreen::showViewer()
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;

    if (cursorIndex == 0 ||
        (!storageChosen && boxBox * maxPkmInBox + cursorIndex - 1 >= TitleLoader::save->maxSlot()))
    {
        return false;
    }

    if (infoMon && infoMon->species() != pksm::Species::None)
    {
        justSwitched = true;
        overlay      = std::make_unique<StorageViewOverlay>(*this, infoMon, moveMon, partyNum,
            selectDimensions, currentlySelecting,
            std::pair<int, int>{storageChosen ? storageBox : -1, cursorIndex - 1},
            pickupMode != PickupMode::SWAP);
    }
    return true;
}

bool StorageScreen::clearBox()
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;

    backHeld = true;
    if (Gui::showChoiceMessage(i18n::localize("BANK_CONFIRM_CLEAR")))
    {
        for (int i = 0; i < (storageChosen ? 30 : maxPkmInBox); i++)
        {
            if (storageChosen)
            {
                Banks::bank->pkm(*TitleLoader::save->emptyPkm(), storageBox, i);
            }
            else if (boxBox * maxPkmInBox + cursorIndex - 1 < TitleLoader::save->maxSlot())
            {
                TitleLoader::save->pkm(*TitleLoader::save->emptyPkm(), boxBox, i, false);
            }
        }
    }
    return false;
}

bool StorageScreen::releasePkm()
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;

    backHeld = true;
    if (!moveMon.empty())
    {
        if (Gui::showChoiceMessage(i18n::localize("BANK_CONFIRM_RELEASE")))
        {
            moveMon.clear();
        }
    }
    else if (cursorIndex != 0 && Gui::showChoiceMessage(i18n::localize("BANK_CONFIRM_RELEASE")))
    {
        if (pickupMode != MULTI || !currentlySelecting)
        {
            if (storageChosen)
            {
                Banks::bank->pkm(*TitleLoader::save->emptyPkm(), storageBox, cursorIndex - 1);
            }
            else if (boxBox * maxPkmInBox + cursorIndex - 1 < TitleLoader::save->maxSlot())
            {
                TitleLoader::save->pkm(
                    *TitleLoader::save->emptyPkm(), boxBox, cursorIndex - 1, false);
                if (TitleLoader::save->generation() == pksm::Generation::LGPE)
                {
                    pksm::SavLGPE* sav = (pksm::SavLGPE*)TitleLoader::save.get();
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
        else if (pickupMode == MULTI && currentlySelecting)
        {
            grabSelection(true);
            moveMon.clear();
        }
    }
    selectDimensions = {0, 0};
    return false;
}

bool StorageScreen::isValidTransfer(const pksm::PKX& moveMon, bool bulkTransfer)
{
    auto invalidReason = TitleLoader::save->invalidTransferReason(moveMon);
    if (invalidReason == pksm::Sav::BadTransferReason::OKAY)
    {
        return true;
    }
    else if (!bulkTransfer)
    {
        Gui::warn(i18n::localize("NO_TRANSFER_PATH") + '\n' +
                  i18n::badTransfer(Configuration::getInstance().language(), invalidReason));
    }
    return false;
}

void StorageScreen::pickupSwap()
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;

    if (storageChosen)
    {
        moveMon.emplace_back(Banks::bank->pkm(storageBox, cursorIndex - 1));
        partyNum.push_back(-1);
        Banks::bank->pkm(*TitleLoader::save->emptyPkm(), storageBox, cursorIndex - 1);
    }
    else if (boxBox * maxPkmInBox + cursorIndex - 1 < TitleLoader::save->maxSlot())
    {
        partyNum.push_back(-1);
        if (TitleLoader::save->generation() == pksm::Generation::LGPE)
        {
            pksm::SavLGPE* sav = (pksm::SavLGPE*)TitleLoader::save.get();
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
        TitleLoader::save->pkm(*TitleLoader::save->emptyPkm(), boxBox, cursorIndex - 1, false);
    }
    else
    {
        return;
    }

    fromStorage      = storageChosen;
    selectDimensions = std::pair{storageChosen ? storageBox : boxBox, cursorIndex - 1};

    if (moveMon.back()->species() == pksm::Species::None)
    {
        moveMon.back() = nullptr;
    }
}

void StorageScreen::pickupSingle()
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;

    if (storageChosen)
    {
        moveMon.emplace_back(Banks::bank->pkm(storageBox, cursorIndex - 1));
        partyNum.push_back(-1);
        Banks::bank->pkm(*TitleLoader::save->emptyPkm(), storageBox, cursorIndex - 1);
    }
    else if (boxBox * maxPkmInBox + cursorIndex - 1 < TitleLoader::save->maxSlot())
    {
        partyNum.push_back(-1);
        if (TitleLoader::save->generation() == pksm::Generation::LGPE)
        {
            pksm::SavLGPE* sav = (pksm::SavLGPE*)TitleLoader::save.get();
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
        TitleLoader::save->pkm(*TitleLoader::save->emptyPkm(), boxBox, cursorIndex - 1, false);
    }
    else
    {
        return;
    }

    fromStorage      = storageChosen;
    selectDimensions = std::pair{1, 1};

    if (moveMon.back()->species() == pksm::Species::None)
    {
        moveMon.back() = nullptr;
    }
}

void StorageScreen::pickupMulti()
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;

    if (currentlySelecting)
    {
        grabSelection(true);
    }
    else
    {
        if (storageChosen || maxPkmInBox == 30)
        {
            selectDimensions.first  = (cursorIndex - 1) % 6;
            selectDimensions.second = (cursorIndex - 1) / 6;
            currentlySelecting      = true;
        }
        else
        {
            selectDimensions.first  = (cursorIndex - 1) % 4;
            selectDimensions.second = (cursorIndex - 1) / 4;
            currentlySelecting      = true;
        }
    }
}

void StorageScreen::postPickup()
{
    if (pickupMode == MULTI && currentlySelecting)
    {
        return;
    }
    if (std::find_if(moveMon.begin(), moveMon.end(),
            [](const std::unique_ptr<pksm::PKX>& pkm) { return (bool)pkm; }) == moveMon.end())
    {
        moveMon.clear();
        partyNum.clear();
        selectDimensions = std::pair{0, 0};
    }
    else if (pickupMode == MULTI)
    {
        scrunchSelection();
    }
}

bool StorageScreen::checkPutDownBounds()
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;

    if (pickupMode == SWAP)
    {
        return storageChosen || boxBox * maxPkmInBox + cursorIndex <= TitleLoader::save->maxSlot();
    }
    else
    {
        if (storageChosen)
        {
            return cursorIndex + (selectDimensions.first - 1) + (selectDimensions.second - 1) * 6 <=
                       30                                               // Checks Y bounds
                && (cursorIndex - 1) % 6 + selectDimensions.first <= 6; // Checks X bounds
        }
        else
        {
            return boxBox * maxPkmInBox + cursorIndex + (selectDimensions.first - 1) +
                           (selectDimensions.second - 1) * (maxPkmInBox / 5) <=
                       TitleLoader::save->maxSlot() // Checks full bounds
                && cursorIndex + (selectDimensions.first - 1) +
                           (selectDimensions.second - 1) * (maxPkmInBox / 5) <=
                       maxPkmInBox // Checks Y bounds
                && (cursorIndex - 1) % (maxPkmInBox / 5) + selectDimensions.first <=
                       (maxPkmInBox / 5); // Checks X bounds
        }
    }
}

void StorageScreen::putDownSwap()
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;

    if (storageChosen && fromStorage)
    {
        Banks::bank->pkm(*Banks::bank->pkm(storageBox, cursorIndex - 1), selectDimensions.first,
            selectDimensions.second);
        Banks::bank->pkm(*moveMon[0], storageBox, cursorIndex - 1);
        moveMon.clear();
        partyNum.clear();
    }
    else if (!storageChosen && !fromStorage)
    {
        TitleLoader::save->pkm(*TitleLoader::save->pkm(boxBox, cursorIndex - 1),
            selectDimensions.first, selectDimensions.second, false);
        TitleLoader::save->pkm(*moveMon[0], boxBox, cursorIndex - 1, false);
        if (TitleLoader::save->generation() == pksm::Generation::LGPE)
        {
            pksm::SavLGPE* save = (pksm::SavLGPE*)TitleLoader::save.get();
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
        std::unique_ptr<pksm::PKX> bankMon =
            storageChosen ? Banks::bank->pkm(storageBox, cursorIndex - 1) : moveMon[0]->clone();
        std::unique_ptr<pksm::PKX> saveMon =
            storageChosen ? moveMon[0]->clone() : TitleLoader::save->pkm(boxBox, cursorIndex - 1);
        pksm::Generation origGen = bankMon->generation();
        if (bankMon->species() == pksm::Species::None)
        {
            bankMon = TitleLoader::save->emptyPkm();
        }
        else if (!isValidTransfer(*bankMon))
        {
            return;
        }
        bankMon = TitleLoader::save->transfer(*bankMon);
        if (!bankMon)
        {
            Gui::warn(fmt::format(i18n::localize("NO_TRANSFER_PATH_SINGLE"), (std::string)origGen,
                (std::string)TitleLoader::save->generation()));
            return;
        }
        if (bankMon->species() == pksm::Species::None ||
            (Configuration::getInstance().transferEdit() ||
                bankMon->generation() == TitleLoader::save->generation()) ||
            Gui::showChoiceMessage(
                i18n::localize("GEN_CHANGE_1") + '\n' + i18n::localize("GEN_CHANGE_2")))
        {
            if (storageChosen)
            {
                if (partyNum[0] != -1)
                {
                    ((pksm::SavLGPE*)TitleLoader::save.get())->partyBoxSlot(partyNum[0], 1001);
                    TitleLoader::save->fixParty();
                }
                TitleLoader::save->pkm(*bankMon, selectDimensions.first, selectDimensions.second,
                    Configuration::getInstance().transferEdit() && fromStorage);
                TitleLoader::save->dex(*bankMon);
                Banks::bank->pkm(*saveMon, storageBox, cursorIndex - 1);
            }
            else
            {
                pksm::SavLGPE* save = (pksm::SavLGPE*)TitleLoader::save.get();
                for (int i = 0; i < (maxPkmInBox / 5); i++)
                {
                    if (save->partyBoxSlot(i) == boxBox * maxPkmInBox + cursorIndex - 1 &&
                        i != partyNum[0])
                    {
                        save->partyBoxSlot(
                            i, selectDimensions.first * maxPkmInBox + selectDimensions.second);
                        break;
                    }
                }
                TitleLoader::save->pkm(*bankMon, boxBox, cursorIndex - 1,
                    Configuration::getInstance().transferEdit() && fromStorage);
                TitleLoader::save->dex(*bankMon);
                Banks::bank->pkm(*saveMon, selectDimensions.first, selectDimensions.second);
            }
            moveMon.clear();
            partyNum.clear();
            fromStorage = !fromStorage;
        }
    }
}

void StorageScreen::putDownNonSwap()
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;

    bool acceptGenChange = Configuration::getInstance().transferEdit();
    bool checkedWithUser = Configuration::getInstance().transferEdit();
    if (storageChosen)
    {
        fromStorage = false;
        for (int y = 0; y < selectDimensions.second; y++)
        {
            for (int x = 0; x < selectDimensions.first; x++)
            {
                int index = x + y * selectDimensions.first;
                std::unique_ptr<pksm::PKX> temPkm =
                    Banks::bank->pkm(storageBox, cursorIndex - 1 + x + y * 6);
                if (moveMon[index])
                {
                    Banks::bank->pkm(*moveMon[index], storageBox, cursorIndex - 1 + x + y * 6);
                }
                else
                {
                    Banks::bank->pkm(
                        *TitleLoader::save->emptyPkm(), storageBox, cursorIndex - 1 + x + y * 6);
                }
                moveMon[index] = std::move(temPkm);

                if (moveMon[index] && moveMon[index]->species() != pksm::Species::None)
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
    else
    {
        for (int y = 0; y < selectDimensions.second; y++)
        {
            for (int x = 0; x < selectDimensions.first; x++)
            {
                int index = x + y * selectDimensions.first;
                if (!moveMon[index] || moveMon[index]->species() == pksm::Species::None)
                {
                    moveMon[index] = TitleLoader::save->emptyPkm();
                }
                else if (!isValidTransfer(*moveMon[index], moveMon.size() > 1))
                {
                    continue;
                }
                if (!TitleLoader::save->transfer(*moveMon[index]))
                {
                    if (moveMon.size() == 1)
                    {
                        Gui::warn(fmt::format(i18n::localize("NO_TRANSFER_PATH_SINGLE"),
                            (std::string)moveMon[index]->generation(),
                            (std::string)TitleLoader::save->generation()));
                    }
                    continue;
                }
                if (!checkedWithUser &&
                    moveMon[index]->generation() != TitleLoader::save->generation())
                {
                    checkedWithUser = true;
                    acceptGenChange = Gui::showChoiceMessage(
                        i18n::localize("GEN_CHANGE_1") + '\n' + i18n::localize("GEN_CHANGE_2"));
                }
                std::unique_ptr<pksm::PKX> temPkm =
                    TitleLoader::save->pkm(boxBox, cursorIndex - 1 + x + y * (maxPkmInBox / 5));
                if (moveMon[index]->generation() == TitleLoader::save->generation() ||
                    acceptGenChange)
                {
                    TitleLoader::save->pkm(*TitleLoader::save->transfer(*moveMon[index]), boxBox,
                        cursorIndex - 1 + x + y * (maxPkmInBox / 5),
                        Configuration::getInstance().transferEdit() && fromStorage);
                    TitleLoader::save->dex(*moveMon[index]);
                    if (partyNum[index] != -1)
                    {
                        ((pksm::SavLGPE*)TitleLoader::save.get())
                            ->partyBoxSlot(partyNum[index],
                                boxBox * maxPkmInBox + cursorIndex - 1 + x + y * (maxPkmInBox / 5));
                    }
                    if (temPkm->species() == pksm::Species::None)
                    {
                        moveMon[index] = nullptr;
                    }
                    else
                    {
                        moveMon[index] = std::move(temPkm);
                    }
                }
            }
        }
        fromStorage = false;
    }
}

void StorageScreen::pickup()
{
    if (moveMon.empty())
    {
        switch (pickupMode)
        {
            case SWAP:
                pickupSwap();
                break;
            case MULTI:
                pickupMulti();
                break;
            case SINGLE:
                pickupSingle();
                break;
        }
    }
    else
    {
        if (checkPutDownBounds())
        {
            if (pickupMode == SWAP)
            {
                putDownSwap();
            }
            else
            {
                putDownNonSwap();
            }
        }
    }

    postPickup();
}

void StorageScreen::doDump(const pksm::PKX& dumpMon)
{
    DateTime now       = DateTime::now();
    std::string folder = fmt::format(
        FMT_STRING("/3ds/PKSM/dumps/{0:d}-{1:d}-{2:d}"), now.year(), now.month(), now.day());
    mkdir(folder.c_str(), 777);
    std::string outPath;
    size_t newFileNumber = 0;

    // Makes sure to handle possible dumping of duplicates in the same second
    // Not really a likely scenario, but much better than overwriting the old one
    do
    {
        if (newFileNumber == 0)
        {
            outPath = fmt::format(FMT_STRING("{:s}/{:d}-{:d}-{:d} - {:d} - {:s} - {:08X}{:s}"),
                folder, now.hour(), now.minute(), now.second(), int(dumpMon.species()),
                dumpMon.nickname(), dumpMon.PID(), dumpMon.extension());
        }
        else
        {
            outPath =
                fmt::format(FMT_STRING("{:s}/{:d}-{:d}-{:d} - {:d} - {:s} - {:08X}({:d}){:s}"),
                    folder, now.hour(), now.minute(), now.second(), int(dumpMon.species()),
                    dumpMon.nickname(), dumpMon.PID(), newFileNumber, dumpMon.extension());
        }
        newFileNumber++;
    }
    while (io::exists(outPath));

    FILE* out = fopen(outPath.c_str(), "wb");
    if (out)
    {
        fwrite(dumpMon.rawData(), 1, dumpMon.getLength(), out);
        fclose(out);
    }
    else
    {
        Gui::error(i18n::localize("FAILED_OPEN_DUMP"), errno);
    }
}

bool StorageScreen::dumpPkm()
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;

    std::unique_ptr<pksm::PKX> dumpMon;
    if (cursorIndex == 0 && moveMon.empty())
    {
        return false;
    }

    if (moveMon.empty())
    {
        if (storageChosen)
        {
            dumpMon = Banks::bank->pkm(storageBox, cursorIndex - 1);
        }
        else if (boxBox * maxPkmInBox + cursorIndex - 1 < TitleLoader::save->maxSlot())
        {
            dumpMon = TitleLoader::save->pkm(boxBox, cursorIndex - 1);
        }
        else
        {
            return false;
        }

        if (!dumpMon || dumpMon->species() == pksm::Species::None)
        {
            return false;
        }
    }

    if (Gui::showChoiceMessage(i18n::localize("BANK_CONFIRM_DUMP")))
    {
        if (moveMon.empty())
        {
            doDump(*dumpMon);
        }
        else
        {
            for (const auto& pkm : moveMon)
            {
                if (pkm)
                {
                    doDump(*pkm);
                }
            }
        }
        return true;
    }
    return false;
}

bool StorageScreen::duplicate()
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;

    if (moveMon.empty() && cursorIndex > 0)
    {
        if (pickupMode == MULTI)
        {
            grabSelection(false);
            postPickup();
        }
        else if (pickupMode != SWAP)
        {
            if (storageChosen)
            {
                moveMon.emplace_back(Banks::bank->pkm(storageBox, cursorIndex - 1));
            }
            else
            {
                if (boxBox * maxPkmInBox + cursorIndex - 1 >= TitleLoader::save->maxSlot())
                {
                    return false;
                }
                moveMon.emplace_back(TitleLoader::save->pkm(boxBox, cursorIndex - 1)->clone());
            }
            if (moveMon.back()->species() == pksm::Species::None)
            {
                moveMon.clear();
            }
            else
            {
                partyNum.push_back(-1);
                selectDimensions = std::pair{1, 1};
            }
        }
    }
    return false;
}

bool StorageScreen::swapBoxWithStorage()
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;

    std::vector<int> unswappedPkm;
    bool acceptGenChange = Configuration::getInstance().transferEdit();
    bool checkedWithUser = Configuration::getInstance().transferEdit();
    for (int i = 0; i < maxPkmInBox; i++)
    {
        if (boxBox * maxPkmInBox + i >= TitleLoader::save->maxSlot())
        {
            break;
        }
        std::unique_ptr<pksm::PKX> temPkm = Banks::bank->pkm(storageBox, i);
        if (temPkm->species() == pksm::Species::None)
        {
            temPkm = TitleLoader::save->emptyPkm();
        }
        if (!checkedWithUser && temPkm->generation() != TitleLoader::save->generation())
        {
            checkedWithUser = true;
            acceptGenChange = Gui::showChoiceMessage(
                i18n::localize("GEN_CHANGE_1") + '\n' + i18n::localize("GEN_CHANGE_2"));
        }
        if (acceptGenChange || temPkm->generation() == TitleLoader::save->generation())
        {
            // Check for transfer path
            temPkm = TitleLoader::save->transfer(*temPkm);
            if (temPkm)
            {
                if (temPkm->species() == pksm::Species::None || isValidTransfer(*temPkm, true))
                {
                    std::unique_ptr<pksm::PKX> otherTemPkm = TitleLoader::save->pkm(boxBox, i);
                    TitleLoader::save->pkm(
                        *temPkm, boxBox, i, Configuration::getInstance().transferEdit());
                    TitleLoader::save->dex(*temPkm);
                    Banks::bank->pkm(*otherTemPkm, storageBox, i);
                }
            }
            else
            {
                unswappedPkm.push_back(i + 1);
            }
        }
        else
        {
            unswappedPkm.push_back(i + 1);
        }
    }
    if (!acceptGenChange && !unswappedPkm.empty())
    {
        std::string unswapped;
        for (int i : unswappedPkm)
        {
            unswapped += std::to_string(i) + ",";
        }
        unswapped.pop_back();
        Gui::warn(i18n::localize("NO_SWAP_BULK") + '\n' + unswapped);
    }
    else if (acceptGenChange && !unswappedPkm.empty())
    {
        std::string unswapped;
        for (int i : unswappedPkm)
        {
            unswapped += std::to_string(i) + ",";
        }
        unswapped.pop_back();
        Gui::warn(i18n::localize("NO_TRANSFER_PATH") + '\n' + unswapped);
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
    int firstCol = 0, lastCol = selectDimensions.first - 1, firstRow = 0,
        lastRow = selectDimensions.second - 1;
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
        if (std::find(removableColumns.begin(), removableColumns.end(), x) !=
                removableColumns.end() ||
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
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;

    int cursorX;
    int cursorY;
    int baseIndex;

    if (storageChosen || maxPkmInBox == 30)
    {
        cursorX   = (cursorIndex - 1) % 6;
        cursorY   = (cursorIndex - 1) / 6;
        baseIndex = std::min(selectDimensions.first, cursorX) +
                    std::min(selectDimensions.second, cursorY) * 6;
    }
    else
    {
        cursorX   = (cursorIndex - 1) % 4;
        cursorY   = (cursorIndex - 1) / 4;
        baseIndex = std::min(selectDimensions.first, cursorX) +
                    std::min(selectDimensions.second, cursorY) * 4;
    }
    // Convert to actual dimensions
    selectDimensions.first  = std::abs(selectDimensions.first - cursorX) + 1;
    selectDimensions.second = std::abs(selectDimensions.second - cursorY) + 1;

    for (int y = 0; y < selectDimensions.second; y++)
    {
        for (int x = 0; x < selectDimensions.first; x++)
        {
            int pickupIndex = baseIndex + x + y * (storageChosen ? 6 : (maxPkmInBox / 5));
            if (storageChosen)
            {
                fromStorage = true;
                moveMon.emplace_back(Banks::bank->pkm(storageBox, pickupIndex));
                partyNum.push_back(-1);
                if (moveMon.back()->species() == pksm::Species::None)
                {
                    *moveMon.rbegin() = nullptr;
                }
                if (remove)
                {
                    Banks::bank->pkm(*TitleLoader::save->emptyPkm(), storageBox, pickupIndex);
                }
            }
            else if (boxBox * maxPkmInBox + pickupIndex - 1 < TitleLoader::save->maxSlot())
            {
                fromStorage = false;
                partyNum.push_back(-1);
                if (TitleLoader::save->generation() == pksm::Generation::LGPE)
                {
                    pksm::SavLGPE* sav = (pksm::SavLGPE*)TitleLoader::save.get();
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
                if (moveMon.back()->species() == pksm::Species::None)
                {
                    *moveMon.rbegin() = nullptr;
                }
                if (remove)
                {
                    TitleLoader::save->pkm(
                        *TitleLoader::save->emptyPkm(), boxBox, pickupIndex, false);
                }
            }
            else
            {
                break;
            }
        }
    }
    currentlySelecting = false;
    if (std::find_if(moveMon.begin(), moveMon.end(),
            [](const std::unique_ptr<pksm::PKX>& pkm) { return (bool)pkm; }) == moveMon.end())
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
