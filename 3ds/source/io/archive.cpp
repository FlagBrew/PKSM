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
#include <sys/stat.h>

static FS_Archive sdmc;
static FS_Archive mData;

Result Archive::init(std::string& execPath)
{
    Result res = 0;
    if (R_FAILED(res = FSUSER_OpenArchive(&sdmc, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, "")))) return res;
    if (!extdataAccessible(UNIQUE_ID))
    {
        if (R_FAILED(res = createPKSMExtdataArchive(execPath))) return res;
    }
    else if (R_FAILED(res = extdata(&mData, UNIQUE_ID))) return res;
    mkdir("/3ds/PKSM", 777);
    mkdir("/3ds/PKSM/assets", 777);
    mkdir("/3ds/PKSM/backups", 777);
    mkdir("/3ds/PKSM/backups/bridge", 777);
    mkdir("/3ds/PKSM/dumps", 777);
    mkdir("/3ds/PKSM/banks", 777);
    mkdir("/3ds/PKSM/songs", 777);
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
    const u32 path[3] = { mediatype, lowid, highid };
    return FSUSER_OpenArchive(archive, ARCHIVE_USER_SAVEDATA, {PATH_BINARY, 12, path});
}

Result Archive::extdata(FS_Archive* archive, u32 extdata)
{
    const u32 path[3] = { MEDIATYPE_SD, extdata, 0 };
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

Result Archive::createPKSMExtdataArchive(std::string& execPath)
{
    u32 ndirs = 100; // TODO
    u32 nfiles = 100; // TODO
    u64 sizeLimit = 0x1000000; // TODO
    
    FS_ExtSaveDataInfo esdi;
    esdi.mediaType = MEDIATYPE_SD;
    esdi.saveId = UNIQUE_ID;
    smdh_s* smdh;
    if (execPath == "")
    {
        smdh = loadSMDH(UNIQUE_ID << 8, 0x00040000, MEDIATYPE_SD);
    }
    else
    {
        smdh = new smdh_s;
        FILE* in = fopen(execPath.c_str(), "rb");
        fseek(in, 0x20, SEEK_SET);
        u32 pos;
        fread(&pos, sizeof(pos), 1, in);
        fseek(in, pos, SEEK_SET);
        fread(smdh, 1, sizeof(smdh_s), in);
        fclose(in);
    }
    
    Result res = FSUSER_CreateExtSaveData(esdi, ndirs, nfiles, sizeLimit, sizeof(smdh_s), (u8*)smdh);
    if (R_SUCCEEDED(res))
    {
		res = extdata(&mData, UNIQUE_ID);
    }
    delete smdh;
    return res;
}
