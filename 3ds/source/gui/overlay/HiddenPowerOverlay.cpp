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

#include "HiddenPowerOverlay.hpp"
#include "Configuration.hpp"
#include "gui.hpp"
#include "pkx/PKX.hpp"

HiddenPowerOverlay::HiddenPowerOverlay(ReplaceableScreen& screen, std::shared_ptr<pksm::PKX> pkm)
    : ReplaceableScreen(&screen, i18n::localize("A_SELECT") + '\n' + i18n::localize("B_BACK")),
      pkm(pkm),
      hid(16, 4)
{
    hid.update(16);
    hid.select(size_t(pkm->hpType()));
}

void HiddenPowerOverlay::drawBottom() const
{
    dim();
    Gui::text(i18n::localize("EDITOR_INST"), 160, 115, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER,
        TextPosY::TOP);
}

void HiddenPowerOverlay::drawTop() const
{
    Gui::sprite(ui_sheet_part_mtx_4x4_idx, 0, 0);
    int x = (hid.index() % 4) * 100;
    int y = (hid.index() / 4) * 60;
    // Selector
    Gui::drawSolidRect(x, y, 99, 59, COLOR_MASKBLACK);
    Gui::drawSolidRect(x, y, 99, 1, COLOR_YELLOW);
    Gui::drawSolidRect(x, y, 1, 59, COLOR_YELLOW);
    Gui::drawSolidRect(x + 98, y, 1, 59, COLOR_YELLOW);
    Gui::drawSolidRect(x, y + 58, 99, 1, COLOR_YELLOW);
    for (int i = 0; i < 16; i++)
    {
        Gui::type(Configuration::getInstance().language(), pksm::Type{u8(i + 1)},
            23 + (i % 4) * 100, 20 + (i / 4) * 60);
    }
}

void HiddenPowerOverlay::update(touchPosition* touch)
{
    hid.update(16);
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_A)
    {
        pkm->hpType(pksm::Type{u8(hid.fullIndex() + 1)});
        parent->removeOverlay();
        return;
    }
    else if (downKeys & KEY_B)
    {
        parent->removeOverlay();
        return;
    }
}
