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
        char* jsonData = new char[oldSize + 1];
        jsonData[oldSize] = '\0';
        stream.read(jsonData, oldSize);
        stream.close();
        mJson = nlohmann::json::parse(jsonData);
        delete[] jsonData;
    }
}

void Configuration::save()
{
    static const std::u16string path = StringUtils::UTF8toUTF16("/config.json");

    std::string writeData = mJson.dump(2);
    writeData.shrink_to_fit();
    size_t size = writeData.size();

    if (oldSize != size)
    {
        FSUSER_DeleteFile(Archive::data(), fsMakePath(PATH_UTF16, path.data()));
    }

    FSStream stream(Archive::data(), path, FS_OPEN_WRITE, oldSize = size);
    stream.write(writeData.data(), size);
    stream.close();
}

std::pair<std::vector<std::string>, std::vector<std::string>> Configuration::extraSaves(std::string id)
{
    std::vector<std::string> folders = {}, files = {};
    if (mJson["extraSaves"].find(id) != mJson.end())
    {
        if (mJson["extraSaves"][id].find("folders") != mJson.end())
        {
            nlohmann::json add = mJson["extraSaves"][id]["folders"];
            for (size_t i = 0; i < add.size(); i++)
            {
                folders.push_back(add[i]);
            }
        }
        if (mJson["extraSaves"][id].find("files") != mJson.end())
        {
            nlohmann::json add = mJson["extraSaves"][id]["folders"];
            for (size_t i = 0; i < add.size(); i++)
            {
                files.push_back(add[i]);
            }
        }
    }
    return {folders, files};
}

void Configuration::extraSaves(std::string id, std::pair<std::vector<std::string>, std::vector<std::string>>& value)
{
    if (!value.first.empty())
    {
        mJson["extraSaves"][id]["folders"] = value.first;
    }
    if (!value.second.empty())
    {
        mJson["extraSaves"][id]["files"] = value.second;
    }
}