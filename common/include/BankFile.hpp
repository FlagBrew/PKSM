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

#ifndef BANKFILE_HPP
#define BANKFILE_HPP

#include "enums/Generation.hpp"
#include "utils/coretypes.h"
#include <cstddef>
#include <span>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

// The bank file format, and nothing else: bytes in, either a validated set of boxes or a typed
// error out. No I/O, no libctru, no GUI, so every format rule can be checked here and exercised
// off-device with fixtures.
namespace BankFile
{
    inline constexpr int VERSION            = 3;
    inline constexpr std::string_view MAGIC = "PKSMBANK";
    inline constexpr int SLOTS_PER_BOX      = 30;
    // The same ceiling the UI enforces when a bank is created or resized.
    inline constexpr int MAX_BOXES = 500;

    struct Header
    {
        char MAGIC[8];
        u32 version;
        u32 boxes;
    };

    static_assert(sizeof(Header) == 16);

    struct Entry
    {
        pksm::Generation gen;
        u8 data[0x148];
        u8 padding[4]; // Pad to 8 bytes
    };

    static_assert(sizeof(Entry) == 0x150);
    static_assert(std::is_trivially_copyable_v<Entry>);

    // Versions 1 and 2 stored a shorter entry; both are widened to Entry on parse.
    inline constexpr size_t OLD_ENTRY_SIZE = 264;
    // Version 1 had no box count in its header.
    inline constexpr size_t V1_HEADER_SIZE = sizeof(Header) - sizeof(u32);
    // Nothing larger can be a bank this build accepts, so nothing larger needs to be read.
    inline constexpr size_t MAX_SIZE =
        sizeof(Header) + size_t(MAX_BOXES) * SLOTS_PER_BOX * sizeof(Entry);

    enum class Error
    {
        TooSmall,       // fewer bytes than the version's header needs
        BadMagic,       // not a bank file at all
        NewerVersion,   // written by a newer PKSM: refuse rather than mangle it
        UnknownVersion, // a version this build has no migration for
        BadBoxCount,    // box count of zero, or beyond MAX_BOXES
    };

    struct Contents
    {
        // The version as found in the file. The contents are always migrated to VERSION.
        u32 sourceVersion = VERSION;
        int boxes         = 0;
        // Exactly boxes * SLOTS_PER_BOX entries. Slots the file did not supply are empty.
        std::vector<Entry> entries;
        // The header claimed more boxes than the file held; the tail was filled with empty slots.
        bool truncated = false;

        bool migrated() const { return sourceVersion != u32(VERSION); }
    };

    // An entry in the "no Pokémon here" state the rest of PKSM expects.
    Entry emptyEntry();

    std::variant<Contents, Error> parse(std::span<const u8> bytes);
}

#endif
