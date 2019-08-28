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

#include "FilterScreen.hpp"
#include "ClickButton.hpp"
#include "MoveOverlay.hpp"
#include "SpeciesOverlay.hpp"
#include "ToggleButton.hpp"
#include "gui.hpp"

FilterScreen::FilterScreen(std::shared_ptr<PKFilter> filter) : filter(filter)
{
    for (int i = 0; i < 4; i++)
    {
        buttons.push_back(std::make_unique<ToggleButton>(26, 126 + 24 * i, 24, 24,
            [this, i]() {
                this->filter->moveEnabled(i, !this->filter->moveEnabled(i));
                return false;
            },
            ui_sheet_checkbox_on_idx, "", 0.0f, 0, ui_sheet_checkbox_off_idx, "", 0.0f, 0, nullptr, true));
        ((ToggleButton*)buttons.back().get())->setState(filter->moveEnabled(i));
        buttons.push_back(std::make_unique<ClickButton>(59, 132 + 24 * i, 15, 12,
            [this, i]() {
                this->addOverlay<MoveOverlay>(this->filter, i);
                return false;
            },
            ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, 0));
    }

    buttons.push_back(std::make_unique<ClickButton>(51, 53, 50, 44,
        [this]() {
            this->addOverlay<SpeciesOverlay>(this->filter);
            return false;
        },
        ui_sheet_icon_party_idx, "", 0.0f, 0));
    buttons.push_back(std::make_unique<ToggleButton>(121, 63, 24, 24,
        [this]() {
            this->filter->speciesEnabled(!this->filter->speciesEnabled());
            return false;
        },
        ui_sheet_checkbox_on_idx, "", 0.0f, 0, ui_sheet_checkbox_off_idx, "", 0.0f, 0, nullptr, true));
    ((ToggleButton*)buttons.back().get())->setState(filter->speciesEnabled());
}

void FilterScreen::drawTop() const
{
    Gui::backgroundTop(false);
    Gui::backgroundAnimatedTop();
}

void FilterScreen::drawBottom() const
{
    Gui::backgroundBottom(false);
    for (auto& button : buttons)
    {
        button->draw();
    }

    Gui::sprite(ui_sheet_emulated_stripe_move_grey_idx, 7, 28);
    Gui::sprite(ui_sheet_emulated_stripe_move_grey_idx, 7, 101);

    for (int i = 0; i < 4; i++)
    {
        Gui::sprite(ui_sheet_emulated_stripe_move_grey_idx, 92, 128 + i * 24);
    }

    Gui::pkm(filter->species(), 0, TitleLoader::save->generation(), 0, 58, 60);

    Gui::text(i18n::localize("FILTER_OPTIONS"), 160, 14, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::text(i18n::localize("B_BACK"), 160, 230.5f, FONT_SIZE_9, FONT_SIZE_9, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
    Gui::text(i18n::localize("SPECIES"), 10, 38, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::CENTER);
    Gui::text(i18n::localize("MOVES"), 10, 111, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::CENTER);

    for (int i = 0; i < 4; i++)
    {
        Gui::text(i18n::move(Configuration::getInstance().language(), filter->move(i)), 95, 138 + i * 24, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::CENTER);
    }
}

void FilterScreen::update(touchPosition* touch)
{
    if (justSwitched)
    {
        if (hidKeysHeld() & KEY_TOUCH)
        {
            return;
        }
        else
        {
            justSwitched = false;
        }
    }
    for (auto& button : buttons)
    {
        button->update(touch);
    }

    u32 kDown = hidKeysDown();
    if (kDown & KEY_B)
    {
        Gui::screenBack();
        return;
    }
}
