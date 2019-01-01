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

#ifndef TITLE_HPP
#define TITLE_HPP

#include <3ds.h>
#include <algorithm>
#include <citro2d.h>
#include <string>
#include "archive.hpp"
#include "gui.hpp"
#include "spi.hpp"
#include "utils.hpp"

class Title 
{
public:
    Title(void) = default;
    ~Title(void);

    bool load(u64 id, FS_MediaType mediaType, FS_CardType cardType);
    CardType SPICardType(void);
    u32 highId(void);
    u32 lowId(void);
    std::string name(void);
    C2D_Image icon(void);
    FS_MediaType mediaType(void);
    FS_CardType cardType(void);

    std::string checkpointPrefix(void);

private:
    u64 mId;
    FS_MediaType mMedia;
    FS_CardType mCard;
    CardType mCardType;
    C2D_Image mIcon;
    std::string mName;
    std::string mPrefix;
};

#endif