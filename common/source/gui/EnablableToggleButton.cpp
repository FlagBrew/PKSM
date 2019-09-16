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

#include "EnablableToggleButton.hpp"
#include "gui.hpp"

EnablableToggleButton::EnablableToggleButton(int x, int y, u16 w, u16 h, const std::function<bool()>& callback, const std::function<bool()>& disabled,
    int onImage, const std::string& onText, FontSize onFontSize, PKSM_Color onTextColor, const std::optional<int>& offImage,
    const std::optional<std::string>& offText, const std::optional<FontSize>& offFontSize, const std::optional<PKSM_Color>& offTextColor,
    const std::optional<int>& disabledImage, const std::optional<std::string>& disabledText, const std::optional<FontSize>& disabledFontSize,
    const std::optional<PKSM_Color> disabledTextColor)
    : ToggleButton(x, y, w, h, callback, onImage, onText, onFontSize, onTextColor, offImage, offText, offFontSize, offTextColor, nullptr, true),
      disabled(disabled),
      disabledImage(disabledImage.value_or(offImage.value_or(onImage))),
      disabledText(disabledText.value_or(offText.value_or(onText))),
      disabledFontSize(disabledFontSize.value_or(offFontSize.value_or(onFontSize))),
      disabledTextColor(disabledTextColor.value_or(offTextColor.value_or(onTextColor)))
{
}

void EnablableToggleButton::draw() const
{
    if (disabled())
    {
        Gui::sprite(disabledImage, xPos, yPos);
        if (!text.empty())
        {
            Gui::text(
                disabledText, xPos + width / 2, yPos + height / 2, disabledFontSize, disabledTextColor, TextPosX::CENTER, TextPosY::CENTER, width);
        }
    }
    else
    {
        Button::draw();
    }
}

bool EnablableToggleButton::update(touchPosition* touch)
{
    if (disabled())
    {
        return false;
    }
    else
    {
        return ToggleButton::update(touch);
    }
}
