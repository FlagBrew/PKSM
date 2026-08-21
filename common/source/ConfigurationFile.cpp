/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2026 Bernardo Giordano, Admiral Fish, piepie62
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

#include "ConfigurationFile.hpp"
#include "nlohmann/json.hpp"
#include <cstdint>
#include <limits>
#include <optional>

namespace
{
    using json = nlohmann::json;

    std::optional<int> integer(const json& value)
    {
        if (value.is_number_unsigned())
        {
            const auto number = value.get<std::uint64_t>();
            if (number <= std::uint64_t(std::numeric_limits<int>::max()))
            {
                return int(number);
            }
        }
        else if (value.is_number_integer())
        {
            const auto number = value.get<std::int64_t>();
            if (number >= std::int64_t(std::numeric_limits<int>::min()) &&
                number <= std::int64_t(std::numeric_limits<int>::max()))
            {
                return int(number);
            }
        }
        return std::nullopt;
    }

    std::optional<int> integerAt(const json& object, std::string_view key)
    {
        const auto value = object.find(key);
        return value == object.end() ? std::nullopt : integer(*value);
    }

    bool hasType(const json& object, std::string_view key, json::value_t type)
    {
        const auto value = object.find(key);
        return value != object.end() && value->type() == type;
    }

    bool validStringArray(const json& value)
    {
        if (!value.is_array())
        {
            return false;
        }
        for (const auto& item : value)
        {
            if (!item.is_string())
            {
                return false;
            }
        }
        return true;
    }

    void addPreSwitchTitles(json& titles, bool legacyMigration)
    {
        // The old v10->v11 migration assigned the first two GBA IDs by GameVersion::R/S, while
        // the bundled current file has always stored them under numeric keys 1/2. Preserve both
        // behaviours so this refactor does not rewrite existing or historically migrated IDs.
        titles["1"]  = legacyMigration ? "0x200" : "0x100";
        titles["2"]  = legacyMigration ? "0x100" : "0x200";
        titles["3"]  = "0x300";
        titles["4"]  = "0x400";
        titles["5"]  = "0x500";
        titles["24"] = "0x0004000000055D00";
        titles["25"] = "0x0004000000055E00";
        titles["26"] = "0x000400000011C500";
        titles["27"] = "0x000400000011C400";
        titles["30"] = "0x0004000000164800";
        titles["31"] = "0x0004000000175E00";
        titles["32"] = "0x00040000001B5000";
        titles["33"] = "0x00040000001B5100";
    }

    void addVirtualConsoleTitles(json& titles)
    {
        titles["35"] = "0x0004000000171000";
        titles["36"] = "0x0004000000170D00";
        titles["37"] = "0x0004000000171100";
        titles["38"] = "0x0004000000171200";
        titles["39"] = "0x0004000000172600";
        titles["40"] = "0x0004000000172700";
        titles["41"] = "0x0004000000172800";
        titles["42"] = "0x600";
        titles["43"] = "0x700";
        titles["44"] = "0x800";
        titles["45"] = "0x900";
    }

