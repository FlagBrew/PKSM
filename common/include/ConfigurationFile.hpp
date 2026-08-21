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

#ifndef CONFIGURATIONFILE_HPP
#define CONFIGURATIONFILE_HPP

#include "enums/Language.hpp"
#include <map>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

// The configuration file format, and nothing else: JSON in, either a fully migrated and validated
// settings value or a typed error out. No I/O, libctru, or GUI is involved, so callers never need
// to know about the JSON representation and every schema rule is testable off-device.
namespace ConfigurationFile
{
    inline constexpr int CURRENT_VERSION = 13;

    struct Settings
    {
        pksm::Language language = pksm::Language::ENG;
        bool autoBackup         = true;
        bool transferEdit       = true;
        bool useExtData         = true;

        int day   = 1;
        int month = 1;
        int year  = 2000;

        std::map<std::string, std::vector<std::string>> extraSaves;
        std::map<std::string, std::string> titles;

        bool writeFileSave = false;
        bool useSaveInfo   = false;
        bool randomMusic   = false;
        bool showBackups   = false;
        std::string apiUrl;
        bool autoUpdate   = true;
        int cloudPageJump = 1;
    };

    enum class Error
    {
        Corrupt,          // not valid JSON, or the root is not an object
        BadFormat,        // a required field is absent, has the wrong type, or cannot be migrated
        FromNewerVersion, // refusing this file prevents an older PKSM from discarding new fields
    };

    struct Contents
    {
        Settings settings;
        int sourceVersion = CURRENT_VERSION;
        // True when migration or normalization changed what should be persisted.
        bool changed = false;
    };

    Settings defaultSettings();
    std::variant<Contents, Error> parse(std::string_view data);
    std::string serialize(const Settings& settings);
}

#endif
