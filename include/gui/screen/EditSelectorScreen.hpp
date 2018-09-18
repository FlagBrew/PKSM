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

#ifndef EDITSELECTORSCREEN_HPP
#define EDITSELECTORSCREEN_HPP

#include "Screen.hpp"
#include <vector>
#include "ViewerScreen.hpp"
#include "Button.hpp"
#include "loader.hpp"

class EditSelectorScreen : public Screen
{
public:
    ~EditSelectorScreen()
    {
        for (Button* button : buttons)
        {
            delete button;
        }
        
        for (Button* button : pkmButtons)
        {
            delete button;
        }

        TitleLoader::save->cryptBoxData(false);
    }
    EditSelectorScreen();
    void draw() const override;
    void update(touchPosition* touch) override;
    ScreenType type() const override { return ScreenType::EDITSELECT; }
private:
    bool lastBox();
    bool nextBox();
    bool editPokemon(std::shared_ptr<PKX> pkm);
    std::vector<Button*> buttons;
    std::array<Button*, 36> pkmButtons;
    std::shared_ptr<ViewerScreen> viewer;
    int cursorPos = 0;
    int box = 0;
};

#endif