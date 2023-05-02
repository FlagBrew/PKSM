/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2022 Bernardo Giordano, Admiral Fish, piepie62
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

#include "EditSelectorScreen.hpp"
#include "AccelButton.hpp"
#include "ClickButton.hpp"
#include "Configuration.hpp"
#include "EditorScreen.hpp"
#include "gui.hpp"
#include "loader.hpp"
#include "pkx/PB7.hpp"
#include "pkx/PK3.hpp"
#include "pkx/PK4.hpp"
#include "pkx/PK5.hpp"
#include "pkx/PK6.hpp"
#include "pkx/PK7.hpp"
#include "pkx/PK8.hpp"
#include "QRScanner.hpp"
#include "sav/SavLGPE.hpp"
#include "ViewOverlay.hpp"
#include <format>
#include <memory>

void EditSelectorScreen::changeBoxName()
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
                TitleLoader::save->boxName(box, input);
            }
        }
        break;
        case pksm::Generation::THREE:
        {
            SwkbdState state;
            swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, 8);
            swkbdSetHintText(&state, i18n::localize("BOX_NAME").c_str());
            swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
            char input[18]  = {0}; // gotta make room for jpn characters going from two width to one
            SwkbdButton ret = swkbdInputText(
                &state, input, TitleLoader::save->language() == pksm::Language::JPN ? 18 : 10);
            input[16] = '\0';
            input[17] = '\0';
            if (ret == SWKBD_BUTTON_CONFIRM)
            {
                TitleLoader::save->boxName(box, input);
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
                TitleLoader::save->boxName(box, input);
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
                TitleLoader::save->boxName(box, input);
            }
        }
        break;
        case pksm::Generation::ONE:
        case pksm::Generation::LGPE:
        case pksm::Generation::UNUSED:
            // Nothing happens
            break;
    }
}

bool EditSelectorScreen::doQR()
{
    std::unique_ptr<pksm::PKX> pkm;
    switch (TitleLoader::save->generation())
    {
        case pksm::Generation::ONE:
            pkm = QRScanner<pksm::PK1>::scan();
            break;
        case pksm::Generation::TWO:
            pkm = QRScanner<pksm::PK2>::scan();
            break;
        case pksm::Generation::THREE:
            pkm = QRScanner<pksm::PK3>::scan();
            break;
        case pksm::Generation::FOUR:
            pkm = QRScanner<pksm::PK4>::scan();
            break;
        case pksm::Generation::FIVE:
            pkm = QRScanner<pksm::PK5>::scan();
            break;
        case pksm::Generation::SIX:
            pkm = QRScanner<pksm::PK6>::scan();
            break;
        case pksm::Generation::SEVEN:
            pkm = QRScanner<pksm::PK7>::scan();
            break;
        case pksm::Generation::EIGHT:
            pkm = QRScanner<pksm::PK8>::scan();
            break;
        case pksm::Generation::UNUSED:
        case pksm::Generation::LGPE:
            return false;
    }

    if (pkm)
    {
        int slot = cursorPos ? cursorPos - 1
                             : 0; // make sure it writes to a good position, AKA not the title bar
        TitleLoader::save->pkm(*pkm, box, slot, false);
        return true;
    }
    return false;
}

