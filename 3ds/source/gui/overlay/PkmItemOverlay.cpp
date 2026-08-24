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

#include "PkmItemOverlay.hpp"
#include "Configuration.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "loader.hpp"
#include "pkx/PK1.hpp"
#include "pkx/PK2.hpp"
#include "pkx/PK3.hpp"
#include "pkx/PKX.hpp"
#include "sav/Sav.hpp"
#include "utils/utils.hpp"
#include <set>

namespace
{
    int index(std::vector<std::pair<int, std::string>>& search, const std::string& v)
    {
        if (v == search[0].second || v == "")
        {
            return 0;
        }
        int index = -1, min = 0, mid = 0, max = search.size();
        while (min <= max)
        {
            mid = min + (max - min) / 2;
            if (search[mid].second == v)
            {
                index = mid;
                break;
            }
            if (search[mid].second < v)
            {
                min = mid + 1;
            }
            else
            {
                max = mid - 1;
            }
        }
        return index >= 0 ? index : 0;
    }
}

PkmItemOverlay::PkmItemOverlay(ReplaceableScreen& screen, pksm::PKX& pkm)
    : ListPickerOverlay(screen, i18n::localize("A_SELECT") + '\n' + i18n::localize("B_BACK"),
          i18n::localize("ITEM")),
      pkm(pkm)
{
    const std::vector<std::string>& rawItems =
        pkm.generation() == pksm::Generation::THREE
            ? i18n::rawItems3(Configuration::getInstance().language())
            : (pkm.generation() == pksm::Generation::TWO ||
                          pkm.generation() == pksm::Generation::ONE
                      ? i18n::rawItems2(Configuration::getInstance().language())
                      : i18n::rawItems(Configuration::getInstance().language()));
    const std::set<int>& availableItems =
        pkm.generation() == pksm::Generation::ONE
            ? pksm::VersionTables::availableItems(pksm::GameVersion::GD)
        : TitleLoader::save ? TitleLoader::save->availableItems()
                            : pksm::VersionTables::availableItems(
                                  pksm::GameVersion::oldestVersion(pkm.generation()));
    for (auto i = availableItems.begin(); i != availableItems.end(); i++)
    {
        if ((rawItems[*i].find("\uFF1F\uFF1F\uFF1F") != std::string::npos ||
                rawItems[*i].find("???") != std::string::npos) ||
            (*i >= 807 && *i <= 835) || (*i >= 927 && *i <= 932))
        {
            continue; // Invalid items and bag Z-Crystals
        }
        items.emplace_back(*i, rawItems[*i]);
    }
    std::sort(items.begin(), items.end(),
        [](const std::pair<int, std::string>& pair1, const std::pair<int, std::string>& pair2)
        {
            if (pair1.first == 0)
            {
                return pair2.first != 0;
            }
            if (pair2.first == 0)
            {
                return false;
            }
            return pair1.second < pair2.second;
        });
    validItems = items;

    hid.update(items.size());
    u16 item =
        pkm.generation() == pksm::Generation::THREE
            ? static_cast<pksm::PK3&>(pkm).heldItem3()
            : (pkm.generation() == pksm::Generation::TWO ? static_cast<pksm::PK2&>(pkm).heldItem2()
                  : (pkm.generation() == pksm::Generation::ONE)
                      ? static_cast<pksm::PK1&>(pkm).heldItem2()
                      : pkm.heldItem());
    int itemIndex =
        index(items, pkm.generation() == pksm::Generation::THREE
                         ? i18n::item3(Configuration::getInstance().language(), item)
                         : (pkm.generation() == pksm::Generation::TWO ||
                                       pkm.generation() == pksm::Generation::ONE
                                   ? i18n::item2(Configuration::getInstance().language(), item)
                                   : i18n::item(Configuration::getInstance().language(), item)));
    // Checks to make sure that it's the correct item and not one with a duplicate name
    if (items[itemIndex].first != item)
    {
        if (items[itemIndex + 1].second == items[itemIndex].second)
        {
            itemIndex++;
        }
        else
        {
            itemIndex--;
        }
    }
    hid.select(itemIndex);
}

std::string PkmItemOverlay::entryLine(size_t index) const
{
    return std::to_string(items[index].first) + " - " + items[index].second;
}

void PkmItemOverlay::filter(const std::string& search)
{
    if (search.empty())
    {
        items = validItems;
        return;
    }
    items.clear();
    items.emplace_back(validItems[0]);
    for (size_t i = 1; i < validItems.size(); i++)
    {
        std::string itemName = validItems[i].second.substr(0, search.size());
        StringUtils::toLower(itemName);
        if (itemName == search)
        {
            items.emplace_back(validItems[i]);
        }
    }
}

bool PkmItemOverlay::commit()
{
    if (pkm.generation() == pksm::Generation::ONE)
    {
        static_cast<pksm::PK1&>(pkm).heldItem2((u16)items[hid.fullIndex()].first);
    }
    else if (pkm.generation() == pksm::Generation::TWO)
    {
        static_cast<pksm::PK2&>(pkm).heldItem2((u16)items[hid.fullIndex()].first);
    }
    else if (pkm.generation() == pksm::Generation::THREE)
    {
        static_cast<pksm::PK3&>(pkm).heldItem3((u16)items[hid.fullIndex()].first);
    }
    else
    {
        pkm.heldItem((u16)items[hid.fullIndex()].first);
    }
    return true;
}
