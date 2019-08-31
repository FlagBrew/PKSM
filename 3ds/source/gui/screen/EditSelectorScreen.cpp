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

#include "EditSelectorScreen.hpp"
#include "AccelButton.hpp"
#include "ClickButton.hpp"
#include "EditorScreen.hpp"
#include "SavLGPE.hpp"
#include "gui.hpp"
#include "loader.hpp"
#include <memory>

static bool dirtyBack = true;

void EditSelectorScreen::changeBoxName()
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
                TitleLoader::save->boxName(box, input);
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
                TitleLoader::save->boxName(box, input);
            }
        }
        break;
        case Generation::LGPE:
        case Generation::UNUSED:
            // Nothing happens
            break;
    }
}

bool EditSelectorScreen::doQR()
{
    QRMode initMode;
    switch (TitleLoader::save->generation())
    {
        case Generation::FOUR:
            initMode = QRMode::PKM4;
            break;
        case Generation::FIVE:
            initMode = QRMode::PKM5;
            break;
        case Generation::SIX:
            initMode = QRMode::PKM6;
            break;
        case Generation::SEVEN:
        default:
            initMode = QRMode::PKM7;
            break;
    }

    std::vector<u8> data = QRScanner::scan(initMode);

    if (!data.empty())
    {
        std::shared_ptr<PKX> pkm = nullptr;

        switch (TitleLoader::save->generation())
        {
            case Generation::FOUR:
                pkm = std::make_shared<PK4>(data.data(), true);
                break;
            case Generation::FIVE:
                pkm = std::make_shared<PK5>(data.data(), true);
                break;
            case Generation::SIX:
                pkm = std::make_shared<PK6>(data.data(), true);
                break;
            case Generation::SEVEN:
                pkm = std::make_shared<PK7>(data.data(), true);
                break;
            default:
                break;
        }

        if (pkm) // Should be true, but just make sure
        {
            int slot = cursorPos ? cursorPos - 1 : 0; // make sure it writes to a good position, AKA not the title bar
            TitleLoader::save->pkm(pkm, box, slot, false);
        }
    }
    return true;
}

EditSelectorScreen::EditSelectorScreen() : Screen(i18n::localize("A_SELECT") + '\n' + i18n::localize("X_CLONE") + '\n' + i18n::localize("B_BACK"))
{
    addOverlay<ViewOverlay>(infoMon, false);

    buttons.push_back(std::make_unique<ClickButton>(283, 211, 34, 28, [this]() { return goBack(); }, ui_sheet_button_back_idx, "", 0.0f, 0));
    instructions.addBox(false, 25, 15, 164, 24, COLOR_GREY, i18n::localize("A_BOX_NAME"), COLOR_WHITE);
    buttons.push_back(std::make_unique<ClickButton>(25, 15, 164, 24, [this]() { return this->clickIndex(0); }, ui_sheet_res_null_idx, "", 0.0f, 0));
    buttons.push_back(std::make_unique<AccelButton>(8, 15, 17, 24, [this]() { return this->prevBox(); }, ui_sheet_res_null_idx, "", 0.0f, 0, 10, 5));
    buttons.push_back(
        std::make_unique<AccelButton>(189, 15, 17, 24, [this]() { return this->nextBox(); }, ui_sheet_res_null_idx, "", 0.0f, 0, 10, 5));
    auto cameraButtonText = Gui::parseText("\uE004+\uE005 \uE01E", FONT_SIZE_14, 0.0f);
    instructions.addCircle(false, 310 - cameraButtonText->maxWidth(FONT_SIZE_14) / 2, 24, 8, COLOR_GREY);
    instructions.addLine(false, 310 - cameraButtonText->maxWidth(FONT_SIZE_14) / 2, 24, 310 - cameraButtonText->maxWidth(FONT_SIZE_14) / 2, 44, 4, COLOR_GREY);
    instructions.addBox(false, 222 - cameraButtonText->maxWidth(FONT_SIZE_14) / 2, 44, 90, 16, COLOR_GREY, i18n::localize("QR_SCANNER"), COLOR_WHITE);
    buttons.push_back(std::make_unique<ClickButton>(310 - cameraButtonText->maxWidth(FONT_SIZE_14), 16, cameraButtonText->maxWidth(FONT_SIZE_14) + 2,
        16, [this]() { return this->doQR(); }, ui_sheet_res_null_idx, "\uE004+\uE005 \uE01E", FONT_SIZE_14, COLOR_BLACK));

    // Pokemon buttons
    u16 y = 45;
    for (u8 row = 0; row < 5; row++)
    {
        u16 x = 4;
        for (u8 column = 0; column < 6; column++)
        {
            pkmButtons[row * 6 + column] = std::make_unique<ClickButton>(
                x, y, 34, 30, [this, row, column]() { return this->clickIndex(row * 6 + column + 1); }, ui_sheet_res_null_idx, "", 0.0f, 0);
            x += 34;
        }
        y += 30;
    }
    for (int i = 0; i < 6; i++)
    {
        int x = (i % 2 == 0 ? 221 : 271);
        int y = (i % 2 == 0 ? 50 + 45 * (i / 2) : 66 + 45 * (i / 2));
        pkmButtons[30 + i] =
            std::make_unique<ClickButton>(x, y, 34, 30, [this, i]() { return this->clickIndex(31 + i); }, ui_sheet_res_null_idx, "", 0.0f, 0);
    }

    viewerButtons.push_back(std::make_unique<ClickButton>(212, 47, 108, 28, [this]() { return this->editPokemon(); }, ui_sheet_button_editor_idx,
        "\uE000: " + i18n::localize("EDIT"), FONT_SIZE_12, COLOR_BLACK));
    viewerButtons.push_back(std::make_unique<ClickButton>(212, 78, 108, 28,
        [this]() {
            menu = false;
            return this->releasePokemon();
        },
        ui_sheet_button_editor_idx, "\uE003: " + i18n::localize("RELEASE"), FONT_SIZE_12, COLOR_BLACK));
    viewerButtons.push_back(std::make_unique<ClickButton>(212, 109, 108, 28,
        [this]() {
            menu = false;
            return this->clonePkm();
        },
        ui_sheet_button_editor_idx, "\uE002: " + i18n::localize("CLONE"), FONT_SIZE_12, COLOR_BLACK));
    TitleLoader::save->cryptBoxData(true);
    box = TitleLoader::save->currentBox() % TitleLoader::save->maxBoxes();
}

