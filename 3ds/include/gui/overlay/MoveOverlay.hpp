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

#ifndef MOVEOVERLAY_HPP
#define MOVEOVERLAY_HPP

#include "ListPickerOverlay.hpp"
#include "pkx/IPKFilterable.hpp"
#include <string>
#include <utility>
#include <vector>

class MoveOverlay : public ListPickerOverlay
{
public:
    MoveOverlay(ReplaceableScreen& screen, pksm::IPKFilterable& pkm, int moveIndex);

protected:
    size_t entryCount() const override { return moves.size(); }

    void filter(const std::string& search) override;
    bool commit() override;
    void discard() override;
    std::string entryLine(size_t index) const override;

private:
    pksm::IPKFilterable& object;
    std::vector<std::pair<pksm::Move, std::string>> moves;
    std::vector<std::pair<pksm::Move, std::string>> validMoves;
    int moveIndex;
};

#endif
