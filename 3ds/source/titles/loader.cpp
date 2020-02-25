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

#include "loader.hpp"
#include "Configuration.hpp"
#include "Directory.hpp"
#include "FSStream.hpp"
#include "Sav.hpp"
#include "Title.hpp"
#include "archive.hpp"
#include "format.h"
#include "gui.hpp"
#include "io.hpp"
#include <atomic>
#include <ctime>
#include <sys/stat.h>

namespace
{
    std::unordered_map<std::u16string, std::shared_ptr<Directory>> directories;

    constexpr char langIds[8] = {
        'E', // USA
        'S', // Spain
        'K', // Korea
        'J', // Japan
        'I', // Italy
        'D', // Germany
        'F', // France
        'O'  // Europe? Definitely some sort of English
    };

    constexpr std::string_view dsIds[9] = {
        "ADA", // Diamond
        "APA", // Pearl
        "CPU", // Platinum
        "IPK", // HeartGold
        "IPG", // SoulSilver
        "IRB", // Black
        "IRA", // White
        "IRE", // Black 2
        "IRD"  // White 2
    };

    std::atomic<bool> cartWasUpdated = false;
    std::atomic_flag continueScan;

    std::string idToSaveName(const std::string& id)
    {
        if (id.size() == 3 || id.size() == 4)
        {
            if (id.substr(0, 3) == "ADA")
            {
                return "POKEMON D.sav";
            }
            if (id.substr(0, 3) == "APA")
            {
                return "POKEMON P.sav";
            }
            if (id.substr(0, 3) == "CPU")
            {
                return "POKEMON PL.sav";
            }
            if (id.substr(0, 3) == "IPK")
            {
                return "POKEMON HG.sav";
            }
            if (id.substr(0, 3) == "IPG")
            {
                return "POKEMON SS.sav";
            }
            if (id.substr(0, 3) == "IRB")
            {
                return "POKEMON B.sav";
            }
            if (id.substr(0, 3) == "IRA")
            {
                return "POKEMON W.sav";
            }
            if (id.substr(0, 3) == "IRE")
            {
                return "POKEMON B2.sav";
            }
            if (id.substr(0, 3) == "IRD")
            {
                return "POKEMON W2.sav";
            }
        }
        return "main";
    }

    std::string idToSaveName(const std::u16string& id)
    {
        if (id.size() == 3 || id.size() == 4)
        {
            if (id.substr(0, 3) == u"ADA")
            {
                return "POKEMON D.sav";
            }
            if (id.substr(0, 3) == u"APA")
            {
                return "POKEMON P.sav";
            }
            if (id.substr(0, 3) == u"CPU")
            {
                return "POKEMON PL.sav";
            }
            if (id.substr(0, 3) == u"IPK")
            {
                return "POKEMON HG.sav";
            }
            if (id.substr(0, 3) == u"IPG")
            {
                return "POKEMON SS.sav";
            }
            if (id.substr(0, 3) == u"IRB")
            {
                return "POKEMON B.sav";
            }
            if (id.substr(0, 3) == u"IRA")
            {
                return "POKEMON W.sav";
            }
            if (id.substr(0, 3) == u"IRE")
            {
                return "POKEMON B2.sav";
            }
            if (id.substr(0, 3) == u"IRD")
            {
                return "POKEMON W2.sav";
            }
        }
        return "main";
    }

    // known 3ds title ids
    constexpr std::array<unsigned long long, 8> ctrTitleIds = {
        0x0004000000055D00, // X
        0x0004000000055E00, // Y
        0x000400000011C400, // OR
        0x000400000011C500, // AS
        0x0004000000164800, // Sun
        0x0004000000175E00, // Moon
        0x00040000001B5000, // Ultrasun
        0x00040000001B5100  // Ultramoon
    };

    bool saveIsFile;
    std::string saveFileName;
    std::shared_ptr<Title> loadedTitle;

