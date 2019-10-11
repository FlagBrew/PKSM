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

#ifndef FORMOVERLAY_HPP
#define FORMOVERLAY_HPP

#include "Configuration.hpp"
#include "HidHorizontal.hpp"
#include "PKFilter.hpp"
#include "PKX.hpp"
#include "ReplaceableScreen.hpp"
#include "i18n.hpp"
#include <memory>
#include <variant>

class FormOverlay : public ReplaceableScreen
{
public:
    FormOverlay(ReplaceableScreen& screen, const std::variant<std::shared_ptr<PKX>, std::shared_ptr<PKFilter>>& object, u8 formCount)
        : ReplaceableScreen(&screen, i18n::localize("A_SELECT") + '\n' + i18n::localize("B_BACK")), object(object), hid(40, 6), formCount(formCount)
    {
        hid.update(40);
        if (object.index() == 0)
        {
            hid.select(std::get<0>(object)->alternativeForm());
        }
        else
        {
            hid.select(std::get<1>(object)->alternativeForm());
        }
    }
    virtual ~FormOverlay() {}
    void drawTop() const override;
    bool replacesTop() const override { return true; }
    void drawBottom() const override;
    void update(touchPosition* touch) override;

private:
    std::variant<std::shared_ptr<PKX>, std::shared_ptr<PKFilter>> object;
    HidHorizontal hid;
    u8 formCount;
};

#endif
