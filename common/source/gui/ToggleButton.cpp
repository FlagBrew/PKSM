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

#include "ToggleButton.hpp"

ToggleButton::ToggleButton(int x, int y, u16 w, u16 h, const std::function<bool()>& callback, int onImage, const std::string& onText,
    float onTextScale, PKSM_Color onTextColor, const std::optional<int>& offImage, const std::optional<std::string>& offText,
    const std::optional<float>& offTextScale, const std::optional<PKSM_Color>& offTextColor, std::vector<std::unique_ptr<ToggleButton>>* radioCategory,
    bool disablable)
    : ClickButton(x, y, w, h, callback, onImage, onText, onTextScale, onTextColor),
      onImage(onImage),
      onText(onText),
      onScale(onTextScale),
      onColor(onTextColor),
      offImage(offImage.value_or(onImage)),
      offText(offText.value_or(onText)),
      offScale(offTextScale.value_or(onTextScale)),
      offColor(offTextColor.value_or(onTextColor)),
      radioCategory(radioCategory),
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
        textScale = offScale;
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
        textScale = onScale;
    }
    currentState = state;
}
