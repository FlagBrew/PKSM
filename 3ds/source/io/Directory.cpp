/*
*   This file is part of PKSM
*   Copyright (C) 2016-2019 Bernardo Giordano, Admiral Fish, piepie62
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

Directory::Directory(FS_Archive archive, const std::u16string& root)
{
	load = false;
	err = 0;
	Handle handle;

	list.clear();

	err = FSUSER_OpenDirectory(&handle, archive, fsMakePath(PATH_UTF16, root.c_str()));
	if (R_FAILED(err))
	{
		return;
	}

	u32 result = 0;
	do {
		FS_DirectoryEntry item;
		err = FSDIR_Read(handle, &result, 1, &item);
		if (result == 1)
		{
			list.push_back(item);
		}
	} while(result);

	err = FSDIR_Close(handle);
	if (R_FAILED(err))
	{
		list.clear();
		return;
	}

	load = true;
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
