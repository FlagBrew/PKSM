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

#include "EditSelectorScreen.hpp"
#include "gui.hpp"
#include "loader.hpp"
#include "EditorScreen.hpp"
#include "ClickButton.hpp"
#include "AccelButton.hpp"
#include "SavLGPE.hpp"
#include <memory>

extern int bobPointer();
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
            char input[18] = {0};
            SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
            input[16] = '\0';
            input[17] = '\0';
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
            char input[34] = {0};
            SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
            input[32] = '\0';
            input[33] = '\0';
            if (ret == SWKBD_BUTTON_CONFIRM)
            {
                TitleLoader::save->boxName(box, input);
            }
        }
        break;
        case Generation::LGPE:
        // Nothing happens
        break;
    }
}

static bool wirelessStuff() { return false; }

bool EditSelectorScreen::doQR()
{
    u8* data = nullptr;
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

    QRScanner::init(initMode, data);

    if (data != nullptr)
    {
        std::shared_ptr<PKX> pkm = nullptr;

        switch (TitleLoader::save->generation())
        {
            case Generation::FOUR:
                pkm = std::make_shared<PK4>(data, true);
                break;
            case Generation::FIVE:
                pkm = std::make_shared<PK5>(data, true);
                break;
            case Generation::SIX:
                pkm = std::make_shared<PK6>(data, true);
                break;
            case Generation::SEVEN:
                pkm = std::make_shared<PK7>(data, true);
                break;
            default:
                break;
        }

        if (pkm) // Should be true, but just make sure
        {
            int slot = cursorPos ? cursorPos - 1 : 0; // make sure it writes to a good position, AKA not the title bar
            TitleLoader::save->pkm(*pkm, box, slot);
        }

        delete data;
    }
    return true;
}

EditSelectorScreen::EditSelectorScreen()
{
    viewer = std::make_shared<ViewerScreen>(nullptr, false);
    
    buttons.push_back(new Button(283, 211, 34, 28, [](){ Gui::screenBack(); return true; }, ui_sheet_button_back_idx, "", 0.0f, 0));
    buttons.push_back(new ClickButton(32, 15, 164, 24, [this](){ return this->clickIndex(0); }, ui_sheet_res_null_idx, "", 0.0f, 0));
    buttons.push_back(new Button(4, 212, 33, 28, &wirelessStuff, ui_sheet_button_wireless_idx, "", 0.0f, 0));
    buttons.push_back(new AccelButton(8, 15, 17, 24, [this](){ return this->lastBox(); }, ui_sheet_res_null_idx, "", 0.0f, 0, 10, 5));
    buttons.push_back(new AccelButton(189, 15, 17, 24, [this](){ return this->nextBox(); }, ui_sheet_res_null_idx, "", 0.0f, 0, 10, 5));

    // Pokemon buttons
    u16 y = 45;
    for (u8 row = 0; row < 5; row++)
    {
        u16 x = 4;
        for (u8 column = 0; column < 6; column++)
        {
            pkmButtons[row*6 + column] = new ClickButton(x, y, 34, 30, [this, row, column](){ return this->clickIndex(row * 6 + column + 1); }, ui_sheet_res_null_idx, "", 0.0f, 0);
            x += 34;
        }
        y += 30;
    }
    for (int i = 0; i < 6; i++)
    {
        int x = (i % 2 == 0 ? 221 : 271);
        int y = (i % 2 == 0 ? 50 + 45 * (i / 2) : 66 + 45 * (i / 2));
        pkmButtons[30 + i] = new ClickButton(x, y, 34, 30, [this, i](){ return this->clickIndex(31 + i); }, ui_sheet_res_null_idx, "", 0.0f, 0);
    }
    TitleLoader::save->cryptBoxData(true);
}

EditSelectorScreen::~EditSelectorScreen()
{
    for (Button* button : buttons)
    {
        delete button;
    }
    
    for (Button* button : pkmButtons)
    {
        delete button;
    }

    if (TitleLoader::save->generation() == Generation::LGPE)
    {
        ((SavLGPE*)TitleLoader::save.get())->compressBox();
    }
    TitleLoader::save->cryptBoxData(false);
}