    std::vector<std::string> scanDirectoryFor(const std::u16string& dir, const std::u16string& id)
    {
        if (directories.count(dir) == 0)
        {
            directories.emplace(dir, std::make_shared<Directory>(Archive::sd(), dir));
        }
        else
        {
            // Attempt to re-read directory
            if (!directories[dir]->loaded())
            {
                directories[dir] = std::make_shared<Directory>(Archive::sd(), dir);
            }
        }
        std::vector<std::string> ret;
        auto& directory = directories[dir];
        if (directory->loaded())
        {
            for (size_t j = 0; j < directory->count(); j++)
            {
                if (directory->folder(j))
                {
                    std::u16string fileName = directory->item(j);
                    if (fileName.substr(0, id.size()) == id)
                    {
                        Directory subdir(Archive::sd(), dir + u"/" + fileName);
                        for (size_t k = 0; k < subdir.count(); k++)
                        {
                            if (subdir.folder(k))
                            {
                                std::string savePath =
                                    StringUtils::UTF16toUTF8(dir + u"/" + fileName + u"/" + subdir.item(k) + u"/") + idToSaveName(id);
                                if (io::exists(savePath))
                                {
                                    ret.emplace_back(savePath);
                                }
                            }
                        }
                    }
                }
            }
        }
        return ret;
    }

    std::vector<std::string> scanDirectoryFor(const std::u16string& dir, const std::string& id)
    {
        return scanDirectoryFor(dir, StringUtils::UTF8toUTF16(id));
    }
}

void TitleLoader::init(void)
{
    continueScan.test_and_set();
}

void TitleLoader::scanTitles(void)
{
    Result res = 0;
    u32 count  = 0;

    // clear title list if filled previously
    nandTitles.clear();

    if (continueScan.test_and_set())
    {
        scanCard();
    }
    else
    {
        return;
    }

    // get title count
    res = AM_GetTitleCount(MEDIATYPE_SD, &count);
    if (R_FAILED(res) || !continueScan.test_and_set())
    {
        return;
    }

    // get title list and check if a title matches the ids we want
    std::vector<u64> ids(count);
    u64* p = ids.data();
    res    = AM_GetTitleList(NULL, MEDIATYPE_SD, count, p);
    if (R_FAILED(res) || !continueScan.test_and_set())
    {
        return;
    }

    for (size_t i = 0; i < ctrTitleIds.size(); i++)
    {
        u64 id = ctrTitleIds.at(i);
        if (continueScan.test_and_set())
        {
            if (std::find(ids.begin(), ids.end(), id) != ids.end())
            {
                auto title = std::make_shared<Title>();
                if (title->load(id, MEDIATYPE_SD, CARD_CTR))
                {
                    nandTitles.push_back(title);
                }
            }
        }
        else
        {
            return;
        }
    }

    // sort the list alphabetically
    std::sort(nandTitles.begin(), nandTitles.end(), [](std::shared_ptr<Title>& l, std::shared_ptr<Title>& r) { return l->ID() < r->ID(); });
}

void TitleLoader::scanSaves(void)
{
    Gui::waitFrame(i18n::localize("SCAN_SAVES"));
    for (size_t i = 0; i < ctrTitleIds.size(); i++)
    {
        u32 uniqueId                   = (u32)ctrTitleIds[i] >> 8;
        std::string id                 = fmt::format(FMT_STRING("0x{:05X}"), uniqueId);
        std::vector<std::string> saves = scanDirectoryFor(u"/3ds/Checkpoint/saves", id);
        if (Configuration::getInstance().showBackups())
        {
            std::vector<std::string> moreSaves = scanDirectoryFor(u"/3ds/PKSM/backups", id);
            saves.insert(saves.end(), moreSaves.begin(), moreSaves.end());
        }
        auto extraSaves = Configuration::getInstance().extraSaves(id);
        if (!extraSaves.empty())
        {
            for (auto& save : extraSaves)
            {
                if (io::exists(save))
                {
                    saves.emplace_back(save);
                }
            }
        }
        sdSaves[id] = saves;
    }

    for (size_t game = 0; game < 9; game++)
    {
        for (size_t lang = 0; lang < 8; lang++)
        {
            std::string id                 = std::string(dsIds[game]) + langIds[lang];
            std::vector<std::string> saves = scanDirectoryFor(u"/3ds/Checkpoint/saves", id);
            if (Configuration::getInstance().showBackups())
            {
                std::vector<std::string> moreSaves = scanDirectoryFor(u"/3ds/PKSM/backups", id);
                saves.insert(saves.end(), moreSaves.begin(), moreSaves.end());
            }
            auto extraSaves = Configuration::getInstance().extraSaves(id);
            if (!extraSaves.empty())
            {
                for (auto& save : extraSaves)
                {
                    if (io::exists(save))
                    {
                        saves.emplace_back(save);
                    }
                }
            }
            sdSaves[id] = saves;
        }
    }
}

