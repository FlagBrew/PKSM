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

#ifndef SEARCHABLEOVERLAY_HPP
#define SEARCHABLEOVERLAY_HPP

#include "ClickButton.hpp"
#include "Configuration.hpp"
#include "gui.hpp"
#include "Hid.hpp"
#include "i18n_ext.hpp"
#include "ReplaceableScreen.hpp"
#include "spritesheets.h"
#include "utils/utils.hpp"
#include <memory>
#include <string>

// A picker overlay: a searchable list of entries the user browses with the D-pad
// and commits with A. This owns the whole search lifecycle - keyboard entry, the
// search button, refiltering when the query changes, selection repair and paging -
// so that subclasses only supply their entries and what selecting one means.
template <HidDirection ListDirection, HidDirection PageDirection>
class SearchableOverlay : public ReplaceableScreen
{
public:
    SearchableOverlay(ReplaceableScreen& screen, const std::string& instructionText,
        const std::string& searchHint, size_t visibleEntries, size_t columns)
        : ReplaceableScreen(&screen, instructionText),
          hid(visibleEntries, columns),
          searchHint(searchHint)
    {
        instructions.addBox(
            false, 75, 30, 170, 23, COLOR_GREY, i18n::localize("SEARCH"), COLOR_WHITE);
        searchButton = std::make_unique<ClickButton>(
            75, 30, 170, 23,
            [this]()
            {
                searchBar();
                return false;
            },
            ui_sheet_emulated_box_search_idx, "", 0, COLOR_BLACK);
    }

    bool replacesTop() const override { return true; }

    void drawBottom() const override
    {
        dim();
        const std::string bottomText = bottomInstructions();
        if (!bottomText.empty())
        {
            Gui::text(
                bottomText, 160, 115, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
        }
        searchButton->draw();
        Gui::sprite(ui_sheet_icon_search_idx, 79, 33);
        Gui::text(searchString, 95, 32, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    }

    void update(touchPosition* touch) override final
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
            filter(searchString);
            oldSearchString = searchString;
        }
        else if (searchString.empty() && !oldSearchString.empty())
        {
            filter("");
            oldSearchString = searchString = "";
        }

        if (hid.fullIndex() >= entryCount())
        {
            hid.select(0);
        }
        hid.update(entryCount());

        u32 downKeys = hidKeysDown();
        if (downKeys & KEY_A)
        {
            if (commit())
            {
                parent->removeOverlay();
            }
            return;
        }
        else if (downKeys & KEY_B)
        {
            discard();
            parent->removeOverlay();
            return;
        }
    }

protected:
    // Number of entries currently shown, after filtering.
    virtual size_t entryCount() const = 0;
    // Narrow the shown entries to those matching search; an empty search restores all of them.
    virtual void filter(const std::string& search) = 0;
    // Apply the highlighted entry. Return false to keep the overlay open.
    virtual bool commit() = 0;

    // Called instead of commit when the user backs out.
    virtual void discard() {}

    virtual std::string bottomInstructions() const { return i18n::localize("EDITOR_INST"); }

    Hid<ListDirection, PageDirection> hid;

private:
    void searchBar()
    {
        SwkbdState state;
        swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, 24);
        swkbdSetHintText(&state, searchHint.c_str());
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

    std::string searchHint;
    std::string searchString    = "";
    std::string oldSearchString = "";
    std::unique_ptr<Button> searchButton;
    bool justSwitched = true;
};

#endif
