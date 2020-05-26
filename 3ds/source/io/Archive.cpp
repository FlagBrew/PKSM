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

#include "Archive.hpp"
#include "Directory.hpp"
#include "File.hpp"
#include "STDirectory.hpp"
#include "csvc.h"
#include "internal_fspxi.hpp"
#include "smdh.hpp"
#include <sys/stat.h>

namespace
{
    constexpr u64 MOVE_BUFFER_SIZE = 16 * 1024;

    constexpr FS_ExtSaveDataInfo PKSM_ARCHIVE_DATA = {MEDIATYPE_SD, 0, 0, UNIQUE_ID, 0};

    Archive sdArchive;
    Archive dataArchive;

    void moveOldBackups()
    {
        STDirectory d("/3ds/PKSM/backup");
        if (d.good())
        {
            for (size_t i = 0; i < d.count(); i++)
            {
                std::string abbreviation = d.item(i).substr(0, d.item(i).find('_'));
                if (abbreviation == "PT")
                {
                    abbreviation = "PL";
                }
                if (abbreviation.size() > 2)
                {
                    // This is not in the correct format, abort
                    continue;
                }
                else if (abbreviation == "P" || abbreviation == "D" || abbreviation == "PL" || abbreviation == "HG" || abbreviation == "SS" ||
                         abbreviation == "B" || abbreviation == "W" || abbreviation == "B2" || abbreviation == "W2")
                {
                    Archive::moveFile(Archive::sd(), "/3ds/PKSM/backup/" + d.item(i) + "/main", Archive::sd(),
                        "/3ds/PKSM/backup/" + d.item(i) + "/POKEMON " + abbreviation + ".sav");
                }

                std::string destFolder = "/3ds/PKSM/backups/";

                if (abbreviation == "P")
                {
                    destFolder += "APAE";
                }
                else if (abbreviation == "D")
                {
                    destFolder += "ADAE";
                }
                else if (abbreviation == "PL")
                {
                    destFolder += "CPUE";
                }
                else if (abbreviation == "HG")
                {
                    destFolder += "IPKE";
                }
                else if (abbreviation == "SS")
                {
                    destFolder += "IPGE";
                }
                else if (abbreviation == "B")
                {
                    destFolder += "IRBO";
                }
                else if (abbreviation == "W")
                {
                    destFolder += "IRAO";
                }
                else if (abbreviation == "B2")
                {
                    destFolder += "IREO";
                }
                else if (abbreviation == "W2")
                {
                    destFolder += "IRDO";
                }
                else if (abbreviation == "X")
                {
                    destFolder += "0x0055D";
                }
                else if (abbreviation == "Y")
                {
                    destFolder += "0x0055E";
                }
                else if (abbreviation == "OR")
                {
                    destFolder += "0x011C4";
                }
                else if (abbreviation == "AS")
                {
                    destFolder += "0x011C5";
                }
                else if (abbreviation == "S")
                {
                    destFolder += "0x01648";
                }
                else if (abbreviation == "M")
                {
                    destFolder += "0x0175E";
                }
                else if (abbreviation == "US")
                {
                    destFolder += "0x01B50";
                }
                else if (abbreviation == "UM")
                {
                    destFolder += "0x01B51";
                }

                mkdir(destFolder.c_str(), 777);

                Archive::moveDir(Archive::sd(), "/3ds/PKSM/backup/" + d.item(i), Archive::sd(), destFolder + '/' + d.item(i));
            }
        }
    }
}

Archive::Archive(FS_ArchiveID id, FS_Path path, bool pxi) : mPXI(pxi)
{
    if (pxi)
    {
        mResult = FSPXI_OpenArchive(fspxiHandle, &mHandle, id, path);
    }
    else
    {
        mResult = FSUSER_OpenArchive(&mHandle, id, path);
    }

    if (R_FAILED(mResult))
    {
        mHandle = 0;
    }
}

Archive::Archive(Archive&& other) : mHandle(other.mHandle), mResult(other.mResult), mPXI(other.mPXI)
{
    // Reset other's handle so it doesn't close this one.
    other.mHandle = 0;
}

Archive& Archive::operator=(Archive&& other)
{
    if (&other != this)
    {
        close();
        mHandle = other.mHandle;
        mPXI    = other.mPXI;
        mResult = other.mResult;
        // Reset other's handle so it doesn't close this one.
        other.mHandle = 0;
    }
    return *this;
}