void TitleLoader::backupSave(const std::string& id)
{
    if (!save)
    {
        return;
    }
    Gui::waitFrame(i18n::localize("LOADER_BACKING_UP"));
    char stringTime[15]   = {0};
    time_t unixTime       = time(NULL);
    struct tm* timeStruct = gmtime((const time_t*)&unixTime);
    std::strftime(stringTime, 14, "%Y%m%d%H%M%S", timeStruct);
    std::string path = "/3ds/PKSM/backups/" + id;
    mkdir(path.c_str(), 777);
    path += '/' + std::string(stringTime) + '/';
    mkdir(path.c_str(), 777);
    path += idToSaveName(id);
    FSStream out = FSStream(Archive::sd(), path, FS_OPEN_WRITE | FS_OPEN_CREATE, TitleLoader::save->getLength());
    if (out.good())
    {
        TitleLoader::save->finishEditing();
        out.write(TitleLoader::save->rawData().get(), TitleLoader::save->getLength());
        TitleLoader::save->beginEditing();
        if (Configuration::getInstance().showBackups())
        {
            sdSaves[id].emplace_back(path);
        }
    }
    else
    {
        Gui::warn(i18n::localize("BAD_OPEN_BACKUP"));
    }
    out.close();
}

bool TitleLoader::load(std::shared_ptr<u8[]> data, size_t size)
{
    save = Sav::getSave(data, size);
    return save != nullptr;
}

bool TitleLoader::load(std::shared_ptr<Title> title)
{
    saveIsFile  = false;
    loadedTitle = title;
    if (title->mediaType() == FS_MediaType::MEDIATYPE_SD || title->cardType() == FS_CardType::CARD_CTR)
    {
        FS_Archive archive;
        Archive::save(&archive, title->mediaType(), title->lowId(), title->highId());
        FSStream in(archive, u"/main", FS_OPEN_READ);
        if (in.good())
        {
            std::shared_ptr<u8[]> data = std::shared_ptr<u8[]>(new u8[in.size()]);
            in.read(data.get(), in.size());
            save = Sav::getSave(data, in.size());
            in.close();
            FSUSER_CloseArchive(archive);
            if (Configuration::getInstance().autoBackup())
            {
                backupSave(title->checkpointPrefix());
            }
            return true;
        }
        else
        {
            Gui::error(i18n::localize("BAD_OPEN_SAVE"), in.result());
            in.close();
            loadedTitle = nullptr;
            FSUSER_CloseArchive(archive);
            return false;
        }
    }
    else
    {
        u32 cap = SPIGetCapacity(title->SPICardType());
        if (cap != 524288)
        {
            Gui::warn(i18n::localize("WRONG_SIZE") + '\n' + i18n::localize("Please report"));
            return false;
        }

        std::shared_ptr<u8[]> data = std::shared_ptr<u8[]>(new u8[cap]);
        u32 sectorSize             = (cap < 0x10000) ? cap : 0x10000;

        for (u32 i = 0; i < cap / sectorSize; ++i)
        {
            SPIReadSaveData(title->SPICardType(), sectorSize * i, &data[sectorSize * i], sectorSize);
        }

        save = Sav::getSave(data, cap);
        if (Configuration::getInstance().autoBackup())
        {
            backupSave(title->checkpointPrefix());
        }
        return save != nullptr;
    }
    Gui::warn(i18n::localize("LOADER_CRITICAL_ERROR"));
    return false;
}

