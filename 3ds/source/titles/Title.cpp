/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2019 Bernardo Giordano, Admiral Fish, piepie62
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

// Allocate once because threading shenanigans
static constexpr Tex3DS_SubTexture dsIconSubt3x = {32, 32, 0.0f, 1.0f, 1.0f, 0.0f};
static C2D_Image dsIcon                         = {nullptr, &dsIconSubt3x};

Title::~Title(void)
{
    if (mIcon.tex && mIcon.tex != dsIcon.tex)
    {
        C3D_TexDelete(mIcon.tex);
        delete mIcon.tex;
    }
}

struct bannerData
{
    u16 version;
    u16 crc;
    u8 reserved[28];
    u8 data[512];
    u16 palette[16];
    u16 titles[8][128];
    u8 reserved2[0x800];
    u8 dsiData[8][512];
    u16 dsiPalettes[8][16];
    u16 dsiSequence[64];
};

static void loadDSIcon(bannerData* iconData)
{
    static constexpr int WIDTH_POW2  = 32;
    static constexpr int HEIGHT_POW2 = 32;
    if (!dsIcon.tex)
    {
        dsIcon.tex = new C3D_Tex;
        C3D_TexInit(dsIcon.tex, WIDTH_POW2, HEIGHT_POW2, GPU_RGB565);
        dsIcon.tex->border = 0xFFFFFFFF;
        C3D_TexSetWrap(dsIcon.tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
    }

    u16* output = (u16*)dsIcon.tex->data;
    for (size_t x = 0; x < 32; x++)
    {
        for (size_t y = 0; y < 32; y++)
        {
            u32 srcOff   = (((y >> 3) * 4 + (x >> 3)) * 8 + (y & 7)) * 4 + ((x & 7) >> 1);
            u32 srcShift = (x & 1) * 4;

            u16 pIndex = (iconData->data[srcOff] >> srcShift) & 0xF;
            u16 color  = 0xFFFF;
            if (pIndex != 0)
            {
                u16 r = iconData->palette[pIndex] & 0x1F;
                u16 g = (iconData->palette[pIndex] >> 5) & 0x1F;
                u16 b = (iconData->palette[pIndex] >> 10) & 0x1F;
                color = (r << 11) | (g << 6) | (g >> 4) | (b);
            }

            u32 dst     = ((((y >> 3) * (WIDTH_POW2 >> 3) + (x >> 3)) << 6) +
                       ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3)));
            output[dst] = color;
        }
    }
}

static C2D_Image loadTextureIcon(smdh_s* smdh)
{
    C3D_Tex* tex                          = new C3D_Tex;
    static const Tex3DS_SubTexture subt3x = {48, 48, 0.0f, 48 / 64.0f, 48 / 64.0f, 0.0f};
    C3D_TexInit(tex, 64, 64, GPU_RGB565);
    tex->border = 0xFFFFFFFF;
    C3D_TexSetWrap(tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);

    u16* dest = (u16*)tex->data + (64 - 48) * 64;
    u16* src  = (u16*)smdh->bigIconData;
    for (int j = 0; j < 48; j += 8)
    {
        std::copy(src, src + 48 * 8, dest);
        src += 48 * 8;
        dest += 64 * 8;
    }

    return C2D_Image{tex, &subt3x};
}

bool Title::load(u64 id, FS_MediaType media, FS_CardType card)
{
    bool loadTitle = false;
    mId            = id;
    mMedia         = media;
    mCard          = card;

    if (mCard == CARD_CTR)
    {
        smdh_s* smdh = loadSMDH(lowId(), highId(), mMedia);
        if (smdh == NULL)
        {
            return false;
        }

        mName   = StringUtils::UTF16toUTF8((char16_t*)smdh->applicationTitles[1].shortDescription);
        mPrefix = StringUtils::format("0x%05X", lowId() >> 8);

        if (Archive::saveAccessible(mMedia, lowId(), highId()))
        {
            loadTitle = true;
            mIcon     = loadTextureIcon(smdh);
        }
        delete smdh;
    }
    else
    {
        u8* headerData = new u8[0x3B4];
        Result res     = FSUSER_GetLegacyRomHeader(mMedia, 0LL, headerData);
        if (R_FAILED(res))
        {
            delete[] headerData;
            return false;
        }

        char _cardTitle[14] = {0};
        char _gameCode[6]   = {0};
        std::copy(headerData, headerData + 12, _cardTitle);
        _cardTitle[13] = '\0';
        std::copy(headerData + 12, headerData + 16, _gameCode);
        _gameCode[5] = '\0';
        mPrefix      = _gameCode;

        bool infrared = headerData[12] == 'I';

        delete[] headerData;
        bannerData* banner = new bannerData;
        FSUSER_GetLegacyBannerData(mMedia, 0LL, (u8*)banner);
        loadDSIcon(banner);
        mIcon = dsIcon;
        delete banner;

        res = SPIGetCardType(&mCardType, infrared);
        if (R_FAILED(res))
        {
            return false;
        }

        mName     = std::string(_cardTitle);
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
