/*
*   This file is part of PKSM
*   Copyright (C) 2016-2018 Bernardo Giordano, Admiral Fish, piepie62
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
    ~StorageScreen()
    {
        for (int i = 0; i < 10; i++)
        {
            delete mainButtons[i];
        }
        for (int i = 0; i < 30; i++)
        {
            delete pkmButtons[i];
        }
        TitleLoader::save->cryptBoxData(false);
    }

    void update(touchPosition* touch) override;
    void draw() const override;

    ScreenType type() const override { return ScreenType::STORAGE; }
private:
    bool swapBoxWithStorage() { return false; }
    bool showViewer();
    bool clearBox();
    bool releasePkm();
    bool dumpPkm() { return false; }
    bool backButton();
    // Have to basically reimplement Hid because two Hids don't go well together
    bool lastBox(bool forceBottom = false);
    bool nextBox(bool forceBottom = false);
    bool setBottomIndex(int index);

    bool storageChosen = false;
    std::array<Button*, 10> mainButtons;
    std::array<Button*, 30> pkmButtons;
    int cursorIndex = 0, storageBox = 0, boxBox = 0;
    std::unique_ptr<ViewerScreen> viewer;
    // Storage implementation
};

#endif