bool TitleLoader::load(std::shared_ptr<Title> title, const std::string& savePath)
{
    saveIsFile   = true;
    saveFileName = savePath;
    loadedTitle  = title;
    FSStream in(Archive::sd(), StringUtils::UTF8toUTF16(savePath), FS_OPEN_READ);
    u32 size;
    std::shared_ptr<u8[]> saveData = nullptr;
    if (in.good())
    {
        size = in.size();
        if (size > 0x200000) // Sane limit for save size as of SWSH 1.1.0
        {
            Gui::error(i18n::localize("WRONG_SIZE"), in.size());
            loadedTitle  = nullptr;
            saveFileName = "";
            in.close();
            return false;
        }
        saveData = std::shared_ptr<u8[]>(new u8[size]);
        in.read(saveData.get(), size);
    }
    else
    {
        Gui::error(i18n::localize("BAD_OPEN_SAVE"), in.result());
        loadedTitle  = nullptr;
        saveFileName = "";
        in.close();
        return false;
    }
    in.close();
    save = Sav::getSave(saveData, size);
    if (!save)
    {
        Gui::warn(saveFileName + '\n' + i18n::localize("SAVE_INVALID"));
        saveFileName = "";
        loadedTitle  = nullptr;
        return false;
    }
    if (Configuration::getInstance().autoBackup())
    {
        if (title)
        {
            backupSave(title->checkpointPrefix());
        }
        else
        {
            bool done = false;
            for (auto i = sdSaves.begin(); !done && i != sdSaves.end(); i++)
            {
                for (auto j = i->second.begin(); j != i->second.end(); j++)
                {
                    if (*j == savePath)
                    {
                        backupSave(i->first);
                        done = true;
                        break;
                    }
                }
            }
        }
    }
    return true;
}

void TitleLoader::saveToTitle(bool ask)
{
    Result res;
    if (loadedTitle)
    {
        if (TitleLoader::cardTitle == loadedTitle &&
            (!ask || Gui::showChoiceMessage(i18n::localize("SAVE_OVERWRITE_1") + '\n' + i18n::localize("SAVE_OVERWRITE_CARD"))))
        {
            auto& title = TitleLoader::cardTitle;
            if (title->cardType() == FS_CardType::CARD_CTR)
            {
                FS_Archive archive;
                Archive::save(&archive, title->mediaType(), title->lowId(), title->highId());
                FSStream out(archive, u"/main", FS_OPEN_WRITE | FS_OPEN_CREATE, save->getLength());
                if (out.good())
                {
                    out.write(save->rawData().get(), save->getLength());
                    if (R_FAILED(res = FSUSER_ControlArchive(archive, ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0)))
                    {
                        out.close();
                        FSUSER_CloseArchive(archive);
                        Gui::error(i18n::localize("FAIL_SAVE_COMMIT"), res);
                        return;
                    }
                }
                else
                {
                    Gui::error(i18n::localize("BAD_OPEN_SAVE"), out.result());
                }
                out.close();
                FSUSER_CloseArchive(archive);
            }
            else
            {
                res          = 0;
                u32 pageSize = SPIGetPageSize(title->SPICardType());
                for (u32 i = 0; i < save->getLength() / pageSize; ++i)
                {
                    res = SPIWriteSaveData(title->SPICardType(), pageSize * i, &save->rawData()[pageSize * i], pageSize);
                    if (R_FAILED(res))
                    {
                        break;
                    }
                    Gui::showRestoreProgress(pageSize * (i + 1), save->getLength());
                }
            }
        }
        else
        {
            // Just a linear search because it's a maximum of eight titles
            for (auto title : TitleLoader::nandTitles)
            {
                if (title == loadedTitle &&
                    (!ask || Gui::showChoiceMessage(i18n::localize("SAVE_OVERWRITE_1") + '\n' + i18n::localize("SAVE_OVERWRITE_INSTALL"))))
                {
                    FS_Archive archive;
                    Archive::save(&archive, title->mediaType(), title->lowId(), title->highId());
                    FSStream out(archive, u"/main", FS_OPEN_WRITE | FS_OPEN_CREATE, save->getLength());
                    if (out.good())
                    {
                        out.write(save->rawData().get(), save->getLength());
                        if (R_FAILED(res = FSUSER_ControlArchive(archive, ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0)))
                        {
                            out.close();
                            FSUSER_CloseArchive(archive);
                            Gui::error(i18n::localize("FAIL_SAVE_COMMIT"), res);
                            return;
                        }
                    }
                    else
                    {
                        Gui::error(i18n::localize("BAD_OPEN_SAVE"), out.result());
                    }
                    out.close();
                    FSUSER_CloseArchive(archive);
                    break; // There can only be one match
                }
            }
        }

        u8 out;
        u64 secureValue = ((u64)SECUREVALUE_SLOT_SD << 32) | (loadedTitle->lowId() & 0xFFFFFF00);
        res             = FSUSER_ControlSecureSave(SECURESAVE_ACTION_DELETE, &secureValue, 8, &out, 1);
        if (R_FAILED(res))
        {
            Gui::error(i18n::localize("SECURE_VALUE_ERROR"), res);
        }
    }
}

