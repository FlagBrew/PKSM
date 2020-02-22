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

#ifndef MOVEOVERLAY_HPP
#define MOVEOVERLAY_HPP

#include "Hid.hpp"
#include "ReplaceableScreen.hpp"
#include <variant>

class PKX;
class PKFilter;
class Button;

class MoveOverlay : public ReplaceableScreen
{
public:
    MoveOverlay(ReplaceableScreen& screen, const std::variant<std::shared_ptr<PKX>, std::shared_ptr<PKFilter>>& pkm, int moveIndex);
    ~MoveOverlay();
    void drawTop() const override;
    bool replacesTop() const override { return true; }
    void drawBottom() const override;
    void update(touchPosition* touch) override;

private:
    void searchBar();
    std::variant<std::shared_ptr<PKX>, std::shared_ptr<PKFilter>> object;
    Hid<HidDirection::VERTICAL, HidDirection::HORIZONTAL> hid;
    std::vector<std::pair<int, std::string>> moves;
    std::vector<std::pair<int, std::string>> validMoves;
    std::string searchString    = "";
    std::string oldSearchString = "";
    std::unique_ptr<Button> searchButton;
    int moveIndex;
    bool justSwitched = true;
};

#endif
