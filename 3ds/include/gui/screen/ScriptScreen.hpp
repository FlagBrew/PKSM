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

#ifndef SCRIPTSCREEN_HPP
#define SCRIPTSCREEN_HPP

#include "Screen.hpp"
#include "STDirectory.hpp"
#include "HidHorizontal.hpp"

#define PICOC_STACKSIZE (32 * 1024)

class ScriptScreen : public Screen
{
public:
    ScriptScreen();

    void draw() const override;
    void update(touchPosition* touch) override;

    ScreenType type() const override { return ScreenType::SCRIPTS; }
private:
    void updateEntries();
    void applyScript();
    void parsePicoCScript(std::string& file);
    std::string currDirString;
    STDirectory currDir;
    std::vector<std::pair<std::string, bool>> currFiles;
    HidHorizontal hid;
    bool sdSearch, cScripts;
};

#endif