void EditSelectorScreen::draw() const
{
    std::shared_ptr<PKX> infoMon = moveMon;
    if (!infoMon && cursorPos != 0)
    {
        if (cursorPos < 31)
        {
            infoMon = TitleLoader::save->pkm(box, cursorPos - 1);
        }
        else
        {
            infoMon = TitleLoader::save->pkm(cursorPos - 31);
        }
    }
    if (infoMon && infoMon->species() == 0)
    {
        infoMon = nullptr;
    }
    //std::shared_ptr<PKX> infoMon = cursorPos == 0 ? nullptr : (cursorPos < 31 ? TitleLoader::save->pkm(box, cursorPos - 1) : TitleLoader::save->pkm(cursorPos - 31));
    C2D_SceneBegin(g_renderTargetBottom);
    Gui::sprite(ui_sheet_emulated_bg_bottom_blue, 0, 0);
    Gui::sprite(ui_sheet_bg_style_bottom_idx, 0, 0);
    Gui::sprite(ui_sheet_bar_bottom_blue_idx, 0, 216);
    Gui::sprite(ui_sheet_stripe_camera_idx, 218, 14);
    Gui::dynamicText("\uE004+\uE005 \uE01E", 311, 15, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, true);

    Gui::sprite(ui_sheet_bar_boxname_with_arrows_idx, 7, 15);
    Gui::sprite(ui_sheet_storage_box_corner_idx, 2, 44);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_horizontal_idx, 202, 44);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_vertical_idx, 2, 193);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_both_idx, 202, 193);
    for (Button* b : buttons)
    {
        b->draw();
    }

    for (int i = 0; i < 6; i++)
    {
        int x = (i % 2 == 0 ? 214 : 264);
        int y = (i % 2 == 0 ? 43 + 45 * (i / 2) : 59 + 45 * (i / 2));
        Gui::sprite(ui_sheet_icon_party_idx, x, y);
    }

    u16 y = 45;
    for (u8 row = 0; row < 5; row++)
    {
        u16 x = 4;
        for (u8 column = 0; column < 6; column++)
        {
            if (TitleLoader::save->generation() == Generation::LGPE && row * 6 + column + box * 30 >= TitleLoader::save->maxSlot())
            {
                C2D_DrawRectSolid(x, y, 0.5f, 34, 30, C2D_Color32(128, 128, 128, 128));
            }
            else
            {
                std::unique_ptr<PKX> pokemon = TitleLoader::save->pkm(box, row * 6 + column);
                if (pokemon->species() > 0)
                {
                    Gui::pkm(pokemon.get(), x, y);
                }
            }
            x += 34;
        }
        y += 30;
    }

    for (int i = 0; i < TitleLoader::save->partyCount(); i++)
    {
        int x = (i % 2 == 0 ? 221 : 271);
        int y = (i % 2 == 0 ? 50 + 45 * (i / 2) : 66 + 45 * (i / 2));
        std::unique_ptr<PKX> pokemon = TitleLoader::save->pkm(i);
        if (pokemon->species() > 0)
        {
            Gui::pkm(pokemon.get(), x, y);
        }
    }

    Gui::dynamicText(25, 18, 164, TitleLoader::save->boxName(box), FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK);

    if (cursorPos == 0)
    {
        int dy = bobPointer();
        if (moveMon)
        {
            Gui::pkm(moveMon.get(), 94, 5 + dy);
        }
        Gui::sprite(ui_sheet_pointer_arrow_idx, 106, -4 + dy);
    }
    else if (cursorPos < 31)
    {
        int tempIndex = cursorPos - 1;
        int yMod = (tempIndex / 6) * 30 + bobPointer();
        if (moveMon)
        {
            Gui::pkm(moveMon.get(), 9 + (tempIndex % 6) * 34, 39 + yMod);
        }
        Gui::sprite(ui_sheet_pointer_arrow_idx, 21 + (tempIndex % 6) * 34, 30 + yMod);
    }
    else
    {
        int x = 238 + ((cursorPos - 1) % 2) * 50;
        int y = ((cursorPos - 1) % 2 == 0 ? 35 : 51) + (((cursorPos - 1) - 30) / 2) * 45 + bobPointer();
        if (moveMon)
        {
            Gui::pkm(moveMon.get(), x - 12, y + 9);
        }
        Gui::sprite(ui_sheet_pointer_arrow_idx, x, y);
    }

    if (infoMon)
    {
        Gui::dynamicText(GFX_BOTTOM, 224, StringUtils::format(i18n::localize("EDITOR_IDS"), infoMon->TID(), infoMon->SID(), infoMon->TSV()),
                        FONT_SIZE_9, FONT_SIZE_9, COLOR_BLACK);
    }

    viewer->draw();
}

