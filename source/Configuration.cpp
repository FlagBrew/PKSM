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

#include "Configuration.hpp"
#include "archive.hpp"
#include "FSStream.hpp"

Configuration::Configuration()
{
    static const std::u16string path = StringUtils::UTF8toUTF16("/config.json");
    FSStream stream(Archive::data(), path, FS_OPEN_READ);

    fprintf(stderr, "First open in constructor: 0x%016lX\n", stream.result());
    
    if (R_FAILED(stream.result()))
    {
        std::ifstream istream("romfs:/config.json");
        istream >> mJson;
        istream.close();
        save();
    }
    else
    {
        oldSize = stream.size();
        fprintf(stderr, "Size in constructor: 0x%016lX\n", stream.result());
        char* jsonData = new char[oldSize];
        stream.read(jsonData, oldSize);
        fprintf(stderr, "Read in constructor: 0x%016lX\n", stream.result());
        stream.close();
        fprintf(stderr, "Close in constructor: 0x%016lX\n", stream.result());
        fprintf(stderr, "Read data %s", jsonData);
        mJson.parse(jsonData);
        fprintf(stderr, "JSON data %s", mJson.dump(2).data());
        delete[] jsonData;
    }
}

void Configuration::save()
{
    static const std::u16string path = StringUtils::UTF8toUTF16("/config.json");

    std::string writeData = mJson.dump(2);
    writeData.shrink_to_fit();
    size_t size = writeData.size();

    if (oldSize > size)
    {
        Result res = FSUSER_DeleteFile(Archive::data(), fsMakePath(PATH_UTF16, path.data()));
        fprintf(stderr, "Delete in save: 0x%016lX\n", res);
    }

    FSStream stream(Archive::data(), path, FS_OPEN_WRITE, oldSize = size);
    fprintf(stderr, "Open in save: 0x%016lX\n", stream.result());
    u32 written = stream.write(writeData.data(), size);
    fprintf(stderr, "Write in save: 0x%016lX\n", stream.result());
    fprintf(stderr, "Size of data written: %lu", written);
    fprintf(stderr, "Data attempted to write: %s", writeData.data());
    stream.close();
    fprintf(stderr, "Close in save: 0x%016lX\n", stream.result());
}