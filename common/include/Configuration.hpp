/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2025 Bernardo Giordano, Admiral Fish, piepie62
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

#include "ConfigurationFile.hpp"
#include "coretypes.h"
#include "enums/GameVersion.hpp"
#include "enums/Language.hpp"
#include "utils/DateTime.hpp"
#include <atomic>
#include <memory>
#include <mutex>
#include <string_view>

class Configuration
{
public:
    static constexpr int CURRENT_VERSION = ConfigurationFile::CURRENT_VERSION;
    using Settings                       = ConfigurationFile::Settings;
    using Snapshot                       = std::shared_ptr<const Settings>;

    static Configuration& getInstance(void)
    {
        static Configuration config;
        return config;
    }

    // A snapshot remains valid and unchanged while setters publish newer settings. Callers that
    // need several values should retain one snapshot rather than perform several independent reads.
    Snapshot snapshot(void) const;

    pksm::Language language(void) const;

    bool autoBackup(void) const;

    bool transferEdit(void) const;

    bool useExtData(void) const;

    int day(void) const;

    int month(void) const;

    int year(void) const;

    Date date(void) const;

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

    bool autoUpdate(void) const;

    int cloudPageJump(void) const;

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

    void autoUpdate(bool value);

    void cloudPageJump(int value);

    void save(void);

private:
    Configuration(void);
    ~Configuration();

    Configuration(const Configuration&)  = delete;
    void operator=(const Configuration&) = delete;

    void publish(Settings settings);
    void loadFromRomfs(void);

    template <typename Mutator>
    void update(Mutator&& mutator)
    {
        std::lock_guard lock(mWriteMutex);
        auto next = std::make_shared<Settings>(*mSettings.load(std::memory_order_acquire));
        mutator(*next);
        mSettings.store(
            std::shared_ptr<const Settings>(std::move(next)), std::memory_order_release);
    }

    std::atomic<Snapshot> mSettings;
    mutable std::mutex mWriteMutex;
};

namespace i18n
{
    const std::string& localize(pksm::Language lang, std::string_view index);

    inline const std::string& localize(std::string_view index)
    {
        return i18n::localize(Configuration::getInstance().language(), index);
    }
}

#endif
