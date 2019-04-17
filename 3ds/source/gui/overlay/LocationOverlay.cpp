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

#include "LocationOverlay.hpp"
#include "gui.hpp"
#include "ClickButton.hpp"

LocationOverlay::LocationOverlay(Screen& screen, std::shared_ptr<PKX> pkm, bool met)
    : Overlay(screen, i18n::localize("A_SELECT") + '\n' + i18n::localize("B_BACK")),
    pkm(pkm), hid(40,2), validLocations(i18n::locations(Configuration::getInstance().language(), pkm->generation())), locations(validLocations), met(met)
{
    instructions.addBox(false, 75, 30, 170, 23, COLOR_GREY, i18n::localize("SEARCH"), COLOR_WHITE);
    searchButton = std::make_unique<ClickButton>(75, 30, 170, 23, [this](){ Gui::setNextKeyboardFunc([this](){ this->searchBar(); }); return false; }, ui_sheet_emulated_box_search_idx, "", 0, 0);
    hid.update(locations.size());
    hid.select(std::distance(locations.begin(), std::find_if(locations.begin(), locations.end(), [pkm, met](const std::pair<u16, std::string>& pair){ return pair.first == (met ? pkm->metLocation() : pkm->eggLocation()); })));
}

void LocationOverlay::draw() const
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
    // Stupid non random-access iterators
    std::map<u16, std::string>::const_iterator locIt = locations.begin();
    for (size_t i = 0; i < hid.page() * hid.maxVisibleEntries(); i++)
    {
        locIt++;
    }
    for (size_t i = 0; i < hid.maxVisibleEntries(); i++)
    {
        x = i < hid.maxVisibleEntries() / 2 ? 4 : 203;
        if (hid.page() * hid.maxVisibleEntries() + i < locations.size())
        {
            Gui::dynamicText(std::to_string(locIt->first) + " - " + locIt->second, x, (i % (hid.maxVisibleEntries() / 2)) * 12, FONT_SIZE_9, FONT_SIZE_9, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
            locIt++;
        }
        else
        {
            break;
        }
    }
}

void LocationOverlay::update(touchPosition* touch)
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
        Gui::setNextKeyboardFunc([this](){ this->searchBar(); });
    }
    searchButton->update(touch);

    if (!searchString.empty() && searchString != oldSearchString)
    {
        locations.clear();
        for (auto i = validLocations.begin(); i != validLocations.end(); i++)
        {
            std::string locName = i->second.substr(0, searchString.size());
            StringUtils::toLower(locName);
            if (locName == searchString)
            {
                locations.emplace(*i);
            }
        }
        oldSearchString = searchString;
    }
    else if (searchString.empty() && !oldSearchString.empty())
    {
        locations = validLocations;
        oldSearchString = searchString = "";
    }
    if (hid.fullIndex() >= locations.size())
    {
        hid.select(0);
    }

    hid.update(locations.size());
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_A)
    {
        auto locIt = locations.begin();
        for (size_t i = 0; i < hid.fullIndex(); i++)
        {
            locIt++;
        }
        if (met)
        {
            pkm->metLocation(locIt->first);
        }
        else
        {
            pkm->eggLocation(locIt->first);
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

void LocationOverlay::searchBar()
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, 20);
    swkbdSetHintText(&state, i18n::localize(met ? "MET_LOCATION" : "EGG_LOCATION").c_str());
    swkbdSetValidation(&state, SWKBD_ANYTHING, 0, 0);
    char input[25] = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[24] = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        searchString = input;
        StringUtils::toLower(searchString);
    }
}
