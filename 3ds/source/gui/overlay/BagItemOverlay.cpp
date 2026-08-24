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
#include "i18n_ext.hpp"
#include "loader.hpp"
#include "sav/Item.hpp"
#include "sav/Sav.hpp"
#include "utils/utils.hpp"

BagItemOverlay::BagItemOverlay(ReplaceableScreen& screen,
    std::vector<std::pair<const std::string*, int>>& items, size_t selected,
    std::pair<pksm::Sav::Pouch, int> pouch, int slot, int& firstEmpty)
    : ListPickerOverlay(screen,
          i18n::localize("A_SELECT") + '\n' + i18n::localize("L_PAGE_PREV") + '\n' +
              i18n::localize("R_PAGE_NEXT") + '\n' + i18n::localize("B_BACK"),
          i18n::localize("ITEM")),
      validItems(items),
      items(items),
      pouch(pouch),
      slot(slot),
      firstEmpty(firstEmpty)
{
    hid.update(items.size());
    hid.select(selected);
}

std::string BagItemOverlay::entryLine(size_t index) const
{
    return *items[index].first;
}

void BagItemOverlay::filter(const std::string& search)
{
    if (search.empty())
    {
        items = validItems;
        return;
    }
    items.clear();
    if (!validItems.empty())
    {
        items.emplace_back(validItems[0]);
    }
    for (auto& it : validItems)
    {
        std::string lowerName = *it.first;
        StringUtils::toLower(lowerName);
        if (lowerName.find(search) != std::string::npos)
        {
            items.emplace_back(it);
        }
    }
}

bool BagItemOverlay::commit()
{
    if (hid.fullIndex() == 0)
    {
        if (firstEmpty != slot)
        {
            static pksm::Item4 emptyItem;
            firstEmpty--;
            for (int i = slot; i < firstEmpty; i++)
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
    return true;
}
