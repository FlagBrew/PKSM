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

#include "FilterScreen.hpp"
#include "ClickButton.hpp"
#include "EnablableToggleButton.hpp"
#include "FormOverlay.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "loader.hpp"
#include "MoveOverlay.hpp"
#include "pkx/PKFilter.hpp"
#include "sav/Sav.hpp"
#include "SpeciesOverlay.hpp"
#include "ToggleButton.hpp"

FilterScreen::FilterScreen(std::shared_ptr<pksm::PKFilter> filter) : filter(filter)
{
    if (filter->species() == pksm::Species::None)
    {
        filter->species(pksm::Species::Bulbasaur);
    }

    buttons.push_back({});

    for (int i = 0; i < 4; i++)
    {
        buttons[0].push_back(std::make_unique<ToggleButton>(
            26, 126 + 24 * i, 24, 24,
            [this, i]()
            {
                this->filter->moveEnabled(i, !this->filter->moveEnabled(i));
                return false;
            },
            ui_sheet_checkbox_on_idx, "", 0.0f, COLOR_BLACK, ui_sheet_checkbox_blank_idx, "", 0.0f,
            COLOR_BLACK, nullptr, true));
        ((ToggleButton*)buttons[0].back().get())->setState(filter->moveEnabled(i));

        buttons[0].push_back(std::make_unique<ToggleButton>(
            53, 130 + 24 * i, 13, 13,
            [this, i]()
            {
                this->filter->moveInversed(i, !this->filter->moveInversed(i));
                return false;
            },
            ui_sheet_emulated_button_filter_positive_idx, "", 0.0f, COLOR_BLACK,
            ui_sheet_emulated_button_filter_negative_idx, "", 0.0f, COLOR_BLACK, nullptr, true));
        ((ToggleButton*)buttons[0].back().get())->setState(!filter->moveInversed(i));

        buttons[0].push_back(std::make_unique<ClickButton>(
            70, 131 + 24 * i, 15, 12,
            [this, i]()
            {
                this->addOverlay<MoveOverlay>(*this->filter, i);
                return false;
            },
            ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));
    }

    buttons[0].push_back(std::make_unique<ClickButton>(
        51, 53, 50, 44,
        [this]()
        {
            this->addOverlay<SpeciesOverlay>(*this->filter);
            return false;
        },
        ui_sheet_icon_party_idx, "", 0.0f, COLOR_BLACK));

    buttons[0].push_back(std::make_unique<ToggleButton>(
        121, 63, 24, 24,
        [this]()
        {
            this->filter->speciesEnabled(!this->filter->speciesEnabled());
            if (!this->filter->speciesEnabled())
            {
                this->filter->alternativeFormEnabled(false);
            }
            return false;
        },
        ui_sheet_checkbox_on_idx, "", 0.0f, COLOR_BLACK, ui_sheet_checkbox_blank_idx, "", 0.0f,
        COLOR_BLACK, nullptr, true));
    ((ToggleButton*)buttons[0].back().get())->setState(filter->speciesEnabled());

    buttons[0].push_back(std::make_unique<ToggleButton>(
        105, 68, 13, 13,
        [this]()
        {
            this->filter->speciesInversed(!this->filter->speciesInversed());
            return false;
        },
        ui_sheet_emulated_button_filter_positive_idx, "", 0.0f, COLOR_BLACK,
        ui_sheet_emulated_button_filter_negative_idx, "", 0.0f, COLOR_BLACK, nullptr, true));
    ((ToggleButton*)buttons[0].back().get())->setState(!filter->speciesInversed());

    buttons[0].push_back(std::make_unique<ClickButton>(
        172, 53, 50, 44,
        [this]()
        {
            if (TitleLoader::save->formCount(this->filter->species()) > 1)
            {
                this->addOverlay<FormOverlay>(
                    *this->filter, TitleLoader::save->formCount(this->filter->species()));
            }
            return false;
        },
        ui_sheet_icon_party_idx, "", 0.0f, COLOR_BLACK));

    buttons[0].push_back(std::make_unique<EnablableToggleButton>(
        242, 63, 24, 24,
        [this]()
        {
            this->filter->alternativeFormEnabled(!this->filter->alternativeFormEnabled());
            return false;
        },
        [this]() { return !this->filter->speciesEnabled(); }, ui_sheet_checkbox_on_idx, "", 0.0f,
        COLOR_BLACK, ui_sheet_checkbox_blank_idx, "", 0.0f, COLOR_BLACK,
        ui_sheet_emulated_checkbox_disabled_idx, "", 0.0f, COLOR_BLACK));
    ((EnablableToggleButton*)buttons[0].back().get())->setState(filter->alternativeFormEnabled());

    buttons[0].push_back(std::make_unique<ToggleButton>(
        226, 68, 13, 13,
        [this]()
        {
            this->filter->alternativeFormInversed(!this->filter->alternativeFormInversed());
            return false;
        },
        ui_sheet_emulated_button_filter_positive_idx, "", 0.0f, COLOR_BLACK,
        ui_sheet_emulated_button_filter_negative_idx, "", 0.0f, COLOR_BLACK, nullptr, true));
    ((ToggleButton*)buttons[0].back().get())->setState(!filter->alternativeFormInversed());
}

void FilterScreen::drawTop() const
{
    Gui::backgroundTop(false);
    Gui::backgroundAnimatedTop();
}

void FilterScreen::drawBottom() const
{
    Gui::backgroundBottom(false);
    for (const auto& button : buttons[tab])
    {
        button->draw();
    }

    Gui::sprite(ui_sheet_emulated_stripe_move_grey_idx, 7, 28);
    Gui::sprite(ui_sheet_emulated_stripe_move_grey_idx, 7, 101);

    for (int i = 0; i < 4; i++)
    {
        Gui::sprite(ui_sheet_emulated_stripe_move_grey_idx, 92, 128 + i * 24);
    }

    Gui::pkm(filter->species(), 0, TitleLoader::save->generation(), filter->gender(), 58, 60);
    Gui::pkm(filter->species(), filter->alternativeForm(), TitleLoader::save->generation(),
        filter->gender(), 179, 60);

    Gui::text(i18n::localize("FILTER_OPTIONS"), 160, 14, FONT_SIZE_14, COLOR_WHITE,
        TextPosX::CENTER, TextPosY::CENTER);
    Gui::text(i18n::localize("B_BACK"), 160, 230.5f, FONT_SIZE_9, COLOR_WHITE, TextPosX::CENTER,
        TextPosY::CENTER);
    Gui::text(i18n::localize("SPECIES"), 10, 38, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT,
        TextPosY::CENTER);
    Gui::text(i18n::localize("MOVES"), 10, 111, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT,
        TextPosY::CENTER);

    for (int i = 0; i < 4; i++)
    {
        Gui::text(i18n::move(Configuration::getInstance().language(), filter->move(i)), 95,
            138 + i * 24, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::CENTER);
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
    for (auto& button : buttons[tab])
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