    bool migrate(json& config, int version)
    {
        if (version < 2)
        {
            config["useSaveInfo"] = false;
        }
        if (version < 3)
        {
            config["randomMusic"] = false;
        }
        if (version < 4)
        {
            if (!hasType(config, "defaults", json::value_t::object))
            {
                return false;
            }
            // These system-derived fields were introduced in v4 and removed again in v9. Their
            // values cannot affect current settings, but supplying typed placeholders lets the
            // intermediate v9 migration validate genuine v1-v3 files.
            config["defaults"]["country"]     = 0;
            config["defaults"]["region"]      = 0;
            config["defaults"]["nationality"] = 0;
        }
        if (version < 5)
        {
            // Version 4 stored each game's paths below a {folders, files} object. Validate the
            // container before iterating it; the old guard was inverted and iterated only when
            // extraSaves was absent or not an object.
            if (!hasType(config, "extraSaves", json::value_t::object))
            {
                return false;
            }
            for (auto& [id, game] : config["extraSaves"].items())
            {
                (void)id;
                if (!game.is_object())
                {
                    return false;
                }
                const auto files = game.find("files");
                json migratedFiles =
                    files != game.end() && files->is_array() ? *files : json::array();
                game = std::move(migratedFiles);
            }
        }
        if (version < 6)
        {
            config.erase("storageSize");
            config["showBackups"] = false;
        }
        if (version < 7)
        {
            if (!hasType(config, "defaults", json::value_t::object) ||
                !integerAt(config["defaults"], "pid"))
            {
                return false;
            }
            config["defaults"]["tid"] = config["defaults"]["pid"];
            config["defaults"].erase("pid");
            config["legalEndpoint"] = "";
        }
        if (version < 8)
        {
            config["autoUpdate"] = true;
        }
        if (version < 9)
        {
            if (!hasType(config, "defaults", json::value_t::object))
            {
                return false;
            }
            auto& defaults = config["defaults"];
            if (!integerAt(defaults, "sid") || !integerAt(defaults, "tid") ||
                !hasType(defaults, "ot", json::value_t::string) ||
                !integerAt(defaults, "nationality") || !integerAt(defaults, "country") ||
                !integerAt(defaults, "region"))
            {
                return false;
            }
            defaults.erase("sid");
            defaults.erase("tid");
            defaults.erase("ot");
            defaults.erase("nationality");
            defaults.erase("country");
            defaults.erase("region");
        }
        if (version < 10)
        {
            config.erase("legalEndpoint");
            config["apiUrl"] = "";
        }
        if (version < 11)
        {
            config["titles"] = json::object();
            addPreSwitchTitles(config["titles"], true);
        }
        if (version < 12)
        {
            if (!hasType(config, "titles", json::value_t::object))
            {
                return false;
            }
            addVirtualConsoleTitles(config["titles"]);
        }
        if (version < 13)
        {
            config["cloudPageJump"] = 1;
        }
        config["version"] = ConfigurationFile::CURRENT_VERSION;
        return true;
    }

    bool validate(const json& config)
    {
        if (!integerAt(config, "version") ||
            *integerAt(config, "version") != ConfigurationFile::CURRENT_VERSION ||
            !integerAt(config, "language") ||
            !hasType(config, "autoBackup", json::value_t::boolean) ||
            !hasType(config, "transferEdit", json::value_t::boolean) ||
            !hasType(config, "useExtData", json::value_t::boolean) ||
            !hasType(config, "defaults", json::value_t::object) ||
            !hasType(config, "extraSaves", json::value_t::object) ||
            !hasType(config, "writeFileSave", json::value_t::boolean) ||
            !hasType(config, "useSaveInfo", json::value_t::boolean) ||
            !hasType(config, "randomMusic", json::value_t::boolean) ||
            !hasType(config, "showBackups", json::value_t::boolean) ||
            !hasType(config, "apiUrl", json::value_t::string) ||
            !hasType(config, "autoUpdate", json::value_t::boolean) ||
            !integerAt(config, "cloudPageJump") ||
            !hasType(config, "titles", json::value_t::object))
        {
            return false;
        }

        const int language = *integerAt(config, "language");
        if (language < 0 || language > std::numeric_limits<u8>::max())
        {
            return false;
        }
        switch (pksm::Language(u8(language)))
        {
            case pksm::Language::JPN:
            case pksm::Language::ENG:
            case pksm::Language::FRE:
            case pksm::Language::ITA:
            case pksm::Language::GER:
            case pksm::Language::SPA:
            case pksm::Language::KOR:
            case pksm::Language::CHS:
            case pksm::Language::CHT:
            case pksm::Language::NL:
            case pksm::Language::PT:
            case pksm::Language::RU:
            case pksm::Language::RO:
                break;
            case pksm::Language::None:
            case pksm::Language::UNUSED:
            case pksm::Language::INVALID:
            default:
                return false;
        }

        const auto& defaults = config["defaults"];
        if (!hasType(defaults, "date", json::value_t::object))
        {
            return false;
        }
        const auto& date = defaults["date"];
        if (!integerAt(date, "day") || !integerAt(date, "month") || !integerAt(date, "year"))
        {
            return false;
        }
        const int day   = *integerAt(date, "day");
        const int month = *integerAt(date, "month");
        const int year  = *integerAt(date, "year");
        if (day < 0 || day > 31 || month < 0 || month > 12 || year < 0 || year > 9999)
        {
            return false;
        }

        for (const auto& [id, saves] : config["extraSaves"].items())
        {
            (void)id;
            if (!validStringArray(saves))
            {
                return false;
            }
        }
        for (const auto& [version, title] : config["titles"].items())
        {
            (void)version;
            if (!title.is_string())
            {
                return false;
            }
        }
        return true;
    }

