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

#include "NatureOverlay.hpp"
#include "Configuration.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "pkx/PKX.hpp"

namespace
{
    constexpr std::string_view stats[] = {"ATTACK", "DEFENSE", "SPEED", "SPATK.", "SPDEF."};
}

NatureOverlay::NatureOverlay(ReplaceableScreen& screen, pksm::PKX& pkm)
    : ReplaceableScreen(&screen, i18n::localize("A_SELECT") + '\n' + i18n::localize("B_BACK")),
      pkm(pkm),
      hid(25, 5)
{
    hid.update(25);
    hid.select(size_t(pkm.nature()));
}

void NatureOverlay::drawBottom() const
{
    dim();
    Gui::text(i18n::localize("EDITOR_INST"), 160, 115, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER,
        TextPosY::TOP);
}

void NatureOverlay::drawTop() const
{
    Gui::sprite(ui_sheet_part_editor_6x6_idx, 0, 0);

    Gui::text(i18n::localize("NEUTRAL"), 0 + 65 / 2, 12, FONT_SIZE_11, COLOR_YELLOW,
        TextPosX::CENTER, TextPosY::TOP);
    for (int i = 0; i < 5; i++)
    {
        Gui::text(std::string("-") + i18n::localize(std::string(stats[i])), i * 67 + 99, 12,
            FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
        Gui::text(std::string("+") + i18n::localize(std::string(stats[i])), 32, i * 40 + 52,
            FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
    }

    int selectorX = (hid.index() % 5) * 67 + 66;
    int selectorY = (hid.index() / 5) * 40 + 40;
    // Selector
    Gui::drawSolidRect(selectorX, selectorY, 66, 39, COLOR_MASKBLACK);
    Gui::drawSolidRect(selectorX, selectorY, 66, 1, COLOR_YELLOW);
    Gui::drawSolidRect(selectorX, selectorY, 1, 39, COLOR_YELLOW);
    Gui::drawSolidRect(selectorX + 65, selectorY, 1, 39, COLOR_YELLOW);
    Gui::drawSolidRect(selectorX, selectorY + 38, 66, 1, COLOR_YELLOW);

    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 5; x++)
        {
            Gui::text(
                i18n::nature(Configuration::getInstance().language(), pksm::Nature{u8(x + y * 5)}),
                x * 67 + 99, y * 40 + 52, FONT_SIZE_11, x == y ? COLOR_YELLOW : COLOR_WHITE,
                TextPosX::CENTER, TextPosY::TOP);
        }
    }
}

void NatureOverlay::update(touchPosition* touch)
{
    hid.update(25);
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_A)
    {
        pkm.nature(pksm::Nature{u8(hid.fullIndex())});
        parent->removeOverlay();
        return;
    }
    else if (downKeys & KEY_B)
    {
        parent->removeOverlay();
        return;
    }
}
