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

#include "DirectoryBrowser.hpp"
#include "STDirectory.hpp"
#include <algorithm>

namespace
{
    // "/dir/" and "romfs:/scripts/" name the same places as "/dir" and "romfs:/scripts".
    // One spelling per place keeps atRoot() a comparison instead of a parse.
    std::string canonical(std::string path)
    {
        while (path.size() > 1 && path.back() == '/' && path[path.size() - 2] != ':')
        {
            path.pop_back();
        }
        // A bare device keeps the slash its children are joined onto, so climbing back
        // out of one lands on the root again.
        if (!path.empty() && path.back() == ':')
        {
            path += '/';
        }
        return path;
    }

    std::string joined(const std::string& base, const std::string& name)
    {
        if (base.empty() || base.back() == '/')
        {
            return base + name;
        }
        return base + '/' + name;
    }

    std::string parentOf(const std::string& path)
    {
        const size_t slash = path.find_last_of('/');
        if (slash == std::string::npos)
        {
            return path;
        }
        // "/dir" lives in "/", and "romfs:/dir" in "romfs:/": both keep the slash the
        // shorter paths are made of.
        if (slash == 0 || path[slash - 1] == ':')
        {
            return path.substr(0, slash + 1);
        }
        return path.substr(0, slash);
    }
}

DirectoryBrowser::DirectoryBrowser(const std::string& root)
    : rootPath(canonical(root)), currentPath(rootPath), readable(false)
{
    moveTo(currentPath);
}

bool DirectoryBrowser::reroot(const std::string& root)
{
    const std::string wanted = canonical(root);
    if (!moveTo(wanted))
    {
        return false;
    }
    rootPath = wanted;
    return true;
}

bool DirectoryBrowser::enter(size_t index)
{
    if (index >= currentEntries.size() || !currentEntries[index].directory)
    {
        return false;
    }
    return moveTo(joined(currentPath, currentEntries[index].name));
}

bool DirectoryBrowser::leave()
{
    if (atRoot())
    {
        return false;
    }
    return moveTo(parentOf(currentPath));
}

std::string DirectoryBrowser::pathOf(size_t index) const
{
    return index < currentEntries.size() ? joined(currentPath, currentEntries[index].name) : "";
}

bool DirectoryBrowser::moveTo(const std::string& path)
{
    STDirectory listing(path);
    if (!listing.good())
    {
        return false;
    }

    std::vector<Entry> read;
    read.reserve(listing.count());
    for (size_t i = 0; i < listing.count(); i++)
    {
        read.push_back({listing.item(i), listing.folder(i)});
    }
    std::sort(read.begin(), read.end(),
        [](const Entry& first, const Entry& second)
        {
            if (first.directory != second.directory)
            {
                return first.directory;
            }
            return first.name < second.name;
        });

    currentPath    = path;
    currentEntries = std::move(read);
    readable       = true;
    return true;
}
