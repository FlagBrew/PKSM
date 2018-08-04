/*
*   This file is part of PKSM
*   Copyright (C) 2016-2018 Bernardo Giordano, Admiral Fish, piepie62
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

#include "FSStream.hpp"

FSStream::FSStream(FS_Archive archive, std::u16string path, u32 flags)
{
	load = false;
	sz = 0;
	off = 0;

	res = FSUSER_OpenFile(&handle, archive, fsMakePath(PATH_UTF16, path.data()), flags, 0);
	if (R_SUCCEEDED(res))
	{
		FSFILE_GetSize(handle, (u64*)&sz);
		load = true;
	}
}

FSStream::FSStream(FS_Archive archive, std::u16string path, u32 flags, u32 _size)
{
	load = false;
	sz = _size;
	off = 0;

	res = FSUSER_OpenFile(&handle, archive, fsMakePath(PATH_UTF16, path.data()), flags, 0);
	if (R_FAILED(res))
	{
		res = FSUSER_CreateFile(archive, fsMakePath(PATH_UTF16, path.data()), 0, sz);
		if (R_SUCCEEDED(res))
		{
			res = FSUSER_OpenFile(&handle, archive, fsMakePath(PATH_UTF16, path.data()), flags, 0);
			if (R_SUCCEEDED(res))
			{
				load = true;
			}
		}
	}
	else
	{
		load = true;
	}
}

Result FSStream::close(void)
{
	res = FSFILE_Close(handle);
	return res;
}

bool FSStream::loaded(void)
{
	return load;
}

Result FSStream::result(void)
{
	return res;
}

u32 FSStream::size(void)
{
	return sz;
}

u32 FSStream::read(void *buf, u32 sz)
{
	u32 rd = 0;
	res = FSFILE_Read(handle, &rd, off, buf, sz);
	off += rd;
	return rd;
}

u32 FSStream::write(const void *buf, u32 sz)
{
	u32 wt = 0;
	res = FSFILE_Write(handle, &wt, off, buf, sz, FS_WRITE_FLUSH);
	off += wt;
	return wt;
}

bool FSStream::eof(void)
{
	return off >= sz;
}

u32 FSStream::offset(void)
{
	return off;
}