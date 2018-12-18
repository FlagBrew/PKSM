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

#include <memory>
#include <list>
#include "Screen.hpp"
#include "Title.hpp"
#include "Directory.hpp"
#include "Button.hpp"
#include "loader.hpp"

#ifndef SAVELOADSCREEN_HPP
#define SAVELOADSCREEN_HPP

class SaveLoadScreen : public Screen
{
public:
    SaveLoadScreen();
    ~SaveLoadScreen()
    {
        for (auto b : buttons)
        {
            delete b;
        }
    }
    void drawSelector(int x, int y) const;
    void draw() const override;
    ScreenType type() const override { return ScreenType::TITLELOAD; }
    void update(touchPosition* touch) override;

private:
    int saveGroup = 0;
    // Has to be mutable because no const operator[]
    mutable std::unordered_map<int, std::vector<std::pair<std::string, std::string>>> saves;
    int firstSave = 0;
    std::vector<Button*> buttons;
    int selectedSave = -1;
    mutable bool platinum = false;
    bool hasTitles[4] = {false, false, false, false};
    std::list<std::string> nandTitles;
    int missingGroups = 4;
    bool selectedGroup = false;
    bool setSelectedSave(int i);
    bool increaseFirstSave()
    {
        int accessSaves = saveGroup;
        if (accessSaves > 3 && accessSaves < 7)
        {
            for (int i = 0; i < 4; i++)
            {
                if (i + 3 >= saveGroup)
                {
                    break;
                }
                if (hasTitles[i])
                {
                    accessSaves++;
                }
            }
        }
        if (firstSave < (int) saves[accessSaves].size() - 1)
        {
            firstSave++;
        }
        return false;
    }
    bool decreaseFirstSave()
    {
        if (firstSave > 0)
        {
            firstSave--;
        }
        return false;
    }
    std::string_view titleName(int index) const;
    int saveIndex(int index) const;

    bool loadSave(void);
};

#endif