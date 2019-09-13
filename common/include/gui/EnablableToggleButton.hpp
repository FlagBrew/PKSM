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

#ifndef ENABLABLETOGGLEBUTTON_HPP
#define ENABLABLETOGGLEBUTTON_HPP

#include "ToggleButton.hpp"

class EnablableToggleButton : public ToggleButton
{
public:
    EnablableToggleButton(int x, int y, u16 w, u16 h, const std::function<bool()>& callback, const std::function<bool()>& disabled, int onImage,
        const std::string& onText, float onTextScale, PKSM_Color onTextColor, const std::optional<int>& offImage = std::nullopt,
        const std::optional<std::string>& offText = std::nullopt, const std::optional<float>& offTextScale = std::nullopt,
        const std::optional<PKSM_Color>& offTextColor = std::nullopt, const std::optional<int>& disabledImage = std::nullopt,
        const std::optional<std::string>& disabledText = std::nullopt, const std::optional<float>& disabledTextScale = std::nullopt,
        const std::optional<PKSM_Color> disabledTextColor = std::nullopt);

    virtual bool update(touchPosition* touch) override;
    virtual void draw() const override;

protected:
    std::function<bool()> disabled;
    int disabledImage;
    std::string disabledText;
    float disabledTextScale;
    PKSM_Color disabledTextColor;
};

#endif