EditSelectorScreen::EditSelectorScreen()
    : Screen(i18n::localize("A_SELECT") + '\n' + i18n::localize("X_CLONE") + '\n' +
             i18n::localize("B_BACK"))
{
    addOverlay<ViewOverlay>(infoMon, false);

    buttons.push_back(std::make_unique<ClickButton>(
        283, 211, 34, 28,
        [this]()
        {
            justSwitched = true;
            return goBack();
        },
        ui_sheet_button_back_idx, "", 0.0f, COLOR_BLACK));
    instructions.addBox(
        false, 25, 15, 164, 24, COLOR_GREY, i18n::localize("A_BOX_NAME"), COLOR_WHITE);
    buttons.push_back(std::make_unique<ClickButton>(
        25, 15, 164, 24, [this]() { return this->clickIndex(0); }, ui_sheet_res_null_idx, "", 0.0f,
        COLOR_BLACK));
    buttons.push_back(std::make_unique<AccelButton>(
        8, 15, 17, 24, [this]() { return this->prevBox(); }, ui_sheet_res_null_idx, "", 0.0f,
        COLOR_BLACK, 10, 5));
    buttons.push_back(std::make_unique<AccelButton>(
        189, 15, 17, 24, [this]() { return this->nextBox(); }, ui_sheet_res_null_idx, "", 0.0f,
        COLOR_BLACK, 10, 5));
    auto cameraButtonText = Gui::parseText("\uE004+\uE005 \uE01E", FONT_SIZE_14, 0.0f);
    instructions.addCircle(
        false, 310 - cameraButtonText->maxWidth(FONT_SIZE_14) / 2, 24, 8, COLOR_GREY);
    instructions.addLine(false, 310 - cameraButtonText->maxWidth(FONT_SIZE_14) / 2, 24,
        310 - cameraButtonText->maxWidth(FONT_SIZE_14) / 2, 44, 4, COLOR_GREY);
    instructions.addBox(false, 222 - cameraButtonText->maxWidth(FONT_SIZE_14) / 2, 44, 90, 16,
        COLOR_GREY, i18n::localize("QR_SCANNER"), COLOR_WHITE);
    buttons.push_back(std::make_unique<ClickButton>(
        310 - cameraButtonText->maxWidth(FONT_SIZE_14), 16,
        cameraButtonText->maxWidth(FONT_SIZE_14) + 2, 16, [this]() { return this->doQR(); },
        ui_sheet_res_null_idx, "\uE004+\uE005 \uE01E", FONT_SIZE_14, COLOR_BLACK));

    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;

    // Pokemon buttons
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
            pkmButtons[row * (maxPkmInBox / 5) + column] = std::make_unique<ClickButton>(
                x, y, 34, 30,
                [this, row, column, maxPkmInBox]()
                { return this->clickIndex(row * (maxPkmInBox / 5) + column + 1); },
                ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK);
        }
    }
    for (int i = 0; i < 6; i++)
    {
        int x                       = (i % 2 == 0 ? 221 : 271);
        int y                       = (i % 2 == 0 ? 50 + 45 * (i / 2) : 66 + 45 * (i / 2));
        pkmButtons[maxPkmInBox + i] = std::make_unique<ClickButton>(
            x, y, 34, 30,
            [this, i, maxPkmInBox]() { return this->clickIndex((maxPkmInBox + 1) + i); },
            ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK);
    }

    viewerButtons.push_back(std::make_unique<ClickButton>(
        212, 47, 108, 28, [this]() { return this->editPokemon(); }, ui_sheet_button_editor_idx,
        "\uE000: " + i18n::localize("EDIT"), FONT_SIZE_12, COLOR_BLACK));
    viewerButtons.push_back(std::make_unique<ClickButton>(
        212, 78, 108, 28,
        [this]()
        {
            menu = false;
            return this->releasePokemon();
        },
        ui_sheet_button_editor_idx, "\uE003: " + i18n::localize("RELEASE"), FONT_SIZE_12,
        COLOR_BLACK));
    viewerButtons.push_back(std::make_unique<ClickButton>(
        212, 109, 108, 28,
        [this]()
        {
            menu = false;
            return this->clonePkm();
        },
        ui_sheet_button_editor_idx, "\uE002: " + i18n::localize("CLONE"), FONT_SIZE_12,
        COLOR_BLACK));
    TitleLoader::save->cryptBoxData(true);
    box = TitleLoader::save->currentBox() % TitleLoader::save->maxBoxes();
}

