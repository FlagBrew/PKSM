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

// Fixtures in, contents or errors out. Runs on the host: no 3DS, no libctru, no GUI.

#include "BankFile.hpp"
#include <cstdio>
#include <cstring>
#include <vector>

namespace
{
    int failures = 0;

    void check(bool condition, const char* what)
    {
        if (!condition)
        {
            failures++;
            std::printf("FAIL: %s\n", what);
        }
    }

    void append(std::vector<u8>& bytes, u32 value)
    {
        for (size_t i = 0; i < sizeof(value); i++)
        {
            bytes.push_back(u8(value >> (8 * i)));
        }
    }

    // A bank file header. Version 1 files stop after the version field.
    std::vector<u8> header(u32 version, u32 boxes)
    {
        std::vector<u8> bytes(BankFile::MAGIC.begin(), BankFile::MAGIC.end());
        append(bytes, version);
        if (version != 1)
        {
            append(bytes, boxes);
        }
        return bytes;
    }

    void body(std::vector<u8>& bytes, size_t entries, size_t entrySize, u8 fill)
    {
        bytes.insert(bytes.end(), entries * entrySize, fill);
    }

    const BankFile::Contents* contentsOf(const std::variant<BankFile::Contents, BankFile::Error>& r)
    {
        return std::get_if<BankFile::Contents>(&r);
    }

    bool isError(
        const std::variant<BankFile::Contents, BankFile::Error>& r, BankFile::Error expected)
    {
        const auto* error = std::get_if<BankFile::Error>(&r);
        return error && *error == expected;
    }

    void currentVersionRoundTrips()
    {
        auto bytes = header(BankFile::VERSION, 2);
        body(bytes, 60, sizeof(BankFile::Entry), 0x11);

        const auto result = BankFile::parse(bytes);
        const auto* out   = contentsOf(result);
        check(out != nullptr, "a well-formed v3 file parses");
        if (!out)
        {
            return;
        }
        check(out->boxes == 2, "box count comes from the header");
        check(out->entries.size() == 60, "one entry per slot");
        check(!out->truncated, "a complete file is not truncated");
        check(!out->migrated(), "a v3 file needs no migration");
        check(out->entries.back().data[0] == 0x11, "the last slot holds the file's bytes");
    }

    // The bug this module exists to make impossible: the header's box count sized the allocation
    // while the file's own length sized the read.
    void hugeBoxCountIsRejected()
    {
        auto bytes = header(BankFile::VERSION, 0xFFFFFFFF);
        body(bytes, 60, sizeof(BankFile::Entry), 0x11);
        check(isError(BankFile::parse(bytes), BankFile::Error::BadBoxCount),
            "a box count past MAX_BOXES is refused");

        auto zero = header(BankFile::VERSION, 0);
        check(isError(BankFile::parse(zero), BankFile::Error::BadBoxCount),
            "a zero box count is refused");
    }

    // A file that ends early keeps the boxes it does hold; the rest come back empty rather than
    // as uninitialized heap.
    void shortFileIsFilledNotRead()
    {
        auto bytes = header(BankFile::VERSION, 2);
        body(bytes, 1, sizeof(BankFile::Entry), 0x22);

        const auto result = BankFile::parse(bytes);
        const auto* out   = contentsOf(result);
        check(out != nullptr, "a short v3 file still parses");
        if (!out)
        {
            return;
        }
        check(out->truncated, "the short file is reported as truncated");
        check(out->entries.size() == 60, "every declared slot is present");
        check(out->entries[0].data[0] == 0x22, "the slot the file held survives");

        const BankFile::Entry empty = BankFile::emptyEntry();
        check(std::memcmp(&out->entries[1], &empty, sizeof(empty)) == 0,
            "slots past the end of the file are empty");
    }

    void version1TakesItsSizeFromTheFile()
    {
        auto bytes = header(1, 0);
        body(bytes, 60, BankFile::OLD_ENTRY_SIZE, 0x33);

        const auto result = BankFile::parse(bytes);
        const auto* out   = contentsOf(result);
        check(out != nullptr, "a v1 file parses");
        if (!out)
        {
            return;
        }
        check(out->boxes == 2, "v1's box count comes from the file's length");
        check(out->migrated(), "a v1 file reports that it was migrated");
        check(out->entries[0].data[0] == 0x33, "the old entry is copied in");
        check(out->entries[0].data[sizeof(BankFile::Entry::data) - 1] == 0xFF,
            "the widened tail of an old entry is empty");
    }

    void version2MigratesWithItsHeaderCount()
    {
        auto bytes = header(2, 1);
        body(bytes, 30, BankFile::OLD_ENTRY_SIZE, 0x44);

        const auto result = BankFile::parse(bytes);
        const auto* out   = contentsOf(result);
        check(out != nullptr, "a v2 file parses");
        if (!out)
        {
            return;
        }
        check(out->boxes == 1 && out->entries.size() == 30, "v2's box count comes from the header");
        check(out->migrated() && !out->truncated, "a complete v2 file only needs migrating");
    }

    void badFilesAreTyped()
    {
        check(isError(BankFile::parse({}), BankFile::Error::TooSmall), "no bytes is TooSmall");

        std::vector<u8> notABank(64, 0);
        check(isError(BankFile::parse(notABank), BankFile::Error::BadMagic),
            "a file without the magic is BadMagic");

        auto magicOnly = header(BankFile::VERSION, 1);
        magicOnly.resize(BankFile::V1_HEADER_SIZE);
        check(isError(BankFile::parse(magicOnly), BankFile::Error::TooSmall),
            "a v3 file with no box count is TooSmall");

        auto newer = header(BankFile::VERSION + 1, 1);
        check(isError(BankFile::parse(newer), BankFile::Error::NewerVersion),
            "a newer version is refused, not migrated");

        auto unknown = header(0, 1);
        check(isError(BankFile::parse(unknown), BankFile::Error::UnknownVersion),
            "version zero has no migration");
    }
}

int main()
{
    currentVersionRoundTrips();
    hugeBoxCountIsRejected();
    shortFileIsFilledNotRead();
    version1TakesItsSizeFromTheFile();
    version2MigratesWithItsHeaderCount();
    badFilesAreTyped();

    if (failures == 0)
    {
        std::printf("BankFile: all checks passed\n");
        return 0;
    }
    std::printf("BankFile: %d check(s) failed\n", failures);
    return 1;
}
