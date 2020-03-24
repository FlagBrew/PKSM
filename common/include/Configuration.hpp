/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2020 Bernardo Giordano, Admiral Fish, piepie62
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

#include "DateTime.hpp"
#include "Language.hpp"
#include "coretypes.h"
#include "nlohmann/json_fwd.hpp"
#include <memory>

class Configuration
{
public:
    static constexpr int CURRENT_VERSION = 8;

    static Configuration& getInstance(void)
    {
        static Configuration config;
        return config;
    }

    Language language(void) const;

    bool autoBackup(void) const;

    bool transferEdit(void) const;

    bool useExtData(void) const;

    u32 defaultTID(void) const;

    u32 defaultSID(void) const;

    std::string defaultOT(void) const;

    int nationality(void) const;

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

    bool writeFileSave(void) const;

    bool useSaveInfo(void) const;

    bool randomMusic(void) const;

    int defaultRegion(void) const;

    int defaultCountry(void) const;

    bool showBackups(void) const;

    std::string legalEndpoint(void) const;

    std::string patronCode(void) const;

    bool alphaChannel(void) const;

    bool autoUpdate(void) const;

    void language(Language lang);

    void autoBackup(bool backup);

    void transferEdit(bool edit);

    void useExtData(bool use);

    void defaultTID(u32 tid);

    void defaultSID(u32 sid);

    void defaultOT(const std::string& ot);

    void nationality(int nation);

    void day(int day);

    void month(int month);

    void year(int year);

    // This assumes that we'll have a way to set them in the config screen, something that I'm not sure about
    // as that would require basically implementing a file browser. Maybe have it be manual, just like Checkpoint?
    // I implemented it just in case
    void extraSaves(const std::string& id, std::vector<std::string>& saves);

    void writeFileSave(bool write);

    void useSaveInfo(bool saveInfo);

    void randomMusic(bool random);

    void defaultRegion(u8 value);

    void defaultCountry(u8 value);

    void showBackups(bool value);

    void legalEndpoint(const std::string& value);

    void patronCode(const std::string& value);

    void alphaChannel(bool value);

    void autoUpdate(bool value);

    void save(void);

private:
    Configuration(void);
    ~Configuration(void);

    Configuration(const Configuration&) = delete;
    void operator=(const Configuration&) = delete;

    void loadFromRomfs(void);

    std::unique_ptr<nlohmann::json> mJson;

    size_t oldSize = 0;
};

namespace i18n
{
    const std::string& localize(Language lang, const std::string& index);
    inline const std::string& localize(const std::string& index) { return i18n::localize(Configuration::getInstance().language(), index); }
}

#endif