void EditSelectorScreen::drawBottom() const
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;

    Gui::sprite(ui_sheet_emulated_bg_bottom_blue, 0, 0);
    Gui::sprite(ui_sheet_bg_style_bottom_idx, 0, 0);
    Gui::sprite(ui_sheet_bar_bottom_blue_idx, 0, 216);
    Gui::sprite(ui_sheet_stripe_camera_idx, 218, 14);

    Gui::sprite(ui_sheet_bar_boxname_with_arrows_idx, 7, 15);
    Gui::sprite(ui_sheet_storage_box_corner_idx, maxPkmInBox == 20 ? 36 : 2, 44);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_horizontal_idx,
        maxPkmInBox == 20 ? 168 : 202, 44);
    Gui::sprite(
        ui_sheet_emulated_storage_box_corner_flipped_vertical_idx, maxPkmInBox == 20 ? 36 : 2, 193);
    Gui::sprite(
        ui_sheet_emulated_storage_box_corner_flipped_both_idx, maxPkmInBox == 20 ? 168 : 202, 193);
    for (const auto& b : buttons)
    {
        b->draw();
    }

    for (int i = 0; i < 6; i++)
    {
        int x = (i % 2 == 0 ? 214 : 264);
        int y = (i % 2 == 0 ? 43 + 45 * (i / 2) : 59 + 45 * (i / 2));
        Gui::sprite(ui_sheet_icon_party_idx, x, y);
        if (TitleLoader::save->generation() == pksm::Generation::LGPE)
        {
            Gui::sprite(ui_sheet_emulated_party_indicator_1_idx + i, x + 34, y + 30);
        }
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
        for (u8 column = 0; column < maxPkmInBox / 5; column++)
        {
            u16 x = 4 + column * 34;
            if (maxPkmInBox == 20)
            {
                x += 34;
            }
            if (TitleLoader::save->generation() == pksm::Generation::LGPE &&
                row * 6 + column + box * 30 >= TitleLoader::save->maxSlot())
            {
                Gui::drawSolidRect(x, y, 34, 30, PKSM_Color(128, 128, 128, 128));
            }
            else
            {
                std::unique_ptr<pksm::PKX> pokemon =
                    TitleLoader::save->pkm(box, row * (maxPkmInBox / 5) + column);
                if (pokemon->species() != pksm::Species::None)
                {
                    Gui::pkm(*pokemon, x, y);
                }
                if (TitleLoader::save->generation() == pksm::Generation::LGPE)
                {
                    int partySlot = std::distance(
                        partyPkm, std::find(partyPkm, partyPkm + 6, box * 30 + row * 6 + column));
                    if (partySlot < 6)
                    {
                        Gui::sprite(
                            ui_sheet_emulated_party_indicator_1_idx + partySlot, x + 26, y + 24);
                    }
                }
            }
        }
    }

    for (int i = 0; i < TitleLoader::save->partyCount(); i++)
    {
        int x                              = (i % 2 == 0 ? 221 : 271);
        int y                              = (i % 2 == 0 ? 50 + 45 * (i / 2) : 66 + 45 * (i / 2));
        std::unique_ptr<pksm::PKX> pokemon = TitleLoader::save->pkm(i);
        if (pokemon->species() != pksm::Species::None)
        {
            Gui::pkm(*pokemon, x, y);
        }
    }

    std::string show = TitleLoader::save->boxName(box);
    if (show.empty())
    {
        show = i18n::localize("BOX") + std::to_string(box);
    }
    Gui::text(show, 107, 18, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);

    if (cursorPos == 0)
    {
        int dy = Gui::pointerBob();
        if (moveMon)
        {
            Gui::pkm(*moveMon, 94, 5 + dy);
        }
        Gui::sprite(ui_sheet_pointer_arrow_idx, 106, -4 + dy);
    }

    else if (cursorPos < (maxPkmInBox + 1))
    {
        int tempIndex = cursorPos - 1;
        int yMod      = (tempIndex / (maxPkmInBox / 5)) * 30 + Gui::pointerBob();
        if (moveMon)
        {
            Gui::pkm(*moveMon, (maxPkmInBox == 20 ? 43 : 9) + (tempIndex % (maxPkmInBox / 5)) * 34,
                39 + yMod);
        }
        Gui::sprite(ui_sheet_pointer_arrow_idx,
            (maxPkmInBox == 20 ? 55 : 21) + (tempIndex % (maxPkmInBox / 5)) * 34, 30 + yMod);
    }
    else
    {
        int x = 238 + ((cursorPos - 1) % 2) * 50;
        int y = ((cursorPos - 1) % 2 == 0 ? 35 : 51) + (((cursorPos - 1) - maxPkmInBox) / 2) * 45 +
                Gui::pointerBob();
        if (moveMon)
        {
            Gui::pkm(*moveMon, x - 12, y + 9);
        }
        Gui::sprite(ui_sheet_pointer_arrow_idx, x, y);
    }

    if (infoMon)
    {
        Gui::text(
            std::vformat(i18n::localize("EDITOR_IDS"),
                std::make_format_args(infoMon->formatTID(), infoMon->formatSID(), infoMon->TSV())),
            160, 224, FONT_SIZE_9, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    }

    if (menu)
    {
        Gui::drawSolidRect(0, 0, 320, 240, COLOR_MASKBLACK);
        for (const auto& button : viewerButtons)
        {
            button->draw();
        }
        buttons[0]->draw();
    }
}

