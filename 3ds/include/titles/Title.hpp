/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2021 Bernardo Giordano, Admiral Fish, piepie62
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

#ifndef TITLE_HPP
#define TITLE_HPP

#include "format.h"
#include "spi.hpp"
#include <3ds.h>
#include <algorithm>
#include <citro2d.h>
#include <string>

class Title
{
public:
    Title(void) = default;
    ~Title(void);

    bool load(u64 id, FS_MediaType mediaType, FS_CardType cardType);
    CardType SPICardType(void) const;
    u32 highId(void) const;
    u32 lowId(void) const;

    u64 ID(void) const { return (u64)highId() << 32 | lowId(); }

    std::string name(void) const;
    C2D_Image icon(void) const;
    FS_MediaType mediaType(void) const;
    FS_CardType cardType(void) const;
    bool gba(void) const;
    bool gb(void) const;

    std::string checkpointPrefix(void) const;

    template <typename StrType = std::string>
        requires std::is_same_v<StrType, std::string> || std::is_same_v<StrType, std::u16string>
    static StrType tidToCheckpointPrefix(u64 tid)
    {
        if constexpr (std::is_same_v<std::string, StrType>)
        {
            return fmt::format<StrType>("0x{:05X}", ((u32)tid) >> 8);
        }
        else if constexpr (std::is_same_v<std::u16string, StrType>)
        {
            return fmt::format<StrType>(u"0x{:05X}", ((u32)tid) >> 8);
        }
    }

private:
    u64 mId;
    FS_MediaType mMedia;
    FS_CardType mCard;
    CardType mCardType;
    C2D_Image mIcon;
    std::string mName;
    std::string mPrefix;
    bool mGba;
    bool mGb;
};

#endif
