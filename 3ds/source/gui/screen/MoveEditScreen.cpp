/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2020 Bernardo Giordano, Admiral Fish, piepie62
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

#include "MoveEditScreen.hpp"
#include "ClickButton.hpp"
#include "Configuration.hpp"
#include "MoveOverlay.hpp"
#include "ViewOverlay.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "pkx/PB7.hpp"

MoveEditScreen::MoveEditScreen(std::shared_ptr<pksm::PKX> pkm) : pkm(pkm)
{
    buttons.push_back(std::make_unique<ClickButton>(283, 211, 34, 28,
        [this]() {
            Gui::screenBack();
            return true;
        },
        ui_sheet_button_back_idx, "", 0.0f, COLOR_BLACK));
    for (int i = 0; i < 4; i++)
    {
        buttons.push_back(std::make_unique<ClickButton>(0, 30 + 20 * i, 240, 20,
            [this, i = i]() {
                moveSelected = i;
                return true;
            },
            ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK));
        buttons.push_back(std::make_unique<ClickButton>(0, 140 + 20 * i, 240, 20,
            [this, i = i]() {
                moveSelected = i + 4;
                return true;
            },
            ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK));
    }

    addOverlay<ViewOverlay>(this->pkm, false);
}

void MoveEditScreen::update(touchPosition* touch)
{
    u32 downKeys = hidKeysDown();

    for (size_t i = 0; i < buttons.size(); i++)
    {
        if (buttons[i]->update(touch))
        {
            return;
        }
    }

    if (downKeys & KEY_B)
    {
        Gui::screenBack();
        return;
    }

    if (downKeys & KEY_A)
    {
        changeMove();
    }
    else if (downKeys & KEY_DOWN)
    {
        if (moveSelected < 7)
        {
            moveSelected++;
        }
    }
    else if (downKeys & KEY_UP)
    {
        if (moveSelected > 0)
        {
            moveSelected--;
        }
    }
}

void MoveEditScreen::drawBottom() const
{
    pksm::Language lang = Configuration::getInstance().language();
    Gui::sprite(ui_sheet_emulated_bg_bottom_blue, 0, 0);
    Gui::sprite(ui_sheet_bg_style_bottom_idx, 0, 0);
    Gui::sprite(ui_sheet_bar_arc_bottom_blue_idx, 0, 206);
    Gui::backgroundAnimatedBottom();

    Gui::sprite(ui_sheet_textbox_name_bottom_idx, 0, 1);

    Gui::sprite(ui_sheet_textbox_relearn_moves_idx, 0, 109);
    for (int i = 0; i < 2; i++)
    {
        Gui::sprite(ui_sheet_stripe_move_editor_row_idx, 0, 30 + i * 40);
        Gui::sprite(ui_sheet_stripe_move_editor_row_idx, 0, 139 + i * 40);
    }
    for (int i = 0; i < 4; i++)
    {
        Gui::sprite(ui_sheet_point_small_idx, 15, 37 + i * 20);
        Gui::sprite(ui_sheet_point_small_idx, 15, 146 + i * 20);
    }
    for (auto& button : buttons)
    {
        button->draw();
    }

    Gui::text(
        i18n::localize("MOVES"), 12, 5, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(i18n::localize("RELEARN_MOVES"), 12, 113, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT,
        TextPosY::TOP);

    for (int i = 0; i < 4; i++)
    {
        Gui::text(i18n::move(lang, pkm->move(i)), 24, 32 + i * 20, FONT_SIZE_12, COLOR_BLACK,
            TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SQUISH, 144);
        if (pkm->generation() < pksm::Generation::SIX)
        {
            Gui::text(i18n::localize("EDITOR_NOT_APPLICABLE_GEN"), 24, 141 + i * 20, FONT_SIZE_12,
                COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        }
        else
        {
            Gui::text(i18n::move(lang, pkm->relearnMove(i)), 24, 141 + i * 20, FONT_SIZE_12,
                COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SQUISH, 144);
        }
    }

    if (moveSelected < 4)
    {
        Gui::sprite(ui_sheet_emulated_pointer_horizontal_flipped_idx, 169 + Gui::pointerBob(),
            31 + moveSelected * 20);
        Gui::text("\uE000", 194, 29 + moveSelected * 20, FONT_SIZE_18, COLOR_BLACK, TextPosX::LEFT,
            TextPosY::TOP);
    }
    else
    {
        Gui::sprite(ui_sheet_emulated_pointer_horizontal_flipped_idx, 169 + Gui::pointerBob(),
            140 + (moveSelected - 4) * 20);
        Gui::text("\uE000", 194, 138 + (moveSelected - 4) * 20, FONT_SIZE_18, COLOR_BLACK,
            TextPosX::LEFT, TextPosY::TOP);
    }
}

void MoveEditScreen::changeMove()
{
    if (moveSelected < 4 || pkm->generation() >= pksm::Generation::SIX)
    {
        addOverlay<MoveOverlay>(pkm, moveSelected);
    }
}
