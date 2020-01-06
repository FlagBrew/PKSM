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
#include "nlohmann/json.hpp"

Configuration::Configuration()
{
    FSStream stream(Archive::data(), u"/config.json", FS_OPEN_READ);

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
        mJson = std::make_unique<nlohmann::json>(nlohmann::json::parse(jsonData, nullptr, false));
        delete[] jsonData;

        if (!mJson->is_object())
        {
            loadFromRomfs();
            Gui::warn(i18n::localize((*mJson)["language"], "CONFIGURATION_FILE_CORRUPTED_1") + '\n' +
                          i18n::localize((*mJson)["language"], "CONFIGURATION_USING_DEFAULT"),
                (*mJson)["language"]);
            return;
        }

        if (!(mJson->contains("version") && (*mJson)["version"].is_number_integer()))
        {
            loadFromRomfs();
            Gui::warn(i18n::localize((*mJson)["language"], "CONFIGURATION_INCORRECT_FORMAT") + '\n' +
                          i18n::localize((*mJson)["language"], "CONFIGURATION_USING_DEFAULT"),
                (*mJson)["language"]);
            return;
        }

        if (!(mJson->contains("language") && (*mJson)["language"].is_number_integer()))
        {
            loadFromRomfs();
            Gui::warn(i18n::localize((*mJson)["language"], "CONFIGURATION_INCORRECT_FORMAT") + '\n' +
                          i18n::localize((*mJson)["language"], "CONFIGURATION_USING_DEFAULT"),
                (*mJson)["language"]);
            return;
        }

        if ((*mJson)["version"].get<int>() != CURRENT_VERSION)
        {
            if ((*mJson)["version"].get<int>() > CURRENT_VERSION)
            {
                loadFromRomfs();
                Gui::warn(i18n::localize((*mJson)["language"], "THE_FUCK") + '\n' + i18n::localize((*mJson)["language"], "DO_NOT_DOWNGRADE"),
                    (*mJson)["language"]);
                return;
            }
            if ((*mJson)["version"].get<int>() < 2)
            {
                (*mJson)["useSaveInfo"] = false;
            }
            if ((*mJson)["version"].get<int>() < 3)
            {
                (*mJson)["randomMusic"] = false;
            }
            if ((*mJson)["version"].get<int>() < 4)
            {
                u8 countryData[4];
                CFGU_GetConfigInfoBlk2(0x4, 0x000B0000, countryData);
                if (!(mJson->contains("defaults") && (*mJson)["defaults"].is_object()))
                {
                    loadFromRomfs();
                    Gui::warn(i18n::localize((*mJson)["language"], "CONFIGURATION_INCORRECT_FORMAT") + '\n' +
                                  i18n::localize((*mJson)["language"], "CONFIGURATION_USING_DEFAULT"),
                        (*mJson)["language"]);
                    return;
                }
                (*mJson)["defaults"]["country"] = countryData[3];
                (*mJson)["defaults"]["region"]  = countryData[2];
                CFGU_SecureInfoGetRegion(countryData);
                (*mJson)["defaults"]["nationality"] = countryData[0];
            }
            if ((*mJson)["version"].get<int>() < 5)
            {
                if (!(mJson->contains("extraSaves") && (*mJson)["extraSaves"].is_object()))
                    for (auto& game : (*mJson)["extraSaves"])
                    {
                        if (!game.is_object())
                        {
                            loadFromRomfs();
                            Gui::warn(i18n::localize((*mJson)["language"], "CONFIGURATION_INCORRECT_FORMAT") + '\n' +
                                          i18n::localize((*mJson)["language"], "CONFIGURATION_USING_DEFAULT"),
                                (*mJson)["language"]);
                            return;
                        }

                        game.erase("folders");
                        if (game.contains("files") && game["files"].is_array())
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
            if ((*mJson)["version"].get<int>() < 6)
            {
                mJson->erase("storageSize");
                (*mJson)["showBackups"] = false;
            }
            if ((*mJson)["version"].get<int>() < 7)
            {
                if (!(mJson->contains("defaults") && (*mJson)["defaults"].is_object()) ||
                    !((*mJson)["defaults"].contains("pid") && (*mJson)["defaults"]["pid"].is_number_integer()))
                {
                    loadFromRomfs();
                    Gui::warn(i18n::localize((*mJson)["language"], "CONFIGURATION_INCORRECT_FORMAT") + '\n' +
                                  i18n::localize((*mJson)["language"], "CONFIGURATION_USING_DEFAULT"),
                        (*mJson)["language"]);
                    return;
                }
                (*mJson)["defaults"]["tid"] = (*mJson)["defaults"]["pid"];
                (*mJson)["defaults"].erase("pid");
                (*mJson)["legalEndpoint"] = "https://flagbrew.org/pksm/legality/check";
            }
            if ((*mJson)["version"].get<int>() < 8)
            {
                (*mJson)["patronCode"]   = "";
                (*mJson)["alphaChannel"] = false;
                (*mJson)["autoUpdate"]   = true;
            }

            (*mJson)["version"] = CURRENT_VERSION;
            save();
        }

        // clang-format off
        if (!(mJson->contains("version") && (*mJson)["version"].is_number_integer()) ||
            !(mJson->contains("language") && (*mJson)["language"].is_number_integer()) ||
            !(mJson->contains("autoBackup") && (*mJson)["autoBackup"].is_boolean()) ||
            !(mJson->contains("transferEdit") && (*mJson)["transferEdit"].is_boolean()) ||
            !(mJson->contains("useExtData") && (*mJson)["useExtData"].is_boolean()) ||
            !(mJson->contains("defaults") && (*mJson)["defaults"].is_object()) ||
            !(mJson->contains("extraSaves") && (*mJson)["extraSaves"].is_object()) ||
            !(mJson->contains("writeFileSave") && (*mJson)["writeFileSave"].is_boolean()) ||
            !(mJson->contains("useSaveInfo") && (*mJson)["useSaveInfo"].is_boolean()) ||
            !(mJson->contains("randomMusic") && (*mJson)["randomMusic"].is_boolean()) ||
            !(mJson->contains("showBackups") && (*mJson)["showBackups"].is_boolean()) ||
            !(mJson->contains("legalEndpoint") && (*mJson)["legalEndpoint"].is_string()) ||
            !(mJson->contains("patronCode") && (*mJson)["patronCode"].is_string()) ||
            !(mJson->contains("alphaChannel") && (*mJson)["alphaChannel"].is_boolean()) ||
            !(mJson->contains("autoUpdate") && (*mJson)["autoUpdate"].is_boolean()) ||
            !((*mJson)["defaults"].contains("tid") && (*mJson)["defaults"]["tid"].is_number_integer()) ||
            !((*mJson)["defaults"].contains("sid") && (*mJson)["defaults"]["sid"].is_number_integer()) ||
            !((*mJson)["defaults"].contains("ot") && (*mJson)["defaults"]["ot"].is_string()) ||
            !((*mJson)["defaults"].contains("nationality") && (*mJson)["defaults"]["nationality"].is_number_integer()) ||
            !((*mJson)["defaults"].contains("country") && (*mJson)["defaults"]["country"].is_number_integer()) ||
            !((*mJson)["defaults"].contains("region") && (*mJson)["defaults"]["region"].is_number_integer()) ||
            !((*mJson)["defaults"].contains("date") && (*mJson)["defaults"]["date"].is_object()) ||
            !((*mJson)["defaults"]["date"].contains("day") && (*mJson)["defaults"]["date"]["day"].is_number_integer()) ||
            !((*mJson)["defaults"]["date"].contains("month") && (*mJson)["defaults"]["date"]["month"].is_number_integer()) ||
            !((*mJson)["defaults"]["date"].contains("year") && (*mJson)["defaults"]["date"]["year"].is_number_integer()))
        // clang-format on
        {
            loadFromRomfs();
            Gui::warn(i18n::localize((*mJson)["language"], "CONFIGURATION_INCORRECT_FORMAT") + '\n' +
                          i18n::localize((*mJson)["language"], "CONFIGURATION_USING_DEFAULT"),
                (*mJson)["language"]);
            return;
        }

        for (auto& game : (*mJson)["extraSaves"])
        {
            if (game.is_array())
            {
                for (auto& save : game)
                {
                    if (!save.is_string())
                    {
                        loadFromRomfs();
                        Gui::warn(i18n::localize((*mJson)["language"], "CONFIGURATION_INCORRECT_FORMAT") + '\n' +
                                      i18n::localize((*mJson)["language"], "CONFIGURATION_USING_DEFAULT"),
                            (*mJson)["language"]);
                        return;
                    }
                }
            }
            else
            {
                loadFromRomfs();
                Gui::warn(i18n::localize((*mJson)["language"], "CONFIGURATION_INCORRECT_FORMAT") + '\n' +
                              i18n::localize((*mJson)["language"], "CONFIGURATION_USING_DEFAULT"),
                    (*mJson)["language"]);
                return;
            }
        }
    }
}

Configuration::~Configuration() {}

void Configuration::save()
{
    std::string writeData = mJson->dump(2);
    writeData.shrink_to_fit();
    size_t size = writeData.size();

    if (oldSize != size)
    {
        Archive::deleteFile(Archive::data(), "/config.json");
    }

    FSStream stream(Archive::data(), u"/config.json", FS_OPEN_WRITE, oldSize = size);
    stream.write(writeData.data(), size);
    stream.close();
}

std::vector<std::string> Configuration::extraSaves(const std::string& id) const
{
    if ((*mJson)["extraSaves"].count(id) > 0)
    {
        return (*mJson)["extraSaves"][id].get<std::vector<std::string>>();
    }
    return {};
}

void Configuration::extraSaves(const std::string& id, std::vector<std::string>& value)
{
    (*mJson)["extraSaves"][id] = value;
}

void Configuration::loadFromRomfs()
{
    FILE* in = fopen("romfs:/config.json", "rt");
    mJson    = std::make_unique<nlohmann::json>(nlohmann::json::parse(in, nullptr, false));
    fclose(in);

    // load system language
    Language systemLanguage;
    CFGU_GetSystemLanguage((u8*)&systemLanguage);
    u8 countryData[4];
    CFGU_GetConfigInfoBlk2(0x4, 0x000B0000, countryData);
    (*mJson)["defaults"]["country"] = countryData[3];
    (*mJson)["defaults"]["region"]  = countryData[2];
    CFGU_SecureInfoGetRegion(countryData);
    (*mJson)["defaults"]["nationality"] = *countryData;

    switch (u8(systemLanguage))
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
    (*mJson)["language"] = u8(systemLanguage);

    save();
}

Language Configuration::language(void) const
{
    return Language((*mJson)["language"].get<u8>());
}

bool Configuration::autoBackup(void) const
{
    return (*mJson)["autoBackup"];
}

bool Configuration::transferEdit(void) const
{
    return (*mJson)["transferEdit"];
}

bool Configuration::useExtData(void) const
{
    return (*mJson)["useExtData"];
}

u32 Configuration::defaultTID(void) const
{
    return (*mJson)["defaults"]["tid"];
}

u32 Configuration::defaultSID(void) const
{
    return (*mJson)["defaults"]["sid"];
}

std::string Configuration::defaultOT(void) const
{
    return (*mJson)["defaults"]["ot"];
}

int Configuration::nationality(void) const
{
    return (*mJson)["defaults"]["nationality"];
}

int Configuration::day(void) const
{
    return (*mJson)["defaults"]["date"]["day"];
}

int Configuration::month(void) const
{
    return (*mJson)["defaults"]["date"]["month"];
}

int Configuration::year(void) const
{
    return (*mJson)["defaults"]["date"]["year"];
}

bool Configuration::writeFileSave(void) const
{
    return (*mJson)["writeFileSave"];
}

bool Configuration::useSaveInfo(void) const
{
    return (*mJson)["useSaveInfo"];
}

bool Configuration::randomMusic(void) const
{
    return (*mJson)["randomMusic"];
}

int Configuration::defaultRegion(void) const
{
    return (*mJson)["defaults"]["region"];
}

int Configuration::defaultCountry(void) const
{
    return (*mJson)["defaults"]["country"];
}

bool Configuration::showBackups(void) const
{
    return (*mJson)["showBackups"];
}

std::string Configuration::legalEndpoint(void) const
{
    return (*mJson)["legalEndpoint"];
}

std::string Configuration::patronCode(void) const
{
    return (*mJson)["patronCode"];
}

bool Configuration::alphaChannel(void) const
{
    return (*mJson)["alphaChannel"];
}

bool Configuration::autoUpdate(void) const
{
    return (*mJson)["autoUpdate"];
}

void Configuration::language(Language lang)
{
    (*mJson)["language"] = u8(lang);
}

void Configuration::autoBackup(bool backup)
{
    (*mJson)["autoBackup"] = backup;
}

void Configuration::transferEdit(bool edit)
{
    (*mJson)["transferEdit"] = edit;
}

void Configuration::useExtData(bool use)
{
    (*mJson)["useExtData"] = use;
}

void Configuration::defaultTID(u32 tid)
{
    (*mJson)["defaults"]["tid"] = tid;
}

void Configuration::defaultSID(u32 sid)
{
    (*mJson)["defaults"]["sid"] = sid;
}

void Configuration::defaultOT(const std::string& ot)
{
    (*mJson)["defaults"]["ot"] = ot;
}

void Configuration::nationality(int nation)
{
    (*mJson)["defaults"]["nationality"] = nation;
}

void Configuration::day(int day)
{
    (*mJson)["defaults"]["date"]["day"] = day;
}

void Configuration::month(int month)
{
    (*mJson)["defaults"]["date"]["month"] = month;
}

void Configuration::year(int year)
{
    (*mJson)["defaults"]["date"]["year"] = year;
}

void Configuration::writeFileSave(bool write)
{
    (*mJson)["writeFileSave"] = write;
}

void Configuration::useSaveInfo(bool saveInfo)
{
    (*mJson)["useSaveInfo"] = saveInfo;
}

void Configuration::randomMusic(bool random)
{
    (*mJson)["randomMusic"] = random;
}

void Configuration::defaultRegion(u8 value)
{
    (*mJson)["defaults"]["region"] = value;
}

void Configuration::defaultCountry(u8 value)
{
    (*mJson)["defaults"]["country"] = value;
}

void Configuration::showBackups(bool value)
{
    (*mJson)["showBackups"] = value;
}

void Configuration::legalEndpoint(const std::string& value)
{
    (*mJson)["legalEndpoint"] = value;
}

void Configuration::patronCode(const std::string& value)
{
    (*mJson)["patronCode"] = value;
}

void Configuration::alphaChannel(bool value)
{
    (*mJson)["alphaChannel"] = value;
}

void Configuration::autoUpdate(bool value)
{
    (*mJson)["autoUpdate"] = value;
}
