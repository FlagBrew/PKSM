/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2022 Bernardo Giordano, Admiral Fish, piepie62
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

#include "STDirectory.hpp"
#include <string.h>

STDirectory::STDirectory(const std::string& root) : mError(0), mGood(false)
{
    DIR* dir = opendir(root.c_str());

    if (dir == NULL)
    {
        mError = (Result)errno;
        closedir(dir);
        return;
    }
    else
    {
        struct dirent* ent;
        while ((ent = readdir(dir)))
        {
            // Don't insert the implicit . and .. folders because ew
            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, ".."))
            {
                mList.emplace_back(ent->d_name, ent->d_type == DT_DIR);
            }
        }
    }

    closedir(dir);
    mGood = true;
}

Result STDirectory::error(void)
{
    return mError;
}

bool STDirectory::good(void)
{
    return mGood;
}

std::string STDirectory::item(size_t index)
{
    return index < mList.size() ? mList.at(index).name : "";
}

bool STDirectory::folder(size_t index)
{
    return index < mList.size() ? mList.at(index).directory : false;
}

size_t STDirectory::count(void)
{
    return mList.size();
}
