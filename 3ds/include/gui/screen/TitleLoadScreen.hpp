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

#ifndef TITLELOADSCREEN_HPP
#define TITLELOADSCREEN_HPP

#include "Language.hpp"
#include "Screen.hpp"
#include <cmath>
#include <memory>

class Title;
class Button;

class TitleLoadScreen : public Screen
{
public:
    TitleLoadScreen();
    void drawTop() const override;
    void drawBottom() const override;
    void update(touchPosition* touch) override;
    void makeInstructions();

private:
    Language oldLang;
    int selectedTitle = -2;
    std::vector<std::string> availableCheckpointSaves;
    int firstSave = -1;
    std::vector<std::unique_ptr<Button>> buttons;
    int selectedSave    = -1;
    bool selectedGame   = false;
    bool uninstGameView = false;
    bool setSelectedSave(int i);
    bool increaseFirstSave()
    {
        if (firstSave < (int)availableCheckpointSaves.size() - 1)
        {
            firstSave++;
        }
        return false;
    }
    bool decreaseFirstSave()
    {
        if (firstSave > -1)
        {
            firstSave--;
        }
        return false;
    }

    std::shared_ptr<Title> titleFromIndex(int i) const;

    bool loadSave(void) const;
};

#endif
