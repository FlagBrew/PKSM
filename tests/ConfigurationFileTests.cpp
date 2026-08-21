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

// JSON fixtures in, immutable settings values or typed errors out. Runs on the host: no 3DS,
// Archive, or GUI dependencies.

#include "ConfigurationFile.hpp"
#include "nlohmann/json.hpp"
#include <cstdio>
#include <string>

namespace
{
    int failures = 0;

    void check(bool condition, const char* what)
    {
        if (!condition)
        {
            failures++;
            std::printf("FAIL: %s\n", what);
        }
    }

    const ConfigurationFile::Contents* contentsOf(
        const std::variant<ConfigurationFile::Contents, ConfigurationFile::Error>& result)
    {
        return std::get_if<ConfigurationFile::Contents>(&result);
    }

    bool isError(const std::variant<ConfigurationFile::Contents, ConfigurationFile::Error>& result,
        ConfigurationFile::Error expected)
    {
        const auto* error = std::get_if<ConfigurationFile::Error>(&result);
        return error && *error == expected;
    }

    std::string fixture()
    {
        FILE* file = std::fopen("../assets/romfs/config.json", "rb");
        if (!file)
        {
            return {};
        }
        std::fseek(file, 0, SEEK_END);
        const long size = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);
        std::string data(size_t(size), '\0');
        data.resize(std::fread(data.data(), 1, data.size(), file));
        std::fclose(file);
        return data;
    }

    void currentFileParsesOnceIntoSettings()
    {
        const auto result = ConfigurationFile::parse(fixture());
        const auto* out   = contentsOf(result);
        check(out != nullptr, "the bundled current configuration parses");
        if (!out)
        {
            return;
        }

        check(!out->changed, "the current configuration needs no rewrite");
        check(out->settings.language == pksm::Language::ENG, "language is typed");
        check(out->settings.autoBackup, "boolean settings are parsed");
        check(out->settings.day == 1 && out->settings.month == 1 && out->settings.year == 2000,
            "the default date is parsed");
        check(out->settings.titles.at("24") == "0x0004000000055D00",
            "title IDs are parsed into the settings value");
    }

    void settingsRoundTripWithoutJsonCallers()
    {
        auto settings                  = ConfigurationFile::defaultSettings();
        settings.language              = pksm::Language::ITA;
        settings.apiUrl                = "https://example.test/";
        settings.extraSaves["example"] = {"/one", "/two"};
        settings.titles["24"]          = "0x1234";

        const auto result = ConfigurationFile::parse(ConfigurationFile::serialize(settings));
        const auto* out   = contentsOf(result);
        check(out != nullptr, "serialized settings parse again");
        if (!out)
        {
            return;
        }
        check(out->settings.language == pksm::Language::ITA, "language round-trips");
        check(out->settings.apiUrl == "https://example.test/", "strings round-trip");
        check(out->settings.extraSaves.at("example").size() == 2, "string arrays round-trip");
        check(out->settings.titles.at("24") == "0x1234", "edited title IDs round-trip");
    }

    nlohmann::json versionFourFixture()
    {
        auto old = nlohmann::json::parse(
            ConfigurationFile::serialize(ConfigurationFile::defaultSettings()));
        old["version"] = 4;
        old.erase("showBackups");
        old.erase("apiUrl");
        old.erase("autoUpdate");
        old.erase("cloudPageJump");
        old.erase("titles");
        old["storageSize"]             = 123;
        old["defaults"]["sid"]         = 1;
        old["defaults"]["pid"]         = 2;
        old["defaults"]["ot"]          = "PKSM";
        old["defaults"]["nationality"] = 3;
        old["defaults"]["country"]     = 4;
        old["defaults"]["region"]      = 5;
        old["extraSaves"]              = {
            {"game", {{"folders", {"ignored"}}, {"files", {"/save-a", "/save-b"}}}}
        };
        return old;
    }

    nlohmann::json historicalFixture(int version)
    {
        auto old = nlohmann::json::parse(
            ConfigurationFile::serialize(ConfigurationFile::defaultSettings()));
        old["version"] = version;

        if (version < 13)
        {
            old.erase("cloudPageJump");
        }
        if (version < 11)
        {
            old.erase("titles");
        }
        else if (version < 12)
        {
            for (const char* id :
                {"35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45"})
            {
                old["titles"].erase(id);
            }
        }
        if (version < 10)
        {
            old.erase("apiUrl");
            old["legalEndpoint"] = "https://example.test/legality";
        }
        if (version < 9)
        {
            old["defaults"]["sid"]         = 1;
            old["defaults"]["tid"]         = 2;
            old["defaults"]["ot"]          = "PKSM";
            old["defaults"]["nationality"] = 3;
            old["defaults"]["country"]     = 4;
            old["defaults"]["region"]      = 5;
        }
        if (version < 8)
        {
            old.erase("autoUpdate");
        }
        if (version < 7)
        {
            old["defaults"]["pid"] = old["defaults"]["tid"];
            old["defaults"].erase("tid");
        }
        if (version < 6)
        {
            old.erase("showBackups");
            old["storageSize"] = 150;
        }
        if (version < 5)
        {
            old["extraSaves"] = {
                {"game", {{"folders", {"ignored"}}, {"files", {"/save-a", "/save-b"}}}}
            };
        }
        else
        {
            old["extraSaves"] = {
                {"game", {"/save-a", "/save-b"}}
            };
        }
        if (version < 4)
        {
            old["defaults"].erase("country");
            old["defaults"].erase("region");
        }
        if (version < 3)
        {
            old.erase("randomMusic");
        }
        if (version < 2)
        {
            old.erase("useSaveInfo");
        }
        return old;
    }

    void versionFourMigratesItsExtraSaves()
    {
        const auto result = ConfigurationFile::parse(versionFourFixture().dump());
        const auto* out   = contentsOf(result);
        check(out != nullptr, "a well-formed v4 configuration migrates");
        if (!out)
        {
            return;
        }

        check(out->changed && out->sourceVersion == 4, "migration requests a rewrite");
        check(
            out->settings.extraSaves.at("game") == std::vector<std::string>({"/save-a", "/save-b"}),
            "v4's nested files array becomes the current extra-save array");
        check(!out->settings.showBackups && out->settings.autoUpdate,
            "later-version defaults are supplied during migration");
        check(out->settings.cloudPageJump == 1, "the v13 page-jump default is supplied");
        check(out->settings.titles.contains("45"), "the title map is migrated through v12");
    }

    void malformedVersionFourContainerIsRejected()
    {
        auto old          = versionFourFixture();
        old["extraSaves"] = nlohmann::json::array();
        check(isError(ConfigurationFile::parse(old.dump()), ConfigurationFile::Error::BadFormat),
            "v4 extraSaves must be an object before migration iterates it");

        old                       = versionFourFixture();
        old["extraSaves"]["game"] = nlohmann::json::array();
        check(isError(ConfigurationFile::parse(old.dump()), ConfigurationFile::Error::BadFormat),
            "each v4 extra-save entry must use the old object schema");
    }

    void everyHistoricalVersionMigrates()
    {
        for (int version = 1; version < ConfigurationFile::CURRENT_VERSION; version++)
        {
            const auto result = ConfigurationFile::parse(historicalFixture(version).dump());
            const auto* out   = contentsOf(result);
            if (!out)
            {
                failures++;
                std::printf("FAIL: historical configuration v%d migrates\n", version);
                continue;
            }
            check(out->sourceVersion == version && out->changed,
                "historical migration records its source version");
            check(out->settings.extraSaves.at("game").size() == 2,
                "historical migration preserves extra saves");
            check(out->settings.titles.contains("45"),
                "historical migration supplies all current title IDs");
            if (version < 11)
            {
                check(out->settings.titles.at("1") == "0x200" &&
                          out->settings.titles.at("2") == "0x100",
                    "the historical v11 migration preserves its GBA title-ID mapping");
            }
        }
    }

    void invalidFilesHaveTypedErrors()
    {
        check(isError(ConfigurationFile::parse("{"), ConfigurationFile::Error::Corrupt),
            "invalid JSON is corrupt");
        check(isError(ConfigurationFile::parse("[]"), ConfigurationFile::Error::Corrupt),
            "a non-object root is corrupt");

        auto config       = nlohmann::json::parse(fixture());
        config["version"] = ConfigurationFile::CURRENT_VERSION + 1;
        check(isError(ConfigurationFile::parse(config.dump()),
                  ConfigurationFile::Error::FromNewerVersion),
            "a newer configuration version is refused");

        config = nlohmann::json::parse(fixture());
        config.erase("randomMusic");
        check(isError(ConfigurationFile::parse(config.dump()), ConfigurationFile::Error::BadFormat),
            "a missing required setting is bad format");

        config             = nlohmann::json::parse(fixture());
        config["language"] = 256;
        check(isError(ConfigurationFile::parse(config.dump()), ConfigurationFile::Error::BadFormat),
            "a language that cannot fit its enum is bad format");

        config                              = nlohmann::json::parse(fixture());
        config["defaults"]["date"]["month"] = 13;
        check(isError(ConfigurationFile::parse(config.dump()), ConfigurationFile::Error::BadFormat),
            "an out-of-range date is bad format");

        config           = historicalFixture(11);
        config["titles"] = "not an object";
        check(isError(ConfigurationFile::parse(config.dump()), ConfigurationFile::Error::BadFormat),
            "a malformed v11 title map is rejected before migration");
    }

    void invalidPageJumpIsNormalized()
    {
        auto config             = nlohmann::json::parse(fixture());
        config["cloudPageJump"] = 0;
        const auto result       = ConfigurationFile::parse(config.dump());
        const auto* out         = contentsOf(result);
        check(out && out->changed, "normalization requests a rewrite");
        check(out && out->settings.cloudPageJump == 1, "page jumps are always positive");
    }
}

int main()
{
    currentFileParsesOnceIntoSettings();
    settingsRoundTripWithoutJsonCallers();
    versionFourMigratesItsExtraSaves();
    malformedVersionFourContainerIsRejected();
    everyHistoricalVersionMigrates();
    invalidFilesHaveTypedErrors();
    invalidPageJumpIsNormalized();

    if (failures == 0)
    {
        std::printf("ConfigurationFile: all checks passed\n");
        return 0;
    }
    std::printf("ConfigurationFile: %d check(s) failed\n", failures);
    return 1;
}
