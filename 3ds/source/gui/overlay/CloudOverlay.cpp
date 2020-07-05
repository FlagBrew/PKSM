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

#include "CloudOverlay.hpp"
#include "ClickButton.hpp"
#include "CloudAccess.hpp"
#include "Configuration.hpp"
#include "ToggleButton.hpp"
#include "gui.hpp"
#include "loader.hpp"

namespace
{
    std::string sortTypeToString(CloudAccess::SortType type)
    {
        switch (type)
        {
            case CloudAccess::SortType::LATEST:
                return "LATEST";
            case CloudAccess::SortType::POPULAR:
                return "POPULAR";
        }
        return "";
    }
}

CloudOverlay::CloudOverlay(ReplaceableScreen& screen, CloudAccess& acc)
    : ReplaceableScreen(&screen), access(acc)
{
    buttons.push_back(std::make_unique<ClickButton>(106, 82, 108, 28,
        [this]() {
            if (access.sortType() == CloudAccess::SortType::LATEST)
            {
                access.sortType(CloudAccess::SortType::POPULAR);
            }
            else
            {
                access.sortType(CloudAccess::SortType::LATEST);
            }
            return false;
        },
        ui_sheet_button_editor_idx, "", FONT_SIZE_12, COLOR_BLACK));

    auto tbutton = std::make_unique<ToggleButton>(106, 113, 108, 28,
        [this]() {
            access.sortDir(!access.sortAscending());
            return access.sortAscending();
        },
        ui_sheet_button_editor_idx, i18n::localize("ASCENDING"), FONT_SIZE_12, COLOR_BLACK,
        ui_sheet_button_editor_idx, i18n::localize("DESCENDING"), FONT_SIZE_12, COLOR_BLACK,
        nullptr, true);
    tbutton->setState(access.sortAscending());
    buttons.push_back(std::move(tbutton));
    tbutton = std::make_unique<ToggleButton>(106, 144, 108, 28,
        [this]() {
            access.filterLegal(!access.filterLegal());
            return access.filterLegal();
        },
        ui_sheet_button_editor_idx, i18n::localize("LEGALITY_LEGAL"), FONT_SIZE_12, COLOR_BLACK,
        ui_sheet_button_editor_idx, i18n::localize("LEGALITY_ANY"), FONT_SIZE_12, COLOR_BLACK,
        nullptr, true);
    tbutton->setState(access.filterLegal());
    buttons.push_back(std::move(tbutton));
    buttons.push_back(std::make_unique<ClickButton>(283, 211, 34, 28,
        [this]() {
            parent->removeOverlay();
            return true;
        },
        ui_sheet_button_back_idx, "", 0.0f, COLOR_BLACK));
}

void CloudOverlay::drawTop() const
{
    dim();
}

void CloudOverlay::drawBottom() const
{
    dim();

    Gui::sprite(ui_sheet_textbox_name_bottom_idx, 0, 1);

    for (auto& button : buttons)
    {
        button->draw();
    }
    Gui::text(i18n::localize("CLOUD_SORT_FILTER"), 25, 5, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT,
        TextPosY::TOP);
    Gui::text(i18n::localize(sortTypeToString(access.sortType())), 160, 96, FONT_SIZE_12,
        COLOR_BLACK, TextPosX::CENTER, TextPosY::CENTER, TextWidthAction::WRAP, 108);
}

void CloudOverlay::update(touchPosition* touch)
{
    if (hidKeysDown() & KEY_B)
    {
        parent->removeOverlay();
        return;
    }
    else
    {
        for (auto& button : buttons)
        {
            if (button->update(touch))
            {
                return;
            }
        }
    }
}