void EditSelectorScreen::update(touchPosition* touch)
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

    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;

    if (moveMon)
    {
        infoMon = moveMon->clone();
    }
    else if (cursorPos != 0)
    {
        if (cursorPos < maxPkmInBox + 1)
        {
            if (box * maxPkmInBox + cursorPos - 1 < TitleLoader::save->maxSlot())
            {
                infoMon = TitleLoader::save->pkm(box, cursorPos - 1);
            }
        }
        else
        {
            infoMon = TitleLoader::save->pkm(cursorPos - (maxPkmInBox + 1));
        }
    }
    else
    {
        infoMon = nullptr;
    }
    if (infoMon && infoMon->species() == pksm::Species::None)
    {
        infoMon = nullptr;
    }

    u32 downKeys   = hidKeysDown();
    u32 heldKeys   = hidKeysHeld();
    u32 repeatKeys = hidKeysDownRepeat();
    u32 upKeys     = hidKeysUp();

    if (menu)
    {
        buttons[0]->update(touch);
        for (size_t i = 0; i < viewerButtons.size(); i++)
        {
            viewerButtons[i]->update(touch);
        }

        if (downKeys & KEY_B)
        {
            menu = false;
        }
        else if (downKeys & KEY_X)
        {
            clonePkm();
            menu = false;
        }
        else if (downKeys & KEY_A)
        {
            editPokemon();
        }
        else if (downKeys & KEY_Y)
        {
            releasePokemon();
        }
    }
    else
    {
        for (size_t i = 0; i < buttons.size(); i++)
        {
            if (buttons[i]->update(touch))
            {
                return;
            }
        }

        for (auto& button : pkmButtons)
        {
            if (button)
            {
                button->update(touch);
            }
        }

        if (downKeys & KEY_A)
        {
            if (cursorPos == 0)
            {
                changeBoxName();
            }
            else if (moveMon && cursorPos < (maxPkmInBox + 1))
            {
                std::unique_ptr<pksm::PKX> tmpMon = nullptr;
                if (box * maxPkmInBox + cursorPos - 1 < TitleLoader::save->maxSlot())
                {
                    tmpMon = TitleLoader::save->pkm(box, cursorPos - 1);
                    if (tmpMon && tmpMon->species() == pksm::Species::None)
                    {
                        tmpMon = nullptr;
                    }
                    TitleLoader::save->pkm(*moveMon, box, cursorPos - 1, false);
                    moveMon = std::move(tmpMon);
                }
            }
            else if (moveMon)
            {
                std::unique_ptr<pksm::PKX> tmpMon = nullptr;
                if (cursorPos - (maxPkmInBox + 1) < TitleLoader::save->partyCount())
                {
                    tmpMon = TitleLoader::save->pkm(cursorPos - (maxPkmInBox + 1));
                }
                if (tmpMon && tmpMon->species() == pksm::Species::None)
                {
                    tmpMon = nullptr;
                }
                TitleLoader::save->pkm(*moveMon, cursorPos - (maxPkmInBox + 1));
                moveMon = std::move(tmpMon);
                TitleLoader::save->fixParty();
            }
            else
            {
                menu = true;
                return;
            }
        }
        else if (downKeys & KEY_B)
        {
            if (moveMon)
            {
                moveMon = nullptr;
            }
            else
            {
                goBack();
                return;
            }
        }
        else if (downKeys & KEY_X)
        {
            clonePkm();
        }
        else if (((heldKeys | downKeys) & KEY_L) && ((heldKeys | downKeys) & KEY_R))
        {
            doQR();
            return;
        }
        else if (repeatKeys & KEY_LEFT)
        {
            if (cursorPos >= (maxPkmInBox + 1))
            {
                if (cursorPos == (maxPkmInBox + 1))
                {
                    cursorPos = (maxPkmInBox / 5);
                }
                else if (cursorPos == (maxPkmInBox + 3))
                {
                    cursorPos = (maxPkmInBox / 5) * 2;
                }
                else if (cursorPos == (maxPkmInBox + 5))
                {
                    cursorPos = (maxPkmInBox / 5) * 3;
                }
                else
                {
                    cursorPos--;
                }
            }
            else if (cursorPos > 1)
            {
                cursorPos--;
            }
            else if (cursorPos == 1)
            {
                prevBox();
                cursorPos = maxPkmInBox;
            }
            else if (cursorPos == 0)
            {
                prevBox();
            }
        }
        else if (repeatKeys & KEY_RIGHT)
        {
            if (cursorPos == 0)
            {
                nextBox();
            }
            else if (cursorPos < maxPkmInBox ||
                     (cursorPos >= (maxPkmInBox + 1) && cursorPos % 2 == 1))
            {
                cursorPos++;
            }
            else if (cursorPos == maxPkmInBox)
            {
                nextBox();
                cursorPos = 1;
            }
            else if (cursorPos == (maxPkmInBox + 2))
            {
                cursorPos = 1;
            }
            else if (cursorPos == (maxPkmInBox + 4))
            {
                cursorPos = ((maxPkmInBox / 5) * 2) + 1;
            }
            else if (cursorPos == (maxPkmInBox + 6))
            {
                cursorPos = ((maxPkmInBox / 5) * 4) + 1;
            }
        }
        else if (repeatKeys & KEY_UP)
        {
            if (cursorPos == 0)
            {
                cursorPos = maxPkmInBox == 20 ? 18 : 27;
            }
            else if (cursorPos <= (maxPkmInBox / 5))
            {
                cursorPos = 0;
            }
            else if (cursorPos >= (maxPkmInBox + 1))
            {
                if (cursorPos > (maxPkmInBox + 2))
                {
                    cursorPos -= 2;
                }
                else
                {
                    cursorPos += 4;
                }
            }
            else
            {
                cursorPos -= maxPkmInBox / 5;
            }
        }
        else if (repeatKeys & KEY_DOWN)
        {
            if (cursorPos >= (maxPkmInBox + 1))
            {
                if (cursorPos < (maxPkmInBox + 5))
                {
                    cursorPos += 2;
                }
                else
                {
                    cursorPos -= 4;
                }
            }
            else if (cursorPos >= (maxPkmInBox == 20 ? 17 : 25))
            {
                cursorPos = 0;
            }
            else if (cursorPos == 0)
            {
                cursorPos = 2;
            }
            else
            {
                cursorPos += maxPkmInBox / 5;
            }
        }
        else if (repeatKeys & (~downKeys) & KEY_R || (upKeys & KEY_R))
        {
            nextBox();
        }
        else if (repeatKeys & (~downKeys) & KEY_L || (upKeys & KEY_L))
        {
            prevBox();
        }
    }
}

