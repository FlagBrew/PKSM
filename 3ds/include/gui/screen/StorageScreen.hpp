/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2021 Bernardo Giordano, Admiral Fish, piepie62
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

#include "pkx/PKFilter.hpp"
#include "pkx/PKX.hpp"
#include "Screen.hpp"
#include <array>
#include <memory>
#include <vector>

class Button;

class StorageScreen : public Screen
{
    friend class StorageViewOverlay;

public:
    StorageScreen();

    void update(touchPosition* touch) override;
    void drawTop() const override;
    void drawBottom() const override;

private:
    static constexpr PKSM_Color COLOR_GREEN_HIGHLIGHT = PKSM_Color(0x50, 0xC0, 0x40, 0xC0);
    static void doDump(const pksm::PKX& dumpMon);

    bool swapBoxWithStorage();
    bool showViewer();
    bool clearBox();
    bool releasePkm();
    bool dumpPkm();
    bool duplicate();
    bool backButton();
    // Have to basically reimplement Hid because two Hids don't go well together
    bool prevBox(bool forceBottom = false);
    bool nextBox(bool forceBottom = false);
    bool prevBoxTop();
    bool nextBoxTop();
    bool clickBottomIndex(int index);
    void setBoxName(bool storage);
    void pickup();
    void pickupSwap();
    void pickupMulti();
    void pickupSingle();
    void postPickup();
    void putDownSwap();
    void putDownNonSwap();
    bool checkPutDownBounds();
    bool isValidTransfer(const pksm::PKX& moveMon, bool bulkTransfer = false);
    void scrunchSelection();
    void grabSelection(bool remove);

    std::array<std::unique_ptr<Button>, 10> mainButtons;
    std::array<std::unique_ptr<Button>, 31> clickButtons;
    std::unique_ptr<pksm::PKX> infoMon = nullptr;
    std::vector<std::unique_ptr<pksm::PKX>> moveMon;
    std::vector<int> partyNum;
    // While selecting, XY coords of original selection.
    // When selected, dimensions of moveMon
    // If pickupMode == SWAP, box number & slot pair
    std::pair<int, int> selectDimensions   = {0, 0};
    std::shared_ptr<pksm::PKFilter> filter = std::make_shared<pksm::PKFilter>();
    int cursorIndex                        = 0;
    int storageBox                         = 0;
    int boxBox                             = 0;

    enum PickupMode : u8
    {
        SINGLE,
        SWAP,
        MULTI
    } pickupMode = SINGLE;

    bool currentlySelecting = false;
    bool justSwitched       = true;
    bool storageChosen      = false;
    bool fromStorage        = false;
    bool backHeld           = false;
};

#endif
