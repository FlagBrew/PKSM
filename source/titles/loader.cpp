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

#include "loader.hpp"
#include "Configuration.hpp"
#include "Directory.hpp"
#include "FSStream.hpp"
#include <ctime>

static constexpr char langIds[8] = {
    'E', //USA
    'S', //Spain
    'K', //Korea
    'J', //Japan
    'I', //Italy
    'D', //Germany
    'F', //France
    'O'  //Europe? Definitely some sort of English
};

static const char* dsIds[9] = {
    "ADA", //Diamond
    "APA", //Pearl
    "CPU", //Platinum
    "IPK", //HeartGold
    "IPG", //SoulSilver
    "IRB", //Black
    "IRA", //White
    "IRE", //Black 2
    "IRD"  //White 2
};

// known 3ds title ids
static constexpr std::array<unsigned long long, 8> ctrTitleIds = {
    0x0004000000055D00, // X
    0x0004000000055E00, // Y
    0x000400000011C400, // OR
    0x000400000011C500, // AS
    0x0004000000164800, // Sun
    0x0004000000175E00, // Moon
    0x00040000001B5000, // Ultrasun
    0x00040000001B5100  // Ultramoon 
};

// title list
std::vector<std::shared_ptr<Title>> TitleLoader::nandTitles;
std::shared_ptr<Title> TitleLoader::cardTitle = nullptr;
std::unordered_map<std::string, std::vector<std::string>> TitleLoader::sdSaves;
std::shared_ptr<Sav> TitleLoader::save;

static bool saveIsFile;
static std::string saveFileName;
static std::shared_ptr<Title> loadedTitle;

void TitleLoader::scanTitles(void)
{
    Result res = 0;
    u32 count = 0;
    
    // clear title list if filled previously
    nandTitles.clear();

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
                u64 id;
                res = AM_GetTitleList(NULL, MEDIATYPE_GAME_CARD, count, &id);
                // check if this id is in our list
                if (R_SUCCEEDED(res) && std::find(ctrTitleIds.begin(), ctrTitleIds.end(), id) != ctrTitleIds.end())
                {
                    auto title = std::shared_ptr<Title>(new Title);
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
            auto title = std::shared_ptr<Title>(new Title);
            if (title->load(0, MEDIATYPE_GAME_CARD, cardType))
            {
                CardType cardType = title->SPICardType();
                u32 saveSize = SPIGetCapacity(cardType);
                u32 sectorSize = (saveSize < 0x10000) ? saveSize : 0x10000;
                u8* saveFile = new u8[saveSize];
                for (u32 i = 0; i < saveSize/sectorSize; ++i)
                {
                    res = SPIReadSaveData(cardType, sectorSize*i, saveFile + sectorSize*i, sectorSize);
                    if (R_FAILED(res))
                    {
                        break;
                    }
                }

                if (R_SUCCEEDED(res) && Sav::isValidDSSave(saveFile))
                {
                    cardTitle = title;
                }

                delete[] saveFile;
            }
        }
    }

    // get title count
    res = AM_GetTitleCount(MEDIATYPE_SD, &count);
    if (R_FAILED(res))
    {
        return;
    }

    // get title list and check if a title matches the ids we want
    std::vector<u64> ids(count);
    u64* p = ids.data();
    res = AM_GetTitleList(NULL, MEDIATYPE_SD, count, p);
    if (R_FAILED(res))
    {
        return;
    }

    for (size_t i = 0; i < ctrTitleIds.size(); i++)
    {
        u64 id = ctrTitleIds.at(i);
        if (std::find(ids.begin(), ids.end(), id) != ids.end())
        {
            auto title = std::shared_ptr<Title>(new Title);
            if (title->load(id, MEDIATYPE_SD, CARD_CTR))
            {
                nandTitles.push_back(title);
            }
        }
    }

    // sort the list alphabetically
    std::sort(nandTitles.begin(), nandTitles.end(), [](std::shared_ptr<Title>& l, std::shared_ptr<Title>& r) {
        return l->name() < r->name();
    });
}

