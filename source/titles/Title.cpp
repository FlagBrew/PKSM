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

#include "Title.hpp"

Title::~Title(void)
{
    if (mCard != CARD_TWL)
        C3D_TexDelete(mIcon.tex);
}

static C2D_Image loadTextureIcon(smdh_s *smdh)
{
    C3D_Tex* tex = new C3D_Tex;
    static const Tex3DS_SubTexture subt3x = { 48, 48, 0.0f, 48/64.0f, 48/64.0f, 0.0f };
    C3D_TexInit(tex, 64, 64, GPU_RGB565);

    u16* dest = (u16*)tex->data + (64-48)*64;
    u16* src = (u16*)smdh->bigIconData;
    for (int j = 0; j < 48; j += 8)
    {
        std::copy(src, src + 48*8, dest);
        src += 48*8;
        dest += 64*8;
    }
    
    return C2D_Image {tex, &subt3x};
}

bool Title::load(u64 id, FS_MediaType media, FS_CardType card)
{
    bool loadTitle = false;
    mId = id;
    mMedia = media;
    mCard = card;

    if (mCard == CARD_CTR)
    {
        smdh_s* smdh = loadSMDH(lowId(), highId(), mMedia);
        if (smdh == NULL)
        {
            return false;
        }

        mName = StringUtils::UTF16toUTF8((char16_t*)smdh->applicationTitles[1].shortDescription);
        mPrefix = StringUtils::format("0x%05X", lowId() >> 8);

        if (Archive::saveAccessible(mMedia, lowId(), highId()))
        {
            loadTitle = true;
            mIcon = loadTextureIcon(smdh);
        }
        delete smdh;
    }
    else
    {
        u8* headerData = new u8[0x3B4];
        Result res = FSUSER_GetLegacyRomHeader(mMedia, 0LL, headerData);
        if (R_FAILED(res))
        {
            delete[] headerData;
            return false;
        }
        
        char _cardTitle[14] = {0};
        char _gameCode[6] = {0};
        std::copy(headerData, headerData + 12, _cardTitle);
        _cardTitle[13] = '\0';
        std::copy(headerData + 12, headerData + 16, _gameCode);
        _gameCode[5] = '\0';
        mPrefix = _gameCode;

        res = SPIGetCardType(&mCardType, (headerData[12] == 'I') ? 1 : 0);
        delete[] headerData;
        if (R_FAILED(res))
        {
            return false;
        }

        mName = std::string(_cardTitle);
        mIcon = Gui::TWLIcon();
        loadTitle = true;
    }

    return loadTitle;
}

u32 Title::highId(void)
{
    return (u32)(mId >> 32);
}

u32 Title::lowId(void)
{
    return (u32)mId;
}

std::string Title::name(void)
{
    return mName;
}

CardType Title::SPICardType(void)
{
    return mCardType;
}

C2D_Image Title::icon(void)
{
    return mIcon;
}

FS_MediaType Title::mediaType(void)
{
    return mMedia;
}

FS_CardType Title::cardType(void)
{
    return mCard;
}

std::string Title::checkpointPrefix(void)
{
    return mPrefix;
}