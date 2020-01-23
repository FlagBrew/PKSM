/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2020 Bernardo Giordano, Admiral Fish, piepie62
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

#ifndef STORAGESCREEN_HPP
#define STORAGESCREEN_HPP

#include "RunnableScreen.hpp"
#include <array>
#include <tuple>

class Button;
class PKX;

// storage, box, slot
class BoxChoice : public RunnableScreen<std::tuple<int, int, int>>
{
public:
    BoxChoice(bool doCrypt);
    ~BoxChoice();

private:
    void drawTop() const override;
    void drawBottom() const override;
    void update(touchPosition* touch) override;

    bool showViewer();
    bool backButton();
    bool prevBox(bool forceBottom = false);
    bool nextBox(bool forceBottom = false);
    bool clickBottomIndex(int index);

    std::array<std::unique_ptr<Button>, 5> mainButtons;
    std::array<std::unique_ptr<Button>, 30> clickButtons;
    std::shared_ptr<PKX> infoMon = nullptr;
    int cursorIndex              = 0;
    int storageBox               = 0;
    int boxBox                   = 0;
    bool justSwitched            = true;
    bool storageChosen           = false;
    bool backHeld                = false;
    bool doCrypt;
};

#endif