void TitleLoader::scanSaves(void)
{
    std::u16string chkpntDir = StringUtils::UTF8toUTF16("/3ds/Checkpoint/saves");
    Directory checkpoint(Archive::sd(), chkpntDir);
    std::u16string sSeparator = StringUtils::UTF8toUTF16("/");
    for (size_t i = 0; i < ctrTitleIds.size(); i++)
    {
        u32 uniqueId = (u32) ctrTitleIds[i] >> 8;
        std::string id = StringUtils::format("0x%05X", uniqueId);
        std::vector<std::string> saves;
        for (size_t j = 0; j < checkpoint.count(); j++)
        {
            if (checkpoint.folder(j))
            {
                std::u16string fileName = checkpoint.item(j);
                if (fileName.substr(0, 7) == StringUtils::UTF8toUTF16(id.c_str()))
                {
                    Directory subdir(Archive::sd(), chkpntDir + sSeparator + fileName);
                    for (size_t k = 0; k < subdir.count(); k++)
                    {
                        if (subdir.folder(k))
                        {
                            std::u16string savePath = chkpntDir + sSeparator + fileName + sSeparator + subdir.item(k)
                                                      + StringUtils::UTF8toUTF16("/main");
                            saves.push_back(StringUtils::UTF16toUTF8(savePath));
                        }
                    }
                }
            }
        }
        auto others = Configuration::getInstance().extraSaves(id);
        if (!others.first.empty())
        {
            for (auto saveDir : others.first)
            {
                Directory dir(Archive::sd(), StringUtils::UTF8toUTF16(saveDir));
                for (size_t j = 0; j < dir.count(); j++)
                {
                    if (dir.folder(j))
                    {
                        saves.push_back(saveDir + StringUtils::UTF16toUTF8(sSeparator + dir.item(j)) + "/main");
                    }
                }
            }
        }
        if (!others.second.empty())
        {
            for (auto save : others.second)
            {
                saves.push_back(save);
            }
        }
        sdSaves[id] = saves;
    }

    for (size_t game = 0; game < 9; game++)
    {
        for (size_t lang = 0; lang < 8; lang++)
        {
            std::string id = std::string(dsIds[game]) + langIds[lang];
            std::vector<std::string> saves;
            for (size_t i = 0; i < checkpoint.count(); i++)
            {
                if (checkpoint.folder(i))
                {
                    std::u16string fileName = checkpoint.item(i);
                    if (StringUtils::UTF16toUTF8(fileName).substr(0, 4) == id)
                    {
                        Directory subdir(Archive::sd(), chkpntDir + sSeparator + fileName);
                        {
                            for(size_t j = 0; j < subdir.count(); j++)
                            {
                                if (subdir.folder(j))
                                {
                                    std::u16string savePath = chkpntDir + sSeparator + fileName + sSeparator + subdir.item(j) + sSeparator
                                                              + fileName.substr(5) + StringUtils::UTF8toUTF16(".sav");
                                    saves.push_back(StringUtils::UTF16toUTF8(savePath));
                                }
                            }
                        }
                    }
                }
            }
            auto others = Configuration::getInstance().extraSaves(id);
            if (!others.first.empty())
            {
                for (auto saveDir : others.first)
                {
                    Directory dir(Archive::sd(), StringUtils::UTF8toUTF16(saveDir));
                    for (size_t i = 0; i < dir.count(); i++)
                    {
                        if (dir.folder(i))
                        {
                            std::u16string thisDir = StringUtils::UTF8toUTF16(saveDir) + sSeparator + dir.item(i);
                            Directory subdir(Archive::sd(), thisDir);
                            for (size_t j = 0; j < subdir.count(); j++)
                            {
                                std::u16string file = subdir.item(j);
                                if (file.substr(file.size() - 3) == StringUtils::UTF8toUTF16("sav"))
                                {
                                    saves.push_back(StringUtils::UTF16toUTF8(thisDir + sSeparator + file));
                                }
                            }
                        }
                    }
                }
            }
            if (!others.second.empty())
            {
                for (auto save : others.second)
                {
                    saves.push_back(save);
                }
            }
            sdSaves[id] = saves;
        }
    }
}

void TitleLoader::backupSave()
{
    char stringTime[15] = {0};
    time_t unixTime = time(NULL);
    struct tm* timeStruct = gmtime((const time_t *)&unixTime);
    std::strftime(stringTime, 14,"%Y%m%d%H%M%S", timeStruct);
    std::string path = "/3ds/PKSM/backups/";
    if (loadedTitle)
    {
        path += loadedTitle->checkpointPrefix();
    }
    else
    {
        path += saveFileName.substr(saveFileName.find_last_of('/') + 1);
        path = path.substr(0, path.find(".sav"));
    }
    mkdir(path.c_str(), 777);
    path += '/' + std::string(stringTime);
    mkdir(path.c_str(), 777);
    if (!loadedTitle || saveIsFile)
    {
        path += '/' + saveFileName.substr(saveFileName.find_last_of('/') + 1);
    }
    else
    {
        if (save->generation() == 4 || save->generation() == 5)
        {
            path += '/' + loadedTitle->name() + ".sav";
        }
        else
        {
            path += "/main";
        }
    }
    FSStream out = FSStream(Archive::sd(), StringUtils::UTF8toUTF16(path), FS_OPEN_WRITE | FS_OPEN_CREATE, TitleLoader::save->length);
    if (out.good())
    {
        out.write(TitleLoader::save->data, TitleLoader::save->length);
    }
    else
    {
        Gui::warn("Could not open backup file!");
    }
    out.close();
}

