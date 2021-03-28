/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2021 Bernardo Giordano, Admiral Fish, piepie62
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

#ifndef SORTOVERLAY_HPP
#define SORTOVERLAY_HPP

#include "Configuration.hpp"
#include "Hid.hpp"
#include "ReplaceableScreen.hpp"
#include "SortScreen.hpp"
#include <array>
#include <string>

class SortOverlay : public ReplaceableScreen
{
public:
    SortOverlay(ReplaceableScreen& screen, SortScreen::SortType& type)
        : ReplaceableScreen(&screen, i18n::localize("A_SELECT") + '\n' + i18n::localize("B_BACK")),
          hid(40, 2),
          out(type)
    {
        hid.update(vals.size());
        hid.select(int(type));
    }
    void drawTop() const override;
    bool replacesTop() const override { return true; }
    void drawBottom() const override;
    void update(touchPosition* touch) override;

private:
    Hid<HidDirection::VERTICAL, HidDirection::HORIZONTAL> hid;
    static constexpr std::array<SortScreen::SortType, 26> vals = {SortScreen::SortType::NONE,
        SortScreen::SortType::DEX, SortScreen::SortType::SPECIESNAME, SortScreen::SortType::FORM,
        SortScreen::SortType::TYPE1, SortScreen::SortType::TYPE2, SortScreen::SortType::HP,
        SortScreen::SortType::ATK, SortScreen::SortType::DEF, SortScreen::SortType::SATK,
        SortScreen::SortType::SDEF, SortScreen::SortType::SPE, SortScreen::SortType::HPIV,
        SortScreen::SortType::ATKIV, SortScreen::SortType::DEFIV, SortScreen::SortType::SATKIV,
        SortScreen::SortType::SDEFIV, SortScreen::SortType::SPEIV, SortScreen::SortType::NATURE,
        SortScreen::SortType::LEVEL, SortScreen::SortType::TID, SortScreen::SortType::HIDDENPOWER,
        SortScreen::SortType::FRIENDSHIP, SortScreen::SortType::NICKNAME,
        SortScreen::SortType::OTNAME, SortScreen::SortType::SHINY};
    SortScreen::SortType& out;
    bool finished = false;
};

#endif
