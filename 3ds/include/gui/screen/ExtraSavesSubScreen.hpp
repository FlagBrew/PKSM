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

#ifndef EXTRASAVESSUBSCREEN_HPP
#define EXTRASAVESSUBSCREEN_HPP

#include "Screen.hpp"
#include <unordered_map>
#include <vector>

class ExtraSavesSubScreen : public Screen
{
public:
    enum Group : s8
    {
        Pt = -1,
        DP,
        HGSS,
        BW,
        B2W2,
        XY,
        ORAS,
        SM,
        USUM
    };
    ExtraSavesSubScreen(Group group);

    void update(touchPosition* touch) override;
    void drawTop(void) const override;
    // Awaiting ideas
    void drawBottom(void) const override;

private:
    void updateSaves();
    std::vector<std::string> currentSaves;
    std::unordered_map<std::string, std::vector<std::string>> dsCurrentSaves;
    std::string addString = "";
    int selectedSave      = -1;
    int firstSave         = 0;
    int numSaves          = 0;
    Group group;
    bool secondSelected = false;
    bool updateConfig   = false;
    // std::vector<std::shared_ptr<Button>> deleteButtons;
};

#endif