EditSelectorScreen::~EditSelectorScreen()
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
    TitleLoader::save->currentBox((u8)box);
}

void EditSelectorScreen::drawBottom() const
{
    Gui::sprite(ui_sheet_emulated_bg_bottom_blue, 0, 0);
    Gui::sprite(ui_sheet_bg_style_bottom_idx, 0, 0);
    Gui::sprite(ui_sheet_bar_bottom_blue_idx, 0, 216);
    Gui::sprite(ui_sheet_stripe_camera_idx, 218, 14);

    Gui::sprite(ui_sheet_bar_boxname_with_arrows_idx, 7, 15);
    Gui::sprite(ui_sheet_storage_box_corner_idx, 2, 44);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_horizontal_idx, 202, 44);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_vertical_idx, 2, 193);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_both_idx, 202, 193);
    for (auto& b : buttons)
    {
        b->draw();
    }

    for (int i = 0; i < 6; i++)
    {
        int x = (i % 2 == 0 ? 214 : 264);
        int y = (i % 2 == 0 ? 43 + 45 * (i / 2) : 59 + 45 * (i / 2));
        Gui::sprite(ui_sheet_icon_party_idx, x, y);
        if (TitleLoader::save->generation() == Generation::LGPE)
        {
            Gui::sprite(ui_sheet_emulated_party_indicator_1_idx + i, x + 34, y + 30);
        }
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
            if (TitleLoader::save->generation() == Generation::LGPE && row * 6 + column + box * 30 >= TitleLoader::save->maxSlot())
            {
                Gui::drawSolidRect(x, y, 34, 30, C2D_Color32(128, 128, 128, 128));
            }
            else
            {
                std::shared_ptr<PKX> pokemon = TitleLoader::save->pkm(box, row * 6 + column);
                if (pokemon->species() > 0)
                {
                    Gui::pkm(*pokemon, x, y);
                }
                if (TitleLoader::save->generation() == Generation::LGPE)
                {
                    int partySlot = std::distance(partyPkm, std::find(partyPkm, partyPkm + 6, box * 30 + row * 6 + column));
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

    for (int i = 0; i < TitleLoader::save->partyCount(); i++)
    {
        int x                        = (i % 2 == 0 ? 221 : 271);
        int y                        = (i % 2 == 0 ? 50 + 45 * (i / 2) : 66 + 45 * (i / 2));
        std::shared_ptr<PKX> pokemon = TitleLoader::save->pkm(i);
        if (pokemon->species() > 0)
        {
            Gui::pkm(*pokemon, x, y);
        }
    }

    Gui::text(TitleLoader::save->boxName(box), 107, 18, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);

    if (cursorPos == 0)
    {
        int dy = Gui::pointerBob();
        if (moveMon)
        {
            Gui::pkm(*moveMon, 94, 5 + dy);
        }
        Gui::sprite(ui_sheet_pointer_arrow_idx, 106, -4 + dy);
    }
    else if (cursorPos < 31)
    {
        int tempIndex = cursorPos - 1;
        int yMod      = (tempIndex / 6) * 30 + Gui::pointerBob();
        if (moveMon)
        {
            Gui::pkm(*moveMon, 9 + (tempIndex % 6) * 34, 39 + yMod);
        }
        Gui::sprite(ui_sheet_pointer_arrow_idx, 21 + (tempIndex % 6) * 34, 30 + yMod);
    }
    else
    {
        int x = 238 + ((cursorPos - 1) % 2) * 50;
        int y = ((cursorPos - 1) % 2 == 0 ? 35 : 51) + (((cursorPos - 1) - 30) / 2) * 45 + Gui::pointerBob();
        if (moveMon)
        {
            Gui::pkm(*moveMon, x - 12, y + 9);
        }
        Gui::sprite(ui_sheet_pointer_arrow_idx, x, y);
    }

    if (infoMon)
    {
        Gui::text(StringUtils::format(i18n::localize("EDITOR_IDS"), infoMon->formatTID(), infoMon->formatSID(), infoMon->TSV()), 160, 224,
            FONT_SIZE_9, FONT_SIZE_9, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    }

    if (menu)
    {
        Gui::drawSolidRect(0, 0, 320, 240, COLOR_MASKBLACK);
        for (auto& button : viewerButtons)
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
    static bool sleep     = true;
    static int sleepTimer = 10;

    if (moveMon)
    {
        infoMon = moveMon;
    }
    else if (cursorPos != 0)
    {
        if (cursorPos < 31)
        {
            if (box * 30 + cursorPos - 1 < TitleLoader::save->maxSlot())
            {
                infoMon = TitleLoader::save->pkm(box, cursorPos - 1);
            }
        }
        else
        {
            infoMon = TitleLoader::save->pkm(cursorPos - 31);
        }
    }
    else
    {
        infoMon = nullptr;
    }
    if (infoMon && (infoMon->encryptionConstant() == 0 && infoMon->species() == 0))
    {
        infoMon = nullptr;
    }

    u32 downKeys = hidKeysDown();
    u32 heldKeys = hidKeysHeld();

    if (menu)
    {
        if (!dirtyBack)
        {
            if (buttons[0]->clicked(touch))
            {
                dirtyBack = true;
            }
            if (buttons[0]->update(touch))
            {
                return;
            }
        }
        else
        {
            if (!buttons[0]->clicked(touch))
            {
                dirtyBack = false;
            }
        }
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
            if (i == 0)
            {
                if (!dirtyBack)
                {
                    if (buttons[i]->clicked(touch))
                    {
                        dirtyBack = true;
                    }
                    if (buttons[i]->update(touch))
                    {
                        return;
                    }
                }
                else
                {
                    if (!buttons[i]->clicked(touch))
                    {
                        dirtyBack = false;
                    }
                }
            }
            else
            {
                buttons[i]->update(touch);
            }
        }

        for (auto& button : pkmButtons)
        {
            button->update(touch);
        }

        if (sleepTimer < 0 && sleep)
        {
            sleep = false;
        }

        if (downKeys & KEY_A)
        {
            if (cursorPos == 0)
            {
                Gui::setNextKeyboardFunc(std::bind(&EditSelectorScreen::changeBoxName, this));
            }
            else if (moveMon && cursorPos < 31)
            {
                std::shared_ptr<PKX> tmpMon = nullptr;
                if (box * 30 + cursorPos - 1 < TitleLoader::save->maxSlot())
                {
                    tmpMon = TitleLoader::save->pkm(box, cursorPos - 1);
                    if (tmpMon && (tmpMon->encryptionConstant() == 0 && tmpMon->species() == 0))
                    {
                        tmpMon = nullptr;
                    }
                    TitleLoader::save->pkm(moveMon, box, cursorPos - 1, false);
                    moveMon = tmpMon;
                }
            }
            else if (moveMon)
            {
                std::shared_ptr<PKX> tmpMon = nullptr;
                if (cursorPos - 31 < TitleLoader::save->partyCount())
                {
                    tmpMon = TitleLoader::save->pkm(cursorPos - 31);
                }
                if (tmpMon && (tmpMon->encryptionConstant() == 0 && tmpMon->species() == 0))
                {
                    tmpMon = nullptr;
                }
                TitleLoader::save->pkm(moveMon, cursorPos - 31);
                moveMon = tmpMon;
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
                Gui::screenBack();
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
        else if (downKeys & KEY_LEFT)
        {
            if (cursorPos >= 31)
            {
                if (cursorPos == 31)
                {
                    cursorPos = 6;
                }
                else if (cursorPos == 33)
                {
                    cursorPos = 18;
                }
                else if (cursorPos == 35)
                {
                    cursorPos = 24;
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
                cursorPos = 30;
            }
            else if (cursorPos == 0)
            {
                prevBox();
            }
            sleep      = true;
            sleepTimer = 10;
        }
        else if (downKeys & KEY_RIGHT)
        {
            if (cursorPos == 0)
            {
                nextBox();
            }
            else if (cursorPos < 30 || (cursorPos >= 31 && cursorPos % 2 == 1))
            {
                cursorPos++;
            }
            else if (cursorPos == 30)
            {
                nextBox();
                cursorPos = 1;
            }
            else if (cursorPos == 32)
            {
                cursorPos = 1;
            }
            else if (cursorPos == 34)
            {
                cursorPos = 13;
            }
            else if (cursorPos == 36)
            {
                cursorPos = 25;
            }
            sleep      = true;
            sleepTimer = 10;
        }
        else if (downKeys & KEY_UP)
        {
            if (cursorPos == 0)
            {
                cursorPos = 27;
            }
            else if (cursorPos <= 6)
            {
                cursorPos = 0;
            }
            else if (cursorPos >= 31)
            {
                if (cursorPos > 32)
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
                cursorPos -= 6;
            }
            sleep      = true;
            sleepTimer = 10;
        }
        else if (downKeys & KEY_DOWN)
        {
            if (cursorPos >= 31)
            {
                if (cursorPos < 35)
                {
                    cursorPos += 2;
                }
                else
                {
                    cursorPos -= 4;
                }
            }
            else if (cursorPos >= 25)
            {
                cursorPos = 0;
            }
            else if (cursorPos == 0)
            {
                cursorPos = 3;
            }
            else
            {
                cursorPos += 6;
            }
            sleep      = true;
            sleepTimer = 10;
        }
        else if (downKeys & KEY_R)
        {
            nextBox();
            sleep      = true;
            sleepTimer = 10;
        }
        else if (downKeys & KEY_L)
        {
            prevBox();
            sleep      = true;
            sleepTimer = 10;
        }
        else if (sleepTimer < 0)
        {
            if (heldKeys & KEY_LEFT)
            {
                if (cursorPos >= 31)
                {
                    if (cursorPos == 31)
                    {
                        cursorPos = 6;
                    }
                    else if (cursorPos == 33)
                    {
                        cursorPos = 18;
                    }
                    else if (cursorPos == 35)
                    {
                        cursorPos = 30;
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
                    cursorPos = 30;
                }
                else if (cursorPos == 0)
                {
                    prevBox();
                }
                sleep = true;
            }
            else if (heldKeys & KEY_RIGHT)
            {
                if (cursorPos == 0)
                {
                    nextBox();
                }
                else if (cursorPos < 30 || (cursorPos >= 31 && cursorPos % 2 == 1))
                {
                    cursorPos++;
                }
                else if (cursorPos == 30)
                {
                    nextBox();
                    cursorPos = 1;
                }
                else if (cursorPos == 32)
                {
                    cursorPos = 1;
                }
                else if (cursorPos == 34)
                {
                    cursorPos = 13;
                }
                else if (cursorPos == 36)
                {
                    cursorPos = 25;
                }
                sleep = true;
            }
            else if (heldKeys & KEY_UP)
            {
                if (cursorPos == 0)
                {
                    cursorPos = 27;
                }
                else if (cursorPos <= 6)
                {
                    cursorPos = 0;
                }
                else if (cursorPos >= 31)
                {
                    if (cursorPos > 32)
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
                    cursorPos -= 6;
                }
                sleep = true;
            }
            else if (heldKeys & KEY_DOWN)
            {
                if (cursorPos >= 31)
                {
                    if (cursorPos < 35)
                    {
                        cursorPos += 2;
                    }
                    else
                    {
                        cursorPos -= 4;
                    }
                }
                else if (cursorPos >= 25)
                {
                    cursorPos = 0;
                }
                else if (cursorPos == 0)
                {
                    cursorPos = 3;
                }
                else
                {
                    cursorPos += 6;
                }
                sleep = true;
            }
            else if (heldKeys & KEY_R)
            {
                nextBox();
                sleep = true;
            }
            else if (heldKeys & KEY_L)
            {
                prevBox();
                sleep = true;
            }

            if (sleep)
                sleepTimer = 10;
        }

        if (sleep)
        {
            sleepTimer--;
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
    if (cursorPos < 31 && cursorPos != 0)
    {
        if (box * 30 + cursorPos - 1 < TitleLoader::save->maxSlot())
        {
            justSwitched = true;
            Gui::setScreen(std::make_unique<EditorScreen>(TitleLoader::save->pkm(box, cursorPos - 1), box, cursorPos - 1));
            return true;
        }
    }
    else if (cursorPos > 30)
    {
        justSwitched = true;
        Gui::setScreen(std::make_unique<EditorScreen>(TitleLoader::save->pkm(cursorPos - 31), 0xFF, cursorPos - 31));
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
            Gui::setNextKeyboardFunc(std::bind(&EditSelectorScreen::changeBoxName, this));
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
    if (cursorPos > 0)
    {
        if (cursorPos > 30)
        {
            if (!moveMon)
            {
                moveMon = TitleLoader::save->pkm(cursorPos - 31);
                if (moveMon && (moveMon->encryptionConstant() == 0 && moveMon->species() == 0))
                {
                    moveMon = nullptr;
                }
            }
            else
            {
                std::shared_ptr<PKX> tmpMon = TitleLoader::save->pkm(cursorPos - 31);
                if ((tmpMon->encryptionConstant() == 0 && tmpMon->species() == 0))
                {
                    tmpMon = nullptr;
                }
                TitleLoader::save->pkm(moveMon, cursorPos - 31);
                moveMon = tmpMon;
                TitleLoader::save->fixParty();
            }
        }
        else if (box * 30 + cursorPos - 1 < TitleLoader::save->maxSlot())
        {
            if (!moveMon)
            {
                moveMon = TitleLoader::save->pkm(box, cursorPos - 1);
                if (moveMon && (moveMon->encryptionConstant() == 0 && moveMon->species() == 0))
                {
                    moveMon = nullptr;
                }
            }
            else
            {
                std::shared_ptr<PKX> tmpMon = TitleLoader::save->pkm(box, cursorPos - 1);
                if ((tmpMon->encryptionConstant() == 0 && tmpMon->species() == 0))
                {
                    tmpMon = nullptr;
                }
                TitleLoader::save->pkm(moveMon, box, cursorPos - 1, false);
                moveMon = tmpMon;
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
        Gui::screenBack();
    }
    return true;
}

bool EditSelectorScreen::releasePokemon()
{
    if (cursorPos != 0 && Gui::showChoiceMessage(i18n::localize("BANK_CONFIRM_RELEASE")))
    {
        if (cursorPos < 31 && box * 30 + cursorPos - 1 < TitleLoader::save->maxSlot())
        {
            TitleLoader::save->pkm(TitleLoader::save->emptyPkm(), box, cursorPos - 1, false);
            if (TitleLoader::save->generation() == Generation::LGPE)
            {
                SavLGPE* sav = (SavLGPE*)TitleLoader::save.get();
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
        else if (cursorPos > 30 && cursorPos - 31 < TitleLoader::save->partyCount())
        {
            if (TitleLoader::save->partyCount() > 1)
            {
                TitleLoader::save->pkm(TitleLoader::save->emptyPkm(), cursorPos - 31);
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
