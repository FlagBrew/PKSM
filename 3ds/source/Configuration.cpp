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

#include "Configuration.hpp"
#include "Archive.hpp"
#include "gui.hpp"
#include "nlohmann/json.hpp"
#include "website.h"

Configuration::Configuration()
{
    auto stream = Archive::data().file(u"/config.json", FS_OPEN_READ);

    if (!stream)
    {
        loadFromRomfs();
    }
    else
    {
        oldSize           = stream->size();
        char* jsonData    = new char[oldSize + 1];
        jsonData[oldSize] = '\0';
        stream->read(jsonData, oldSize);
        stream->close();
        mJson = std::make_unique<nlohmann::json>(nlohmann::json::parse(jsonData, nullptr, false));
        delete[] jsonData;

        if (!mJson->is_object())
        {
            loadFromRomfs();
            Gui::warn(i18n::localize((*mJson)["language"], "CONFIGURATION_FILE_CORRUPTED_1") +
                          '\n' +
                          i18n::localize((*mJson)["language"], "CONFIGURATION_USING_DEFAULT"),
                (*mJson)["language"]);
            return;
        }

        if (!(mJson->contains("version") && (*mJson)["version"].is_number_integer()))
        {
            loadFromRomfs();
            Gui::warn(i18n::localize((*mJson)["language"], "CONFIGURATION_INCORRECT_FORMAT") +
                          '\n' +
                          i18n::localize((*mJson)["language"], "CONFIGURATION_USING_DEFAULT"),
                (*mJson)["language"]);
            return;
        }

        if (!(mJson->contains("language") && (*mJson)["language"].is_number_integer()))
        {
            loadFromRomfs();
            Gui::warn(i18n::localize((*mJson)["language"], "CONFIGURATION_INCORRECT_FORMAT") +
                          '\n' +
                          i18n::localize((*mJson)["language"], "CONFIGURATION_USING_DEFAULT"),
                (*mJson)["language"]);
            return;
        }

        if ((*mJson)["version"].get<int>() != CURRENT_VERSION)
        {
            if ((*mJson)["version"].get<int>() > CURRENT_VERSION)
            {
                loadFromRomfs();
                Gui::warn(i18n::localize((*mJson)["language"], "THE_FUCK") + '\n' +
                              i18n::localize((*mJson)["language"], "DO_NOT_DOWNGRADE"),
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
                    Gui::warn(
                        i18n::localize((*mJson)["language"], "CONFIGURATION_INCORRECT_FORMAT") +
                            '\n' +
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
                            Gui::warn(i18n::localize(
                                          (*mJson)["language"], "CONFIGURATION_INCORRECT_FORMAT") +
                                          '\n' +
                                          i18n::localize(
                                              (*mJson)["language"], "CONFIGURATION_USING_DEFAULT"),
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
                    !((*mJson)["defaults"].contains("pid") &&
                        (*mJson)["defaults"]["pid"].is_number_integer()))
                {
                    loadFromRomfs();
                    Gui::warn(
                        i18n::localize((*mJson)["language"], "CONFIGURATION_INCORRECT_FORMAT") +
                            '\n' +
                            i18n::localize((*mJson)["language"], "CONFIGURATION_USING_DEFAULT"),
                        (*mJson)["language"]);
                    return;
                }
                (*mJson)["defaults"]["tid"] = (*mJson)["defaults"]["pid"];
                (*mJson)["defaults"].erase("pid");
                (*mJson)["legalEndpoint"] = WEBSITE_URL "pksm/legality/check";
            }
            if ((*mJson)["version"].get<int>() < 8)
            {
                (*mJson)["patronCode"]   = "";
                (*mJson)["alphaChannel"] = false;
                (*mJson)["autoUpdate"]   = true;
            }
            if ((*mJson)["version"].get<int>() < 9)
            {
                if (!(mJson->contains("defaults") && (*mJson)["defaults"].is_object()) ||
                    !((*mJson)["defaults"].contains("sid") &&
                        (*mJson)["defaults"]["sid"].is_number_integer()) ||
                    !((*mJson)["defaults"].contains("tid") &&
                        (*mJson)["defaults"]["tid"].is_number_integer()) ||
                    !((*mJson)["defaults"].contains("ot") &&
                        (*mJson)["defaults"]["ot"].is_string()) ||
                    !((*mJson)["defaults"].contains("nationality") &&
                        (*mJson)["defaults"]["nationality"].is_number_integer()) ||
                    !((*mJson)["defaults"].contains("country") &&
                        (*mJson)["defaults"]["country"].is_number_integer()) ||
                    !((*mJson)["defaults"].contains("region") &&
                        (*mJson)["defaults"]["region"].is_number_integer()))
                {
                    loadFromRomfs();
                    Gui::warn(
                        i18n::localize((*mJson)["language"], "CONFIGURATION_INCORRECT_FORMAT") +
                            '\n' +
                            i18n::localize((*mJson)["language"], "CONFIGURATION_USING_DEFAULT"),
                        (*mJson)["language"]);
                    return;
                }
                (*mJson)["defaults"].erase("sid");
                (*mJson)["defaults"].erase("tid");
                (*mJson)["defaults"].erase("ot");
                (*mJson)["defaults"].erase("nationality");
                (*mJson)["defaults"].erase("country");
                (*mJson)["defaults"].erase("region");
            }
            if ((*mJson)["version"].get<int>() < 10)
            {
                mJson->erase("legalEndpoint");
                (*mJson)["apiUrl"]    = "";
                (*mJson)["useApiUrl"] = false;
            }
            if ((*mJson)["version"].get<int>() < 11)
            {
                (*mJson)["titles"] = nlohmann::json::object();

                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::R)]  = "0x100";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::S)]  = "0x200";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::E)]  = "0x300";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::FR)] = "0x400";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::LG)] = "0x500";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::X)] =
                    "0x0004000000055D00";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::Y)] =
                    "0x0004000000055E00";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::OR)] =
                    "0x000400000011C400";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::AS)] =
                    "0x000400000011C500";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::SN)] =
                    "0x0004000000164800";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::MN)] =
                    "0x0004000000175E00";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::US)] =
                    "0x00040000001B5000";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::UM)] =
                    "0x00040000001B5100";
            }
            if ((*mJson)["version"].get<int>() < 12)
            {
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::RD)] =
                    "0x0004000000171000";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::GN)] =
                    "0x0004000000170D00";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::BU)] =
                    "0x0004000000171100";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::YW)] =
                    "0x0004000000171200";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::GD)] =
                    "0x0004000000172600";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::SV)] =
                    "0x0004000000172700";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::C)] =
                    "0x0004000000172800";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::GP)] =
                    "0x600";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::GE)] =
                    "0x700";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::SW)] =
                    "0x800";
                (*mJson)["titles"][std::to_string((u32)pksm::GameVersion::SH)] =
                    "0x900";
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
            !(mJson->contains("apiUrl") && (*mJson)["apiUrl"].is_string()) ||
            !(mJson->contains("useApiUrl") && (*mJson)["useApiUrl"].is_boolean()) ||
            !(mJson->contains("patronCode") && (*mJson)["patronCode"].is_string()) ||
            !(mJson->contains("alphaChannel") && (*mJson)["alphaChannel"].is_boolean()) ||
            !(mJson->contains("autoUpdate") && (*mJson)["autoUpdate"].is_boolean()) ||
            !(mJson->contains("titles") && (*mJson)["titles"].is_object()) ||
            !((*mJson)["defaults"].contains("date") && (*mJson)["defaults"]["date"].is_object()) ||
            !((*mJson)["defaults"]["date"].contains("day") && (*mJson)["defaults"]["date"]["day"].is_number_integer()) ||
            !((*mJson)["defaults"]["date"].contains("month") && (*mJson)["defaults"]["date"]["month"].is_number_integer()) ||
            !((*mJson)["defaults"]["date"].contains("year") && (*mJson)["defaults"]["date"]["year"].is_number_integer()))
        // clang-format on
        {
            loadFromRomfs();
            Gui::warn(i18n::localize((*mJson)["language"], "CONFIGURATION_INCORRECT_FORMAT") +
                          '\n' +
                          i18n::localize((*mJson)["language"], "CONFIGURATION_USING_DEFAULT"),
                (*mJson)["language"]);
            return;
        }

        for (const auto& game : (*mJson)["extraSaves"])
        {
            if (game.is_array())
            {
                for (const auto& save : game)
                {
                    if (!save.is_string())
                    {
                        loadFromRomfs();
                        Gui::warn(
                            i18n::localize((*mJson)["language"], "CONFIGURATION_INCORRECT_FORMAT") +
                                '\n' +
                                i18n::localize((*mJson)["language"], "CONFIGURATION_USING_DEFAULT"),
                            (*mJson)["language"]);
                        return;
                    }
                }
            }
            else
            {
                loadFromRomfs();
                Gui::warn(i18n::localize((*mJson)["language"], "CONFIGURATION_INCORRECT_FORMAT") +
                              '\n' +
                              i18n::localize((*mJson)["language"], "CONFIGURATION_USING_DEFAULT"),
                    (*mJson)["language"]);
                return;
            }
        }

        for (const auto& version : (*mJson)["titles"])
        {
            if (!version.is_string())
            {
                loadFromRomfs();
                Gui::warn(i18n::localize((*mJson)["language"], "CONFIGURATION_INCORRECT_FORMAT") +
                              '\n' +
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

    Archive::data().deleteFile("/config.json");
    Archive::data().createFile(u"/config.json", 0, oldSize = size);
    auto stream = Archive::data().file(u"/config.json", FS_OPEN_WRITE, oldSize = size);
    if (stream)
    {
        stream->write(writeData.data(), size);
        stream->close();
    }
}

void Configuration::loadFromRomfs()
{
    FILE* in = fopen("romfs:/config.json", "rt");
    mJson    = std::make_unique<nlohmann::json>(nlohmann::json::parse(in, nullptr, false));
    fclose(in);

    // load system language
    pksm::Language systemLanguage;
    CFGU_GetSystemLanguage((u8*)&systemLanguage);

    switch (u8(systemLanguage))
    {
        case CFG_LANGUAGE_JP:
            systemLanguage = pksm::Language::JPN;
            break;
        case CFG_LANGUAGE_EN:
            systemLanguage = pksm::Language::ENG;
            break;
        case CFG_LANGUAGE_FR:
            systemLanguage = pksm::Language::FRE;
            break;
        case CFG_LANGUAGE_DE:
            systemLanguage = pksm::Language::GER;
            break;
        case CFG_LANGUAGE_IT:
            systemLanguage = pksm::Language::ITA;
            break;
        case CFG_LANGUAGE_ES:
            systemLanguage = pksm::Language::SPA;
            break;
        case CFG_LANGUAGE_ZH:
            systemLanguage = pksm::Language::CHS;
            break;
        case CFG_LANGUAGE_KO:
            systemLanguage = pksm::Language::KOR;
            break;
        case CFG_LANGUAGE_NL:
            systemLanguage = pksm::Language::NL;
            break;
        case CFG_LANGUAGE_PT:
            systemLanguage = pksm::Language::PT;
            break;
        case CFG_LANGUAGE_RU:
            systemLanguage = pksm::Language::RU;
            break;
        case CFG_LANGUAGE_TW:
            systemLanguage = pksm::Language::CHT;
            break;
        default:
            systemLanguage = pksm::Language::ENG;
            break;
    }
    (*mJson)["language"] = u8(systemLanguage);

    save();
}

pksm::Language Configuration::language(void) const
{
    return pksm::Language((*mJson)["language"].get<u8>());
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

bool Configuration::showBackups(void) const
{
    return (*mJson)["showBackups"];
}

std::string Configuration::apiUrl(void) const
{
    return (*mJson)["apiUrl"];
}

bool Configuration::useApiUrl(void) const
{
    return (*mJson)["useApiUrl"];
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

std::vector<std::string> Configuration::extraSaves(const std::string& id) const
{
    if ((*mJson)["extraSaves"].count(id) > 0)
    {
        return (*mJson)["extraSaves"][id].get<std::vector<std::string>>();
    }
    return {};
}

std::string Configuration::titleId(pksm::GameVersion version) const
{
    std::string v = std::to_string((u32)version);
    if ((*mJson)["titles"].count(v) > 0)
    {
        return (*mJson)["titles"][v].get<std::string>();
    }
    return "";
}

void Configuration::language(pksm::Language lang)
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

void Configuration::showBackups(bool value)
{
    (*mJson)["showBackups"] = value;
}

void Configuration::apiUrl(const std::string& value)
{
    (*mJson)["apiUrl"] = value;
}

void Configuration::useApiUrl(bool value)
{
    (*mJson)["useApiUrl"] = value;
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

void Configuration::extraSaves(const std::string& id, const std::vector<std::string>& value)
{
    (*mJson)["extraSaves"][id] = value;
}

void Configuration::titleId(pksm::GameVersion version, const std::string& id)
{
    (*mJson)["titles"][std::to_string((u32)version)] = id;
}
