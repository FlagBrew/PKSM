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

#ifndef SAVEINDEX_HPP
#define SAVEINDEX_HPP

#include <functional>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// The Checkpoint and PKSM backup directories, indexed in one pass.
//
// Directory names begin with the title prefix whose saves they contain. The old
// scan walked every root listing once for every known prefix. SaveIndex instead
// matches each root entry against the few prefix lengths in use, walks a matching
// entry once, and remembers the paths for cheap per-title lookups.
class SaveIndex
{
public:
    struct Target
    {
        std::string id;
        std::string saveFileName;
    };

    struct DirectoryEntry
    {
        std::string name;
        bool directory;
    };

    using Directory     = std::vector<DirectoryEntry>;
    using ReadDirectory = std::function<std::optional<Directory>(const std::string& path)>;
    using FileExists    = std::function<bool(const std::string& path)>;
    using KeepGoing     = std::function<bool()>;

    // Reads each root once. Subdirectories are only read when their name starts
    // with a target ID, and are read once even if IDs overlap.
    [[nodiscard]] static SaveIndex build(std::span<const std::string_view> roots,
        std::span<const Target> targets, const ReadDirectory& readDirectory,
        const FileExists& fileExists, const KeepGoing& keepGoing);

    // The reference remains valid for the lifetime of the index.
    [[nodiscard]] const std::vector<std::string>& saves(std::string_view id) const;

private:
    struct StringHash
    {
        using is_transparent = void;

        [[nodiscard]] size_t operator()(std::string_view value) const noexcept;
    };

    using Saves =
        std::unordered_map<std::string, std::vector<std::string>, StringHash, std::equal_to<>>;

    Saves indexedSaves;
};

#endif