    ConfigurationFile::Settings toSettings(const json& config)
    {
        ConfigurationFile::Settings settings;
        settings.language      = pksm::Language(u8(*integerAt(config, "language")));
        settings.autoBackup    = config["autoBackup"].get<bool>();
        settings.transferEdit  = config["transferEdit"].get<bool>();
        settings.useExtData    = config["useExtData"].get<bool>();
        settings.day           = *integerAt(config["defaults"]["date"], "day");
        settings.month         = *integerAt(config["defaults"]["date"], "month");
        settings.year          = *integerAt(config["defaults"]["date"], "year");
        settings.writeFileSave = config["writeFileSave"].get<bool>();
        settings.useSaveInfo   = config["useSaveInfo"].get<bool>();
        settings.randomMusic   = config["randomMusic"].get<bool>();
        settings.showBackups   = config["showBackups"].get<bool>();
        settings.apiUrl        = config["apiUrl"].get<std::string>();
        settings.autoUpdate    = config["autoUpdate"].get<bool>();
        settings.cloudPageJump = *integerAt(config, "cloudPageJump");

        for (const auto& [id, saves] : config["extraSaves"].items())
        {
            settings.extraSaves.emplace(id, saves.get<std::vector<std::string>>());
        }
        for (const auto& [version, title] : config["titles"].items())
        {
            settings.titles.emplace(version, title.get<std::string>());
        }
        return settings;
    }
}

ConfigurationFile::Settings ConfigurationFile::defaultSettings()
{
    Settings settings;
    json titles = json::object();
    addPreSwitchTitles(titles, false);
    addVirtualConsoleTitles(titles);
    for (const auto& [version, title] : titles.items())
    {
        settings.titles.emplace(version, title.get<std::string>());
    }
    return settings;
}

std::variant<ConfigurationFile::Contents, ConfigurationFile::Error> ConfigurationFile::parse(
    std::string_view data)
{
    json config = json::parse(data.begin(), data.end(), nullptr, false);
    if (config.is_discarded() || !config.is_object())
    {
        return Error::Corrupt;
    }

    const auto sourceVersion = integerAt(config, "version");
    if (!sourceVersion || *sourceVersion < 1 || !integerAt(config, "language"))
    {
        return Error::BadFormat;
    }
    if (*sourceVersion > CURRENT_VERSION)
    {
        return Error::FromNewerVersion;
    }

    if (*sourceVersion < CURRENT_VERSION && !migrate(config, *sourceVersion))
    {
        return Error::BadFormat;
    }
    if (!validate(config))
    {
        return Error::BadFormat;
    }

    Contents contents;
    contents.settings      = toSettings(config);
    contents.sourceVersion = *sourceVersion;
    contents.changed       = *sourceVersion != CURRENT_VERSION;
    if (contents.settings.cloudPageJump < 1)
    {
        contents.settings.cloudPageJump = 1;
        contents.changed                = true;
    }
    return contents;
}

std::string ConfigurationFile::serialize(const Settings& settings)
{
    json extraSaves = json::object();
    for (const auto& [id, saves] : settings.extraSaves)
    {
        extraSaves[id] = saves;
    }

    json titles = json::object();
    for (const auto& [version, title] : settings.titles)
    {
        titles[version] = title;
    }

    const json config = {
        {"version",       CURRENT_VERSION                                        },
        {"language",      u8(settings.language)                                  },
        {"autoBackup",    settings.autoBackup                                    },
        {"transferEdit",  settings.transferEdit                                  },
        {"useExtData",    settings.useExtData                                    },
        {"defaults",      {{"date", {{"day", settings.day}, {"month", settings.month},
                                   {"year", settings.year}}}}},
        {"extraSaves",    std::move(extraSaves)                                  },
        {"titles",        std::move(titles)                                      },
        {"writeFileSave", settings.writeFileSave                                 },
        {"useSaveInfo",   settings.useSaveInfo                                   },
        {"randomMusic",   settings.randomMusic                                   },
        {"showBackups",   settings.showBackups                                   },
        {"apiUrl",        settings.apiUrl                                        },
        {"autoUpdate",    settings.autoUpdate                                    },
        {"cloudPageJump", settings.cloudPageJump                                 },
    };
    return config.dump(2);
}
