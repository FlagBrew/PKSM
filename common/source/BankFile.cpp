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

#include "BankFile.hpp"
#include <algorithm>
#include <cstring>

namespace
{
    u32 readU32(std::span<const u8> bytes, size_t offset)
    {
        u32 value;
        std::memcpy(&value, bytes.data() + offset, sizeof(value));
        return value;
    }
}

BankFile::Entry BankFile::emptyEntry()
{
    Entry entry;
    std::fill_n(reinterpret_cast<u8*>(&entry), sizeof(entry), u8(0xFF));
    return entry;
}

std::variant<BankFile::Contents, BankFile::Error> BankFile::parse(std::span<const u8> bytes)
{
    if (bytes.size() < V1_HEADER_SIZE)
    {
        return Error::TooSmall;
    }
    if (std::memcmp(bytes.data(), MAGIC.data(), MAGIC.size()) != 0)
    {
        return Error::BadMagic;
    }

    const u32 version = readU32(bytes, MAGIC.size());

    // NOTE: THIS IS THE CONVERSION SECTION. WILL NEED TO BE MODIFIED WHEN THE FORMAT IS CHANGED
    size_t headerSize;
    size_t entrySize;
    switch (version)
    {
        case 1:
            headerSize = V1_HEADER_SIZE;
            entrySize  = OLD_ENTRY_SIZE;
            break;
        case 2:
            headerSize = sizeof(Header);
            entrySize  = OLD_ENTRY_SIZE;
            break;
        case VERSION:
            headerSize = sizeof(Header);
            entrySize  = sizeof(Entry);
            break;
        default:
            return version > u32(VERSION) ? Error::NewerVersion : Error::UnknownVersion;
    }
    if (bytes.size() < headerSize)
    {
        return Error::TooSmall;
    }

    const size_t body = bytes.size() - headerSize;
    // Wider than the header's u32 so that no box count can wrap the slot arithmetic below.
    u64 boxes;
    if (version == 1)
    {
        // Version 1 stored no box count: the file's own length is the only source.
        boxes = body / entrySize / SLOTS_PER_BOX;
    }
    else
    {
        boxes = readU32(bytes, MAGIC.size() + sizeof(u32));
    }
    if (boxes == 0 || boxes > u64(MAX_BOXES))
    {
        return Error::BadBoxCount;
    }

    Contents out;
    out.sourceVersion  = version;
    out.boxes          = int(boxes);
    const size_t slots = size_t(out.boxes) * SLOTS_PER_BOX;
    // The header's box count is a claim about the file; the file's length is the fact. Never read
    // past what is actually there, and never hand back a slot the file did not fill.
    const size_t readable = std::min(slots, body / entrySize);
    out.truncated         = readable < slots;

    out.entries.assign(slots, emptyEntry());
    for (size_t i = 0; i < readable; i++)
    {
        // entrySize <= sizeof(Entry) for every version above, so the tail of a migrated entry keeps
        // the empty fill.
        std::memcpy(&out.entries[i], bytes.data() + headerSize + i * entrySize, entrySize);
    }
    return out;
}
