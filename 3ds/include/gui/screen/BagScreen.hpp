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

#ifndef BAGSCREEN_HPP
#define BAGSCREEN_HPP

#include "sav/Sav.hpp"
#include "Screen.hpp"
#include "utils/SmallVector.hpp"

class Button;

class BagScreen : public Screen
{
public:
    BagScreen();

    void update(touchPosition* touch) override;
    void drawTop() const override;
    void drawBottom() const override;

private:
    SmallVector<std::pair<pksm::Sav::Pouch, int>, 15> limits;
    SmallVector<std::pair<pksm::Sav::Pouch, std::span<const int>>, 15> allowedItems;
    std::vector<std::unique_ptr<Button>> amountButtons;
    std::vector<std::unique_ptr<Button>> buttons;
    static constexpr std::array<u16, 35> lgpeKeyItems = {101, 102, 103, 113, 115, 121, 122, 123,
        124, 125, 126, 127, 128, 442, 632, 651, 872, 873, 874, 875, 876, 877, 878, 885, 886, 887,
        888, 889, 890, 891, 892, 893, 894, 895, 896};

    static constexpr std::array<u16, 19> rgbyKeyItems = {
        6, 45, 48, 69, 41, 42, 64, 77, 71, 74, 70, 31, 76, 73, 43, 72, 63, 78, 5};

    bool clickIndex(int i);
    bool switchPouch(int i);
    void editItem();
    void editCount(bool up, int selected);
    void setCount(int selected);
    bool canEdit(pksm::Sav::Pouch pouch, const pksm::Item& item) const;
    void updateFirstEmpty();

    std::span<const int> itemsForPouch(pksm::Sav::Pouch pouch) const;

    int currentPouch    = 0;
    int selectedItem    = 0;
    int firstItem       = 0;
    int firstEmpty      = 0;
    bool selectingPouch = true;
    bool justSwitched   = true;
};

#endif
