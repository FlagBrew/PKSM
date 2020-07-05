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

#ifndef TOGGLEBUTTON_HPP
#define TOGGLEBUTTON_HPP

#include "ClickButton.hpp"
#include <memory>
#include <optional>

class ToggleButton : public ClickButton
{
public:
    ToggleButton(int x, int y, u16 w, u16 h, const std::function<bool()>& callback, int onImage,
        const std::string& onText, FontSize onFontSize, PKSM_Color onTextColor,
        const std::optional<int>& offImage                        = std::nullopt,
        const std::optional<std::string>& offText                 = std::nullopt,
        const std::optional<FontSize>& offFontSize                = std::nullopt,
        const std::optional<PKSM_Color>& offTextColor             = std::nullopt,
        std::vector<std::unique_ptr<ToggleButton>>* radioCategory = nullptr,
        bool disablable                                           = false);
    ~ToggleButton(void) {}

    virtual bool update(touchPosition* touch) override;
    virtual void setState(bool newState);

protected:
    std::vector<std::unique_ptr<ToggleButton>>* radioCategory;
    std::string onText;
    std::string offText;
    PKSM_Color onColor;
    PKSM_Color offColor;
    FontSize onFontSize;
    FontSize offFontSize;
    int onImage;
    int offImage;
    bool currentState = true, disablable;
};

#endif
