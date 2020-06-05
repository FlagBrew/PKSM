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
#include "../io/internal_fspxi.hpp"
#include "Archive.hpp"
#include "Configuration.hpp"
#include "DateTime.hpp"
#include "Directory.hpp"
#include "Title.hpp"
#include "format.h"
#include "gui.hpp"
#include "io.hpp"
#include "sav/Sav.hpp"
#include "utils/crypto.hpp"
#include <3ds.h>
#include <atomic>
#include <format.h>
#include <sys/stat.h>

namespace
{
    struct GbaHeader
    {
        u8 magic[4];       // .SAV
        u8 padding1[12];   // Always 0xFF
        u8 cmac[0x10];     // CMAC. MUST BE RECALCULATED ON SAVE
        u8 padding2[0x10]; // Always 0xFF
        u32 contentId;     // Always 1
        u32 savesMade;     // Check this to find which save to load
        u64 titleId;
        u8 sdCid[0x10];
        u32 saveOffset; // Always 0x200
        u32 saveSize;
        u8 padding3[8];      // Always 0xFF
        u8 arm7Registers[8]; // Not really sure what to do with this. Seems to be 0x7F 00 00 00 00 00 00 00?
        u8 padding4[0x198];  // Get it to the proper size
    };

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

    std::array<u64, 5> vcTitleIds                              = {0, 0, 0, 0, 0};
    std::array<u64, 8> ctrTitleIds                             = {0, 0, 0, 0, 0, 0, 0, 0};
    constexpr std::array<pksm::GameVersion, 13> searchVersions = {pksm::GameVersion::S, pksm::GameVersion::R, pksm::GameVersion::E,
        pksm::GameVersion::FR, pksm::GameVersion::LG, pksm::GameVersion::X, pksm::GameVersion::Y, pksm::GameVersion::OR, pksm::GameVersion::AS,
        pksm::GameVersion::SN, pksm::GameVersion::MN, pksm::GameVersion::US, pksm::GameVersion::UM};

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
        for (const auto& tid : vcTitleIds)
        {
            std::string chkPrefix = Title::tidToCheckpointPrefix<std::string>(tid);
            if (chkPrefix == id)
            {
                return "00000001.sav";
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
        for (const auto& tid : vcTitleIds)
        {
            std::u16string chkPrefix = Title::tidToCheckpointPrefix<std::u16string>(tid);
            if (chkPrefix == id)
            {
                return "00000001.sav";
            }
        }
        return "main";
    }

    bool saveIsFile;
    std::string saveFileName;
    std::shared_ptr<Title> loadedTitle;

    std::vector<std::string> scanDirectoryFor(const std::u16string& dir, const std::u16string& id)
    {
        if (directories.count(dir) == 0)
        {
            std::shared_ptr<Directory> d = Archive::sd().directory(dir);
            if (d)
            {
                directories.emplace(dir, d);
            }
        }
        else
        {
            // Attempt to re-read directory
            if (!directories[dir]->loaded())
            {
                std::shared_ptr<Directory> d = Archive::sd().directory(dir);
                if (d)
                {
                    directories[dir] = d;
                }
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
                        auto subdir = Archive::sd().directory(dir + u"/" + fileName);
                        if (subdir)
                        {
                            for (size_t k = 0; k < subdir->count(); k++)
                            {
                                if (subdir->folder(k))
                                {
                                    std::string savePath =
                                        StringUtils::UTF16toUTF8(dir + u"/" + fileName + u"/" + subdir->item(k) + u"/") + idToSaveName(id);
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
        }
        return ret;
    }

    std::vector<std::string> scanDirectoryFor(const std::u16string& dir, const std::string& id)
    {
        return scanDirectoryFor(dir, StringUtils::UTF8toUTF16(id));
    }

    // file must be at header address. On return, will be at the end of the save described by the header.
    std::array<u8, 0x10> calcGbaCMAC(File& file, const GbaHeader& header)
    {
        std::array<u8, 0x10> ret;
        constexpr size_t READBLOCK_SIZE = 0x1000;
        std::array<u8, 32> hashData;

        // Who the hell came up with this shit? Nintendo, please fire whatever employee thought this was a good idea
        // CMAC = AES-CMAC(SHA256("CTR-SIGN" + titleID + SHA256("CTR-SAV0" + SHA256(0x30..0x200 + the entire save itself))))
        // FSPXI_CalcSavegameMAC does the AES-CMAC, CTR-SIGN, and the CTR-SAV0 step
        pksm::crypto::SHA256 context;
        file.seek(0x30, SEEK_CUR);
        size_t sha_end_idx = header.saveSize + 0x200 - 0x30;
        u8* readblock      = new u8[READBLOCK_SIZE];
        size_t read        = 0;
        for (size_t i = 0; i < sha_end_idx; i += read)
        {
            size_t readSize = std::min(sha_end_idx - i, READBLOCK_SIZE);
            read            = file.read(readblock, readSize);
            context.update(readblock, readSize);
        }
        delete[] readblock;

        hashData = context.finish();

        FSPXI_CalcSavegameMAC(fspxiHandle, std::get<1>(file.getRawHandle()), hashData.data(), hashData.size(), ret.data(), ret.size());

        return ret;
    }
}

void TitleLoader::init(void)
{
    continueScan.test_and_set();

    reloadTitleIds();
}

void TitleLoader::reloadTitleIds(void)
{
    size_t vcIndex  = 0;
    size_t ctrIndex = 0;
    for (size_t i = 0; i < searchVersions.size(); i++)
    {
        std::string id = Configuration::getInstance().titleId(searchVersions[i]);
        if ((pksm::Generation)searchVersions[i] <= pksm::Generation::THREE)
        {
            vcTitleIds[vcIndex++] = strtoull(id.c_str(), nullptr, 16);
        }
        else
        {
            ctrTitleIds[ctrIndex++] = strtoull(id.c_str(), nullptr, 16);
        }
    }
}

void TitleLoader::scanTitles(void)
{
    Result res = 0;
    u32 count  = 0;

    // clear title lists if filled previously
    ctrTitles.clear();
    vcTitles.clear();

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

    for (u64& id : ctrTitleIds)
    {
        if (continueScan.test_and_set())
        {
            if (std::find(ids.begin(), ids.end(), id) != ids.end())
            {
                auto title = std::make_shared<Title>();
                if (title->load(id, MEDIATYPE_SD, CARD_CTR))
                {
                    ctrTitles.push_back(title);
                }
            }
        }
        else
        {
            return;
        }
    }

    for (u64& id : vcTitleIds)
    {
        if (continueScan.test_and_set())
        {
            if (std::find(ids.begin(), ids.end(), id) != ids.end())
            {
                auto title = std::make_shared<Title>();
                if (title->load(id, MEDIATYPE_SD, CARD_CTR))
                {
                    vcTitles.push_back(title);
                }
            }
        }
        else
        {
            return;
        }
    }

    // Titles are already sorted by GameVersion
}

void TitleLoader::scanSaves(void)
{
    Gui::waitFrame(i18n::localize("SCAN_SAVES"));
    auto scan = [](auto& tids) {
        for (auto& tid : tids)
        {
            std::string id                 = fmt::format(FMT_STRING("0x{:05X}"), ((u32)tid) >> 8);
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
    };

    scan(vcTitleIds);
    scan(ctrTitleIds);

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
    DateTime now     = DateTime::now();
    std::string path = fmt::format(FMT_STRING("/3ds/PKSM/backups/{0:s}"), id);
    mkdir(path.c_str(), 777);
    path +=
        fmt::format(FMT_STRING("/{0:d}-{1:d}-{2:d}_{3:d}-{4:d}-{5:d}/"), now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
    mkdir(path.c_str(), 777);
    path += idToSaveName(id);
    FILE* out = fopen(path.c_str(), "wb");
    if (out)
    {
        TitleLoader::save->finishEditing();
        fwrite(TitleLoader::save->rawData().get(), 1, TitleLoader::save->getLength(), out);
        fclose(out);
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
}

bool TitleLoader::load(std::shared_ptr<u8[]> data, size_t size)
{
    save = pksm::Sav::getSave(data, size);
    return save != nullptr;
}

bool TitleLoader::load(std::shared_ptr<Title> title)
{
    saveIsFile  = false;
    loadedTitle = title;
    if (title->mediaType() == FS_MediaType::MEDIATYPE_SD || title->cardType() == FS_CardType::CARD_CTR)
    {
        Archive archive;
        std::unique_ptr<File> in;
        if (title->gba())
        {
            archive                   = Archive::saveAndContents(title->mediaType(), title->lowId(), title->highId(), true);
            constexpr u32 pathData[5] = {
                1,   // Save data
                1,   // TMD content index
                3,   // Type: save data?
                0, 0 // No EXEFS file name needed
            };
            in = archive.file(FS_Path{PATH_BINARY, sizeof(pathData), pathData}, FS_OPEN_READ);
        }
        else
        {
            archive = Archive::save(title->mediaType(), title->lowId(), title->highId(), false);
            in      = archive.file(u"/main", FS_OPEN_READ);
        }
        if (in)
        {
            std::shared_ptr<u8[]> data;
            size_t size;
            // Have to get to the correct GBA save and sidestep the stupid size shit
            if (title->gba())
            {
                static constexpr u8 ZEROS[0x20]    = {0};
                static constexpr u8 FULL_FS[0x20]  = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
                std::unique_ptr<GbaHeader> header1 = std::make_unique<GbaHeader>();
                in->read(header1.get(), sizeof(GbaHeader));
                // Save uninitialized; we'd get garbage that probably goes out of bounds
                if (!memcmp(header1.get(), ZEROS, sizeof(ZEROS)))
                {
                    Gui::warn("Uninitialized save");
                    // Dummy data
                    data = std::shared_ptr<u8[]>(new u8[1]);
                    size = 1;
                }
                // Save initialized only in bottom save. Only check it.
                else if (!memcmp(header1.get(), FULL_FS, sizeof(FULL_FS)))
                {
                    Gui::warn("First save absent");
                    // If the first header is garbage FF, we have to search for the second. It can only be at one of these possible sizes + 0x200 (for
                    // the size of the first header)
                    static constexpr u32 POSSIBLE_SAVE_SIZES[] = {
                        0x400,   // 8kbit
                        0x2000,  // 64kbit
                        0x8000,  // 256kbit
                        0x10000, // 512kbit
                        0x20000, // 1024kbit/1Mbit
                    };
                    for (auto& size : POSSIBLE_SAVE_SIZES)
                    {
                        // Go to the possible offset
                        in->seek(size + sizeof(GbaHeader), SEEK_SET);
                        // Read what may be a header
                        in->read(header1.get(), sizeof(GbaHeader));
                        // If it's a header, we found it! Break.
                        if (R_SUCCEEDED(in->result()) && !memcmp(header1->magic, ".SAV", 4))
                        {
                            break;
                        }
                    }
                    if (R_SUCCEEDED(in->result()))
                    {
                        // Seek back to the beginning of this header
                        in->seek(-0x200, SEEK_CUR);
                        auto cmac    = calcGbaCMAC(*in, *header1);
                        bool invalid = (bool)memcmp(cmac.data(), header1->cmac, cmac.size());

                        if (invalid)
                        {
                            Gui::warn("Invalid single CMAC");
                            data = std::shared_ptr<u8[]>(new u8[1]);
                            size = 1;
                        }
                        else
                        {
                            size = header1->saveSize;
                            data = std::shared_ptr<u8[]>(new u8[size]);
                            // Always 0x200 after the second header
                            in->seek(sizeof(GbaHeader) * 2 + size, SEEK_SET);
                            in->read(data.get(), size);
                            in->close();
                        }
                    }
                    // Reached end of file? No header present at all? Something weird happened; we can't handle that
                    else
                    {
                        data = std::shared_ptr<u8[]>(new u8[1]);
                        size = 1;
                    }
                }
                // Both headers are initialized. Compare CMACs and such
                else
                {
                    std::unique_ptr<GbaHeader> header2 = std::make_unique<GbaHeader>();
                    in->seek(header1->saveSize, SEEK_CUR);
                    in->read(header2.get(), sizeof(GbaHeader));

                    // Check the first CMAC
                    in->seek(0, SEEK_SET);
                    auto cmac         = calcGbaCMAC(*in, *header1);
                    bool firstInvalid = (bool)memcmp(cmac.data(), header1->cmac, cmac.size());

                    // Check the second CMAC
                    in->seek(sizeof(GbaHeader) + header1->saveSize, SEEK_SET);
                    cmac               = calcGbaCMAC(*in, *header2);
                    bool secondInvalid = (bool)memcmp(cmac.data(), header2->cmac, cmac.size());

                    if (firstInvalid)
                    {
                        Gui::warn("First CMAC is invalid");
                        // Both CMACs are invalid. Run and hide.
                        if (secondInvalid)
                        {
                            Gui::warn("Second CMAC is invalid");
                            // Dummy data
                            data = std::shared_ptr<u8[]>(new u8[1]);
                            size = 1;
                        }
                        // The second CMAC is the only valid one. Use it
                        else
                        {
                            size = header2->saveSize;
                            data = std::shared_ptr<u8[]>(new u8[size]);
                            // Always 0x200 after the second header
                            in->seek(sizeof(GbaHeader) * 2 + size, SEEK_SET);
                            in->read(data.get(), size);
                            in->close();
                        }
                    }
                    else
                    {
                        // The first CMAC is the only valid one. Use it
                        if (secondInvalid)
                        {
                            Gui::warn("Second CMAC is invalid");
                            size = header1->saveSize;
                            data = std::shared_ptr<u8[]>(new u8[size]);
                            // Always 0x200 after the first header
                            in->seek(sizeof(GbaHeader), SEEK_SET);
                            in->read(data.get(), size);
                            in->close();
                        }
                        else
                        {
                            // Will include rollover (header1->savesMade == 0xFFFFFFFF)
                            // This is proper logic according to https://github.com/d0k3/GodMode9/issues/494
                            if (header2->savesMade == header1->savesMade + 1)
                            {
                                size = header2->saveSize;
                                data = std::shared_ptr<u8[]>(new u8[size]);
                                // Always 0x200 after the second header
                                in->seek(sizeof(GbaHeader) * 2 + size, SEEK_SET);
                                in->read(data.get(), size);
                                in->close();
                            }
                            else
                            {
                                size = header1->saveSize;
                                data = std::shared_ptr<u8[]>(new u8[size]);
                                // Always 0x200 after the first header
                                in->seek(sizeof(GbaHeader), SEEK_SET);
                                in->read(data.get(), size);
                                in->close();
                            }
                        }
                    }
                }
            }
            else
            {
                size = in->size();
                data = std::shared_ptr<u8[]>(new u8[size]);
                in->read(data.get(), size);
                in->close();
            }
            save = pksm::Sav::getSave(data, size);
            if (save)
            {
                if (Configuration::getInstance().autoBackup())
                {
                    backupSave(title->checkpointPrefix());
                }
            }
            else
            {
                Gui::error(i18n::localize("SAVE_INVALID"), -1);
            }
            return save != nullptr;
        }
        else
        {
            Gui::error(i18n::localize("BAD_OPEN_SAVE"), archive.result());
            loadedTitle = nullptr;
            return false;
        }
        archive.close();
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

        save = pksm::Sav::getSave(data, cap);
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
    FILE* in     = fopen(savePath.c_str(), "rb");
    u32 size;
    std::shared_ptr<u8[]> saveData = nullptr;
    if (in)
    {
        fseek(in, 0, SEEK_END);
        size = ftell(in);
        rewind(in);
        if (size > 0x200000) // Sane limit for save size as of SWSH 1.1.0
        {
            Gui::error(i18n::localize("WRONG_SIZE"), size);
            loadedTitle  = nullptr;
            saveFileName = "";
            fclose(in);
            return false;
        }
        saveData = std::shared_ptr<u8[]>(new u8[size]);
        fread(saveData.get(), 1, size, in);
        fclose(in);
    }
    else
    {
        Gui::error(i18n::localize("BAD_OPEN_SAVE"), errno);
        loadedTitle  = nullptr;
        saveFileName = "";
        return false;
    }
    save = pksm::Sav::getSave(saveData, size);
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
                Archive archive           = Archive::save(title->mediaType(), title->lowId(), title->highId(), false);
                std::unique_ptr<File> out = archive.file(u"/main", FS_OPEN_WRITE);

                if (out)
                {
                    out->write(save->rawData().get(), save->getLength());
                    if (R_FAILED(res = archive.commit()))
                    {
                        out->close();
                        archive.close();
                        Gui::error(i18n::localize("FAIL_SAVE_COMMIT"), res);
                        return;
                    }
                    out->close();
                    archive.close();
                }
                else
                {
                    Gui::error(i18n::localize("BAD_OPEN_SAVE"), archive.result());
                }
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
                    Gui::showRestoreProgress((pageSize * (i + 1)) / 1024, save->getLength() / 1024);
                }
            }
        }
        else
        {
            // Just a linear search because it's a maximum of thirteen titles
            auto doSave = [&](const auto& titles) {
                for (auto& title : titles)
                {
                    if (title == loadedTitle &&
                        (!ask || Gui::showChoiceMessage(i18n::localize("SAVE_OVERWRITE_1") + '\n' + i18n::localize("SAVE_OVERWRITE_INSTALL"))))
                    {
                        Archive archive;
                        std::unique_ptr<File> out;
                        if (title->gba())
                        {
                            archive                   = Archive::saveAndContents(title->mediaType(), title->lowId(), title->highId(), true);
                            constexpr u32 pathData[5] = {
                                1,   // Save data
                                1,   // TMD content index
                                3,   // Type: save data?
                                0, 0 // No EXEFS file name needed
                            };
                            out = archive.file(FS_Path{PATH_BINARY, sizeof(pathData), pathData}, FS_OPEN_READ | FS_OPEN_WRITE);
                        }
                        else
                        {
                            archive = Archive::save(title->mediaType(), title->lowId(), title->highId(), false);
                            out     = archive.file(u"/main", FS_OPEN_WRITE);
                        }

                        if (out)
                        {
                            if (title->gba())
                            {
                                static constexpr u8 ZEROS[0x20]   = {0};
                                static constexpr u8 FULL_FS[0x20] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

                                // Get which slot to save to
                                std::unique_ptr<GbaHeader> header1 = std::make_unique<GbaHeader>();
                                out->read(header1.get(), sizeof(GbaHeader));
                                // No clue how to handle an uninitialized save.
                                if (memcmp(header1.get(), ZEROS, sizeof(ZEROS)))
                                {
                                    // If the top save is uninitialized, grab the bottom save's header and copy it to the top's. Then write data
                                    if (!memcmp(header1.get(), FULL_FS, sizeof(FULL_FS)))
                                    {
                                        static constexpr u32 POSSIBLE_SAVE_SIZES[] = {
                                            0x400,   // 8kbit
                                            0x2000,  // 64kbit
                                            0x8000,  // 256kbit
                                            0x10000, // 512kbit
                                            0x20000, // 1024kbit/1Mbit
                                        };
                                        // Search for bottom header
                                        for (auto& size : POSSIBLE_SAVE_SIZES)
                                        {
                                            // Go to the possible offset
                                            out->seek(size + sizeof(GbaHeader), SEEK_SET);
                                            // Read what may be a header
                                            out->read(header1.get(), sizeof(GbaHeader));
                                            // If it's a header, we found it! Break.
                                            if (R_SUCCEEDED(out->result()) && !memcmp(header1->magic, ".SAV", 4))
                                            {
                                                break;
                                            }
                                        }
                                        if (R_SUCCEEDED(out->result()))
                                        {
                                            // Doesn't matter whether this CMAC is valid or not. We just need to update it
                                            out->seek(0, SEEK_SET);
                                            // Increment save count
                                            header1->savesMade++;
                                            out->write(header1.get(), sizeof(GbaHeader));
                                            out->write(save->rawData().get(), save->getLength());

                                            out->seek(0, SEEK_SET);
                                            auto cmac = calcGbaCMAC(*out, *header1);
                                            out->seek(offsetof(GbaHeader, cmac), SEEK_SET);
                                            out->write(cmac.data(), cmac.size());
                                            out->close();
                                        }
                                    }
                                    // Otherwise, compare the top and bottom save counts. If we loaded from the top, save in the bottom; if we loaded
                                    // from the bottom, save in the top
                                    else
                                    {
                                        std::unique_ptr<GbaHeader> header2 = std::make_unique<GbaHeader>();
                                        out->seek(header1->saveSize, SEEK_CUR);
                                        out->read(header2.get(), sizeof(GbaHeader));

                                        // Check the first CMAC
                                        out->seek(0, SEEK_SET);
                                        auto cmac         = calcGbaCMAC(*out, *header1);
                                        bool firstInvalid = (bool)memcmp(cmac.data(), header1->cmac, cmac.size());

                                        // Check the second CMAC
                                        out->seek(sizeof(GbaHeader) + header1->saveSize, SEEK_SET);
                                        cmac               = calcGbaCMAC(*out, *header2);
                                        bool secondInvalid = (bool)memcmp(cmac.data(), header2->cmac, cmac.size());

                                        if (firstInvalid)
                                        {
                                            // Just save to the first with header2->savesMade+1 as save number for simplicity; whether or not the
                                            // second save was valid to begin with is immaterial
                                            header1->savesMade = header2->savesMade + 1;
                                            out->seek(0, SEEK_SET);
                                            out->write(header1.get(), sizeof(GbaHeader));
                                            out->write(save->rawData().get(), save->getLength());
                                            out->seek(0, SEEK_SET);

                                            cmac = calcGbaCMAC(*out, *header1);
                                            out->seek(offsetof(GbaHeader, cmac), SEEK_SET);
                                            out->write(cmac.data(), cmac.size());
                                            out->close();
                                        }
                                        else
                                        {
                                            // If the second is valid and we loaded from it, save over first save
                                            if (!secondInvalid && header2->savesMade == header1->savesMade + 1)
                                            {
                                                header1->savesMade = header2->savesMade + 1;
                                                out->seek(0, SEEK_SET);
                                                out->write(header1.get(), sizeof(GbaHeader));
                                                out->write(save->rawData().get(), save->getLength());
                                                out->seek(0, SEEK_SET);

                                                cmac = calcGbaCMAC(*out, *header1);
                                                out->seek(offsetof(GbaHeader, cmac), SEEK_SET);
                                                out->write(cmac.data(), cmac.size());
                                                out->close();
                                            }
                                            // Otherwise, save over the second save
                                            else
                                            {
                                                out->seek(sizeof(GbaHeader) + header1->saveSize, SEEK_SET);
                                                header2->savesMade = header1->savesMade + 1;
                                                out->write(header2.get(), sizeof(GbaHeader));
                                                out->write(save->rawData().get(), save->getLength());
                                                out->seek(sizeof(GbaHeader) + header1->saveSize, SEEK_SET);

                                                cmac = calcGbaCMAC(*out, *header1);
                                                out->seek(sizeof(GbaHeader) + header1->saveSize + offsetof(GbaHeader, cmac), SEEK_SET);
                                                out->write(cmac.data(), cmac.size());
                                                out->close();
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    Gui::warn(i18n::localize("UNINIT_GBA_SAVE"));
                                }
                            }
                            else
                            {
                                out->write(save->rawData().get(), save->getLength());
                            }
                            if (!title->gba() && R_FAILED(res = archive.commit()))
                            {
                                out->close();
                                archive.close();
                                Gui::error(i18n::localize("FAIL_SAVE_COMMIT"), res);
                                return;
                            }
                            out->close();
                            archive.close();
                        }
                        else
                        {
                            Gui::error(i18n::localize("BAD_OPEN_SAVE"), archive.result());
                        }
                        break; // There can only be one match
                    }
                }
            };

            doSave(ctrTitles);
            doSave(vcTitles);
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
        FILE* out = fopen(saveFileName.c_str(), "wb");
        if (out)
        {
            fwrite(save->rawData().get(), 1, save->getLength(), out);
            fclose(out);
        }
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

std::string TitleLoader::savePath()
{
    if (saveIsFile)
    {
        return saveFileName;
    }
    return "";
}

void TitleLoader::exit()
{
    continueScan.clear();
    ctrTitles.clear();
    vcTitles.clear();
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
                static constexpr std::array<u64, 8> originalIDs = {
                    0x0004000000055D00, // X
                    0x0004000000055E00, // Y
                    0x000400000011C400, // OR
                    0x000400000011C500, // AS
                    0x0004000000164800, // SN
                    0x0004000000175E00, // MN
                    0x00040000001B5000, // US
                    0x00040000001B5100  // UM
                };
                ret = true;
                u64 id;
                res = AM_GetTitleList(NULL, MEDIATYPE_GAME_CARD, count, &id);
                // check if this id is in our list
                if (R_SUCCEEDED(res) && std::find(originalIDs.begin(), originalIDs.end(), id) != originalIDs.end())
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

                if (R_SUCCEEDED(res) && pksm::Sav::isValidDSSave(saveFile))
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
