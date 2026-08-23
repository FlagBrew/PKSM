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
#include <algorithm>

namespace
{
    std::string childPath(std::string_view parent, std::string_view child)
    {
        std::string path;
        path.reserve(parent.size() + child.size() + 1);
        path.append(parent);
        path.push_back('/');
        path.append(child);
        return path;
    }
}

size_t SaveIndex::StringHash::operator()(std::string_view value) const noexcept
{
    // Taking every key as a view gives owned strings and lookup views the same
    // hash, enabling allocation-free heterogeneous lookups in the maps below.
    return std::hash<std::string_view>{}(value);
}

SaveIndex SaveIndex::build(std::span<const std::string_view> roots, std::span<const Target> targets,
    const ReadDirectory& readDirectory, const FileExists& fileExists, const KeepGoing& keepGoing)
{
    std::unordered_map<std::string, std::string, StringHash, std::equal_to<>> targetFiles;
    std::vector<size_t> prefixLengths;
    targetFiles.reserve(targets.size());
    prefixLengths.reserve(targets.size());
    for (const auto& target : targets)
    {
        if (targetFiles.try_emplace(target.id, target.saveFileName).second)
        {
            prefixLengths.emplace_back(target.id.size());
        }
    }
    std::ranges::sort(prefixLengths);
    const auto uniqueEnd = std::ranges::unique(prefixLengths).begin();
    prefixLengths.erase(uniqueEnd, prefixLengths.end());

    SaveIndex result;
    result.indexedSaves.reserve(targetFiles.size());
    std::vector<decltype(targetFiles)::const_iterator> matches;
    matches.reserve(prefixLengths.size());

    for (const auto root : roots)
    {
        if (!keepGoing())
        {
            break;
        }
        const std::string rootPath{root};
        const auto entries = readDirectory(rootPath);
        if (!entries)
        {
            continue;
        }

        for (const auto& entry : *entries)
        {
            if (!keepGoing())
            {
                return result;
            }
            if (!entry.directory)
            {
                continue;
            }

            matches.clear();
            for (const size_t length : prefixLengths)
            {
                if (length > entry.name.size())
                {
                    break;
                }
                const auto match = targetFiles.find(std::string_view{entry.name}.substr(0, length));
                if (match != targetFiles.end())
                {
                    matches.emplace_back(match);
                }
            }
            if (matches.empty())
            {
                continue;
            }

            const std::string titleDirectory = childPath(rootPath, entry.name);
            const auto snapshots             = readDirectory(titleDirectory);
            if (!snapshots)
            {
                continue;
            }
            for (const auto& snapshot : *snapshots)
            {
                if (!keepGoing())
                {
                    return result;
                }
                if (!snapshot.directory)
                {
                    continue;
                }

                const std::string snapshotDirectory = childPath(titleDirectory, snapshot.name);
                for (const auto match : matches)
                {
                    const std::string path = childPath(snapshotDirectory, match->second);
                    if (fileExists(path))
                    {
                        result.indexedSaves[match->first].emplace_back(path);
                    }
                }
            }
        }
    }
    return result;
}

const std::vector<std::string>& SaveIndex::saves(std::string_view id) const
{
    static const std::vector<std::string> empty;
    const auto found = indexedSaves.find(id);
    return found == indexedSaves.end() ? empty : found->second;
}
