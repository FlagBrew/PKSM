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

#ifndef EDITSELECTORSCREEN_HPP
#define EDITSELECTORSCREEN_HPP

#include "Screen.hpp"
#include <array>
#include <vector>

class Button;
class PKX;

class EditSelectorScreen : public Screen
{
public:
    ~EditSelectorScreen();
    EditSelectorScreen();
    // Done with Overlay
    void drawTop() const override {}
    void drawBottom() const override;
    void update(touchPosition* touch) override;

private:
    bool prevBox();
    bool nextBox();
    bool editPokemon();
    void changeBoxName();
    bool clickIndex(int i);
    bool doQR();
    bool releasePokemon();
    bool clonePkm();
    bool goBack();
    std::vector<std::unique_ptr<Button>> buttons;
    std::array<std::unique_ptr<Button>, 36> pkmButtons;
    std::vector<std::unique_ptr<Button>> viewerButtons;
    std::shared_ptr<PKX> moveMon = nullptr;
    std::shared_ptr<PKX> infoMon = nullptr;
    int cursorPos                = 0;
    int box                      = 0;
    bool justSwitched            = true;
    bool menu                    = false;
};

#endif
