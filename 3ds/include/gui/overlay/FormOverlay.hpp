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

#ifndef FORMOVERLAY_HPP
#define FORMOVERLAY_HPP

#include "Configuration.hpp"
#include "Hid.hpp"
#include "ReplaceableScreen.hpp"
#include "pkx/IPKFilterable.hpp"
#include <memory>
#include <variant>

class FormOverlay : public ReplaceableScreen
{
public:
    FormOverlay(ReplaceableScreen& screen, pksm::IPKFilterable& object, u16 formCount);
    void drawTop() const override;
    bool replacesTop() const override { return true; }
    void drawBottom() const override;
    void update(touchPosition* touch) override;

private:
    pksm::IPKFilterable& object;
    Hid<HidDirection::HORIZONTAL, HidDirection::HORIZONTAL> hid;
    u16 formCount;
};

#endif
