/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2020 Bernardo Giordano, Admiral Fish, piepie62
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

#include "File.hpp"
#include "internal_fspxi.hpp"

File::File(Handle handle) : mHandle(handle), mOffset(0)
{
    mResult = FSFILE_GetSize(handle, &mSize);
}

File::File(FSPXI_File handle) : mHandle(handle), mOffset(0)
{
    mResult = FSPXI_GetFileSize(fspxiHandle, handle, &mSize);
}

Result File::close(void)
{
    switch (mHandle.index())
    {
        case 0:
            return mResult = FSFILE_Close(std::get<0>(mHandle));
        case 1:
            return mResult = FSPXI_CloseFile(fspxiHandle, std::get<1>(mHandle));
    }
    return -1; // Cannot happen
}

Result File::result(void)
{
    return mResult;
}

u64 File::size(void)
{
    return mSize;
}

u32 File::read(void* buf, u32 sz)
{
    u32 rd = 0;
    switch (mHandle.index())
    {
        case 0:
            mResult = FSFILE_Read(std::get<0>(mHandle), &rd, mOffset, buf, sz);
            break;
        case 1:
            mResult = FSPXI_ReadFile(fspxiHandle, std::get<1>(mHandle), &rd, mOffset, buf, sz);
            break;
    }

    if (R_FAILED(mResult))
    {
        if (rd > sz)
        {
            rd = sz;
        }
    }
    mOffset += rd;
    return rd;
}

u32 File::write(const void* buf, u32 sz)
{
    u32 wt = 0;
    switch (mHandle.index())
    {
        case 0:
            mResult = FSFILE_Write(std::get<0>(mHandle), &wt, mOffset, buf, sz, FS_WRITE_FLUSH);
            break;
        case 1:
            mResult = FSPXI_WriteFile(
                fspxiHandle, std::get<1>(mHandle), &wt, mOffset, buf, sz, FS_WRITE_FLUSH);
            break;
    }
    mOffset += wt;
    return wt;
}

bool File::eof(void)
{
    return mOffset >= mSize;
}

u64 File::offset(void)
{
    return mOffset;
}

void File::seek(s64 offset, int from)
{
    switch (from)
    {
        case SEEK_SET:
            mOffset = offset;
            break;
        case SEEK_CUR:
            mOffset += offset;
            break;
        case SEEK_END:
            mOffset = mSize - offset;
            break;
        default:
            break;
    }
}

std::variant<Handle, FSPXI_File> File::getRawHandle(void)
{
    return mHandle;
}

Result File::resize(u64 size)
{
    switch (mHandle.index())
    {
        case 0:
            return mResult = FSFILE_SetSize(std::get<0>(mHandle), size);
        case 1:
            return mResult = FSPXI_SetFileSize(fspxiHandle, std::get<1>(mHandle), size);
    }
    return -1; // Cannot happen
}
