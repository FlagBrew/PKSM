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

#include "mysterygift.hpp"
#include "BZ2.hpp"
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
    std::vector<u8> mysteryGiftData;
}

void MysteryGift::init(pksm::Generation g)
{
    mysteryGiftData.clear();

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
        std::vector<u8> data;
        int error = BZ2::decompress(f, data);

        if (error == BZ_OK)
        {
            mysteryGiftSheet = nlohmann::json::parse(data.begin(), data.end(), nullptr, false);
        }

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
        int error = BZ2::decompress(f, mysteryGiftData);

        if (error != BZ_OK)
        {
            // TODO
        }

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
            wc = std::make_unique<pksm::WC4>(mysteryGiftData.data() + offset);
        }
        else if (entry["type"] == "pgt")
        {
            wc = std::make_unique<pksm::PGT>(mysteryGiftData.data() + offset);
        }
        else
        {
            wc = std::make_unique<pksm::PCD>(mysteryGiftData.data() + offset);
        }
        return wc;
    }
    else if (gen == "5")
    {
        return std::make_unique<pksm::PGF>(mysteryGiftData.data() + offset);
    }
    else if (gen == "6")
    {
        return std::make_unique<pksm::WC6>(mysteryGiftData.data() + offset,
            entry["type"].get<std::string>().find("full") != std::string::npos);
    }
    else if (gen == "7")
    {
        return std::make_unique<pksm::WC7>(mysteryGiftData.data() + offset,
            entry["type"].get<std::string>().find("full") != std::string::npos);
    }
    else if (gen == "LGPE")
    {
        return std::make_unique<pksm::WB7>(mysteryGiftData.data() + offset,
            entry["type"].get<std::string>().find("full") != std::string::npos);
    }
    else if (gen == "8")
    {
        return std::make_unique<pksm::WC8>(mysteryGiftData.data() + offset);
    }
    else
    {
        return nullptr;
    }
}

void MysteryGift::exit(void)
{
    mysteryGiftData.clear();
    mysteryGiftSheet.clear();
}

std::vector<nlohmann::json> MysteryGift::wondercards()
{
    return mysteryGiftSheet["matches"];
}

MysteryGift::giftData MysteryGift::wondercardInfo(size_t index)
{
    nlohmann::json entry = mysteryGiftSheet["wondercards"][index];
    giftData ret(entry["name"].get<std::string>(), entry["game"].get<std::string>(),
        entry["species"].get<int>(), entry["form"].get<int>(),
        pksm::Gender(entry["gender"].get<int>()),
        entry.contains("released") ? entry["released"].get<bool>() : true);
    return ret;
}
