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
#include "FSStream.hpp"
#include "archive.hpp"
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
        oldSize           = stream.size();
        char* jsonData    = new char[oldSize + 1];
        jsonData[oldSize] = '\0';
        stream.read(jsonData, oldSize);
        stream.close();
        mJson = nlohmann::json::parse(jsonData, nullptr, false);
        delete[] jsonData;

        if (mJson.is_discarded())
        {
            Gui::warn("Configuration file is corrupted!", "Using default.");
            loadFromRomfs();
        }
        else if (!mJson.contains("version"))
        {
            Gui::warn("Version not found in config file!", "Using default.");
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
                mJson["defaults"]["region"]  = countryData[2];
                CFGU_SecureInfoGetRegion(countryData);
                mJson["defaults"]["nationality"] = countryData[0];
            }
            if (mJson["version"].get<int>() < 5)
            {
                for (auto& game : mJson["extraSaves"])
                {
                    game.erase("folders");
                    if (game.count("files") > 0)
                    {
                        nlohmann::json tmp = game["files"];
                        game               = tmp;
                    }
                    else
                    {
                        game = nlohmann::json::array();
                    }
                }
            }
            if (mJson["version"].get<int>() < 6)
            {
                mJson.erase("storageSize");
                mJson["showBackups"] = false;
            }

            mJson["version"] = CURRENT_VERSION;
            save();
        }
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

std::vector<std::string> Configuration::extraSaves(const std::string& id)
{
    if (mJson["extraSaves"].count(id) > 0)
    {
        return mJson["extraSaves"][id].get<std::vector<std::string>>();
    }
    return {};
}

void Configuration::extraSaves(const std::string& id, std::vector<std::string>& value)
{
    mJson["extraSaves"][id] = value;
}

void Configuration::loadFromRomfs()
{
    FILE* in = fopen("romfs:/config.json", "rt");
    mJson    = nlohmann::json::parse(in, nullptr, false);
    fclose(in);

    // load system language
    u8 systemLanguage;
    CFGU_GetSystemLanguage(&systemLanguage);
    u8 countryData[4];
    CFGU_GetConfigInfoBlk2(0x4, 0x000B0000, countryData);
    mJson["defaults"]["country"] = countryData[3];
    mJson["defaults"]["region"]  = countryData[2];
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
