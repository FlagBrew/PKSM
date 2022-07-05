/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2021 Bernardo Giordano, Admiral Fish, piepie62
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

#include "Directory.hpp"
#include "internal_fspxi.hpp"
#include <array>

#define DIRECTORY_READ_SIZE 10

Directory::Directory(Handle handle) : err(0), load(false)
{
    u32 result = 0;
    std::array<FS_DirectoryEntry, DIRECTORY_READ_SIZE> tmp;
    do
    {
        err = FSDIR_Read(handle, &result, DIRECTORY_READ_SIZE, tmp.data());
        if (result > 0)
        {
            list.insert(list.end(), tmp.begin(), tmp.begin() + result);
        }
    }
    while (result);

    if (R_FAILED(err))
    {
        list.clear();
    }
    else
    {
        load = true;
    }

    FSDIR_Close(handle);
}

Directory::Directory(FSPXI_Directory handle)
{
    load = false;
    err  = 0;

    u32 result = 0;
    std::array<FS_DirectoryEntry, DIRECTORY_READ_SIZE> tmp;
    do
    {
        err = FSPXI_ReadDirectory(fspxiHandle, handle, &result, DIRECTORY_READ_SIZE, tmp.data());
        if (result > 0)
        {
            list.insert(list.end(), tmp.begin(), tmp.begin() + result);
        }
    }
    while (result);

    if (R_FAILED(err))
    {
        list.clear();
    }
    else
    {
        load = true;
    }

    FSPXI_CloseDirectory(fspxiHandle, handle);
}

Result Directory::error(void) const
{
    return err;
}

bool Directory::loaded(void) const
{
    return load;
}

std::u16string Directory::item(size_t index) const
{
    return (char16_t*)list.at(index).name;
}

bool Directory::folder(size_t index) const
{
    return index < list.size() ? list.at(index).attributes == FS_ATTRIBUTE_DIRECTORY : false;
}

size_t Directory::count(void) const
{
    return list.size();
}
