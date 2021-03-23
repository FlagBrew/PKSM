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

#ifndef ARCHIVE_HPP
#define ARCHIVE_HPP

#include "Directory.hpp"
#include "File.hpp"
#include "utils.hpp"
#include <3ds.h>

class Archive
{
private:
    static Result createPKSMExtdataArchive(const std::string& execPath);

public:
    Archive(FS_ArchiveID id, FS_Path path, bool pxi);
    Archive() : mHandle(0), mResult(-1), mPXI(false) {}
    ~Archive() { close(); }
    Archive(const Archive&) = delete;
    Archive& operator=(const Archive&) = delete;
    Archive(Archive&&);
    Archive& operator=(Archive&&);

    static Result init(const std::string& execPath);
    static void exit(void);

    static Archive& sd(void);
    static Archive& data(void);
    static Archive save(FS_MediaType mediatype, u32 lowid, u32 highid, bool pxi);
    static Archive saveAndContents(
        FS_MediaType mediatype, u32 lowid, u32 highid, bool pxi, u32 pathWord4 = 0);
    static Archive extdata(u32 extdata, bool pxi);

    // As these do manual directory traversal, an FS_Path overload is not possible
    static Result moveDir(
        Archive& src, const std::u16string& dir, Archive& dst, const std::u16string& dest);
    static Result copyDir(
        Archive& src, const std::u16string& dir, Archive& dst, const std::u16string& dest);
    Result deleteDir(const std::u16string& path);

    static Result moveFile(Archive& src, FS_Path file, Archive& dst, FS_Path dest);
    static Result copyFile(Archive& src, FS_Path file, Archive& dst, FS_Path dest);
    Result createDir(FS_Path dir, u32 attributes);
    std::unique_ptr<Directory> directory(FS_Path path);
    Result createFile(FS_Path file, u32 attributes, u64 size);
    std::unique_ptr<File> file(FS_Path file, u32 flags, u32 attributes = 0);
    Result deleteFile(FS_Path file);

    static Result moveDir(
        Archive& src, const std::string& dir, Archive& dst, const std::string& dest)
    {
        return moveDir(src, StringUtils::UTF8toUTF16(dir), dst, StringUtils::UTF8toUTF16(dest));
    }

    static Result moveFile(
        Archive& src, const std::u16string& file, Archive& dst, const std::u16string& dest)
    {
        return moveFile(
            src, fsMakePath(PATH_UTF16, file.c_str()), dst, fsMakePath(PATH_UTF16, dest.c_str()));
    }
    static Result moveFile(
        Archive& src, const std::string& file, Archive& dst, const std::string& dest)
    {
        return moveFile(src, StringUtils::UTF8toUTF16(file), dst, StringUtils::UTF8toUTF16(dest));
    }

    static Result copyDir(
        Archive& src, const std::string& dir, Archive& dst, const std::string& dest)
    {
        return copyDir(src, StringUtils::UTF8toUTF16(dir), dst, StringUtils::UTF8toUTF16(dest));
    }

    static Result copyFile(
        Archive& src, const std::u16string& file, Archive& dst, const std::u16string& dest)
    {
        return copyFile(
            src, fsMakePath(PATH_UTF16, file.c_str()), dst, fsMakePath(PATH_UTF16, dest.c_str()));
    }
    static Result copyFile(
        Archive& src, const std::string& file, Archive& dst, const std::string& dest)
    {
        return copyFile(src, StringUtils::UTF8toUTF16(file), dst, StringUtils::UTF8toUTF16(dest));
    }

    Result createDir(const std::u16string& path, u32 attributes)
    {
        return createDir(fsMakePath(PATH_UTF16, path.c_str()), attributes);
    }
    Result createDir(const std::string& path, u32 attributes)
    {
        return createDir(StringUtils::UTF8toUTF16(path), attributes);
    }

    std::unique_ptr<Directory> directory(const std::u16string& path)
    {
        return directory(fsMakePath(PATH_UTF16, path.c_str()));
    }
    std::unique_ptr<Directory> directory(const std::string& path)
    {
        return directory(StringUtils::UTF8toUTF16(path));
    }

    Result deleteDir(const std::string& path) { return deleteDir(StringUtils::UTF8toUTF16(path)); }

    Result createFile(const std::u16string& path, u32 attributes, u64 size)
    {
        return createFile(fsMakePath(PATH_UTF16, path.c_str()), attributes, size);
    }
    Result createFile(const std::string& path, u32 attributes, u64 size)
    {
        return createFile(StringUtils::UTF8toUTF16(path), attributes, size);
    }

    std::unique_ptr<File> file(const std::u16string& path, u32 flags, u32 attributes = 0)
    {
        return file(fsMakePath(PATH_UTF16, path.c_str()), flags, attributes);
    }
    std::unique_ptr<File> file(const std::string& path, u32 flags, u32 attributes = 0)
    {
        return file(StringUtils::UTF8toUTF16(path), flags, attributes);
    }

    Result deleteFile(const std::u16string& path)
    {
        return deleteFile(fsMakePath(PATH_UTF16, path.c_str()));
    }
    Result deleteFile(const std::string& path)
    {
        return deleteFile(StringUtils::UTF8toUTF16(path));
    }

    Result commit();
    Result close();

    Result result() const { return mResult; }

private:
    u64 mHandle;
    mutable Result mResult;
    bool mPXI;
};

#endif
