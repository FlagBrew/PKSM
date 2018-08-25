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

// title list
std::vector<std::shared_ptr<Title>> TitleLoader::nandTitles;
std::shared_ptr<Title> TitleLoader::cardTitle = nullptr;

// local gui variables and functions
static const size_t rowlen = 4;
static const size_t collen = 8;

static int selectorX(size_t i)
{
    return 50*((i % (rowlen*collen)) % collen);
}

static int selectorY(size_t i)
{
    return 20 + 50*((i % (rowlen*collen)) / collen);
}

static void drawSelector(size_t idx)
{
    static const int w = 2;
    const int x = selectorX(idx);
    const int y = selectorY(idx);
    C2D_DrawRectSolid(         x,          y, 0.5f, 50,       50, C2D_Color32(255, 255, 255, 100)); 
    C2D_DrawRectSolid(         x,          y, 0.5f, 50,        w, COLOR_YELLOW); // top
    C2D_DrawRectSolid(         x,      y + w, 0.5f,  w, 50 - 2*w, COLOR_YELLOW); // left
    C2D_DrawRectSolid(x + 50 - w,      y + w, 0.5f,  w, 50 - 2*w, COLOR_YELLOW); // right
    C2D_DrawRectSolid(         x, y + 50 - w, 0.5f, 50,        w, COLOR_YELLOW); // bottom
}

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
}

std::shared_ptr<Sav> TitleLoader::load(std::shared_ptr<Title>)
{
    return nullptr;
}

// std::shared_ptr<Sav> TitleLoader::load(void)
// {
//     // start to retrieve our nandTitles
//     Threads::create((ThreadFunc)scan);

//     // create hid object
//     Hid hid(rowlen * collen, collen);
//     while (aptMainLoop() & !(hidKeysDown() & KEY_B))
//     {
//         // update selector
//         hidScanInput();
//         hid.update(nandTitles.size());

//         // start frame
//         C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
//         C2D_TargetClear(g_renderTargetTop, COLOR_BLACK);
//         C2D_TargetClear(g_renderTargetBottom, COLOR_BLACK);

//         C2D_SceneBegin(g_renderTargetTop);
//         Gui::backgroundTop(false);

//         for (size_t k = hid.page() * hid.maxVisibleEntries(); 
//             k < hid.page() * hid.maxVisibleEntries() + hid.maxEntries(nandTitles.size()) + 1; 
//             k++)
//         {
//             C2D_DrawImageAt(nandTitles.at(k)->icon(), selectorX(k) + 1, selectorY(k) + 1, 0.5f, NULL, 1.0f, 1.0f);
//         }

//         if (nandTitles.size() > 0)
//         {
//             drawSelector(hid.index());
//         }

//         C2D_SceneBegin(g_renderTargetBottom);
//         Gui::backgroundBottom(false);

//         // TODO: remove, debug
//         if (nandTitles.size() > 0)
//             Gui::dynamicText(nandTitles.at(hid.fullIndex())->name(), 10, 10, 0.5f, 0.5f, COLOR_WHITE);
        
//         C3D_FrameEnd(0);
//         Gui::clearTextBufs();  
//     }

//     return nullptr;
// }

void TitleLoader::exit()
{
    nandTitles.clear();
    cardTitle = nullptr;
}