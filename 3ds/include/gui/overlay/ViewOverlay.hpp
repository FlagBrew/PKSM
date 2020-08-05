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

#ifndef VIEWOVERLAY_HPP
#define VIEWOVERLAY_HPP

#include "ReplaceableScreen.hpp"
#include "pkx/PKX.hpp"
#include <functional>
#include <variant>

class ViewOverlay : public ReplaceableScreen
{
public:
    // if it's not green, it's blue
    ViewOverlay(ReplaceableScreen& screen, std::unique_ptr<pksm::PKX>& pkm, bool green,
        const std::string& instr = "")
        : ReplaceableScreen(&screen, instr), pokemon(pkm), green(green)
    {
    }
    ViewOverlay(
        ReplaceableScreen& screen, pksm::PKX& pkm, bool green, const std::string& instr = "")
        : ReplaceableScreen(&screen, instr), pokemon(pkm), green(green)
    {
    }
    virtual void drawTop() const override;
    bool replacesTop() const override { return true; }
    virtual void drawBottom() const override {}
    bool handlesUpdate() const override { return false; }
    void update(touchPosition* touch) override {}

protected:
    std::variant<std::reference_wrapper<std::unique_ptr<pksm::PKX>>,
        std::reference_wrapper<pksm::PKX>>
        pokemon;
    pksm::PKX& getPKM() const;
    bool green;

private:
    void drawPkm(pksm::PKX& pkm) const;
    void drawEmpty() const;
};

#endif
