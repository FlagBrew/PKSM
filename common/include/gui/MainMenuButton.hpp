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

#ifndef MAINMENUBUTTON_HPP
#define MAINMENUBUTTON_HPP

#include "Button.hpp"
#include "ui_sheet.h"

// A clone of Button that adds the main menu image and centers the text differently
class MainMenuButton : public Button
{
public:
    MainMenuButton(int x, int y, u16 w, u16 h, const std::function<bool()>& callback, int image,
        const std::string& text, FontSize size, PKSM_Color textColor, int imageY)
        : Button(x, y, w, h, callback, ui_sheet_mainmenu_button_idx, text, size, textColor)
    {
        menuImage    = image;
        this->imageY = imageY;
    }

    void draw() const override;

private:
    int menuImage, imageY;
};

#endif
