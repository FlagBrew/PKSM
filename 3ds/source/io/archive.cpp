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

#include "archive.hpp"
#include "Directory.hpp"
#include "FSStream.hpp"
#include "STDirectory.hpp"
#include "smdh.hpp"
#include <sys/stat.h>

static FS_Archive sdmc;
static FS_Archive mData;

static constexpr size_t MOVE_BUFFER_SIZE = 16 * 1024;

static constexpr FS_ExtSaveDataInfo PKSM_ARCHIVE_DATA = {MEDIATYPE_SD, 0, 0, UNIQUE_ID, 0};

Result Archive::moveDir(FS_Archive src, const std::u16string& dir, FS_Archive dst, const std::u16string& dest)
{
    Result res;
    if (R_FAILED(res = FSUSER_CreateDirectory(dst, fsMakePath(PATH_UTF16, dest.data()), 0)) && res != (long)0xC82044BE && res != (long)0xC82044B9)
        return res;
    Directory d(src, dir);
    std::u16string srcDir = dir.back() == u'/' ? dir : dir + u'/';
    std::u16string dstDir = dest.back() == u'/' ? dest : dest + u'/';
    if (d.loaded())
    {
        for (size_t i = 0; i < d.count(); i++)
        {
            if (d.folder(i))
            {
                if (R_FAILED(res = moveDir(src, srcDir + d.item(i), dst, dstDir + d.item(i))))
                    return res;
            }
            else
            {
                if (R_FAILED(res = moveFile(src, srcDir + d.item(i), dst, dstDir + d.item(i))))
                    return res;
            }
        }
    }
    else
    {
        return d.error();
    }

    FSUSER_DeleteDirectory(src, fsMakePath(PATH_UTF16, dir.data()));

    if (res == (long)0xC82044BE || res == (long)0xC82044B9)
        return 0;

    return res;
}

Result Archive::moveFile(FS_Archive src, const std::u16string& file, FS_Archive dst, const std::u16string& dest)
{
    Result res = 0;
    FSStream stream(src, file, FS_OPEN_READ);
    if (stream.good())
    {
        size_t target = stream.size();
        deleteFile(dst, dest);
        FSStream out(dst, dest, FS_OPEN_WRITE, target);
        if (out.good())
        {
            size_t written = 0;
            u8* data       = new u8[MOVE_BUFFER_SIZE];
            while (written < target)
            {
                stream.read(data, std::min(MOVE_BUFFER_SIZE, target - written));
                if (R_FAILED(res = stream.result()))
                {
                    break;
                }
                written += out.write(data, std::min(MOVE_BUFFER_SIZE, target - written));
                if (R_FAILED(res = out.result()))
                {
                    break;
                }
            }
            stream.close();
            out.close();
            delete[] data;
            if (R_SUCCEEDED(res))
            {
                deleteFile(src, file);
            }
        }
        else
        {
            res = out.result();
            out.close();
        }
    }
    else
    {
        res = stream.result();
        stream.close();
    }
    return res;
}

Result Archive::copyFile(FS_Archive src, const std::u16string& file, FS_Archive dst, const std::u16string& dest)
{
    Result res = 0;
    FSStream stream(src, file, FS_OPEN_READ);
    if (stream.good())
    {
        size_t target = stream.size();
        deleteFile(dst, dest);
        FSStream out(dst, dest, FS_OPEN_WRITE, target);
        if (out.good())
        {
            size_t written = 0;
            u8* data       = new u8[MOVE_BUFFER_SIZE];
            while (written < target)
            {
                stream.read(data, std::min(MOVE_BUFFER_SIZE, target - written));
                if (R_FAILED(res = stream.result()))
                {
                    break;
                }
                written += out.write(data, std::min(MOVE_BUFFER_SIZE, target - written));
                if (R_FAILED(res = out.result()))
                {
                    break;
                }
            }
            stream.close();
            out.close();
            delete[] data;
        }
        else
        {
            res = out.result();
            out.close();
        }
    }
    else
    {
        res = stream.result();
        stream.close();
    }
    return res;
}

Result Archive::deleteFile(FS_Archive archive, const std::u16string& file)
{
    return FSUSER_DeleteFile(archive, fsMakePath(PATH_UTF16, file.c_str()));
}

static void moveOldBackups()
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

