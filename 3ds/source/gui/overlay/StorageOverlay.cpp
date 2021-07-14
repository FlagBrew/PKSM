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

#include "StorageOverlay.hpp"
#include "BankSelectionScreen.hpp"
#include "BoxOverlay.hpp"
#include "ClickButton.hpp"
#include "Configuration.hpp"
#include "FilterScreen.hpp"
#include "SortScreen.hpp"
#include "banks.hpp"
#include "gui.hpp"
#include "loader.hpp"
#include "sav/Sav.hpp"

StorageOverlay::StorageOverlay(ReplaceableScreen& screen, bool store, int& boxBox, int& storageBox,
    std::shared_ptr<pksm::PKFilter> filter)
    : ReplaceableScreen(&screen, i18n::localize("B_BACK")),
      filter(filter),
      boxBox(boxBox),
      storageBox(storageBox),
      storage(store)
{
    buttons.push_back(std::make_unique<ClickButton>(
        106, 63, 108, 28,
        [this]()
        {
            Gui::setScreen(std::make_unique<SortScreen>(storage));
            parent->removeOverlay();
            return true;
        },
        ui_sheet_button_editor_idx, i18n::localize("SORT"), FONT_SIZE_12, COLOR_BLACK));
    buttons.push_back(std::make_unique<ClickButton>(
        106, 94, 108, 28,
        [this]()
        {
            Gui::setScreen(std::make_unique<FilterScreen>(this->filter));
            parent->removeOverlay();
            return true;
        },
        ui_sheet_button_editor_idx, i18n::localize("FILTER"), FONT_SIZE_12, COLOR_BLACK));
    buttons.push_back(std::make_unique<ClickButton>(
        106, 125, 108, 28, [this]() { return selectBox(); }, ui_sheet_button_editor_idx,
        i18n::localize("BOX_JUMP"), FONT_SIZE_12, COLOR_BLACK));
    buttons.push_back(std::make_unique<ClickButton>(
        106, 156, 108, 28,
        [this]()
        {
            Gui::setScreen(std::make_unique<BankSelectionScreen>(this->storageBox));
            parent->removeOverlay();
            return true;
        },
        ui_sheet_button_editor_idx, i18n::localize("BANK_SWITCH"), FONT_SIZE_12, COLOR_BLACK));
    buttons.push_back(std::make_unique<ClickButton>(
        283, 211, 34, 28,
        [this]()
        {
            parent->removeOverlay();
            return true;
        },
        ui_sheet_button_back_idx, "", 0.0f, COLOR_BLACK));
}

void StorageOverlay::drawTop() const
{
    dim();
}

void StorageOverlay::drawBottom() const
{
    dim();

    for (const auto& button : buttons)
    {
        button->draw();
    }
}

void StorageOverlay::update(touchPosition* touch)
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

bool StorageOverlay::selectBox()
{
    std::vector<std::string> boxes;
    boxes.reserve(Banks::bank->boxes());
    if (storage)
    {
        for (int i = 0; i < Banks::bank->boxes(); i++)
        {
            boxes.emplace_back(Banks::bank->boxName(i));
        }
        addOverlay<BoxOverlay>(boxes, storageBox);
    }
    else
    {
        for (int i = 0; i < TitleLoader::save->maxBoxes(); i++)
        {
            boxes.emplace_back(TitleLoader::save->boxName(i));
        }
        addOverlay<BoxOverlay>(boxes, boxBox);
    }
    return true;
}
