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

#include "Configuration.hpp"
#include "Archive.hpp"
#include "Presenter.hpp"
#include <cstdio>

namespace
{
    std::string readFile(FILE* file)
    {
        if (!file || fseek(file, 0, SEEK_END) != 0)
        {
            return {};
        }
        const long length = ftell(file);
        if (length <= 0 || fseek(file, 0, SEEK_SET) != 0)
        {
            return {};
        }

        std::string contents(size_t(length), '\0');
        contents.resize(fread(contents.data(), 1, contents.size(), file));
        return contents;
    }

    pksm::Language systemLanguage()
    {
        u8 language;
        CFGU_GetSystemLanguage(&language);
        switch (language)
        {
            case CFG_LANGUAGE_JP:
                return pksm::Language::JPN;
            case CFG_LANGUAGE_EN:
                return pksm::Language::ENG;
            case CFG_LANGUAGE_FR:
                return pksm::Language::FRE;
            case CFG_LANGUAGE_DE:
                return pksm::Language::GER;
            case CFG_LANGUAGE_IT:
                return pksm::Language::ITA;
            case CFG_LANGUAGE_ES:
                return pksm::Language::SPA;
            case CFG_LANGUAGE_ZH:
                return pksm::Language::CHS;
            case CFG_LANGUAGE_KO:
                return pksm::Language::KOR;
            case CFG_LANGUAGE_NL:
                return pksm::Language::NL;
            case CFG_LANGUAGE_PT:
                return pksm::Language::PT;
            case CFG_LANGUAGE_RU:
                return pksm::Language::RU;
            case CFG_LANGUAGE_TW:
                return pksm::Language::CHT;
            default:
                return pksm::Language::ENG;
        }
    }

    pksm::Notice noticeFor(ConfigurationFile::Error error)
    {
        switch (error)
        {
            case ConfigurationFile::Error::Corrupt:
                return pksm::Notice::ConfigCorrupt;
            case ConfigurationFile::Error::BadFormat:
                return pksm::Notice::ConfigBadFormat;
            case ConfigurationFile::Error::FromNewerVersion:
                return pksm::Notice::ConfigFromNewerVersion;
        }
        return pksm::Notice::ConfigBadFormat;
    }
}

Configuration::Configuration()
{
    auto stream = Archive::data().file(u"/config.json", FS_OPEN_READ);
    if (!stream)
    {
        loadFromRomfs();
        return;
    }

    std::string jsonData(stream->size(), '\0');
    jsonData.resize(stream->read(jsonData.data(), jsonData.size()));
    stream->close();

    auto result = ConfigurationFile::parse(jsonData);
    if (auto* contents = std::get_if<ConfigurationFile::Contents>(&result))
    {
        publish(std::move(contents->settings));
        if (contents->changed)
        {
            save();
        }
        return;
    }

    const auto error = std::get<ConfigurationFile::Error>(result);
    loadFromRomfs();
    pksm::present::show(noticeFor(error));
}

Configuration::~Configuration() = default;

void Configuration::publish(Settings settings)
{
    mSettings.store(
        std::shared_ptr<const Settings>(std::make_shared<Settings>(std::move(settings))),
        std::memory_order_release);
}

Configuration::Snapshot Configuration::snapshot() const
{
    return mSettings.load(std::memory_order_acquire);
}

void Configuration::save()
{
    // Serialize one coherent snapshot and prevent a concurrent setter from being acknowledged
    // before this write while still being absent from it. Readers do not take the writer mutex.
    std::lock_guard lock(mWriteMutex);
    std::string writeData =
        ConfigurationFile::serialize(*mSettings.load(std::memory_order_acquire));
    writeData.shrink_to_fit();
    const size_t size = writeData.size();

    Archive::data().deleteFile("/config.json");
    Archive::data().createFile(u"/config.json", 0, size);
    auto stream = Archive::data().file(u"/config.json", FS_OPEN_WRITE, size);
    if (stream)
    {
        stream->write(writeData.data(), size);
        stream->close();
    }
}

void Configuration::loadFromRomfs()
{
    Settings settings = ConfigurationFile::defaultSettings();
    if (FILE* file = fopen("romfs:/config.json", "rt"))
    {
        const std::string jsonData = readFile(file);
        fclose(file);
        auto result = ConfigurationFile::parse(jsonData);
        if (auto* contents = std::get_if<ConfigurationFile::Contents>(&result))
        {
            settings = std::move(contents->settings);
        }
    }

    settings.language = systemLanguage();
    publish(std::move(settings));
    save();
}

