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

#ifndef STORAGESCREEN_HPP
#define STORAGESCREEN_HPP

#include "Screen.hpp"
#include "Sav.hpp"
#include "PKX.hpp"
#include "ViewerScreen.hpp"
#include "Button.hpp"
#include "loader.hpp"
#include <array>

class StorageScreen : public Screen
{
public:
    StorageScreen();
    ~StorageScreen();

    void update(touchPosition* touch) override;
    void draw() const override;

    ScreenType type() const override { return ScreenType::STORAGE; }
private:
    bool swapBoxWithStorage();
    bool showViewer();
    bool clearBox();
    bool releasePkm();
    bool dumpPkm();
    bool duplicate();
    bool backButton();
    // Have to basically reimplement Hid because two Hids don't go well together
    bool lastBox(bool forceBottom = false);
    bool nextBox(bool forceBottom = false);
    bool prevBoxTop();
    bool nextBoxTop();
    bool clickBottomIndex(int index);
    void setBoxName(bool storage);
    void pickup();
    bool isValidTransfer(std::shared_ptr<PKX> moveMon, bool bulkTransfer = false);

    bool storageChosen = false;
    bool fromStorage = false;
    std::array<Button*, 10> mainButtons;
    std::array<Button*, 31> clickButtons;
    int cursorIndex = 0, storageBox = 0, boxBox = 0;
    std::unique_ptr<ViewerScreen> viewer;
    std::shared_ptr<PKX> moveMon = nullptr;
    int partyNum = -1;
    bool justSwitched = true;
    Bank bank;
};

#endif
