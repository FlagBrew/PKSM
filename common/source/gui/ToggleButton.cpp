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

#include "ToggleButton.hpp"

ToggleButton::ToggleButton(int x, int y, u16 w, u16 h, const std::function<bool()>& callback,
    int onImage, const std::string& onText, FontSize onFontSize, PKSM_Color onTextColor,
    const std::optional<int>& offImage, const std::optional<std::string>& offText,
    const std::optional<FontSize>& offFontSize, const std::optional<PKSM_Color>& offTextColor,
    std::vector<std::unique_ptr<ToggleButton>>* radioCategory, bool disablable)
    : ClickButton(x, y, w, h, callback, onImage, onText, onFontSize, onTextColor),
      radioCategory(radioCategory),
      onText(onText),
      offText(offText.value_or(onText)),
      onColor(onTextColor),
      offColor(offTextColor.value_or(onTextColor)),
      onFontSize(onFontSize),
      offFontSize(offFontSize.value_or(onFontSize)),
      onImage(onImage),
      offImage(offImage.value_or(onImage)),
      disablable(disablable)
{
}

bool ToggleButton::update(touchPosition* touch)
{
    if (!isClicked && clicked(touch))
    {
        isClicked = true;
        if (!currentState)
        {
            setState(true);
        }
        else if (disablable)
        {
            setState(false);
        }
        return noArg();
    }
    else
    {
        isClicked = clicked(touch);
    }
    return false;
}

void ToggleButton::setState(bool state)
{
    if (!state)
    {
        key       = offImage;
        text      = offText;
        textColor = offColor;
        size      = offFontSize;
    }
    else
    {
        if (radioCategory)
        {
            for (auto& button : *radioCategory)
            {
                if (button.get() != this)
                {
                    button->setState(false);
                }
            }
        }
        key       = onImage;
        text      = onText;
        textColor = onColor;
        size      = onFontSize;
    }
    currentState = state;
}
