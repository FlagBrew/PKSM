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

#ifndef BANKSELECTIONSCREEN_HPP
#define BANKSELECTIONSCREEN_HPP

#include "Screen.hpp"
#include "HidVertical.hpp"
#include "banks.hpp"
#include <vector>
#include <string>

class BankSelectionScreen : public Screen
{
public:
    BankSelectionScreen(int& storageBox) : hid(40, 2), strings(Banks::bankNames()), storageBox(storageBox)
    {
        int newBankNum = 0;
        while (std::find_if(strings.begin(), strings.end(), [&newBankNum](const std::pair<std::string, int>& v){ return v.first == i18n::localize("NEW_BANK") + " " + std::to_string(newBankNum); }) != strings.end())
        {
            newBankNum++;
        }
        strings.push_back({i18n::localize("NEW_BANK") + " " + std::to_string(newBankNum), 1});
        hid.update(strings.size());
        hid.select(std::distance(strings.begin(), std::find_if(strings.begin(), strings.end(), [](const std::pair<std::string, int>& v){ return v.first == Banks::bank->name(); })));
    }
    void draw() const override;
    void update(touchPosition* touch) override;
    ScreenType type() const override { return ScreenType::SELECTOR; }
private:
    void renameBank();
    void resizeBank();
    HidVertical hid;
    std::vector<std::pair<std::string, int>> strings;
    bool finished = false;
    int& storageBox;
};

#endif
