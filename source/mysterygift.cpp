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

void MysteryGift::init(Generation g)
{
    std::ifstream sheet(StringUtils::format("romfs:/mg/sheet%s.json", genToString(g).c_str()));
    sheet >> mysteryGiftSheet;
    sheet.close();

    std::ifstream data(StringUtils::format("romfs:/mg/data%s.bin", genToString(g).c_str()), std::ios::binary | std::ios::ate);
    size_t size = data.tellg();
    data.seekg(0, std::ios::beg);

    mysteryGiftData = new u8[size];
    data.read((char*)mysteryGiftData, size);
    data.close();
}

std::unique_ptr<WCX> MysteryGift::wondercard(size_t index)
{
    std::string gen = mysteryGiftSheet["gen"];

    auto entry = mysteryGiftSheet["wondercards"][index];

    u32 offset = entry["offset"];
    u32 size = entry["size"];

    u8 *data = new u8[size];
    std::copy(mysteryGiftData + offset, mysteryGiftData + offset + size, data);

    if (gen == "4")
    {
        std::unique_ptr<WCX> wc = nullptr;
        if (entry["type"] == "wc4")
        {
            wc = std::make_unique<WC4>(data);
        }
        else
        {
            wc = std::make_unique<PGT>(data);
        }
        delete[] data;
        return wc;
    }
    else if (gen == "5")
    {
        PGF *pgf = new PGF(data);
        delete[] data;
        return std::unique_ptr<WCX>(pgf);
    }
    else if (gen == "6")
    {
        WC6 *wc6 = new WC6(data, entry["type"].get<std::string>().find("full") != std::string::npos);
        delete[] data;
        return std::unique_ptr<WCX>(wc6);
    }
    else if (gen == "7")
    {
        WC7 *wc7 = new WC7(data, entry["type"].get<std::string>().find("full") != std::string::npos);
        delete[] data;
        return std::unique_ptr<WCX>(wc7);
    }
    else if (gen == "LGPE")
    {
        WB7 *wb7 = new WB7(data, entry["type"].get<std::string>().find("full") != std::string::npos);
        delete[] data;
        return std::unique_ptr<WCX>(wb7);
    }
    else
    {
        delete[] data;
        return nullptr;
    }
}

void MysteryGift::exit(void)
{
    delete[] mysteryGiftData;
    mysteryGiftSheet.clear();
}

std::vector<nlohmann::json> MysteryGift::wondercards()
{
    return mysteryGiftSheet["matches"];
}

MysteryGift::giftData MysteryGift::wondercardInfo(size_t index)
{
    giftData ret;
    nlohmann::json entry = mysteryGiftSheet["wondercards"][index];
    ret.name = entry["name"].get<std::string>();
    ret.game = entry["game"].get<std::string>();
    ret.form = entry["form"];
    ret.species = entry["species"];
    return ret;
}