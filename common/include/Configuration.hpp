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

#include "Language.hpp"
#include "coretypes.h"
#include "json.hpp"

class Configuration
{
public:
    static constexpr int CURRENT_VERSION = 8;

    static Configuration& getInstance(void)
    {
        static Configuration config;
        return config;
    }

    Language language(void) const { return Language(mJson["language"].get<u8>()); }

    bool autoBackup(void) const { return mJson["autoBackup"]; }

    bool transferEdit(void) const { return mJson["transferEdit"]; }

    bool useExtData(void) const { return mJson["useExtData"]; }

    u32 defaultTID(void) const { return mJson["defaults"]["tid"]; }

    u32 defaultSID(void) const { return mJson["defaults"]["sid"]; }

    std::string defaultOT(void) const { return mJson["defaults"]["ot"]; }

    int nationality(void) const { return mJson["defaults"]["nationality"]; }

    int day(void) const { return mJson["defaults"]["date"]["day"]; }

    int month(void) const { return mJson["defaults"]["date"]["month"]; }

    int year(void) const { return mJson["defaults"]["date"]["year"]; }

    // Files
    std::vector<std::string> extraSaves(const std::string& id) const;

    bool writeFileSave(void) const { return mJson["writeFileSave"]; }

    bool useSaveInfo(void) const { return mJson["useSaveInfo"]; }

    bool randomMusic(void) const { return mJson["randomMusic"]; }

    int defaultRegion(void) const { return mJson["defaults"]["region"]; }

    int defaultCountry(void) const { return mJson["defaults"]["country"]; }

    bool showBackups(void) const { return mJson["showBackups"]; }

    std::string legalEndpoint(void) const { return mJson["legalEndpoint"]; }

    std::string patronCode(void) const { return mJson["patronCode"]; }

    bool alphaChannel(void) const { return mJson["alphaChannel"]; }

    bool autoUpdate(void) const { return mJson["autoUpdate"]; }

    void language(Language lang) { mJson["language"] = u8(lang); }

    void autoBackup(bool backup) { mJson["autoBackup"] = backup; }

    void transferEdit(bool edit) { mJson["transferEdit"] = edit; }

    void useExtData(bool use) { mJson["useExtData"] = use; }

    void defaultTID(u32 tid) { mJson["defaults"]["tid"] = tid; }

    void defaultSID(u32 sid) { mJson["defaults"]["sid"] = sid; }

    void defaultOT(const std::string& ot) { mJson["defaults"]["ot"] = ot; }

    void nationality(int nation) { mJson["defaults"]["nationality"] = nation; }

    void day(int day) { mJson["defaults"]["date"]["day"] = day; }

    void month(int month) { mJson["defaults"]["date"]["month"] = month; }

    void year(int year) { mJson["defaults"]["date"]["year"] = year; }

    // This assumes that we'll have a way to set them in the config screen, something that I'm not sure about
    // as that would require basically implementing a file browser. Maybe have it be manual, just like Checkpoint?
    // I implemented it just in case
    void extraSaves(const std::string& id, std::vector<std::string>& saves);

    void writeFileSave(bool write) { mJson["writeFileSave"] = write; }

    void useSaveInfo(bool saveInfo) { mJson["useSaveInfo"] = saveInfo; }

    void randomMusic(bool random) { mJson["randomMusic"] = random; }

    void defaultRegion(u8 value) { mJson["defaults"]["region"] = value; }

    void defaultCountry(u8 value) { mJson["defaults"]["country"] = value; }

    void showBackups(bool value) { mJson["showBackups"] = value; }

    void legalEndpoint(const std::string& value) { mJson["legalEndpoint"] = value; }

    void patronCode(const std::string& value) { mJson["patronCode"] = value; }

    void alphaChannel(bool value) { mJson["alphaChannel"] = value; }

    void autoUpdate(bool value) { mJson["autoUpdate"] = value; }

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

namespace i18n
{
    const std::string& localize(Language lang, const std::string& index);
    inline const std::string& localize(const std::string& index) { return i18n::localize(Configuration::getInstance().language(), index); }
}

#endif
