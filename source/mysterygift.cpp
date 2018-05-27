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

#include "mysterygift.hpp"

static nlohmann::json mysteryGiftSheet;
static u8* mysteryGiftData;

void MysteryGift::init(u8 gen)
{
    std::ifstream sheet(StringUtils::format("romfs:/mg/sheet%d.json", gen));
    sheet >> mysteryGiftSheet;
    sheet.close();

    std::ifstream data(StringUtils::format("romfs:/mg/data%d.bin", gen), std::ios::binary);
    data.seekg(0, std::ios::end);
    size_t size = data.tellg();
    data.seekg(0, std::ios::beg);

    mysteryGiftData = new u8[size];
    data.read((char*)mysteryGiftData, size);
    data.close();
}

void MysteryGift::exit(void)
{
    delete[] mysteryGiftData;
}