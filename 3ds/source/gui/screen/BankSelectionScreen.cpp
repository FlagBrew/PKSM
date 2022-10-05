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

#include "BankSelectionScreen.hpp"
#include "banks.hpp"
#include "Configuration.hpp"
#include "format.h"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include <algorithm>

BankSelectionScreen::BankSelectionScreen(int& storageBox)
    : hid(40, 2), strings(Banks::bankNames()), storageBox(storageBox)
{
    int newBankNum = 0;
    while (std::find_if(strings.begin(), strings.end(),
               [&newBankNum](const std::pair<std::string, int>& v)
               { return v.first == "New Bank " + std::to_string(newBankNum); }) != strings.end())
    {
        newBankNum++;
    }
    strings.emplace_back(("New Bank " + std::to_string(newBankNum)).substr(0, 10), 1);
    hid.update(strings.size());
    hid.select(std::distance(strings.begin(),
        std::find_if(strings.begin(), strings.end(),
            [](const std::pair<std::string, int>& v) { return v.first == Banks::bank->name(); })));
}

void BankSelectionScreen::drawBottom() const
{
    Gui::sprite(ui_sheet_part_info_bottom_idx, 0, 0);
    Gui::text(i18n::localize("X_RENAME") + "\n" + i18n::localize("Y_RESIZE") + "\n" +
                  i18n::localize("START_DELETE"),
        160, 120, FONT_SIZE_18, COLOR_BLACK, TextPosX::CENTER, TextPosY::CENTER);
}

void BankSelectionScreen::drawTop() const
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
        x = i < hid.maxVisibleEntries() / 2 ? 4 : 204;
        if (hid.page() * hid.maxVisibleEntries() + i < strings.size())
        {
            Gui::text(strings[hid.page() * hid.maxVisibleEntries() + i].first, x,
                (i % (hid.maxVisibleEntries() / 2)) * 12, FONT_SIZE_9, COLOR_WHITE, TextPosX::LEFT,
                TextPosY::TOP);
            Gui::text(std::to_string(strings[hid.page() * hid.maxVisibleEntries() + i].second),
                x + 192, (i % (hid.maxVisibleEntries() / 2)) * 12, FONT_SIZE_9, COLOR_WHITE,
                TextPosX::RIGHT, TextPosY::TOP);
        }
        else
        {
            break;
        }
    }
}

void BankSelectionScreen::update(touchPosition* touch)
{
    hid.update(strings.size());
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_A)
    {
        auto& res = strings[hid.fullIndex()];
        if (res.first != Banks::bank->name())
        {
            if (Banks::bank->hasChanged() &&
                Gui::showChoiceMessage(i18n::localize("BANK_SAVE_CHANGES")))
            {
                Banks::bank->save();
            }
            if (Banks::loadBank(res.first, res.second))
            {
                storageBox = 0;
            }
        }
        Gui::screenBack();
        return;
    }
    else if (downKeys & KEY_B)
    {
        Gui::screenBack();
        return;
    }
    else if (downKeys & KEY_X)
    {
        renameBank();
    }
    else if (downKeys & KEY_Y)
    {
        resizeBank();
    }
    else if (downKeys & KEY_START)
    {
        if (hid.fullIndex() == strings.size() - 1)
        {
            return;
        }
        if (strings.size() > 2)
        {
            if (Gui::showChoiceMessage(
                    fmt::format(i18n::localize("BANK_DELETE"), strings[hid.fullIndex()].first)))
            {
                auto i = strings.begin() + hid.fullIndex();
                Banks::removeBank(i->first);
                strings.erase(i);
            }
        }
    }
}

void BankSelectionScreen::renameBank()
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, 10);
    swkbdSetHintText(&state, i18n::localize("BANK_NAME").c_str());
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    char input[41]  = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[40]       = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        std::string string(input);
        if (std::find_if(strings.begin(), strings.end(),
                [&string](const std::pair<std::string, int>& v)
                { return v.first == string; }) != strings.end())
        {
            Gui::warn(i18n::localize("NO_DUPES"));
        }
        else
        {
            if (hid.fullIndex() != strings.size() - 1)
            {
                Gui::waitFrame(i18n::localize("RENAMING_BANK"));
                Banks::renameBank(strings[hid.fullIndex()].first, string);
            }
            strings[hid.fullIndex()].first = string;
        }
    }
}

void BankSelectionScreen::resizeBank()
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NUMPAD, 2, 3);
    swkbdSetFeatures(&state, SWKBD_FIXED_WIDTH);
    swkbdSetValidation(&state, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
    char input[4]   = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[3]        = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        int num = std::max(1, std::min(std::stoi(input), BANK_MAX_SIZE));
        if (hid.fullIndex() != strings.size() - 1)
        {
            Banks::setBankSize(strings[hid.fullIndex()].first, num);
        }
        strings[hid.fullIndex()].second = num;
    }
}
