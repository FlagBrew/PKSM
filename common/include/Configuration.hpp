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

#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "types.h"
#include "json.hpp"
#include "i18n.hpp"
#include "utils.hpp"

class Configuration
{
public:
    static constexpr int CURRENT_VERSION = 5;

    static Configuration& getInstance(void)
    {
        static Configuration config;
        return config;
    }

    Language language(void) const
    {
        return mJson["language"];
    }

    bool autoBackup(void) const
    {
        return mJson["autoBackup"];
    }

    int storageSize(void) const
    {
        return mJson["storageSize"];
    }

    bool transferEdit(void) const
    {
        return mJson["transferEdit"];
    }

    bool useExtData(void) const
    {
        return mJson["useExtData"];
    }

    u32 defaultTID(void) const
    {
        return mJson["defaults"]["pid"];
    }

    u32 defaultSID(void) const
    {
        return mJson["defaults"]["sid"];
    }

    std::string defaultOT(void) const
    {
        return mJson["defaults"]["ot"];
    }

    int nationality(void) const
    {
        return mJson["defaults"]["nationality"];
    }

    int day(void)
    {
        return mJson["defaults"]["date"]["day"];
    }
    
    int month(void)
    {
        return mJson["defaults"]["date"]["month"];
    }

    int year(void)
    {
        return mJson["defaults"]["date"]["year"];
    }

    // Files
    std::vector<std::string> extraSaves(const std::string& id);

    bool writeFileSave(void)
    {
        return mJson["writeFileSave"];
    }

    bool useSaveInfo(void)
    {
        return mJson["useSaveInfo"];
    }

    bool randomMusic(void)
    {
        return mJson["randomMusic"];
    }

    int defaultRegion(void)
    {
        return mJson["defaults"]["region"];
    }

    int defaultCountry(void)
    {
        return mJson["defaults"]["country"];
    }

    void language(Language lang)
    {
        mJson["language"] = lang;
    }

    void autoBackup(bool backup)
    {
        mJson["autoBackup"] = backup;
    }

    void storageSize(int size)
    {
        mJson["storageSize"] = size;
    }

    void transferEdit(bool edit)
    {
        mJson["transferEdit"] = edit;
    }

    void useExtData(bool use)
    {
        mJson["useExtData"] = use;
    }

    void defaultTID(u32 pid)
    {
        mJson["defaults"]["pid"] = pid;
    }

    void defaultSID(u32 sid)
    {
        mJson["defaults"]["sid"] = sid;
    }

    void defaultOT(const std::string& ot)
    {
        mJson["defaults"]["ot"] = ot;
    }

    void nationality(int nation)
    {
        mJson["defaults"]["nationality"] = nation;
    }

    void day(int day)
    {
        mJson["defaults"]["date"]["day"] = day;
    }
    
    void month(int month)
    {
        mJson["defaults"]["date"]["month"] = month;
    }

    void year(int year)
    {
        mJson["defaults"]["date"]["year"] = year;
    }

    // This assumes that we'll have a way to set them in the config screen, something that I'm not sure about
    // as that would require basically implementing a file browser. Maybe have it be manual, just like Checkpoint?
    // I implemented it just in case
    void extraSaves(const std::string& id, std::vector<std::string>& saves);

    void writeFileSave(bool write)
    {
        mJson["writeFileSave"] = write;
    }

    void useSaveInfo(bool saveInfo)
    {
        mJson["useSaveInfo"] = saveInfo;
    }

    void randomMusic(bool random)
    {
        mJson["randomMusic"] = random;
    }

    void defaultRegion(u8 value)
    {
        mJson["defaults"]["region"] = value;
    }

    void defaultCountry(u8 value)
    {
        mJson["defaults"]["country"] = value;
    }

    void save(void);

private:
    Configuration(void);
    ~Configuration(void) {}

    Configuration(Configuration const&) = delete;
    void operator=(Configuration const&) = delete;

    void loadFromRomfs(void);

    nlohmann::json mJson;

    size_t oldSize = 0;
};

#endif
