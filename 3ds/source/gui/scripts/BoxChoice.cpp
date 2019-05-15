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

#include "BoxChoice.hpp"
#include "AccelButton.hpp"
#include "ClickButton.hpp"
#include "Configuration.hpp"
#include "TitleLoadScreen.hpp"
#include "banks.hpp"
#include "gui.hpp"
#include "random.hpp"

// storage, box, slot
auto result = std::make_tuple(0, -1, -1);

static bool backHeld = false;

extern int bobPointer();

BoxChoice::BoxChoice(bool doCrypt) : doCrypt(doCrypt)
{
    mainButtons[0] = new Button(
        212, 47, 108, 28, [this]() { return this->showViewer(); }, ui_sheet_button_editor_idx, i18n::localize("VIEW"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[1] = new Button(4, 212, 33, 28, [this]() { return false; }, ui_sheet_res_null_idx, "", 0.0f, 0);
    mainButtons[2] = new Button(283, 211, 34, 28, [this]() { return this->backButton(); }, ui_sheet_button_back_idx, "", 0.0f, 0);
    mainButtons[3] = new AccelButton(8, 15, 17, 24, [this]() { return this->prevBox(true); }, ui_sheet_res_null_idx, "", 0.0f, 0, 10, 5);
    mainButtons[4] = new AccelButton(189, 15, 17, 24, [this]() { return this->nextBox(true); }, ui_sheet_res_null_idx, "", 0.0f, 0, 10, 5);

    // Pokemon buttons
    u16 y = 45;
    for (u8 row = 0; row < 5; row++)
    {
        u16 x = 4;
        for (u8 column = 0; column < 6; column++)
        {
            clickButtons[row * 6 + column] = new ClickButton(
                x, y, 34, 30, [this, row, column]() { return this->clickBottomIndex(row * 6 + column + 1); }, ui_sheet_res_null_idx, "", 0.0f, 0);
            x += 34;
        }
        y += 30;
    }

    if (doCrypt)
    {
        TitleLoader::save->cryptBoxData(true);
    }
}

BoxChoice::~BoxChoice()
{
    for (auto button : mainButtons)
    {
        delete button;
    }
    for (auto button : clickButtons)
    {
        delete button;
    }
    if (doCrypt)
    {
        TitleLoader::save->cryptBoxData(false);
    }
}

void BoxChoice::drawBottom() const
{
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
                Gui::drawSolidRect(x, y, 34, 30, C2D_Color32(128, 128, 128, 128));
            }
            else
            {
                std::shared_ptr<PKX> pokemon = TitleLoader::save->pkm(boxBox, row * 6 + column);
                if (pokemon->species() > 0)
                {
                    Gui::pkm(*pokemon, x, y);
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
            Gui::sprite(ui_sheet_pointer_arrow_idx, 106, -4 + dy);
        }
        else
        {
            int tempIndex = cursorIndex - 1;
            int yMod      = (tempIndex / 6) * 30 + bobPointer();
            Gui::sprite(ui_sheet_pointer_arrow_idx, 21 + (tempIndex % 6) * 34, 30 + yMod);
        }
    }

    if (currentOverlay)
    {
        Gui::drawSolidRect(0, 0, 320, 240, COLOR_MASKBLACK);
    }
}

void BoxChoice::drawTop() const
{
    if (!currentOverlay)
    {
        Gui::sprite(ui_sheet_emulated_bg_top_green, 0, 0);
        Gui::sprite(ui_sheet_bg_style_top_idx, 0, 0);
        Gui::backgroundAnimatedTop();
        Gui::sprite(ui_sheet_bar_arc_top_green_idx, 0, 0);

        Gui::sprite(ui_sheet_textbox_pksm_idx, 261, 3);
        Gui::text("PKSM", 394, 7, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::RIGHT, TextPosY::TOP);

        Gui::sprite(ui_sheet_bar_boxname_empty_idx, 44, 21);
        Gui::text("\uE004", 45 + 24 / 2, 24, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
        Gui::text("\uE005", 225 + 24 / 2, 24, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
        Gui::text(Banks::bank->boxName(storageBox), 69 + 156 / 2, 24, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);

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
                Gui::sprite(ui_sheet_pointer_arrow_idx, 147, 2 + dy);
            }
            else
            {
                int tempIndex = cursorIndex - 1;
                int yMod      = (tempIndex / 6) * 30 + bobPointer();
                Gui::sprite(ui_sheet_pointer_arrow_idx, 62 + (tempIndex % 6) * 34, 51 + yMod);
            }
        }

        if (infoMon)
        {
            Gui::text(infoMon->nickname(), 276, 61, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            std::string info = "#" + std::to_string(infoMon->species());
            Gui::text(info, 273, 77, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            info      = i18n::localize("LV") + std::to_string(infoMon->level());
            auto text = Gui::parseText(info, FONT_SIZE_12, 0.0f);
            int width = text->maxWidth(FONT_SIZE_12);
            Gui::text(text, 375 - width, 77, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            if (infoMon->gender() == 0)
            {
                Gui::sprite(ui_sheet_icon_male_idx, 362 - width, 80);
            }
            else if (infoMon->gender() == 1)
            {
                Gui::sprite(ui_sheet_icon_female_idx, 364 - width, 80);
            }
            else if (infoMon->gender() == 2)
            {
                Gui::sprite(ui_sheet_icon_genderless_idx, 364 - width, 80);
            }
            if (infoMon->shiny())
            {
                Gui::sprite(ui_sheet_icon_shiny_idx, 352 - width, 81);
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

            info = infoMon->otName() + '\n' + i18n::localize("LOADER_ID") + std::to_string(infoMon->TID());
            Gui::text(info, 276, 141, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);

            Gui::text(i18n::nature(Configuration::getInstance().language(), infoMon->nature()), 276, 181, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK,
                TextPosX::LEFT, TextPosY::TOP);
            info  = i18n::localize("IV") + ": ";
            text  = Gui::parseText(info, FONT_SIZE_12, 0.0f);
            width = text->maxWidth(FONT_SIZE_12);
            Gui::text(text, 276, 197, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            info = StringUtils::format("%2i/%2i/%2i", infoMon->iv(0), infoMon->iv(1), infoMon->iv(2));
            Gui::text(info, 276 + width + 70 / 2, 197, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
            info = StringUtils::format("%2i/%2i/%2i", infoMon->iv(4), infoMon->iv(5), infoMon->iv(3));
            Gui::text(info, 276 + width + 70 / 2, 209, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
            Gui::format(*infoMon, 276, 213);
        }
    }
}

std::tuple<int, int, int> BoxChoice::run()
{
    while (aptMainLoop() && !finished())
    {
        hidScanInput();
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

        extern C3D_RenderTarget* g_renderTargetTop;
        Gui::target(GFX_TOP);
        C2D_TargetClear(g_renderTargetTop, COLOR_BLACK);
        drawTop();
        Gui::drawCurrentText();

        extern C3D_RenderTarget* g_renderTargetBottom;
        Gui::target(GFX_BOTTOM);
        C2D_TargetClear(g_renderTargetBottom, COLOR_BLACK);
        drawBottom();
        Gui::drawCurrentText();

        touchPosition touch;
        hidTouchRead(&touch);
        update(&touch);

        if (!aptIsHomeAllowed() && aptIsHomePressed())
        {
            Gui::setDoHomeDraw();
        }

        Gui::drawNoHome();

        C3D_FrameEnd(0);
    }
    return result;
}

void BoxChoice::update(touchPosition* touch)
{
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
    if (!currentOverlay)
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
            if (cursorIndex != 0)
            {
                result = std::make_tuple(storageChosen ? 1 : 0, storageChosen ? storageBox : boxBox, cursorIndex);
                done   = true;
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
        else if (buttonCooldown <= 0)
        {
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
                    cursorIndex   = 27;
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
                    cursorIndex   = 0;
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

            if (sleep)
                buttonCooldown = 10;
        }
        if (sleep)
            buttonCooldown--;
    }
    else
    {
        if (kDown & KEY_B)
        {
            backButton();
        }
    }
}

bool BoxChoice::prevBox(bool forceBottom)
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

bool BoxChoice::nextBox(bool forceBottom)
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

bool BoxChoice::backButton()
{
    if (!backHeld)
    {
        backHeld = true;
        if (currentOverlay)
        {
            currentOverlay = nullptr;
        }
        else
        {
            result = std::make_tuple(0, -1, -1);
            done   = true;
        }
    }
    return true;
}

bool BoxChoice::showViewer()
{
    if (cursorIndex == 0 || (!storageChosen && boxBox * 30 + cursorIndex - 1 >= TitleLoader::save->maxSlot()))
    {
        return false;
    }

    if (infoMon && infoMon->species() != 0)
    {
        currentOverlay = std::make_unique<ViewOverlay>(*this, infoMon, true);
    }
    return true;
}

bool BoxChoice::clickBottomIndex(int index)
{
    if (cursorIndex == index && !storageChosen)
    {
        if (cursorIndex != 0)
        {
            result = std::make_tuple(storageChosen ? 1 : 0, storageChosen ? storageBox : boxBox, cursorIndex);
            done   = true;
        }
    }
    else
    {
        cursorIndex   = index;
        storageChosen = false;
    }
    return false;
}
