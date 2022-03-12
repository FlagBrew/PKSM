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

#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "coretypes.h"
#include "enums/GameVersion.hpp"
#include "enums/Language.hpp"
#include "nlohmann/json_fwd.hpp"
#include "utils/DateTime.hpp"
#include <memory>

class Configuration
{
public:
    static constexpr int CURRENT_VERSION = 12;

    static Configuration& getInstance(void)
    {
        static Configuration config;
        return config;
    }

    pksm::Language language(void) const;

    bool autoBackup(void) const;

    bool transferEdit(void) const;

    bool useExtData(void) const;

    int day(void) const;

    int month(void) const;

    int year(void) const;

    Date date(void) const
    {
        Date ret = Date::today();
        if (day() != 0)
        {
            ret.day(day());
        }
        if (month() != 0)
        {
            ret.month(month());
        }
        if (year() != 0)
        {
            ret.year(year());
        }
        return ret;
    }

    // Files
    std::vector<std::string> extraSaves(const std::string& id) const;

    // Allows setting title IDs of versions. Can be used to edit romhacks or GB[A] VC. Support not
    // guaranteed for the former!
    std::string titleId(pksm::GameVersion version) const;

    bool writeFileSave(void) const;

    bool useSaveInfo(void) const;

    bool randomMusic(void) const;

    bool showBackups(void) const;

    std::string apiUrl(void) const;

    bool useApiUrl(void) const;

    std::string patronCode(void) const;

    bool alphaChannel(void) const;

    bool autoUpdate(void) const;

    void language(pksm::Language lang);

    void autoBackup(bool backup);

    void transferEdit(bool edit);

    void useExtData(bool use);

    void day(int day);

    void month(int month);

    void year(int year);

    // the below aged well

    // This assumes that we'll have a way to set them in the config screen, something that I'm not
    // sure about as that would require basically implementing a file browser. Maybe have it be
    // manual, just like Checkpoint? I implemented it just in case
    void extraSaves(const std::string& id, const std::vector<std::string>& saves);

    void titleId(pksm::GameVersion version, const std::string& id);

    void writeFileSave(bool write);

    void useSaveInfo(bool saveInfo);

    void randomMusic(bool random);

    void showBackups(bool value);

    void apiUrl(const std::string& value);

    void useApiUrl(bool value);

    void patronCode(const std::string& value);

    void alphaChannel(bool value);

    void autoUpdate(bool value);

    void save(void);

private:
    Configuration(void);
    ~Configuration();

    Configuration(const Configuration&) = delete;
    void operator=(const Configuration&) = delete;

    void loadFromRomfs(void);

    std::unique_ptr<nlohmann::json> mJson;

    size_t oldSize = 0;
};

namespace i18n
{
    const std::string& localize(pksm::Language lang, const std::string& index);
    inline const std::string& localize(const std::string& index)
    {
        return i18n::localize(Configuration::getInstance().language(), index);
    }
}

#endif