void TitleLoader::saveChanges()
{
    save->finishEditing();
    if (saveIsFile)
    {
        // No need to check size; if it was read successfully, that means that it has the correct size
        FSStream out(Archive::sd(), StringUtils::UTF8toUTF16(saveFileName), FS_OPEN_WRITE);
        out.write(save->rawData().get(), save->getLength());
        out.close();
        if (Configuration::getInstance().writeFileSave())
        {
            saveToTitle(true);
        }
    }
    else
    {
        saveToTitle(false);
    }
    save->beginEditing();
}

void TitleLoader::exit()
{
    continueScan.clear();
    nandTitles.clear();
    cardTitle   = nullptr;
    loadedTitle = nullptr;
}

bool TitleLoader::scanCard()
{
    static bool isScanning = false;
    if (isScanning)
    {
        return false;
    }
    else
    {
        isScanning = true;
    }
    bool ret   = false;
    cardTitle  = nullptr;
    Result res = 0;
    u32 count  = 0;
    // check for cartridge and push at the beginning of the title list
    FS_CardType cardType;
    res = FSUSER_GetCardType(&cardType);
    if (R_SUCCEEDED(res))
    {
        if (cardType == CARD_CTR)
        {
            res = AM_GetTitleCount(MEDIATYPE_GAME_CARD, &count);
            if (R_SUCCEEDED(res) && count > 0)
            {
                ret = true;
                u64 id;
                res = AM_GetTitleList(NULL, MEDIATYPE_GAME_CARD, count, &id);
                // check if this id is in our list
                if (R_SUCCEEDED(res) && std::find(ctrTitleIds.begin(), ctrTitleIds.end(), id) != ctrTitleIds.end())
                {
                    auto title = std::make_shared<Title>();
                    if (title->load(id, MEDIATYPE_GAME_CARD, cardType))
                    {
                        cardTitle = title;
                    }
                }
            }
        }
        else
        {
            // ds game card, behave differently
            // load the save and check for known patterns
            auto title = std::make_shared<Title>();
            if (title->load(0, MEDIATYPE_GAME_CARD, cardType))
            {
                ret                            = true;
                CardType spiCardType           = title->SPICardType();
                u32 saveSize                   = SPIGetCapacity(spiCardType);
                u32 sectorSize                 = (saveSize < 0x10000) ? saveSize : 0x10000;
                std::shared_ptr<u8[]> saveFile = std::shared_ptr<u8[]>(new u8[saveSize]);
                for (u32 i = 0; i < saveSize / sectorSize; ++i)
                {
                    res = SPIReadSaveData(spiCardType, sectorSize * i, &saveFile[sectorSize * i], sectorSize);
                    if (R_FAILED(res))
                    {
                        break;
                    }
                }

                if (R_SUCCEEDED(res) && Sav::isValidDSSave(saveFile))
                {
                    cardTitle = title;
                }
            }
            else
            {
                if (title->checkpointPrefix() != "") // It only failed at SPIGetCardType
                {
                    ret = true;
                }
                // Otherwise keep trying
            }
        }
    }
    isScanning     = false;
    cartWasUpdated = true;
    return ret;
}

bool TitleLoader::cardWasUpdated()
{
    if (cartWasUpdated)
    {
        cartWasUpdated = false;
        return true;
    }
    return false;
}
