/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2026 Bernardo Giordano, Admiral Fish, piepie62
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

#include "SaveIndex.hpp"
#include <cstdio>
#include <unordered_map>
#include <unordered_set>

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

    struct MemoryFilesystem
    {
        std::unordered_map<std::string, SaveIndex::Directory> directories;
        std::unordered_set<std::string> files;
        std::unordered_map<std::string, size_t> reads;

        std::optional<SaveIndex::Directory> read(const std::string& path)
        {
            reads[path]++;
            const auto found = directories.find(path);
            if (found == directories.end())
            {
                return std::nullopt;
            }
            return found->second;
        }

        bool exists(const std::string& path) const { return files.contains(path); }
    };

    void directoriesAreIndexedOnce()
    {
        constexpr std::string_view roots[] = {"/checkpoint", "/backups"};
        const SaveIndex::Target targets[]  = {
            {"0x0055D",  "main"         },
            // A valid longer ID also starts with the first ID. The old prefix scan found
            // both, and the index must preserve that behavior without reading twice.
            {"0x0055DA", "special"      },
            {"ADAE",     "POKEMON D.sav"},
            // Duplicate configured IDs still describe one index key.
            {"0x0055D",  "main"         }
        };

        MemoryFilesystem fs;
        fs.directories["/checkpoint"] = {
            {"README.txt",                  false},
            {"other game",                  true },
            {"0x0055D Pokemon X",           true },
            {"0x0055DA overlapping prefix", true },
            {"ADAE Pokemon Diamond",        true }
        };
        fs.directories["/checkpoint/0x0055D Pokemon X"] = {
            {"slot-a",            true },
            {"notes.txt",         false},
            {"slot-without-save", true }
        };
        fs.directories["/checkpoint/0x0055DA overlapping prefix"] = {
            {"slot-b", true}
        };
        fs.directories["/checkpoint/ADAE Pokemon Diamond"] = {
            {"slot-ds", true}
        };
        fs.directories["/backups"] = {
            {"0x0055D", true}
        };
        fs.directories["/backups/0x0055D"] = {
            {"2026-08-23", true}
        };

        fs.files = {"/checkpoint/0x0055D Pokemon X/slot-a/main",
            "/checkpoint/0x0055DA overlapping prefix/slot-b/main",
            "/checkpoint/0x0055DA overlapping prefix/slot-b/special",
            "/checkpoint/ADAE Pokemon Diamond/slot-ds/POKEMON D.sav",
            "/backups/0x0055D/2026-08-23/main"};

        const SaveIndex index = SaveIndex::build(
            roots, targets, [&fs](const std::string& path) { return fs.read(path); },
            [&fs](const std::string& path) { return fs.exists(path); }, [] { return true; });

        const std::vector<std::string> expectedX = {"/checkpoint/0x0055D Pokemon X/slot-a/main",
            "/checkpoint/0x0055DA overlapping prefix/slot-b/main",
            "/backups/0x0055D/2026-08-23/main"};
        check(index.saves("0x0055D") == expectedX,
            "a title lookup preserves root, title, and snapshot order");
        check(
            index.saves("0x0055DA") ==
                std::vector<std::string>{"/checkpoint/0x0055DA overlapping prefix/slot-b/special"},
            "overlapping configured prefixes are both indexed");
        check(
            index.saves("ADAE") ==
                std::vector<std::string>{"/checkpoint/ADAE Pokemon Diamond/slot-ds/POKEMON D.sav"},
            "DS prefixes use their configured save filename");
        check(index.saves("missing").empty(), "an unknown title has no saves");

        check(fs.reads["/checkpoint"] == 1 && fs.reads["/backups"] == 1,
            "each save root is read exactly once");
        check(fs.reads["/checkpoint/0x0055DA overlapping prefix"] == 1,
            "one folder matching multiple IDs is still read exactly once");
        check(fs.reads["/checkpoint/other game"] == 0, "unrelated title folders are not traversed");
    }

    void cancellationStopsBeforeIo()
    {
        constexpr std::string_view roots[] = {"/checkpoint"};
        const SaveIndex::Target targets[]  = {
            {"ADAE", "POKEMON D.sav"}
        };
        size_t reads = 0;

        const SaveIndex index = SaveIndex::build(
            roots, targets,
            [&reads](const std::string&) -> std::optional<SaveIndex::Directory>
            {
                reads++;
                return SaveIndex::Directory{};
            },
            [](const std::string&) { return true; }, [] { return false; });

        check(reads == 0, "a cancelled build performs no directory I/O");
        check(index.saves("ADAE").empty(), "a cancelled build returns an empty partial index");
    }
}

int main()
{
    directoriesAreIndexedOnce();
    cancellationStopsBeforeIo();

    if (failures == 0)
    {
        std::printf("SaveIndex: all checks passed\n");
        return 0;
    }
    std::printf("SaveIndex: %d check(s) failed\n", failures);
    return 1;
}
