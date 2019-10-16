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

#ifndef ARCHIVE_HPP
#define ARCHIVE_HPP

#include "utils.hpp"
#include <3ds.h>

namespace Archive
{
    Result init(const std::string& execPath);
    void exit(void);
    FS_Archive sd(void);
    FS_Archive data(void);
    Result save(FS_Archive* archive, FS_MediaType mediatype, u32 lowid, u32 highid);
    Result extdata(FS_Archive* archive, u32 extdata);
    bool saveAccessible(FS_MediaType mediatype, u32 lowid, u32 highid);
    bool extdataAccessible(u32 id);
    Result createPKSMExtdataArchive(const std::string& execPath);
    Result moveDir(FS_Archive src, const std::u16string& dir, FS_Archive dst, const std::u16string& dest);
    Result moveFile(FS_Archive src, const std::u16string& file, FS_Archive dst, const std::u16string& dest);
    Result copyFile(FS_Archive src, const std::u16string& file, FS_Archive dst, const std::u16string& dest);
    Result deleteFile(FS_Archive archive, const std::u16string& file);
    inline Result moveDir(FS_Archive src, const std::string& dir, FS_Archive dst, const std::string& dest)
    {
        return moveDir(src, StringUtils::UTF8toUTF16(dir), dst, StringUtils::UTF8toUTF16(dest));
    }
    inline Result moveFile(FS_Archive src, const std::string& file, FS_Archive dst, const std::string& dest)
    {
        return moveFile(src, StringUtils::UTF8toUTF16(file), dst, StringUtils::UTF8toUTF16(dest));
    }
    inline Result copyFile(FS_Archive src, const std::string& file, FS_Archive dst, const std::string& dest)
    {
        return copyFile(src, StringUtils::UTF8toUTF16(file), dst, StringUtils::UTF8toUTF16(dest));
    }
    inline Result deleteFile(FS_Archive archive, const std::string& file) { return deleteFile(archive, StringUtils::UTF8toUTF16(file)); }
}

#endif