bool EditSelectorScreen::prevBox()
{
    if (box == 0)
    {
        box = TitleLoader::save->maxBoxes() - 1;
    }
    else
    {
        box--;
    }
    return false;
}

bool EditSelectorScreen::nextBox()
{
    box++;
    if (box == TitleLoader::save->maxBoxes())
    {
        box = 0;
    }
    return false;
}

bool EditSelectorScreen::editPokemon()
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;
    if (cursorPos < (maxPkmInBox + 1) && cursorPos != 0)
    {
        if (box * maxPkmInBox + cursorPos - 1 < TitleLoader::save->maxSlot())
        {
            justSwitched = true;
            Gui::setScreen(std::make_unique<EditorScreen>(
                TitleLoader::save->pkm(box, cursorPos - 1), box, cursorPos - 1));
            return true;
        }
    }
    else if (cursorPos > maxPkmInBox)
    {
        justSwitched = true;
        Gui::setScreen(
            std::make_unique<EditorScreen>(TitleLoader::save->pkm(cursorPos - (maxPkmInBox + 1)),
                EditorScreen::PARTY_MAGIC_NUM, cursorPos - (maxPkmInBox + 1)));
        return true;
    }

    return false;
}

bool EditSelectorScreen::clickIndex(int i)
{
    if (cursorPos == i)
    {
        if (cursorPos == 0)
        {
            changeBoxName();
        }
        else
        {
            menu         = true;
            justSwitched = true;
        }
        return true;
    }
    else
    {
        cursorPos = i;
        return false;
    }
}