Result Archive::moveDir(Archive& src, const std::u16string& dir, Archive& dst, const std::u16string& dest)
{
    Result res;
    if (R_FAILED(res = dst.createDir(dest, 0)) && res != (long)0xC82044BE && res != (long)0xC82044B9)
        return res;
    auto d                = src.directory(dir);
    std::u16string srcDir = dir.back() == u'/' ? dir : dir + u'/';
    std::u16string dstDir = dest.back() == u'/' ? dest : dest + u'/';
    if (d)
    {
        for (size_t i = 0; i < d->count(); i++)
        {
            if (d->folder(i))
            {
                if (R_FAILED(res = moveDir(src, srcDir + d->item(i), dst, dstDir + d->item(i))))
                    return res;
            }
            else
            {
                if (R_FAILED(res = moveFile(src, srcDir + d->item(i), dst, dstDir + d->item(i))))
                    return res;
            }
        }
    }
    else
    {
        return src.result();
    }

    res = src.deleteDir(dir);

    if (res == (long)0xC82044BE || res == (long)0xC82044B9)
        return 0;

    return res;
}

Result Archive::copyDir(Archive& src, const std::u16string& dir, Archive& dst, const std::u16string& dest)
{
    Result res;
    if (R_FAILED(res = dst.createDir(dest, 0)) && res != (long)0xC82044BE && res != (long)0xC82044B9)
        return res;
    auto d                = src.directory(dir);
    std::u16string srcDir = dir.back() == u'/' ? dir : dir + u'/';
    std::u16string dstDir = dest.back() == u'/' ? dest : dest + u'/';
    if (d)
    {
        for (size_t i = 0; i < d->count(); i++)
        {
            if (d->folder(i))
            {
                if (R_FAILED(res = moveDir(src, srcDir + d->item(i), dst, dstDir + d->item(i))))
                    return res;
            }
            else
            {
                if (R_FAILED(res = moveFile(src, srcDir + d->item(i), dst, dstDir + d->item(i))))
                    return res;
            }
        }
    }
    else
    {
        return src.result();
    }

    if (res == (long)0xC82044BE || res == (long)0xC82044B9)
        return 0;

    return res;
}

Result Archive::moveFile(Archive& src, FS_Path file, Archive& dst, FS_Path dest)
{
    Result res  = 0;
    auto stream = src.file(file, FS_OPEN_READ);
    if (stream)
    {
        u64 target = stream->size();
        dst.deleteFile(dest);
        dst.createFile(dest, 0, target);
        auto out = dst.file(dest, FS_OPEN_WRITE);
        if (out)
        {
            size_t written = 0;
            u8* data       = new u8[MOVE_BUFFER_SIZE];
            while (written < target)
            {
                stream->read(data, std::min(MOVE_BUFFER_SIZE, target - written));
                if (R_FAILED(res = stream->result()))
                {
                    break;
                }
                written += out->write(data, std::min(MOVE_BUFFER_SIZE, target - written));
                if (R_FAILED(res = out->result()))
                {
                    break;
                }
            }
            stream->close();
            out->close();
            delete[] data;
            if (R_SUCCEEDED(res))
            {
                src.deleteFile(file);
            }
        }
        else
        {
            res = dst.result();
            stream->close();
        }
    }
    else
    {
        res = src.result();
    }
    return res;
}

Result Archive::copyFile(Archive& src, FS_Path file, Archive& dst, FS_Path dest)
{
    Result res  = 0;
    auto stream = src.file(file, FS_OPEN_READ);
    if (stream)
    {
        u64 target = stream->size();
        dst.deleteFile(dest);
        dst.createFile(dest, 0, target);
        auto out = dst.file(dest, FS_OPEN_WRITE);
        if (out)
        {
            size_t written = 0;
            u8* data       = new u8[MOVE_BUFFER_SIZE];
            while (written < target)
            {
                stream->read(data, std::min(MOVE_BUFFER_SIZE, target - written));
                if (R_FAILED(res = stream->result()))
                {
                    break;
                }
                written += out->write(data, std::min(MOVE_BUFFER_SIZE, target - written));
                if (R_FAILED(res = out->result()))
                {
                    break;
                }
            }
            stream->close();
            out->close();
            delete[] data;
        }
        else
        {
            res = dst.result();
            stream->close();
        }
    }
    else
    {
        res = src.result();
    }
    return res;
}

