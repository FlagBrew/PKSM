/*
*   This file is part of PKSM
*   Copyright (C) 2016-2018 Bernardo Giordano, Admiral Fish, piepie62
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

#include <3ds.h>
#include "json.hpp"
#include "i18n.hpp"
#include "utils.hpp"

class Configuration
{
public:
    static constexpr int CURRENT_VERSION = 1;

    static Configuration& getInstance(void)
    {
        static Configuration config;
        return config;
    }

    Language language(void) const
    {
        return mLang;
    }

    bool autoBackup(void) const
    {
        return mAutoBackup;
    }

    int storageSize(void) const
    {
        return mSize;
    }

    bool transferEdit(void) const
    {
        return mTferEdit;
    }

    bool useExtData(void) const
    {
        return mExtData;
    }

    u32 defaultPID(void) const
    {
        return mPid;
    }

    u32 defaultSID(void) const
    {
        return mSid;
    }

    std::string defaultOT(void) const
    {
        return mOt;
    }

    int nationality(void) const
    {
        return mNationality;
    }

    int day(void)
    {
        return mDay;
    }
    
    int month(void)
    {
        return mMonth;
    }

    int year(void)
    {
        return mYear;
    }

    void language(Language lang)
    {
        mLang = lang;
    }

    void autoBackup(bool backup)
    {
        mAutoBackup = backup;
    }

    void storageSize(int size)
    {
        mSize = size;
    }

    void transferEdit(bool edit)
    {
        mTferEdit = edit;
    }

    void useExtData(bool use)
    {
        mExtData = use;
    }

    void defaultPID(u32 pid)
    {
        mPid = pid;
    }

    void defaultSID(u32 sid)
    {
        mSid = sid;
    }

    void defaultOT(std::string ot)
    {
        mOt = ot;
    }

    void nationality(int nation)
    {
        mNationality = nation;
    }

    void day(int day)
    {
        mDay = day;
    }
    
    void month(int month)
    {
        mMonth = month;
    }

    void year(int year)
    {
        mYear = year;
    }

    void save(void);

private:
    Configuration(void);
    ~Configuration(void) {}

    Configuration(Configuration const&) = delete;
    void operator=(Configuration const&) = delete;

    nlohmann::json mJson;
    int mYear, mMonth, mDay, mNationality, mSize;
    u32 mSid, mPid;
    bool mTferEdit, mExtData, mAutoBackup;
    std::string mOt;
    Language mLang;

    size_t oldSize = 0;
};

#endif