bool EditSelectorScreen::clonePkm()
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;
    if (cursorPos > 0)
    {
        if (cursorPos > maxPkmInBox)
        {
            if (!moveMon)
            {
                moveMon = TitleLoader::save->pkm(cursorPos - (maxPkmInBox + 1));
                if (moveMon && moveMon->species() == pksm::Species::None)
                {
                    moveMon = nullptr;
                }
            }
            else
            {
                std::unique_ptr<pksm::PKX> tmpMon =
                    TitleLoader::save->pkm(cursorPos - (maxPkmInBox + 1));
                if (tmpMon && tmpMon->species() == pksm::Species::None)
                {
                    tmpMon = nullptr;
                }
                TitleLoader::save->pkm(*moveMon, cursorPos - (maxPkmInBox + 1));
                moveMon = std::move(tmpMon);
                TitleLoader::save->fixParty();
            }
        }
        else if (box * maxPkmInBox + cursorPos - 1 < TitleLoader::save->maxSlot())
        {
            if (!moveMon)
            {
                moveMon = TitleLoader::save->pkm(box, cursorPos - 1);
                if (moveMon && moveMon->species() == pksm::Species::None)
                {
                    moveMon = nullptr;
                }
            }
            else
            {
                std::unique_ptr<pksm::PKX> tmpMon = TitleLoader::save->pkm(box, cursorPos - 1);
                if (tmpMon->species() == pksm::Species::None)
                {
                    tmpMon = nullptr;
                }
                TitleLoader::save->pkm(*moveMon, box, cursorPos - 1, false);
                moveMon = std::move(tmpMon);
            }
        }
    }
    return false;
}

bool EditSelectorScreen::goBack()
{
    if (menu)
    {
        menu = false;
    }
    else
    {
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
        TitleLoader::save->currentBox((u8)box);
        Gui::screenBack();
    }
    return true;
}

bool EditSelectorScreen::releasePokemon()
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                         TitleLoader::save->language() != pksm::Language::JPN)
                       ? 20
                       : 30;
    if (cursorPos != 0 && Gui::showChoiceMessage(i18n::localize("BANK_CONFIRM_RELEASE")))
    {
        if (cursorPos < (maxPkmInBox + 1) &&
            box * maxPkmInBox + cursorPos - 1 < TitleLoader::save->maxSlot())
        {
            TitleLoader::save->pkm(*TitleLoader::save->emptyPkm(), box, cursorPos - 1, false);
            if (TitleLoader::save->generation() == pksm::Generation::LGPE)
            {
                pksm::SavLGPE* sav = (pksm::SavLGPE*)TitleLoader::save.get();
                for (int i = 0; i < sav->partyCount(); i++)
                {
                    if (sav->partyBoxSlot(i) == box * 30 + cursorPos - 1)
                    {
                        sav->partyBoxSlot(i, 1001);
                        sav->fixParty();
                    }
                }
            }
        }
        else if (cursorPos > maxPkmInBox &&
                 cursorPos - (maxPkmInBox + 1) < TitleLoader::save->partyCount())
        {
            if (TitleLoader::save->partyCount() > 1)
            {
                TitleLoader::save->pkm(
                    *TitleLoader::save->emptyPkm(), cursorPos - (maxPkmInBox + 1));
                TitleLoader::save->fixParty();
            }
            else
            {
                Gui::warn(i18n::localize("NO_PARTY_EMPTY"));
            }
        }
    }
    menu = false;
    return true;
}
