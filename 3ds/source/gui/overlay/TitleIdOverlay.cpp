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

#include "TitleIdOverlay.hpp"
#include "Archive.hpp"
#include "Configuration.hpp"
#include "GameVersion.hpp"
#include "Species.hpp"
#include "gui.hpp"
#include "loader.hpp"
#include "thread.hpp"

namespace
{
    constexpr size_t NUM_TITLES                           = 13;
    constexpr const char* TITLE_ABBREVIATIONS[NUM_TITLES] = {"R", "S", "E", "FR", "LG", "X", "Y", "OR", "AS", "S", "M", "US", "UM"};
    constexpr const char* TITLE_ID_DEFAULTS[NUM_TITLES]   = {"", "", "", "", "", "0x0004000000055D00", "0x0004000000055E00", "0x000400000011C400",
        "0x000400000011C500", "0x0004000000164800", "0x0004000000175E00", "0x00040000001B5000", "0x00040000001B5100"};
    constexpr Species TITLE_SPECIES[NUM_TITLES]      = {Species::Groudon, Species::Kyogre, Species::Rayquaza, Species::Charizard, Species::Venusaur,
        Species::Xerneas, Species::Yveltal, Species::Groudon, Species::Kyogre, Species::Solgaleo, Species::Lunala, Species::Necrozma,
        Species::Necrozma};
    constexpr int TITLE_FORMS[NUM_TITLES]            = {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 2};
    constexpr GameVersion TITLE_VERSIONS[NUM_TITLES] = {GameVersion::R, GameVersion::S, GameVersion::E, GameVersion::FR, GameVersion::LG,
        GameVersion::X, GameVersion::Y, GameVersion::OR, GameVersion::AS, GameVersion::SN, GameVersion::MN, GameVersion::US, GameVersion::UM};
}

TitleIdOverlay::TitleIdOverlay(ReplaceableScreen& screen) : ReplaceableScreen(&screen), hid(40, 8) {}

void TitleIdOverlay::drawTop() const
{
    Gui::sprite(ui_sheet_part_mtx_5x8_idx, 0, 0);

    int selectorX = (hid.index() % 8) * 50;
    int selectorY = (hid.index() / 8) * 48;
    // Selector can never be in an invalid position
    Gui::drawSolidRect(selectorX, selectorY, 49, 47, COLOR_MASKBLACK);
    Gui::drawSolidRect(selectorX, selectorY, 49, 1, COLOR_YELLOW);
    Gui::drawSolidRect(selectorX, selectorY, 1, 47, COLOR_YELLOW);
    Gui::drawSolidRect(selectorX + 48, selectorY, 1, 47, COLOR_YELLOW);
    Gui::drawSolidRect(selectorX, selectorY + 46, 49, 1, COLOR_YELLOW);

    for (size_t i = 0; i < NUM_TITLES; i++)
    {
        int x = i % 8;
        int y = i / 8;
        Gui::pkm(TITLE_SPECIES[i], TITLE_FORMS[i], (Generation)TITLE_VERSIONS[i], Gender::Genderless, x * 50 + 7, y * 48 + 2);
        Gui::text(TITLE_ABBREVIATIONS[i], x * 50 + 25, y * 48 + 34, FONT_SIZE_9, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
    }
}

void TitleIdOverlay::drawBottom() const
{
    dim();
}

void TitleIdOverlay::update(touchPosition* touch)
{
    hid.update(NUM_TITLES);

    u32 kDown = hidKeysDown();

    if (kDown & KEY_B)
    {
        TitleLoader::reloadTitleIds();
        Threads::create([](void*) { TitleLoader::scanTitles(); }, nullptr, 16 * 1024);
        parent->removeOverlay();
        return;
    }
    if (kDown & KEY_A)
    {
        Configuration::getInstance().titleId(TITLE_VERSIONS[hid.fullIndex()], getNewTitleId());
    }
}

std::string TitleIdOverlay::getNewTitleId() const
{
    static constexpr int NUM_CHARS = 18;
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_QWERTY, 3, NUM_CHARS);
    swkbdSetButton(&state, SWKBD_BUTTON_MIDDLE, i18n::localize("RESET").c_str(), false);
    swkbdSetHintText(&state, i18n::localize("TITLE_ID").c_str());
    std::string titleId = Configuration::getInstance().titleId(TITLE_VERSIONS[hid.fullIndex()]);
    if (titleId.empty())
    {
        swkbdSetInitialText(&state, "0x00040000");
    }
    else
    {
        swkbdSetInitialText(&state, titleId.c_str());
    }
    swkbdSetValidation(&state, SWKBD_ANYTHING, SWKBD_FILTER_CALLBACK, 0);
    swkbdSetFilterCallback(&state,
        [](void*, const char** ppMessage, const char* text, size_t textlen) {
            // Explicitly allow setting a TID to nothing
            if (textlen == 0)
            {
                return SWKBD_CALLBACK_OK;
            }
            u64 tid = strtoull(text, nullptr, 16);
            // If I can't open a title's contents archive, chances are it doesn't exist
            Archive a = Archive::saveAndContents(MEDIATYPE_SD, (u32)tid, (u32)(tid >> 32), false);
            if (R_FAILED(a.result()))
            {
                a = Archive::saveAndContents(MEDIATYPE_GAME_CARD, (u32)tid, (u32)(tid >> 32), false);
                if (R_FAILED(a.result()))
                {
                    *ppMessage = i18n::localize("TITLE_ID_NOT_FOUND").c_str();
                    return SWKBD_CALLBACK_CONTINUE;
                }
            }
            return SWKBD_CALLBACK_OK;
        },
        nullptr);
    char input[NUM_CHARS * 3 / 2 + 1] = {0};
    SwkbdButton ret                   = swkbdInputText(&state, input, sizeof(input));
    input[NUM_CHARS * 3 / 2]          = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        titleId = input;
        if (!(titleId[0] == '0' && (titleId[1] == 'x' || titleId[1] == 'X')))
        {
            titleId = "0x" + titleId;
        }
    }
    else if (ret == SWKBD_BUTTON_MIDDLE)
    {
        titleId = TITLE_ID_DEFAULTS[hid.fullIndex()];
    }
    return titleId;
}
