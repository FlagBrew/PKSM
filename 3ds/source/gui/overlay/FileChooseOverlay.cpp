/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2025 Bernardo Giordano, Admiral Fish, piepie62
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

#include "FileChooseOverlay.hpp"
#include "Configuration.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"

FileChooseOverlay::FileChooseOverlay(
    ReplaceableScreen& screen, std::string& retString, const std::string& rootString)
    : ReplaceableScreen(&screen, i18n::localize("A_SELECT") + '\n' + i18n::localize("B_BACK")),
      browser(rootString),
      string(retString),
      hid(9, 1)
{
    if (!browser.good())
    {
        Gui::warn(i18n::localize("FOLDER_DOESNT_EXIST"));
    }
}

void FileChooseOverlay::drawTop() const
{
    const auto& entries = browser.entries();

    // slight change to stripy top
    Gui::drawSolidRect(0, 0, 400, 240, PKSM_Color(26, 35, 126, 255));
    for (int x = -240; x < 400; x += 7)
    {
        Gui::drawLine(x, 0, x + 240, 240, 2, COLOR_LINEBLUE);
    }
    Gui::drawSolidRect(0, 0, 400, 25, PKSM_Color(15, 22, 89, 255));

    if (!entries.empty())
    {
        Gui::drawSolidRect(0, 20 + hid.index() * 25, 400, 25, PKSM_Color(128, 128, 128, 255));
        Gui::drawSolidRect(1, 21 + hid.index() * 25, 398, 23, COLOR_MASKBLACK);
    }

    for (size_t i = hid.page() * hid.maxVisibleEntries();
         i < (hid.page() + 1) * hid.maxVisibleEntries(); i++)
    {
        if (i >= entries.size())
        {
            break;
        }
        else
        {
            Gui::sprite(entries[i].directory ? ui_sheet_icon_folder_idx : ui_sheet_icon_script_idx,
                3, 23 + i % hid.maxVisibleEntries() * 25);
        }
    }
    for (size_t i = hid.page() * hid.maxVisibleEntries();
         i < (hid.page() + 1) * hid.maxVisibleEntries(); i++)
    {
        if (i >= entries.size())
        {
            break;
        }
        else
        {
            Gui::text(entries[i].name, 30, 24 + (i % hid.maxVisibleEntries() * 25), FONT_SIZE_11,
                COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        }
    }

    // Leaving space for the icon
    Gui::text(browser.path(), 15, 2, FONT_SIZE_11, COLOR_YELLOW, TextPosX::LEFT, TextPosY::TOP);
}

void FileChooseOverlay::update(touchPosition* touch)
{
    hid.update(browser.entries().size());
    u32 down = hidKeysDown();
    if (down & KEY_B)
    {
        if (browser.atRoot())
        {
            parent->removeOverlay();
            return;
        }
        else if (browser.leave())
        {
            hid.select(0);
        }
    }
    else if (down & KEY_A)
    {
        const auto& entries = browser.entries();
        if (!entries.empty())
        {
            const size_t index = hid.fullIndex();
            if (entries[index].directory)
            {
                if (browser.enter(index))
                {
                    hid.select(0);
                }
                else
                {
                    Gui::warn(i18n::localize("FOLDER_DOESNT_EXIST"));
                }
            }
            else
            {
                if (Gui::showChoiceMessage((i18n::localize("FILE_CONFIRM_CHOICE") + '\n') + '\'' +
                                           entries[index].name + '\''))
                {
                    string = browser.pathOf(index);
                    parent->removeOverlay();
                }
            }
        }
    }
}