void EditSelectorScreen::update(touchPosition* touch)
{
    static bool sleep = true;
    static int sleepTimer = 10;

    std::shared_ptr<PKX> infoMon = moveMon;
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
    if (infoMon && infoMon->species() == 0)
    {
        infoMon = nullptr;
    }
    viewer->setPkm(infoMon);

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

    u32 downKeys = hidKeysDown();
    u32 heldKeys = hidKeysHeld();

    for (Button* button : pkmButtons)
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
            }
            if (tmpMon && tmpMon->species() == 0)
            {
                tmpMon = nullptr;
            }
            TitleLoader::save->pkm(*moveMon, box, cursorPos - 1);
            moveMon = tmpMon;
        }
        else
        {
            editPokemon(infoMon);
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
        if (cursorPos > 0)
        {
            if (cursorPos > 30)
            {
                if (!moveMon)
                {
                    moveMon = TitleLoader::save->pkm(cursorPos - 31)->clone();
                    if (moveMon && moveMon->species() == 0)
                    {
                        moveMon = nullptr;
                    }
                }
                else
                {
                    std::shared_ptr<PKX> tmpMon = TitleLoader::save->pkm(cursorPos - 31);
                    if (tmpMon->species() == 0)
                    {
                        tmpMon = nullptr;
                    }
                    TitleLoader::save->pkm(*moveMon, cursorPos - 31);
                    moveMon = tmpMon;
                }
            }
            else if (box * 30 + cursorPos - 1 < TitleLoader::save->maxSlot())
            {
                if (!moveMon)
                {
                    moveMon = TitleLoader::save->pkm(box, cursorPos - 1);
                    if (moveMon && moveMon->species() == 0)
                    {
                        moveMon = nullptr;
                    }
                }
                else
                {
                    std::shared_ptr<PKX> tmpMon = TitleLoader::save->pkm(box, cursorPos - 1);
                    if (tmpMon->species() == 0)
                    {
                        tmpMon = nullptr;
                    }
                    TitleLoader::save->pkm(*moveMon, box, cursorPos - 1);
                    moveMon = tmpMon;
                }
            }
        }
    }
    else if ((heldKeys & KEY_L) && (heldKeys & KEY_R))
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
            lastBox();
            cursorPos = 30;
        }
        else if (cursorPos == 0)
        {
            lastBox();
        }
        sleep = true;
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
        sleep = true;
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
        sleep = true;
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
        sleep = true;
        sleepTimer = 10;
    }
    else if (downKeys & KEY_R)
    {
        nextBox();
        sleep = true;
        sleepTimer = 10;
    }
    else if (downKeys & KEY_L)
    {
        lastBox();
        sleep = true;
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
                lastBox();
                cursorPos = 30;
            }
            else if (cursorPos == 0)
            {
                lastBox();
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
            lastBox();
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

bool EditSelectorScreen::lastBox()
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

bool EditSelectorScreen::editPokemon(std::shared_ptr<PKX> pkm)
{
    if (cursorPos < 31 && cursorPos != 0)
    {
        Gui::setScreen(std::make_unique<EditorScreen>(viewer, pkm, box, cursorPos - 1));
        return true;
    }
    else if (cursorPos > 30)
    {
        Gui::setScreen(std::make_unique<EditorScreen>(viewer, pkm, 0xFF, cursorPos - 31));
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
        else if (cursorPos < 31 && box * 30 + cursorPos - 1 < TitleLoader::save->maxSlot())
        {
            editPokemon(TitleLoader::save->pkm(box, cursorPos - 1));
            dirtyBack = true;
        }
        else if (cursorPos > 30)
        {
            editPokemon(TitleLoader::save->pkm(cursorPos - 31));
            dirtyBack = true;
        }
        return true;
    }
    else
    {
        cursorPos = i;
        return false;
    }
}