Result Archive::init(const std::string& execPath)
{
    Result res = 0;
    if (R_FAILED(res = FSUSER_OpenArchive(&sdmc, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""))))
        return res;
    if (R_FAILED(res = extdata(&mData, UNIQUE_ID)))
    {
        if (R_FAILED(res = createPKSMExtdataArchive(execPath)))
            return res;

        if (R_FAILED(res = FSUSER_CreateFile(data(), fsMakePath(PATH_UTF16, u"/sizeCheck"), 0, 1)) && res != (long)0xC82044B9)
            return res;
    }
    else
    {
        Handle checkFile;
        if (R_FAILED(res = FSUSER_OpenFile(&checkFile, data(), fsMakePath(PATH_UTF16, u"/sizeCheck"), FS_OPEN_READ, 0)))
        {
            FSFILE_Close(checkFile);
            if (R_FAILED(res = moveDir(data(), u"/", sd(), u"/3ds/PKSM/extdata")))
                return res;

            if (R_FAILED(res = FSUSER_CloseArchive(mData)))
                return res;
            if (R_FAILED(res = FSUSER_DeleteExtSaveData(PKSM_ARCHIVE_DATA)))
                return res;
            if (R_FAILED(res = createPKSMExtdataArchive(execPath)))
                return res;

            if (R_FAILED(res = moveDir(sd(), u"/3ds/PKSM/extdata", data(), u"/")))
                return res;

            if (R_FAILED(res = FSUSER_CreateFile(data(), fsMakePath(PATH_UTF16, u"/sizeCheck"), 0, 1)) && res != (long)0xC82044B9)
                return res;
        }
        else
        {
            FSFILE_Close(checkFile);
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
    FSUSER_CreateDirectory(Archive::data(), fsMakePath(PATH_UTF16, u"/banks"), 0);
    FSUSER_DeleteDirectoryRecursively(Archive::sd(), fsMakePath(PATH_UTF16, u"/3ds/PKSM/additionalassets"));

    moveOldBackups();
    FSUSER_DeleteDirectoryRecursively(Archive::sd(), fsMakePath(PATH_UTF16, u"/3ds/PKSM/backup"));
    return res;
}

void Archive::exit(void)
{
    FSUSER_CloseArchive(mData);
    FSUSER_CloseArchive(sdmc);
}

FS_Archive Archive::sd(void)
{
    return sdmc;
}

FS_Archive Archive::data(void)
{
    return mData;
}

Result Archive::save(FS_Archive* archive, FS_MediaType mediatype, u32 lowid, u32 highid)
{
    const u32 path[3] = {mediatype, lowid, highid};
    return FSUSER_OpenArchive(archive, ARCHIVE_USER_SAVEDATA, {PATH_BINARY, 12, path});
}

Result Archive::extdata(FS_Archive* archive, u32 extdata)
{
    const u32 path[3] = {MEDIATYPE_SD, extdata, 0};
    return FSUSER_OpenArchive(archive, ARCHIVE_EXTDATA, {PATH_BINARY, 12, path});
}

bool Archive::saveAccessible(FS_MediaType mediatype, u32 lowid, u32 highid)
{
    FS_Archive archive;
    Result res = save(&archive, mediatype, lowid, highid);
    if (R_SUCCEEDED(res))
    {
        FSUSER_CloseArchive(archive);
        return true;
    }
    return false;
}

bool Archive::extdataAccessible(u32 id)
{
    FS_Archive archive;
    Result res = extdata(&archive, id);
    if (R_SUCCEEDED(res))
    {
        FSUSER_CloseArchive(archive);
        return true;
    }
    return false;
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
        smdh = new smdh_s;
        FSStream in(Archive::sd(), execPath, FS_OPEN_READ);
        in.seek(0x20, SEEK_SET);
        u32 pos;
        in.read(&pos, sizeof(pos));
        in.seek(pos, SEEK_SET);
        in.read(smdh, sizeof(smdh_s));
        in.close();
    }

    Result res = FSUSER_CreateExtSaveData(PKSM_ARCHIVE_DATA, ndirs, nfiles, sizeLimit, sizeof(smdh_s), (u8*)smdh);
    if (R_SUCCEEDED(res))
    {
        res = extdata(&mData, UNIQUE_ID);
    }
    delete smdh;
    return res;
}
