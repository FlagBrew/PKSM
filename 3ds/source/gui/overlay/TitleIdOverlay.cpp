/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2022 Bernardo Giordano, Admiral Fish, piepie62
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
#include "gui.hpp"
#include "loader.hpp"
#include "Species.hpp"
#include "thread.hpp"
#include <array>

namespace
{
    // i know GN is supposed to be Green JPN and Blue elsewhere, but it doesn't matter, core doesn't
    // get the memo for what title it is
    constexpr size_t NUM_TITLES                           = 20;
    constexpr const char* TITLE_ABBREVIATIONS[NUM_TITLES] = {"R", "G", "B", "Y", "G", "S", "C", "R",
        "S", "E", "FR", "LG", "X", "Y", "OR", "AS", "S", "M", "US", "UM"};
    // the GB titles here use the TIDs for the English versions (except Green of course)
    constexpr const char* TITLE_ID_DEFAULTS[NUM_TITLES] = {"0x0004000000171000",
        "0x0004000000170D00", "0x0004000000171100", "0x0004000000171200", "0x0004000000172600",
        "0x0004000000172700", "0x0004000000172800", "0x100", "0x200", "0x300", "0x400", "0x500",
        "0x0004000000055D00", "0x0004000000055E00", "0x000400000011C400", "0x000400000011C500",
        "0x0004000000164800", "0x0004000000175E00", "0x00040000001B5000", "0x00040000001B5100"};
    constexpr pksm::Species TITLE_SPECIES[NUM_TITLES]   = {pksm::Species::Charmander,
          pksm::Species::Bulbasaur, pksm::Species::Squirtle, pksm::Species::Pikachu,
          pksm::Species::HoOh, pksm::Species::Lugia, pksm::Species::Suicune, pksm::Species::Groudon,
          pksm::Species::Kyogre, pksm::Species::Rayquaza, pksm::Species::Charizard,
          pksm::Species::Venusaur, pksm::Species::Xerneas, pksm::Species::Yveltal,
          pksm::Species::Groudon, pksm::Species::Kyogre, pksm::Species::Solgaleo,
          pksm::Species::Lunala, pksm::Species::Necrozma, pksm::Species::Necrozma};
    constexpr int TITLE_FORMS[NUM_TITLES]               = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 2};
    constexpr pksm::GameVersion TITLE_VERSIONS[NUM_TITLES] = {pksm::GameVersion::RD,
        pksm::GameVersion::GN, pksm::GameVersion::BU, pksm::GameVersion::YW, pksm::GameVersion::GD,
        pksm::GameVersion::SV, pksm::GameVersion::C, pksm::GameVersion::R, pksm::GameVersion::S,
        pksm::GameVersion::E, pksm::GameVersion::FR, pksm::GameVersion::LG, pksm::GameVersion::X,
        pksm::GameVersion::Y, pksm::GameVersion::OR, pksm::GameVersion::AS, pksm::GameVersion::SN,
        pksm::GameVersion::MN, pksm::GameVersion::US, pksm::GameVersion::UM};
}

TitleIdOverlay::TitleIdOverlay(ReplaceableScreen& screen) : ReplaceableScreen(&screen), hid(40, 8)
{
}

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
        Gui::pkm(TITLE_SPECIES[i], TITLE_FORMS[i], (pksm::Generation)TITLE_VERSIONS[i],
            pksm::Gender::Genderless, x * 50 + 7, y * 48 + 2);
        Gui::text(TITLE_ABBREVIATIONS[i], x * 50 + 25, y * 48 + 34, FONT_SIZE_9, COLOR_WHITE,
            TextPosX::CENTER, TextPosY::TOP);
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
        Threads::executeTask(TitleLoader::scanTitles);
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
    if (titleId.size() != 18 &&
        titleId.size() != 16) // All title IDs must be an optional 0x + 16 hex characters
    {
        swkbdSetInitialText(&state, "0x00040000");
    }
    else
    {
        swkbdSetInitialText(&state, titleId.c_str());
    }
    swkbdSetValidation(&state, SWKBD_ANYTHING, SWKBD_FILTER_CALLBACK, 0);
    swkbdSetFilterCallback(
        &state,
        [](void*, const char** ppMessage, const char* text, size_t textlen)
        {
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
                a = Archive::saveAndContents(
                    MEDIATYPE_GAME_CARD, (u32)tid, (u32)(tid >> 32), false);
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
        if (titleId.empty())
        {
            titleId = TITLE_ID_DEFAULTS[hid.fullIndex()];
        }
        else if (!(titleId[0] == '0' && (titleId[1] == 'x' || titleId[1] == 'X')))
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
