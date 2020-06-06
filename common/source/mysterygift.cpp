/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2020 Bernardo Giordano, Admiral Fish, piepie62
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
#include "io.hpp"
#include "nlohmann/json.hpp"
#include "utils.hpp"
#include "wcx/PCD.hpp"
#include "wcx/PGF.hpp"
#include "wcx/PGT.hpp"
#include "wcx/WB7.hpp"
#include "wcx/WC4.hpp"
#include "wcx/WC6.hpp"
#include "wcx/WC7.hpp"
#include "wcx/WC8.hpp"

namespace
{
    nlohmann::json mysteryGiftSheet;
    u8* mysteryGiftData;
}

void MysteryGift::init(pksm::Generation g)
{
    // Just in case cleanup did not occur properly
    delete[] mysteryGiftData;
    mysteryGiftData = nullptr;

    std::string sheetPath = "/3ds/PKSM/mysterygift/sheet" + (std::string)g + ".json.bz2";
    std::string dataPath  = "/3ds/PKSM/mysterygift/data" + (std::string)g + ".bin.bz2";
    if (!io::exists(sheetPath) || !io::exists(dataPath))
    {
        sheetPath = "romfs:/mg/sheet" + (std::string)g + ".json.bz2";
        dataPath  = "romfs:/mg/data" + (std::string)g + ".bin.bz2";
    }

    FILE* f = fopen(sheetPath.c_str(), "rb");
    if (f != NULL)
    {
        fseek(f, 0, SEEK_END);
        size_t size          = ftell(f);
        unsigned int destLen = 700 * 1024; // big enough
        char* s              = new char[size];
        char* d              = new char[destLen]();
        rewind(f);
        fread(s, 1, size, f);

        int r = BZ2_bzBuffToBuffDecompress(d, &destLen, s, size, 0, 0);
        if (r == BZ_OK)
        {
            mysteryGiftSheet = nlohmann::json::parse(d);
        }

        delete[] s;
        delete[] d;

        fclose(f);
    }

    if (mysteryGiftSheet.is_discarded())
    {
        mysteryGiftSheet            = nlohmann::json::object();
        mysteryGiftSheet["gen"]     = "BAD";
        mysteryGiftSheet["matches"] = nlohmann::json::array();
    }

    f = fopen(dataPath.c_str(), "rb");
    if (f != NULL)
    {
        fseek(f, 0, SEEK_END);
        u32 size             = ftell(f);
        unsigned int destLen = 800 * 1024;
        char* s              = new char[size];
        mysteryGiftData      = new u8[destLen]();
        rewind(f);
        fread(s, 1, size, f);

        int r = BZ2_bzBuffToBuffDecompress((char*)mysteryGiftData, &destLen, s, size, 0, 0);
        if (r != BZ_OK)
        {
            // TODO
        }

        delete[] s;

        fclose(f);
    }
}

std::unique_ptr<pksm::WCX> MysteryGift::wondercard(size_t index)
{
    std::string gen = mysteryGiftSheet["gen"];

    auto entry = mysteryGiftSheet["wondercards"][index];

    u32 offset = entry["offset"];

    if (gen == "4")
    {
        std::unique_ptr<pksm::WCX> wc = nullptr;
        if (entry["type"] == "wc4")
        {
            wc = std::make_unique<pksm::WC4>(mysteryGiftData + offset);
        }
        else if (entry["type"] == "pgt")
        {
            wc = std::make_unique<pksm::PGT>(mysteryGiftData + offset);
        }
        else
        {
            wc = std::make_unique<pksm::PCD>(mysteryGiftData + offset);
        }
        return wc;
    }
    else if (gen == "5")
    {
        return std::make_unique<pksm::PGF>(mysteryGiftData + offset);
    }
    else if (gen == "6")
    {
        return std::make_unique<pksm::WC6>(mysteryGiftData + offset, entry["type"].get<std::string>().find("full") != std::string::npos);
    }
    else if (gen == "7")
    {
        return std::make_unique<pksm::WC7>(mysteryGiftData + offset, entry["type"].get<std::string>().find("full") != std::string::npos);
    }
    else if (gen == "LGPE")
    {
        return std::make_unique<pksm::WB7>(mysteryGiftData + offset, entry["type"].get<std::string>().find("full") != std::string::npos);
    }
    else if (gen == "8")
    {
        return std::make_unique<pksm::WC8>(mysteryGiftData + offset);
    }
    else
    {
        return nullptr;
    }
}

void MysteryGift::exit(void)
{
    delete[] mysteryGiftData;
    mysteryGiftData = nullptr;
    mysteryGiftSheet.clear();
}

std::vector<nlohmann::json> MysteryGift::wondercards()
{
    return mysteryGiftSheet["matches"];
}

MysteryGift::giftData MysteryGift::wondercardInfo(size_t index)
{
    nlohmann::json entry = mysteryGiftSheet["wondercards"][index];
    giftData ret(entry["name"].get<std::string>(), entry["game"].get<std::string>(), entry["species"].get<int>(), entry["form"].get<int>(),
        pksm::Gender(entry["gender"].get<int>()), entry.contains("released") ? entry["released"].get<bool>() : true);
    return ret;
}
