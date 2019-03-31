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
    BankSelectionScreen(const std::string& current) : hid(40, 2), strings(Banks::bankNames()), previous(std::distance(strings.begin(), std::find_if(strings.begin(), strings.end(), [current](const std::pair<std::string, int>& v){ return v.first == current; })))
    {
        int newBankNum = 0;
        while (std::find_if(strings.begin(), strings.end(), [&newBankNum](const std::pair<std::string, int>& v){ return v.first == i18n::localize("NEW_BANK") + " " + std::to_string(newBankNum); }) != strings.end())
        {
            newBankNum++;
        }
        strings.push_back({i18n::localize("NEW_BANK") + " " + std::to_string(newBankNum), 1});
        hid.update(strings.size());
        hid.select(previous);
    }
    std::pair<std::string, int> run();
    void draw() const override;
    void update(touchPosition* touch) override;
    ScreenType type() const override { return ScreenType::SELECTOR; }
private:
    void renameBank();
    void resizeBank();
    std::function<void()> keyboardFunc = nullptr;
    HidVertical hid;
    std::vector<std::pair<std::string, int>> strings;
    size_t previous;
    bool finished = false;
};

#endif
