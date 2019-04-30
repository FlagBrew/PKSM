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

#ifndef BAGSCREEN_HPP
#define BAGSCREEN_HPP

#include "BagItemOverlay.hpp"
#include "Button.hpp"
#include "Sav.hpp"
#include "Screen.hpp"

class BagScreen : public Screen
{
public:
    BagScreen();
    virtual ~BagScreen();

    void update(touchPosition* touch) override;
    void draw() const override;

    ScreenType type() const { return ScreenType::BAG; }

private:
    std::vector<std::pair<Pouch, int>> limits;
    std::map<Pouch, std::vector<int>> allowedItems;
    std::vector<Button*> amountButtons;
    int currentPouch = 0;
    std::vector<Button*> buttons;
    int selectedItem    = 0;
    int firstItem       = 0;
    int firstEmpty      = 0;
    bool selectingPouch = false;
    bool clickIndex(int i);
    bool switchPouch(int i);
    void editItem();
    void editCount(bool up, int selected);
    void setCount(int selected);
    bool canEdit(Pouch pouch, Item& item) const;

    std::array<u16, 35> lgpeKeyItems = {101, 102, 103, 113, 115, 121, 122, 123, 124, 125, 126, 127, 128, 442, 632, 651, 872, 873, 874, 875, 876, 877,
        878, 885, 886, 887, 888, 889, 890, 891, 892, 893, 894, 895, 896};

    bool justSwitched = true;
};

#endif
