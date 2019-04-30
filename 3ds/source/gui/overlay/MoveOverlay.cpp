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

#include "MoveOverlay.hpp"
#include "ClickButton.hpp"
#include "Configuration.hpp"
#include "PB7.hpp"
#include "gui.hpp"
#include "loader.hpp"

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

MoveOverlay::MoveOverlay(Screen& screen, std::shared_ptr<PKX> pkm, int moveIndex)
    : Overlay(screen, i18n::localize("A_SELECT") + '\n' + i18n::localize("B_BACK")), pkm(pkm), moveIndex(moveIndex), hid(40, 2)
{
    instructions.addBox(false, 75, 30, 170, 23, COLOR_GREY, i18n::localize("SEARCH"), COLOR_WHITE);
    const std::vector<std::string>& rawMoves = i18n::rawMoves(Configuration::getInstance().language());
    for (int i = 1; i <= TitleLoader::save->maxMove(); i++)
    {
        if (i >= 622 && i <= 658)
            continue;
        moves.push_back({i, rawMoves[i]});
    }
    static const auto less = [](const std::pair<int, std::string>& pair1, const std::pair<int, std::string>& pair2) {
        return pair1.second < pair2.second;
    };
    std::sort(moves.begin(), moves.end(), less);
    moves.insert(moves.begin(), {0, rawMoves[0]});
    validMoves = moves;

    hid.update(moves.size());
    if (moveIndex < 4)
    {
        hid.select((u16)index(moves, i18n::move(Configuration::getInstance().language(), pkm->move(moveIndex))));
    }
    else
    {
        if (pkm->gen6())
        {
            PK6* pk6 = ((PK6*)pkm.get());
            hid.select((u16)index(moves, i18n::move(Configuration::getInstance().language(), pk6->relearnMove(moveIndex - 4))));
        }
        else if (pkm->gen7())
        {
            PK7* pk7 = ((PK7*)pkm.get());
            hid.select((u16)index(moves, i18n::move(Configuration::getInstance().language(), pk7->relearnMove(moveIndex - 4))));
        }
    }
    searchButton = new ClickButton(75, 30, 170, 23,
        [this]() {
            Gui::setNextKeyboardFunc([this]() { this->searchBar(); });
            return false;
        },
        ui_sheet_emulated_box_search_idx, "", 0, 0);
}

void MoveOverlay::draw() const
{
    C2D_SceneBegin(g_renderTargetBottom);
    dim();
    Gui::staticText(i18n::localize("EDITOR_INST"), 160, 115, FONT_SIZE_18, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
    searchButton->draw();
    Gui::sprite(ui_sheet_icon_search_idx, 79, 33);
    Gui::dynamicText(searchString, 95, 32, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);

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
        if (hid.page() * hid.maxVisibleEntries() + i < moves.size())
        {
            Gui::dynamicText(std::to_string(moves[hid.page() * hid.maxVisibleEntries() + i].first) + " - " +
                                 moves[hid.page() * hid.maxVisibleEntries() + i].second,
                x, (i % (hid.maxVisibleEntries() / 2)) * 12, FONT_SIZE_9, FONT_SIZE_9, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        }
        else
        {
            break;
        }
    }
}

void MoveOverlay::update(touchPosition* touch)
{
    if (justSwitched && ((hidKeysHeld() | hidKeysDown()) & KEY_TOUCH))
    {
        return;
    }
    else if (justSwitched)
    {
        justSwitched = false;
    }

    if (hidKeysDown() & KEY_X)
    {
        Gui::setNextKeyboardFunc([this]() { this->searchBar(); });
    }
    searchButton->update(touch);

    if (!searchString.empty() && searchString != oldSearchString)
    {
        moves.clear();
        moves.push_back(validMoves[0]);
        for (size_t i = 1; i < validMoves.size(); i++)
        {
            std::string itemName = validMoves[i].second.substr(0, searchString.size());
            StringUtils::toLower(itemName);
            if (itemName == searchString)
            {
                moves.push_back(validMoves[i]);
            }
        }
        oldSearchString = searchString;
    }
    else if (searchString.empty() && !oldSearchString.empty())
    {
        moves           = validMoves;
        oldSearchString = searchString = "";
    }
    if (hid.fullIndex() >= moves.size())
    {
        hid.select(0);
    }

    hid.update(moves.size());
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_A)
    {
        if (moveIndex < 4)
        {
            pkm->move(moveIndex, (u16)moves[hid.fullIndex()].first);
        }
        else
        {
            if (pkm->generation() == Generation::SIX)
            {
                ((PK6*)pkm.get())->relearnMove(moveIndex - 4, (u16)moves[hid.fullIndex()].first);
            }
            else if (pkm->generation() == Generation::SEVEN)
            {
                ((PK7*)pkm.get())->relearnMove(moveIndex - 4, (u16)moves[hid.fullIndex()].first);
            }
            else if (pkm->generation() == Generation::LGPE)
            {
                ((PB7*)pkm.get())->relearnMove(moveIndex - 4, (u16)moves[hid.fullIndex()].first);
            }
        }
        screen.removeOverlay();
        return;
    }
    else if (downKeys & KEY_B)
    {
        screen.removeOverlay();
        return;
    }
}

void MoveOverlay::searchBar()
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
