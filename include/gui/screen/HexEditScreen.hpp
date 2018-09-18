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

#ifndef HEXEDITSCREEN_HPP
#define HEXEDITSCREEN_HPP

#include "Screen.hpp"
#include "Button.hpp"
#include "PKX.hpp"
#include "Hid.hpp"
#include "gui.hpp"
#include <vector>

class HexEditScreen : public Screen
{
public:
    HexEditScreen(std::shared_ptr<PKX> pkm);
    void draw() const override;
    void update(touchPosition* touch) override;

    ScreenType type() const override { return ScreenType::HEXEDIT; }
private:
    enum SecurityLevel
    {
        NORMAL,
        OPEN,
        UNRESTRICTED
    };
    std::pair<std::string, SecurityLevel> describe(int i) const;
    bool toggleBit(int selected, int offset);
    bool editNumber(bool high, bool up);
    std::pair<std::string, SecurityLevel> selectedDescription;
    std::vector<int> selectBytes;
    std::shared_ptr<PKX> pkm;
    Hid hid;
    SecurityLevel level = SecurityLevel::NORMAL;
    class HexEditButton : public Button
    {
    public:
        HexEditButton(int x, int y, int w, int h, std::function<bool()> callback, int image, std::string text, bool toggle)
            : Button(x, y, w, h, callback, image, text, FONT_SIZE_11, COLOR_WHITE), toggle(toggle) {}
        void draw() const override
        {
            Gui::sprite(key, xPos, yPos);
            Gui::staticText(text, xPos + 27, yPos, textScale, textScale, textColor);
        }
        bool update(touchPosition* touch)
        {
            if (clicked(touch))
            {
                if (toggle)
                {
                    key = key == ui_sheet_emulated_toggle_green_idx ? ui_sheet_emulated_toggle_red_idx : ui_sheet_emulated_toggle_green_idx;
                }
                return noArg();
            }
            return false;
        }
        void setToggled(bool flag)
        {
            key = flag ? ui_sheet_emulated_toggle_green_idx : ui_sheet_emulated_toggle_red_idx;
        }
        bool isToggle()
        {
            return toggle;
        }
    private:
        bool toggle;
    };
    // Normally I would just use the same buttons for every byte, but since there are some odd things that can be done,
    // I think that this is the better solution. It allows for every byte to have its own set of buttons, allowing bytes
    // to have toggles, bitsetters, and just plain hexediting
    std::vector<std::vector<HexEditButton*>> buttons;
    // For Super Secret Mode
    std::vector<Button*> secretButtons;
};

#endif