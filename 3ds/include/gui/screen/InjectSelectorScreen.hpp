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

#include "Screen.hpp"
#include "HidHorizontal.hpp"
#include "gui.hpp"
#include "mysterygift.hpp"
#include "QRScanner.hpp"
#include "Button.hpp"
#include "ToggleButton.hpp"
#include <vector>

#ifndef INJECTSELECTORSCREEN_HPP
#define INJECTSELECTORSCREEN_HPP

class InjectSelectorScreen : public Screen
{
public:
    InjectSelectorScreen();
    virtual ~InjectSelectorScreen();
    void update(touchPosition* touch) override;
    void draw(void) const override;
    ScreenType type() const override { return ScreenType::EVENTS; }
private:
    bool doQR(void);
    bool toggleFilter(const std::string& lang);
    bool toggleFilter(u8 type);
    HidHorizontal hid;
    std::vector<nlohmann::json> wondercards;
    std::vector<Button*> buttons;
    std::vector<ToggleButton*> langFilters;
    std::vector<ToggleButton*> typeFilters;

    bool dump = false;
    bool updateGifts = false;
    HidHorizontal dumpHid;
    void dumpCard(void) const;
    std::vector<MysteryGift::giftData> gifts;
    std::string langFilter = "";
    int typeFilter = -1;
};

#endif