Result Archive::createFile(FS_Path file, u32 attributes, u64 size)
{
    if (mPXI)
    {
        return mResult = FSPXI_CreateFile(fspxiHandle, mHandle, file, attributes, size);
    }
    else
    {
        return mResult = FSUSER_CreateFile(mHandle, file, attributes, size);
    }
}

std::unique_ptr<File> Archive::file(FS_Path file, u32 flags, u32 attributes)
{
    if (mPXI)
    {
        FSPXI_File f;
        if (R_SUCCEEDED(mResult = FSPXI_OpenFile(fspxiHandle, &f, mHandle, file, flags, attributes)))
        {
            return std::unique_ptr<File>(new File(f));
        }
    }
    else
    {
        Handle f;
        if (R_SUCCEEDED(mResult = FSUSER_OpenFile(&f, mHandle, file, flags, attributes)))
        {
            return std::unique_ptr<File>(new File(f));
        }
    }
    return nullptr;
}

Result Archive::deleteFile(FS_Path file)
{
    if (mPXI)
    {
        return FSPXI_DeleteFile(fspxiHandle, mHandle, file);
    }
    else
    {
        return FSUSER_DeleteFile(mHandle, file);
    }
}

Result Archive::createDir(FS_Path file, u32 attributes)
{
    if (mPXI)
    {
        return mResult = FSPXI_CreateDirectory(fspxiHandle, mHandle, file, attributes);
    }
    else
    {
        return mResult = FSUSER_CreateDirectory(mHandle, file, attributes);
    }
}

std::unique_ptr<Directory> Archive::directory(FS_Path dir)
{
    if (mPXI)
    {
        FSPXI_Directory d;
        if (R_SUCCEEDED(mResult = FSPXI_OpenDirectory(fspxiHandle, &d, mHandle, dir)))
        {
            return std::unique_ptr<Directory>(new Directory(d));
        }
    }
    else
    {
        Handle d;
        if (R_SUCCEEDED(mResult = FSUSER_OpenDirectory(&d, mHandle, dir)))
        {
            return std::unique_ptr<Directory>(new Directory(d));
        }
    }
    return nullptr;
}

Result Archive::deleteDir(const std::u16string& dir)
{
    if (mPXI)
    {
        Result res;
        auto d                = directory(dir);
        std::u16string srcDir = dir.back() == u'/' ? dir : dir + u'/';
        if (d)
        {
            for (size_t i = 0; i < d->count(); i++)
            {
                if (d->folder(i))
                {
                    if (R_FAILED(res = deleteDir(srcDir + d->item(i))))
                        return res;
                }
                else
                {
                    if (R_FAILED(res = deleteFile(srcDir + d->item(i))))
                        return res;
                }
            }
            res = FSPXI_DeleteDirectory(fspxiHandle, mHandle, fsMakePath(PATH_UTF16, dir.c_str()));
        }
        else
        {
            return result();
        }
        return res;
    }
    else
    {
        return FSUSER_DeleteDirectoryRecursively(mHandle, fsMakePath(PATH_UTF16, dir.c_str()));
    }
}

