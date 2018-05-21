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

static std::vector<Title> titles;

void TitleLoader::scan(void)
{
    // known 3ds title ids
    static const std::vector<u64> ctrTitleIds = {
        0x0004000000055D00, // X
        0x0004000000055E00, // Y
        0x000400000011C400, // OR
        0x000400000011C500, // AS
        0x0004000000164800, // Sun
        0x0004000000175E00, // Moon
        0x00040000001B5000, // Ultrasun
        0x00040000001B5100  // Ultramoon 
    };

    // clear title list if filled previously
    titles.clear();

    // get title count
    u32 count = 0;
    Result res = AM_GetTitleCount(MEDIATYPE_SD, &count);
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

    for (size_t i = 0, sz = ctrTitleIds.size(); i < sz; i++)
    {
        u64 id = ctrTitleIds.at(i);
        if (std::find(ids.begin(), ids.end(), id) != ids.end())
        {
            Title title;
            if (title.load(id, MEDIATYPE_SD, CARD_CTR))
            {
                titles.push_back(title);
            }
        }
    }

    std::sort(titles.begin(), titles.end(), [](Title& l, Title& r) {
        return l.name() < r.name();
    });

    // get our cartridge
    FS_CardType cardType;
    res = FSUSER_GetCardType(&cardType);
    if (R_FAILED(res))
    {
        return;
    }

    if (cardType == CARD_CTR)
    {
        // do the same
        res = AM_GetTitleCount(MEDIATYPE_GAME_CARD, &count);
        if (R_FAILED(res) || count <= 0)
        {
            return;
        }

        u64 id;
        res = AM_GetTitleList(NULL, MEDIATYPE_GAME_CARD, count, &id);
        if (R_FAILED(res))
        {
            return;
        }

        // check if this id is in our list
        if (std::find(ctrTitleIds.begin(), ctrTitleIds.end(), id) != ctrTitleIds.end())
        {
            Title title;
            if (title.load(id, MEDIATYPE_GAME_CARD, cardType))
            {
                titles.push_back(title);
            }
        }
    }
    else
    {
        // ds game card, behave differently
        // load the save and check for known patterns
        Title title;
        if (title.load(0, MEDIATYPE_GAME_CARD, cardType))
        {
            CardType cardType = title.SPICardType();
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
                titles.insert(titles.begin(), title);
            }

            delete[] saveFile;
        }
    }
}

std::unique_ptr<Sav> TitleLoader::load(void)
{
    Threads::create((ThreadFunc)scan);

    while (aptMainLoop() & !(hidKeysDown() & KEY_B))
    {
        hidScanInput();

        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_SceneBegin(g_renderTargetTop);
        Gui::backgroundTop();
        C2D_SceneBegin(g_renderTargetBottom);
        Gui::backgroundBottom();
        C3D_FrameEnd(0);
        Gui::clearTextBufs();
    }

    return nullptr;
}