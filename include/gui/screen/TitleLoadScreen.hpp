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

#include "Screen.hpp"
#include <memory>
#include "Title.hpp"
#include "Directory.hpp"
#include "Button.hpp"
#include "loader.hpp"

#ifndef TITLELOADSCREEN_HPP
#define TITLELOADSCREEN_HPP

class TitleLoadScreen : public Screen
{
public:
    TitleLoadScreen();
    void draw() const override;
    ScreenType type() const override { return ScreenType::TITLELOAD; }
    void update(touchPosition* touch) override;
private:
    int selectedTitle = -2;
    std::vector<std::string> availableCheckpointSaves;
    int firstSave = -1;
    std::vector<Button*> buttons;
    int selectedSave = -1;
    bool selectedGame = false;
    bool setSelectedSave(int i)
    {
        selectedSave = i;
        return false;
    }
    bool increaseFirstSave()
    {
        if (firstSave > availableCheckpointSaves.size() - 1)
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

    std::shared_ptr<Title> titleFromIndex(int i) const
    {
        if (i == -1)
        {
            return TitleLoader::cardTitle;
        }
        else if (i == -2)
        {
            return nullptr;
        }
        else if (i < TitleLoader::nandTitles.size())
        {
            return TitleLoader::nandTitles[i];
        }
        return nullptr;
    }

    bool loadSave(void) const;
};

#endif