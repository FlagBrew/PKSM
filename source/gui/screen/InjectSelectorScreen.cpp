/*
*   This file is part of PKSM
*   Copyright (C) 2016-2018 Bernardo Giordano, Admiral Fish, piepie62
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

#include "InjectSelectorScreen.hpp"
#include "InjectorScreen.hpp"
#include "WC7.hpp"

static u8 test[] = { 0xf5, 0x01, 0x41, 0x20, 0x20, 0x20, 0x70, 0x20, 0x6f, 0x20, 0x77, 0x20, 0x65, 0x20, 0x72, 0x20, 0x66, 0x20, 0x75, 0x20, 0x6c, 0x20, 0x20, 0x20, 0x47, 0x20, 0x61, 0x20, 0x72, 0x20, 0x63, 0x20, 0x68, 0x20, 0x6f, 0x20, 0x6d, 0x20, 0x70, 0x20, 0x20, 0x20, 0x66, 0x20, 0x6f, 0x20, 0x72, 0x20, 0x20, 0x20, 0x59, 0x20, 0x6f, 0x20, 0x75, 0x20, 0x21, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x85, 0x2d, 0x33, 0x01, 0x2c, 0x20, 0x01, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0xff, 0x20, 0x91, 0x2b, 0xf5, 0x01, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x20, 0xfa, 0x20, 0xa3, 0x20, 0x51, 0x01, 0x5b, 0x20, 0xf2, 0x20, 0xbd, 0x01, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0xff, 0x20, 0x20, 0x03, 0x20, 0x20, 0x41, 0x9c, 0x32, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x20, 0x57, 0x20, 0x49, 0x20, 0x4e, 0x20, 0x54, 0x20, 0x45, 0x20, 0x52, 0x20, 0x32, 0x20, 0x30, 0x20, 0x31, 0x20, 0x33, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x32, 0x20, 0x20, 0x20, 0x02, 0x28, 0x6b, 0xee, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 };

InjectSelectorScreen::InjectSelectorScreen() : hid(10, 2)
{
    for (int i = 0; i < 200; i++)
    {
        wondercards.push_back({StringUtils::format("Wondercard %d", i), 774, rand() % 14});
    }
}

void InjectSelectorScreen::update(touchPosition* touch)
{
    Screen::update();
    hid.update(wondercards.size());
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_B)
    {
        Gui::screenBack();
        return;
    }
    if (downKeys & KEY_A)
    {
        Gui::setScreen(std::unique_ptr<Screen>(new InjectorScreen(std::unique_ptr<WCX>(new WC6(test, false)))));
        return;
    }
}

void InjectSelectorScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    Gui::backgroundTop(true);

    Gui::dynamicText(GFX_TOP, 4, "Event Database", FONT_SIZE_14, FONT_SIZE_14, C2D_Color32(140, 158, 255, 255));

    for (size_t i = 0; i < 10; i++)
    {
        if (i % 2 == 0)
        {
            int x = 20;
            int y = 41 + (i / 2) * 37;
            if (i == 0)
            {
                Gui::sprite(i == hid.index() ? ui_sheet_eventmenu_bar_selected_idx : ui_sheet_eventmenu_bar_unselected_idx, x, y);
            }
            else if (i == 8)
            {
                Gui::sprite(i == hid.index() ? ui_sheet_emulated_eventmenu_bar_selected_flipped_vertical_idx : ui_sheet_emulated_eventmenu_bar_unselected_flipped_vertical_idx, x, y);
            }
            else
            {
                C2D_DrawRectSolid(x, y, 0.5f, 178, 34, i == hid.index() ? C2D_Color32(0x3D, 0x5A, 0xFE, 0xFF) : C2D_Color32(0x8C, 0x9E, 0xFF, 0xFF));
            }
        }
        else
        {
            int x = 201;
            int y = 41 + (i / 2) * 37;
            if (i == 1)
            {
                Gui::sprite(i == hid.index() ? ui_sheet_emulated_eventmenu_bar_selected_flipped_horizontal_idx : ui_sheet_emulated_eventmenu_bar_unselected_flipped_horizontal_idx, x, y);
            }
            else if (i == 9)
            {
                Gui::sprite(i == hid.index() ? ui_sheet_emulated_eventmenu_bar_selected_flipped_both_idx : ui_sheet_emulated_eventmenu_bar_unselected_flipped_both_idx, x, y);
            }
            else
            {
                C2D_DrawRectSolid(x, y, 0.5f, 178, 34, i == hid.index() ? C2D_Color32(0x3D, 0x5A, 0xFE, 0xFF) : C2D_Color32(0x8C, 0x9E, 0xFF, 0xFF));
            }
        }
    }

    for (size_t i = hid.page() * 10; i < hid.page() * 10 + 10; i++)
    {
        if (i >= wondercards.size())
        {
            break;
        }
        else
        {
            int x = i % 2 == 0 ? 21 : 201;
            int y = 43 + ((i % 10) / 2) * 37;
            // TODO: get rid of this placeholder
            #define SAVEGENERATION 7
            Gui::pkm(wondercards[i].species, wondercards[i].form, SAVEGENERATION, x, y);
            Gui::dynamicText(x + 34, y + 10, 143, wondercards[i].name, FONT_SIZE_11, FONT_SIZE_11, i == hid.fullIndex() ? C2D_Color32(232, 234, 246, 255) : C2D_Color32(26, 35, 126, 255));
        }
    }

    C2D_SceneBegin(g_renderTargetBottom);
    Gui::backgroundBottom(true);
    Gui::dynamicText(GFX_BOTTOM, 224, "Press \uE000 to continue or \uE001 to return.", FONT_SIZE_11, FONT_SIZE_11, C2D_Color32(197, 202, 233, 255));

    Gui::sprite(ui_sheet_eventmenu_page_indicator_idx, 65, 13);

    Gui::staticText("\uE004", 75, 17, FONT_SIZE_18, FONT_SIZE_18, C2D_Color32(197, 202, 233, 255), false);
    Gui::staticText("\uE005", 228, 17, FONT_SIZE_18, FONT_SIZE_18, C2D_Color32(197, 202, 233, 255), false);
    Gui::dynamicText(92, 20, 136, StringUtils::format("%d/%d", hid.page() + 1, wondercards.size() % 10 == 0 ? wondercards.size() / 10 : wondercards.size() / 10 + 1), FONT_SIZE_12, FONT_SIZE_12, C2D_Color32(197, 202, 233, 255));
}