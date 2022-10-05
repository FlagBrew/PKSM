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

#include "BagItemOverlay.hpp"
#include "gui.hpp"
#include "loader.hpp"
#include "sav/Item.hpp"
#include "sav/Sav.hpp"
#include "utils/utils.hpp"

void BagItemOverlay::drawBottom() const
{
    dim();
    searchButton->draw();
    Gui::sprite(ui_sheet_icon_search_idx, 79, 33);
    Gui::text(searchString, 95, 32, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
}

void BagItemOverlay::drawTop() const
{
    Gui::sprite(ui_sheet_part_editor_20x2_idx, 0, 0);
    int x = hid.index() < hid.maxVisibleEntries() / 2 ? 2 : 200;
    int y = (hid.index() % (hid.maxVisibleEntries() / 2)) * 12;
    Gui::drawSolidRect(x, y, 198, 11, COLOR_MASKBLACK);
    Gui::drawSolidRect(x, y, 198, 1, COLOR_YELLOW);
    Gui::drawSolidRect(x, y, 1, 11, COLOR_YELLOW);
    Gui::drawSolidRect(x, y + 10, 198, 1, COLOR_YELLOW);
    Gui::drawSolidRect(x + 197, y, 1, 11, COLOR_YELLOW);
    for (size_t i = 0; i < hid.maxVisibleEntries(); i++)
    {
        if (i + hid.page() * hid.maxVisibleEntries() >= items.size())
        {
            break;
        }
        x = i < hid.maxVisibleEntries() / 2 ? 4 : 203;
        Gui::text(*items[i + hid.page() * hid.maxVisibleEntries()].first, x,
            (i % (hid.maxVisibleEntries() / 2)) * 12, FONT_SIZE_9, COLOR_WHITE, TextPosX::LEFT,
            TextPosY::TOP);
    }
}

void BagItemOverlay::update(touchPosition* touch)
{
    if (justSwitched && (hidKeysHeld() & KEY_TOUCH))
    {
        return;
    }
    else if (justSwitched)
    {
        justSwitched = false;
    }

    if (hidKeysDown() & KEY_X)
    {
        searchBar();
    }
    searchButton->update(touch);

    if (!searchString.empty() && searchString != oldSearchString)
    {
        items.clear();
        items.emplace_back(validItems[0]);
        for (size_t i = 1; i < validItems.size(); i++)
        {
            std::string itemName = validItems[i].first->substr(0, searchString.size());
            StringUtils::toLower(itemName);
            if (itemName == searchString)
            {
                items.emplace_back(validItems[i]);
            }
        }
        oldSearchString = searchString;
    }
    else if (searchString.empty() && !oldSearchString.empty())
    {
        items           = validItems;
        oldSearchString = searchString = "";
    }
    if (hid.fullIndex() >= items.size())
    {
        hid.select(0);
    }
    u32 downKeys = hidKeysDown();
    hid.update(items.size());
    if (downKeys & KEY_A)
    {
        if (hid.fullIndex() == 0)
        {
            if (firstEmpty != slot)
            {
                static pksm::Item4 emptyItem;
                for (int i = slot; i < --firstEmpty; i++)
                {
                    auto item = TitleLoader::save->item(pouch.first, i + 1);
                    TitleLoader::save->item(*item, pouch.first, i);
                }
                TitleLoader::save->item(emptyItem, pouch.first, firstEmpty);
            }
        }
        else
        {
            auto item = TitleLoader::save->item(pouch.first, slot);
            if (item->generation() == pksm::Generation::ONE)
            {
                ((pksm::Item1*)item.get())->id1(items[hid.fullIndex()].second);
            }
            else if (item->generation() == pksm::Generation::TWO)
            {
                ((pksm::Item2*)item.get())->id2(items[hid.fullIndex()].second);
            }
            else if (item->generation() == pksm::Generation::THREE)
            {
                ((pksm::Item3*)item.get())->id3(items[hid.fullIndex()].second);
            }
            else
            {
                item->id(items[hid.fullIndex()].second);
            }
            if (item->count() == 0)
            {
                item->count(1);
            }
            TitleLoader::save->item(*item, pouch.first, slot);
            if (slot == firstEmpty)
            {
                firstEmpty = std::min(firstEmpty + 1, pouch.second);
            }
        }
        parent->removeOverlay();
    }
    else if (downKeys & KEY_B)
    {
        parent->removeOverlay();
    }
}

void BagItemOverlay::searchBar()
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, 20);
    swkbdSetHintText(&state, i18n::localize("ITEM").c_str());
    swkbdSetValidation(&state, SWKBD_ANYTHING, 0, 0);
    char input[25]  = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[24]       = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        searchString = input;
        StringUtils::toLower(searchString);
    }
}