bool TitleLoader::load(std::shared_ptr<Title> title)
{
    saveIsFile = false;
    loadedTitle = title;
    if (title->mediaType() == FS_MediaType::MEDIATYPE_SD || title->cardType() == FS_CardType::CARD_CTR)
    {
        FS_Archive archive;
        Archive::save(&archive, title->mediaType(), title->lowId(), title->highId());
        FSStream in(archive, u"/main", FS_OPEN_READ);
        if (in.good())
        {
            u8* data = new u8[in.size()];
            in.read(data, in.size());
            save = Sav::getSave(data, in.size());
            in.close();
            delete[] data;
            FSUSER_CloseArchive(archive);
            return true;
        }
        else
        {
            Gui::warn("Could not open save!");
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
            Gui::warn("Wrong size for this game!", std::string("Please report to FlagBrew"));
            return false;
        }

        u8* data = new u8[cap];
        u32 sectorSize = (cap < 0x10000) ? cap : 0x10000;

        for (u32 i = 0; i < cap / sectorSize; ++i) {
            SPIReadSaveData(title->SPICardType(), sectorSize * i, data + sectorSize * i, sectorSize);
        }

        save = Sav::getSave(data, cap);
        if (save != nullptr)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    Gui::warn("This should never happen!");
    return false;
}

bool TitleLoader::load(std::shared_ptr<Title> title, std::string savePath)
{
    saveIsFile = true;
    saveFileName = savePath;
    loadedTitle = title;
    FSStream in(Archive::sd(), StringUtils::UTF8toUTF16(savePath), FS_OPEN_READ);
    u32 size;
    u8* saveData = nullptr;
    if (in.good())
    {
        size = in.size();
        saveData = new u8[size];
        in.read(saveData, size);
    }
    else
    {
        Gui::warn("Could not open save!");
        loadedTitle = nullptr;
        saveFileName = "";
        in.close();
        return false;
    }
    in.close();
    save = Sav::getSave(saveData, size);
    if (Configuration::getInstance().autoBackup())
    {
        Threads::create((ThreadFunc)&TitleLoader::backupSave);
    }
    delete[] saveData;
    return true;
}

void TitleLoader::saveToTitle(bool ask)
{
    // Just an extra check
    if (loadedTitle)
    {
        if (TitleLoader::cardTitle == loadedTitle && (!ask || Gui::showChoiceMessage("Would you like to write changes to", std::string("the game card?"))))
        {
            auto& title = TitleLoader::cardTitle;
            if (title->cardType() == FS_CardType::CARD_CTR)
            {
                FS_Archive archive;
                Archive::save(&archive, title->mediaType(), title->lowId(), title->highId());
                FSStream out(archive, u"/main", FS_OPEN_WRITE | FS_OPEN_CREATE, save->length);
                if (out.good())
                {
                    out.write(save->data, save->length);
                }
                else
                {
                    Gui::warn("Could not open save!");
                }
                out.close();
                FSUSER_CloseArchive(archive);
            }
            else
            {
                u32 pageSize = SPIGetPageSize(title->SPICardType());

                for (u32 i = 0; i < save->length / pageSize; ++i)
                {
                    
                    if (R_FAILED(SPIWriteSaveData(title->SPICardType(), pageSize * i, save->data + pageSize * i, pageSize)))
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            // Just a linear search because it's a maximum of eight titles
            for (auto title : TitleLoader::nandTitles)
            {
                if (title == loadedTitle && (!ask && Gui::showChoiceMessage("Would you like to write changes to", std::string("the installed title?"))))
                {
                    FS_Archive archive;
                    Archive::save(&archive, title->mediaType(), title->lowId(), title->highId());
                    FSStream out(archive, u"/main", FS_OPEN_WRITE | FS_OPEN_CREATE, save->length);
                    if (out.good())
                    {
                        out.write(save->data, save->length);
                    }
                    else
                    {
                        Gui::warn("Could not open save!");
                    }
                    out.close();
                    FSUSER_CloseArchive(archive);
                    break; // There can only be one match
                }
            }
        }
    }
}

void TitleLoader::saveChanges()
{
    save->resign();
    if (saveIsFile)
    {
        // No need to check size; if it was read successfully, that means that it has the correct size
        FSStream out(Archive::sd(), StringUtils::UTF8toUTF16(saveFileName), FS_OPEN_WRITE);
        out.write(save->data, save->length);
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
}

void TitleLoader::exit()
{
    nandTitles.clear();
    cardTitle = nullptr;
    loadedTitle = nullptr;
}