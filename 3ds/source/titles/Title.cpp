/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2022 Bernardo Giordano, Admiral Fish, piepie62
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
#include "Archive.hpp"
#include "smdh.hpp"
#include "utils/logging.hpp"
#include <format>

// Allocate once because threading shenanigans
namespace
{
    constexpr Tex3DS_SubTexture dsIconSubt3x = {32, 32, 0.0f, 1.0f, 1.0f, 0.0f};
    C3D_Tex dsIconTex;
    C2D_Image dsIcon = {nullptr, &dsIconSubt3x};

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

    void loadDSIcon(bannerData* iconData)
    {
        Logging::debug("Title::loadDSIcon - Loading DS icon");
        static constexpr int WIDTH_POW2  = 32;
        static constexpr int HEIGHT_POW2 = 32;
        if (!dsIcon.tex)
        {
            dsIcon.tex = &dsIconTex;
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
                           ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) |
                               ((x & 4) << 2) | ((y & 4) << 3)));
                output[dst] = color;
            }
        }
        Logging::debug("Title::loadDSIcon - DS icon loaded successfully");
    }

    C2D_Image loadTextureIcon(smdh_s* smdh)
    {
        Logging::debug("Title::loadTextureIcon - Loading texture icon");
        C3D_Tex* tex                              = new C3D_Tex;
        static constexpr Tex3DS_SubTexture subt3x = {48, 48, 0.0f, 48 / 64.0f, 48 / 64.0f, 0.0f};
        C3D_TexInit(tex, 64, 64, GPU_RGB565);
        tex->border = 0xFFFFFFFF;
        C3D_TexSetWrap(tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);

        u16* dest = (u16*)tex->data + (64 - 48) * 64;
        u16* src  = (u16*)smdh->bigIconData;
        for (int j = 0; j < 48; j += 8)
        {
            std::copy(src, src + 48 * 8, dest);
            src  += 48 * 8;
            dest += 64 * 8;
        }

        Logging::debug("Title::loadTextureIcon - Texture icon loaded successfully");
        return C2D_Image{tex, &subt3x};
    }
}

Title::~Title(void)
{
    if (mIcon.tex && mIcon.tex != dsIcon.tex)
    {
        C3D_TexDelete(mIcon.tex);
        delete mIcon.tex;
    }
}

bool Title::load(u64 id, FS_MediaType media, FS_CardType card)
{
    Logging::info("Title::load - Loading title ID: {:016X}, media type: {}, card type: {}", id,
        (int)media, (int)card);
    bool loadTitle = false;
    mGba           = false;
    mGb            = false;
    mId            = id;
    mMedia         = media;
    mCard          = card;

    if (mCard == CARD_CTR)
    {
        Logging::debug("Title::load - Loading CTR card title");
        smdh_s* smdh = loadSMDH(lowId(), highId(), mMedia);
        if (smdh == NULL)
        {
            Logging::error("Title::load - Failed to load SMDH for title ID: {:016X}", id);
            return false;
        }

        mName   = StringUtils::UTF16toUTF8((char16_t*)smdh->applicationTitles[1].shortDescription);
        mPrefix = std::format("0x{:05X}", lowId() >> 8);
        Logging::debug("Title::load - Loaded title name: {}, prefix: {}", mName, mPrefix);

        Archive archive = Archive::save(mMedia, lowId(), highId(), false);
        if (R_SUCCEEDED(archive.result()))
        {
            Logging::debug("Title::load - Successfully opened save archive");
            loadTitle                 = true;
            mIcon                     = loadTextureIcon(smdh);
            std::unique_ptr<File> out = archive.file(u"/main", FS_OPEN_READ);
            if (!out)
            {
                Logging::debug("Title::load - No /main file found, checking for /sav.dat");
                out = archive.file(u"/sav.dat", FS_OPEN_READ);
                if (out)
                {
                    Logging::debug("Title::load - Found /sav.dat, marking as GB title");
                    mGb = true;
                    out->close();
                }
            }
            else
            {
                Logging::debug("Title::load - Found /main file");
                out->close();
            }
        }
        // Is it a GBA save? GBA saves are not in the normal archive format
        else
        {
            Logging::debug("Title::load - Regular save archive not found, checking for GBA save");
            archive = Archive::saveAndContents(mMedia, lowId(), highId(), true);
            if (R_SUCCEEDED(archive.result()))
            {
                Logging::debug("Title::load - Found saveAndContents archive");
                static constexpr u32 pathData[5] = {
                    1,   // Save data
                    1,   // TMD content index
                    3,   // Type: save data?
                    0, 0 // No EXEFS file name needed
                };
                auto out =
                    archive.file(FS_Path{PATH_BINARY, sizeof(pathData), pathData}, FS_OPEN_READ);
                if (out)
                {
                    Logging::debug("Title::load - Successfully opened GBA save file");
                    mGba      = true;
                    loadTitle = true;
                    mIcon     = loadTextureIcon(smdh);
                    out->close();
                }
                else
                {
                    Logging::warning(
                        "Title::load - Failed to open GBA save file, archive result: {}",
                        archive.result());
                }
                archive.close();
            }
            else
            {
                Logging::warning(
                    "Title::load - Failed to open saveAndContents archive: {}", archive.result());
            }
        }
        delete smdh;
    }
    else
    {
        Logging::debug("Title::load - Loading DS card title");
        u8* headerData = new u8[0x3B4];
        Result res     = FSUSER_GetLegacyRomHeader(mMedia, 0LL, headerData);
        if (R_FAILED(res))
        {
            Logging::error("Title::load - Failed to get legacy ROM header: {}", res);
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
        Logging::debug("Title::load - DS card title: {}, game code: {}, infrared: {}", _cardTitle,
            _gameCode, infrared);

        delete[] headerData;
        bannerData* banner = new bannerData;
        res                = FSUSER_GetLegacyBannerData(mMedia, 0LL, (u8*)banner);
        if (R_FAILED(res))
        {
            Logging::warning("Title::load - Failed to get legacy banner data: {}", res);
        }
        loadDSIcon(banner);
        mIcon = dsIcon;
        delete banner;

        res = SPIGetCardType(&mCardType, infrared);
        if (R_FAILED(res))
        {
            Logging::error("Title::load - Failed to get SPI card type: {}", res);
            return false;
        }
        Logging::debug("Title::load - SPI card type: {}", (int)mCardType);

        mName     = std::string(_cardTitle);
        loadTitle = true;
    }

    Logging::info("Title::load - Title loaded successfully: {}, loadTitle: {}", mName, loadTitle);
    return loadTitle;
}

u32 Title::highId(void) const
{
    return (u32)(mId >> 32);
}

u32 Title::lowId(void) const
{
    return (u32)mId;
}

std::string Title::name(void) const
{
    return mName;
}

CardType Title::SPICardType(void) const
{
    return mCardType;
}

C2D_Image Title::icon(void) const
{
    return mIcon;
}

FS_MediaType Title::mediaType(void) const
{
    return mMedia;
}

FS_CardType Title::cardType(void) const
{
    return mCard;
}

bool Title::gba(void) const
{
    return mGba;
}

bool Title::gb(void) const
{
    return mGb;
}

std::string Title::checkpointPrefix(void) const
{
    return mPrefix;
}
