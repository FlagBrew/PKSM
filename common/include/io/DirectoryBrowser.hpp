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

#ifndef DIRECTORYBROWSER_HPP
#define DIRECTORYBROWSER_HPP

#include <string>
#include <vector>

// One directory being browsed, rooted so a caller cannot walk above where it started.
//
// STDirectory answers index-level questions: how many entries, the name of one, whether
// it is a folder. Every browser built on that answered the same further questions for
// itself: what order entries belong in, how to join a child path, how to strip one back
// off, and when the walk has reached the point it may not pass. DirectoryBrowser owns
// those; callers ask only what is on screen and where a selection points.
//
// Navigation is atomic. enter() and leave() read the destination before committing to
// it, so a browser never lands somewhere it cannot list.
class DirectoryBrowser
{
public:
    struct Entry
    {
        std::string name;
        bool directory;
    };

    // Lists root. good() is false when it cannot be read.
    explicit DirectoryBrowser(const std::string& root);

    // Moves both the root and the listing. The current location is kept when the new
    // root cannot be read.
    bool reroot(const std::string& root);

    // Folders first, then files, each alphabetical. Never contains . or ..
    [[nodiscard]] const std::vector<Entry>& entries() const { return currentEntries; }

    [[nodiscard]] const std::string& path() const { return currentPath; }

    [[nodiscard]] bool good() const { return readable; }

    [[nodiscard]] bool atRoot() const { return currentPath == rootPath; }

    // Descends into entries()[index]. False when it is not a readable folder.
    bool enter(size_t index);

    // Climbs one level. False at the root, where a caller decides what leaving means.
    bool leave();

    // Where entries()[index] lives, or "" when the index is out of range.
    [[nodiscard]] std::string pathOf(size_t index) const;

private:
    bool moveTo(const std::string& path);

    std::string rootPath;
    std::string currentPath;
    std::vector<Entry> currentEntries;
    bool readable;
};

#endif
