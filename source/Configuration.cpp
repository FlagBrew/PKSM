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

#include "Configuration.hpp"
#include "archive.hpp"
#include "FSStream.hpp"
#include "gui.hpp"

Configuration::Configuration()
{
    static const std::u16string path = StringUtils::UTF8toUTF16("/config.json");
    FSStream stream(Archive::data(), path, FS_OPEN_READ);
    
    if (R_FAILED(stream.result()))
    {
        loadFromRomfs();
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

        if (mJson.find("version") == mJson.end())
        {
            loadFromRomfs();
        }
        else if (mJson["version"].get<int>() != CURRENT_VERSION)
        {
            if (mJson["version"].get<int>() > CURRENT_VERSION)
            {
                Gui::warn(i18n::localize("THE_FUCK"), i18n::localize("DO_NOT_DOWNGRADE"));
                return;
            }
            if (mJson["version"].get<int>() < 2)
            {
                mJson["useSaveInfo"] = false;
            }
            if (mJson["version"].get<int>() < 3)
            {
                mJson["randomMusic"] = false;
            }
            if (mJson["version"].get<int>() < 4)
            {
                u8 countryData[4];
                CFGU_GetConfigInfoBlk2(0x4, 0x000B0000, countryData);
                mJson["defaults"]["country"] = countryData[3];
                mJson["defaults"]["region"] = countryData[2];
                CFGU_SecureInfoGetRegion(countryData);
                mJson["defaults"]["nationality"] = countryData[0];
            }

            mJson["version"] = CURRENT_VERSION;
            save();
        }
    }
}

void Configuration::save()
{
    FS_Archive archive = Archive::data();
    static const std::u16string path = StringUtils::UTF8toUTF16("/config.json");

    archive = Archive::sd();
    mJson["useExtdata"] = false;

    std::string writeData = mJson.dump(2);
    writeData.shrink_to_fit();
    size_t size = writeData.size();

    if (oldSize != size)
    {
        FSUSER_DeleteFile(archive, fsMakePath(PATH_UTF16, path.data()));
    }

    FSStream stream(archive, path, FS_OPEN_WRITE, oldSize = size);
    stream.write(writeData.data(), size);
    stream.close();
}

std::pair<std::vector<std::string>, std::vector<std::string>> Configuration::extraSaves(std::string id)
{
    std::vector<std::string> folders = {}, files = {};
    if (mJson["extraSaves"].find(id) != mJson["extraSaves"].end())
    {
        if (mJson["extraSaves"][id].find("folders") != mJson["extraSaves"][id].end())
        {
            nlohmann::json add = mJson["extraSaves"][id]["folders"];
            for (size_t i = 0; i < add.size(); i++)
            {
                folders.push_back(add[i]);
            }
        }
        if (mJson["extraSaves"][id].find("files") != mJson["extraSaves"][id].end())
        {
            nlohmann::json add = mJson["extraSaves"][id]["files"];
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

void Configuration::loadFromRomfs()
{
    std::ifstream istream("romfs:/config.json");
    istream >> mJson;
    istream.close();

    // load system language
    u8 systemLanguage;
    CFGU_GetSystemLanguage(&systemLanguage);
    u8 countryData[4];
    CFGU_GetConfigInfoBlk2(0x4, 0x000B0000, countryData);
    mJson["defaults"]["country"] = countryData[3];
    mJson["defaults"]["region"] = countryData[2];
    CFGU_SecureInfoGetRegion(countryData);
    mJson["defaults"]["nationality"] = *countryData;
    
    switch (systemLanguage)
    {
        case CFG_LANGUAGE_JP:
            systemLanguage = Language::JP;
            break;
        case CFG_LANGUAGE_EN:
            systemLanguage = Language::EN;
            break;
        case CFG_LANGUAGE_FR:
            systemLanguage = Language::FR;
            break;
        case CFG_LANGUAGE_DE:
            systemLanguage = Language::DE;
            break;
        case CFG_LANGUAGE_IT:
            systemLanguage = Language::IT;
            break;
        case CFG_LANGUAGE_ES:
            systemLanguage = Language::ES;
            break;
        case CFG_LANGUAGE_ZH:
            systemLanguage = Language::ZH;
            break;
        case CFG_LANGUAGE_KO:
            systemLanguage = Language::KO;
            break;
        case CFG_LANGUAGE_NL:
            systemLanguage = Language::NL;
            break;
        case CFG_LANGUAGE_PT:
            systemLanguage = Language::PT;
            break;
        case CFG_LANGUAGE_RU:
            systemLanguage = Language::RU;
            break;
        case CFG_LANGUAGE_TW:
            systemLanguage = Language::TW;
            break;
        default:
            systemLanguage = Language::EN;
            break;
    }
    mJson["language"] = systemLanguage;

    save();
}