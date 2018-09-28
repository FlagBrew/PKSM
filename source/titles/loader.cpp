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

// title list
std::vector<std::shared_ptr<Title>> TitleLoader::nandTitles;
std::shared_ptr<Title> TitleLoader::cardTitle = nullptr;
std::unordered_map<std::string, std::vector<std::string>> TitleLoader::sdSaves;
std::shared_ptr<Sav> TitleLoader::save;

void TitleLoader::scan(void)
{
    // known 3ds title ids
    static const std::vector<unsigned long long> ctrTitleIds = {
        0x0004000000055D00, // X
        0x0004000000055E00, // Y
        0x000400000011C400, // OR
        0x000400000011C500, // AS
        0x0004000000164800, // Sun
        0x0004000000175E00, // Moon
        0x00040000001B5000, // Ultrasun
        0x00040000001B5100  // Ultramoon 
    };

    Result res = 0;

    // clear title list if filled previously
    nandTitles.clear();

    // check for cartridge and push at the beginning of the title list

    // get our cartridge
    FS_CardType cardType;
    res = FSUSER_GetCardType(&cardType);
    if (R_FAILED(res))
    {
        return;
    }

    u32 count = 0;

    if (cardType == CARD_CTR)
    {
        // get count of titles
        res = AM_GetTitleCount(MEDIATYPE_GAME_CARD, &count);
        if (R_FAILED(res))
        {
            return;
        }

        if (count > 0)
        {
            u64 id;
            res = AM_GetTitleList(NULL, MEDIATYPE_GAME_CARD, count, &id);
            if (R_FAILED(res))
            {
                return;
            }

            // check if this id is in our list
            if (std::find(ctrTitleIds.begin(), ctrTitleIds.end(), id) != ctrTitleIds.end())
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
                    if (StringUtils::UTF16toUTF8(fileName).substr(0, 7) == id)
                    {
                        Directory subdir(Archive::sd(), chkpntDir + sSeparator + fileName);
                        {
                            for(size_t j = 0; j < subdir.count(); j++)
                            {
                                if (subdir.folder(j))
                                {
                                    std::u16string savePath = chkpntDir + sSeparator + fileName + sSeparator + subdir.item(j) + sSeparator
                                                              + subdir.item(j).substr(5) + StringUtils::UTF8toUTF16(".sav");
                                    saves.push_back(StringUtils::UTF16toUTF8(savePath));
                                }
                            }
                        }
                    }
                }
            }
            sdSaves[id] = saves;
        }
    }
}

static std::string folderPrefix()
{
    switch (TitleLoader::save->version())
    {
        case 10:
            return "d";
            break;
        case 11:
            return "p";
            break;
        case 12:
            return "pt";
            break;
        case 7:
            return "hg";
            break;
        case 8:
            return "ss";
            break;
        case 20:
            return "b";
            break;
        case 21:
            return "w";
            break;
        case 22:
            return "b2";
            break;
        case 23:
            return "w2";
            break;
        case 24:
            return "x";
            break;
        case 25:
            return "y";
            break;
        case 26:
            return "or";
            break;
        case 27:
            return "as";
            break;
        case 30:
            return "su";
            break;
        case 31:
            return "mo";
            break;
        case 32:
            return "us";
            break;
        case 33:
            return "um";
            break;
        default:
            Gui::warn("Couldn't identify save type!");
            return "";
    }
}

static std::string backupName()
{
    switch (TitleLoader::save->version())
    {
        case 10:
            return "POKEMON D.sav";
            break;
        case 11:
            return "POKEMON P.sav";
            break;
        case 12:
            return "POKEMON PT.sav";
            break;
        case 7:
            return "POKEMON HG.sav";
            break;
        case 8:
            return "POKEMON SS.sav";
            break;
        case 20:
            return "POKEMON B.sav";
            break;
        case 21:
            return "POKEMON W.sav";
            break;
        case 22:
            return "POKEMON B2.sav";
            break;
        case 23:
            return "POKEMON W2.sav";
            break;
        case 24:
        case 25:
        case 26:
        case 27:
        case 30:
        case 31:
        case 32:
        case 33:
            return "main";
            break;
        default:
            Gui::warn("Couldn't identify save type!", std::string("This should really never happen!"));
            return "";
    }
}

void TitleLoader::backupSave()
{
    char stringTime[15] = {0};
    time_t unixTime = time(NULL);
    struct tm* timeStruct = gmtime((const time_t *)&unixTime);
    std::strftime(stringTime, 14,"%Y%m%d%H%M%S", timeStruct);
    std::string gameFolder = "/3ds/PKSM/backups/" + folderPrefix();
    mkdir(gameFolder.c_str(), 777);
    std::string folderName = gameFolder + '/' + stringTime;
    mkdir(folderName.c_str(), 777);
    std::string filePath = folderName + '/' + backupName();
    FSStream out = FSStream(Archive::sd(), StringUtils::UTF8toUTF16(filePath), FS_OPEN_WRITE | FS_OPEN_CREATE, TitleLoader::save->length);
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

void TitleLoader::load(std::shared_ptr<Title> title)
{
    return;
}

void TitleLoader::load(std::string savePath)
{
    FSStream in(Archive::sd(), StringUtils::UTF8toUTF16(savePath), FS_OPEN_READ);
    u32 size = in.size();
    u8* saveData = new u8[size];
    in.read(saveData, size);
    in.close();
    save = Sav::getSave(saveData, size);
    if (Configuration::getInstance().autoBackup())
    {
        Threads::create((ThreadFunc)&TitleLoader::backupSave);
    }
}

void TitleLoader::exit()
{
    nandTitles.clear();
    cardTitle = nullptr;
}