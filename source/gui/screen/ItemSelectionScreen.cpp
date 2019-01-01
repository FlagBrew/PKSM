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

#include "ItemSelectionScreen.hpp"
#include "gui.hpp"
#include "Configuration.hpp"
#include "loader.hpp"

namespace {
    int index(std::vector<std::pair<int, std::string>>& search, std::string v)
    {
        if (v == search[0].second || v == "")
        {
            return 0;
        }
        int index = -1, min = 0, mid = 0, max = search.size();
        while (min <= max)
        {
            mid = min + (max-min)/2;
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

ItemSelectionScreen::ItemSelectionScreen(std::shared_ptr<PKX> pkm) : SelectionScreen(pkm), hid(40, 2)
{
    std::vector<std::string> rawItems = i18n::rawItems(Configuration::getInstance().language());
    for (int i = 1; i < TitleLoader::save->maxItem(); i++)
    {
        if (rawItems[i].find("\uFF1F\uFF1F\uFF1F") != std::string::npos || rawItems[i].find("???") != std::string::npos) continue;
        items.push_back({i, rawItems[i]});
    }
    static const auto less = [](const std::pair<int, std::string>& pair1, const std::pair<int, std::string>& pair2){ return pair1.second < pair2.second; };
    std::sort(items.begin(), items.end(), less);
    items.insert(items.begin(), {0, rawItems[0]});

    hid.update(items.size());
    int itemIndex = index(items, i18n::item(Configuration::getInstance().language(), pkm->heldItem()));
    // Checks to make sure that it's the correct item and not one with a duplicate name
    if (items[itemIndex].first != pkm->heldItem())
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
    hid.select(index(items, i18n::item(Configuration::getInstance().language(), pkm->heldItem())));
}

void ItemSelectionScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    Gui::sprite(ui_sheet_part_editor_20x2_idx, 0, 0);
    int x = hid.index() < hid.maxVisibleEntries() / 2 ? 2 : 200;
    int y = (hid.index() % (hid.maxVisibleEntries() / 2)) * 12;
    C2D_DrawRectSolid(x, y, 0.5f, 198, 11, COLOR_MASKBLACK);
    C2D_DrawRectSolid(x, y, 0.5f, 198, 1, COLOR_YELLOW);
    C2D_DrawRectSolid(x, y, 0.5f, 1, 11, COLOR_YELLOW);
    C2D_DrawRectSolid(x, y + 10, 0.5f, 198, 1, COLOR_YELLOW);
    C2D_DrawRectSolid(x + 197, y, 0.5f, 1, 11, COLOR_YELLOW);
    for (size_t i = 0; i < hid.maxVisibleEntries(); i++)
    {
        x = i < hid.maxVisibleEntries() / 2 ? 4 : 203;
        std::string text;
        if (hid.page() * hid.maxVisibleEntries() + i < items.size())
        {
            text = std::to_string(items[hid.page() * hid.maxVisibleEntries() + i].first) + " - " + items[hid.page() * hid.maxVisibleEntries() + i].second;
        }
        else
        {
            text = "0 - " + items[0].second;
        }
        Gui::dynamicText(text, x, (i % (hid.maxVisibleEntries() / 2)) * 12, FONT_SIZE_9, FONT_SIZE_9, COLOR_WHITE);
    }
}

void ItemSelectionScreen::update(touchPosition* touch)
{
    hid.update(items.size());
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_A)
    {
        pkm->heldItem((u16) items[hid.fullIndex()].first);
        done = true;
        return;
    }
    else if (downKeys & KEY_B)
    {
        done = true;
        return;
    }
}