Result Archive::init(const std::string& execPath)
{
    Result res = 0;
    if (R_FAILED(res = svcControlService(SERVICEOP_STEAL_CLIENT_SESSION, &fspxiHandle, "PxiFS0")))
        return res;

    sdArchive = Archive{ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""), false};
    if (R_FAILED(sd().result()))
        return res;
    dataArchive = extdata(UNIQUE_ID, false);
    if (R_FAILED(data().result()))
    {
        if (R_FAILED(res = createPKSMExtdataArchive(execPath)))
            return res;

        data() = extdata(UNIQUE_ID, false);

        if (R_FAILED(res = data().createFile(fsMakePath(PATH_UTF16, u"/sizeCheck"), 0, 1)) && res != (long)0xC82044B9)
            return res;
    }
    else
    {
        auto checkFile = data().file(fsMakePath(PATH_UTF16, u"/sizeCheck"), FS_OPEN_READ);
        if (!checkFile)
        {
            if (R_FAILED(res = moveDir(data(), u"/", sd(), u"/3ds/PKSM/extdata")))
                return res;

            if (R_FAILED(res = data().close()))
                return res;
            if (R_FAILED(res = FSUSER_DeleteExtSaveData(PKSM_ARCHIVE_DATA)))
                return res;
            if (R_FAILED(res = createPKSMExtdataArchive(execPath)))
                return res;

            data() = extdata(UNIQUE_ID, false);

            if (R_FAILED(res = moveDir(sd(), u"/3ds/PKSM/extdata", data(), u"/")))
                return res;

            if (R_FAILED(res = data().createFile(fsMakePath(PATH_UTF16, u"/sizeCheck"), 0, 1)) && res != (long)0xC82044B9)
                return res;
        }
        else
        {
            checkFile->close();
        }
    }
    mkdir("/3ds", 777);
    mkdir("/3ds/PKSM", 777);
    mkdir("/3ds/PKSM/assets", 777);
    mkdir("/3ds/PKSM/backups", 777);
    mkdir("/3ds/PKSM/backups/bridge", 777);
    mkdir("/3ds/PKSM/dumps", 777);
    mkdir("/3ds/PKSM/banks", 777);
    mkdir("/3ds/PKSM/songs", 777);
    mkdir("/3ds/PKSM/mysterygift", 777);
    Archive::data().createDir(fsMakePath(PATH_UTF16, u"/banks"), 0);
    Archive::sd().deleteDir(u"/3ds/PKSM/additionalassets");

    moveOldBackups();
    Archive::sd().deleteDir(u"/3ds/PKSM/backup");
    return res;
}

void Archive::exit(void)
{
    sd().close();
    data().close();

    svcCloseHandle(fspxiHandle);
}

Archive& Archive::sd()
{
    return sdArchive;
}

Archive& Archive::data()
{
    return dataArchive;
}

Archive Archive::save(FS_MediaType mediatype, u32 lowid, u32 highid, bool pxi)
{
    const u32 path[3] = {mediatype, lowid, highid};
    return Archive{ARCHIVE_USER_SAVEDATA, {PATH_BINARY, sizeof(path), path}, pxi};
}

Archive Archive::saveAndContents(FS_MediaType mediatype, u32 lowid, u32 highid, bool pxi, u32 pathWord4)
{
    const u32 path[4] = {lowid, highid, mediatype, pathWord4};
    return Archive{ARCHIVE_SAVEDATA_AND_CONTENT, {PATH_BINARY, sizeof(path), path}, pxi};
}

Archive Archive::extdata(u32 extdata, bool pxi)
{
    const u32 path[3] = {MEDIATYPE_SD, extdata, 0};
    return Archive{ARCHIVE_EXTDATA, {PATH_BINARY, sizeof(path), path}, pxi};
}

Result Archive::close()
{
    if (mPXI)
    {
        return mResult = FSPXI_CloseArchive(fspxiHandle, mHandle);
    }
    else
    {
        return mResult = FSUSER_CloseArchive(mHandle);
    }
}

Result Archive::commit()
{
    if (mPXI)
    {
        return mResult = FSPXI_CommitSaveData(fspxiHandle, mHandle, ARCHIVE_ACTION_COMMIT_SAVE_DATA);
    }
    else
    {
        return mResult = FSUSER_ControlArchive(mHandle, ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0);
    }
}

Result Archive::createPKSMExtdataArchive(const std::string& execPath)
{
    u32 ndirs     = 100;                // TODO
    u32 nfiles    = 200;                // TODO
    u64 sizeLimit = 0xFFFFFFFFFFFFFFFF; // TODO

    smdh_s* smdh;
    if (execPath == "")
    {
        smdh = loadSMDH(UNIQUE_ID << 8, 0x00040000, MEDIATYPE_SD);
    }
    else
    {
        smdh     = new smdh_s;
        FILE* in = fopen(execPath.c_str(), "rb");
        if (in)
        {
            fseek(in, 0x20, SEEK_SET);
            u32 pos;
            fread(&pos, sizeof(pos), 1, in);
            fseek(in, pos, SEEK_SET);
            fread(smdh, sizeof(smdh_s), 1, in);
            fclose(in);
        }
        else
        {
            return -errno;
        }
    }

    Result res = FSUSER_CreateExtSaveData(PKSM_ARCHIVE_DATA, ndirs, nfiles, sizeLimit, sizeof(smdh_s), (u8*)smdh);
    delete smdh;
    return res;
}