pksm::Language Configuration::language() const
{
    return snapshot()->language;
}

bool Configuration::autoBackup() const
{
    return snapshot()->autoBackup;
}

bool Configuration::transferEdit() const
{
    return snapshot()->transferEdit;
}

bool Configuration::useExtData() const
{
    return snapshot()->useExtData;
}

int Configuration::day() const
{
    return snapshot()->day;
}

int Configuration::month() const
{
    return snapshot()->month;
}

int Configuration::year() const
{
    return snapshot()->year;
}

Date Configuration::date() const
{
    const auto settings = snapshot();
    Date date           = Date::today();
    if (settings->day != 0)
    {
        date.day(settings->day);
    }
    if (settings->month != 0)
    {
        date.month(settings->month);
    }
    if (settings->year != 0)
    {
        date.year(settings->year);
    }
    return date;
}

bool Configuration::writeFileSave() const
{
    return snapshot()->writeFileSave;
}

bool Configuration::useSaveInfo() const
{
    return snapshot()->useSaveInfo;
}

bool Configuration::randomMusic() const
{
    return snapshot()->randomMusic;
}

bool Configuration::showBackups() const
{
    return snapshot()->showBackups;
}

std::string Configuration::apiUrl() const
{
    return snapshot()->apiUrl;
}

bool Configuration::autoUpdate() const
{
    return snapshot()->autoUpdate;
}

int Configuration::cloudPageJump() const
{
    return snapshot()->cloudPageJump;
}

std::vector<std::string> Configuration::extraSaves(const std::string& id) const
{
    const auto settings = snapshot();
    const auto saves    = settings->extraSaves.find(id);
    return saves == settings->extraSaves.end() ? std::vector<std::string>{} : saves->second;
}

std::string Configuration::titleId(pksm::GameVersion version) const
{
    const auto settings = snapshot();
    const auto title    = settings->titles.find(std::to_string(u32(version)));
    return title == settings->titles.end() ? std::string{} : title->second;
}

void Configuration::language(pksm::Language lang)
{
    update([lang](Settings& settings) { settings.language = lang; });
}

void Configuration::autoBackup(bool backup)
{
    update([backup](Settings& settings) { settings.autoBackup = backup; });
}

void Configuration::transferEdit(bool edit)
{
    update([edit](Settings& settings) { settings.transferEdit = edit; });
}

void Configuration::useExtData(bool use)
{
    update([use](Settings& settings) { settings.useExtData = use; });
}

void Configuration::day(int value)
{
    update([value](Settings& settings) { settings.day = value; });
}

void Configuration::month(int value)
{
    update([value](Settings& settings) { settings.month = value; });
}

void Configuration::year(int value)
{
    update([value](Settings& settings) { settings.year = value; });
}

void Configuration::writeFileSave(bool write)
{
    update([write](Settings& settings) { settings.writeFileSave = write; });
}

void Configuration::useSaveInfo(bool saveInfo)
{
    update([saveInfo](Settings& settings) { settings.useSaveInfo = saveInfo; });
}

void Configuration::randomMusic(bool random)
{
    update([random](Settings& settings) { settings.randomMusic = random; });
}

void Configuration::showBackups(bool value)
{
    update([value](Settings& settings) { settings.showBackups = value; });
}

void Configuration::apiUrl(const std::string& value)
{
    update([&value](Settings& settings) { settings.apiUrl = value; });
}

void Configuration::autoUpdate(bool value)
{
    update([value](Settings& settings) { settings.autoUpdate = value; });
}

void Configuration::cloudPageJump(int value)
{
    update([value](Settings& settings) { settings.cloudPageJump = value > 0 ? value : 1; });
}

void Configuration::extraSaves(const std::string& id, const std::vector<std::string>& value)
{
    update([&id, &value](Settings& settings) { settings.extraSaves[id] = value; });
}

void Configuration::titleId(pksm::GameVersion version, const std::string& id)
{
    update(
        [version, &id](Settings& settings) { settings.titles[std::to_string(u32(version))] = id; });
}
