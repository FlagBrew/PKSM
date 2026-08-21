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

#ifndef GPSSPAGE_HPP
#define GPSSPAGE_HPP

#include "enums/Generation.hpp"
#include "utils/coretypes.h"
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

// One page of GPSS search results, and nothing else: a response body in, either a validated page
// or a typed error out. No curl, no libctru, no GUI, so every rule the server's JSON has to obey
// is checked in one place and exercised off-device with fixtures.
//
// Both GPSS searches - single Pokémon and bundles - come back through here. They differ in the
// array they return, in the name of the field they report errors in, and in nothing else, so the
// difference is a Kind rather than a second copy of the parser.
namespace Gpss
{
    enum class Kind
    {
        Pokemon, // api/v2/gpss/search/pokemon: one Pokémon per entry
        Bundles  // api/v2/gpss/search/bundles: up to six per entry
    };

    inline constexpr size_t MAX_BUNDLE_SIZE = 6;

    // One Pokémon, decoded once when the page is parsed rather than on every draw.
    struct Mon
    {
        pksm::Generation generation = pksm::Generation::UNUSED;
        std::vector<u8> data;
        bool legal = false;
        // The code that downloads this Pokémon on its own. Empty if the server did not send one.
        std::string downloadCode;
    };

    // A bundle. A Pokémon search returns one Pokémon per entry, so the screens can index both
    // shapes the same way.
    struct Entry
    {
        // The code that downloads the whole bundle. Empty on a Pokémon page.
        std::string downloadCode;
        std::vector<Mon> pokemon;
    };

    struct Page
    {
        // Items on the server across every page, as the server counts them. The browser only uses
        // it to notice that the search moved under it between two fetches.
        int total = 0;
        // Never below 1: a page the caller can be looking at is a page that exists.
        int pages = 1;
        std::vector<Entry> entries;
    };

    enum class Error
    {
        NotJson,  // the body did not parse at all
        NotAPage, // no total, no page count, or no item array of the right type
        BadEntry, // an entry lacks a field the screens dereference
    };

    std::variant<Page, Error> parse(std::string_view body, Kind kind);

    // The code GPSS puts in a rejection body, under a field name that depends on the search.
    // nullopt when the body carries no such code.
    std::optional<int> errorCode(std::string_view body, Kind kind);
}

#endif
