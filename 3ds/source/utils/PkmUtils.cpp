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

#include "PkmUtils.hpp"
#include "Configuration.hpp"
#include "Generation.hpp"
#include "PB7.hpp"
#include "PK4.hpp"
#include "PK5.hpp"
#include "PK6.hpp"
#include "PK7.hpp"
#include "PK8.hpp"
#include "genToPkx.hpp"
#include "random.hpp"
#include "utils.hpp"
#include <stdio.h>
#include <sys/stat.h>

namespace
{
    std::unique_ptr<PK4> g3Default   = nullptr;
    bool g3Save                      = false;
    std::unique_ptr<PK4> g4Default   = nullptr;
    bool g4Save                      = false;
    std::unique_ptr<PK5> g5Default   = nullptr;
    bool g5Save                      = false;
    std::unique_ptr<PK6> g6Default   = nullptr;
    bool g6Save                      = false;
    std::unique_ptr<PK7> g7Default   = nullptr;
    bool g7Save                      = false;
    std::unique_ptr<PK8> g8Default   = nullptr;
    bool g8Save                      = false;
    std::unique_ptr<PB7> lgpeDefault = nullptr;
    bool lgpeSave                    = false;

    template <Generation::EnumType gen>
    std::unique_ptr<typename GenToPkx<gen>::PKX> loadPkm(const std::string& fileName)
    {
        std::string pkmFile = "/3ds/PKSM/defaults/" + fileName;
        struct stat statStruct;

        if (stat(pkmFile.c_str(), &statStruct) == 0)
        {
            auto ret = PKX::getPKM<gen>(nullptr, size_t(statStruct.st_size));
            if (ret)
            {
                FILE* file = fopen(fileName.c_str(), "rb");
                if (file)
                {
                    u8* data = new u8[ret->getLength()];
                    fread(data, 1, ret->getLength(), file);
                    fclose(file);
                    ret = PKX::getPKM<gen>(data);
                    delete[] data;
                    return ret;
                }
            }
        }

        switch (gen)
        {
            case Generation::THREE:
                g3Save = true;
                break;
            case Generation::FOUR:
                g4Save = true;
                break;
            case Generation::FIVE:
                g5Save = true;
                break;
            case Generation::SIX:
                g6Save = true;
                break;
            case Generation::SEVEN:
                g7Save = true;
                break;
            case Generation::EIGHT:
                g8Save = true;
                break;
            case Generation::LGPE:
                lgpeSave = true;
                break;
        }
        auto ret = PKX::getPKM<gen>(nullptr);

        ret->otName("PKSM");
        ret->TID(12345);
        ret->SID(54321);
        ret->ball(Ball::Poke);
        ret->encryptionConstant((u32)randomNumbers());
        switch (ret->generation())
        {
            case Generation::THREE:
                ret->version(GameVersion::E);
                ret->metLocation(0x0010); // Route 101
                break;
            case Generation::FOUR:
                ret->version(GameVersion::Pt);
                ret->metLocation(0x0010); // Route 201, DPPt
                break;
            case Generation::FIVE:
                ret->version(GameVersion::B);
                ret->metLocation(0x000e); // Route 1, BWB2W2
                break;
            case Generation::SIX:
                ret->version(GameVersion::Y);
                ret->metLocation(0x0008); // Route 1, XY
                break;
            case Generation::SEVEN:
                ret->version(GameVersion::SN);
                ret->metLocation(0x0006); // Route 1, SMUSUM
                break;
            case Generation::LGPE:
                ret->version(GameVersion::GP);
                ret->metLocation(0x0003); // Route 1, LGPE
                break;
            case Generation::EIGHT:
                ret->version(GameVersion::SW);
                ret->metLocation(0x000C); // Route 1, SWSH
                break;
            default:
                break;
        }
        ret->fixMoves();
        ret->PID((u32)randomNumbers());
        ret->language(Configuration::getInstance().language());
        ret->metLevel(1);
        if constexpr (gen == Generation::SIX)
        {
            ((PK6*)ret.get())->otMemory(1);
            ((PK6*)ret.get())->otFeeling(0);
            ((PK6*)ret.get())->otIntensity(1);
        }

        ret->species(Species::Bulbasaur);
        std::string nick = Species::Bulbasaur.localize(ret->language());
        if (ret->generation() < Generation::FIVE)
        {
            nick = StringUtils::toUpper(nick);
        }
        ret->nickname(nick);
        ret->nicknamed(false);
        ret->alternativeForm(0);
        ret->setAbility(0);
        ret->PID(PKX::getRandomPID(ret->species(), ret->gender(), ret->version(), ret->nature(), ret->alternativeForm(), ret->abilityNumber(),
            ret->PID(), ret->generation()));

        return ret;
    }
}

void PkmUtils::initDefaults()
{
    g4Default   = loadPkm<Generation::FOUR>("default.pk4");
    g5Default   = loadPkm<Generation::FIVE>("default.pk5");
    g6Default   = loadPkm<Generation::SIX>("default.pk6");
    g7Default   = loadPkm<Generation::SEVEN>("default.pk7");
    g8Default   = loadPkm<Generation::EIGHT>("default.pk8");
    lgpeDefault = loadPkm<Generation::LGPE>("default.pb7");
    saveDefaults();
}

void PkmUtils::saveDefaults()
{
    if (g4Save)
    {
        FILE* out = fopen("/3ds/PKSM/defaults/default.pk4", "wb");
        if (out)
        {
            fwrite(g4Default->rawData(), 1, g4Default->getLength(), out);
            fclose(out);
            g4Save = false;
        }
    }
    if (g5Save)
    {
        FILE* out = fopen("/3ds/PKSM/defaults/default.pk5", "wb");
        if (out)
        {
            fwrite(g5Default->rawData(), 1, g5Default->getLength(), out);
            fclose(out);
            g5Save = false;
        }
    }
    if (g6Save)
    {
        FILE* out = fopen("/3ds/PKSM/defaults/default.pk6", "wb");
        if (out)
        {
            fwrite(g6Default->rawData(), 1, g6Default->getLength(), out);
            fclose(out);
            g6Save = false;
        }
    }
    if (g7Save)
    {
        FILE* out = fopen("/3ds/PKSM/defaults/default.pk7", "wb");
        if (out)
        {
            fwrite(g7Default->rawData(), 1, g7Default->getLength(), out);
            fclose(out);
            g7Save = false;
        }
    }
    if (g8Save)
    {
        FILE* out = fopen("/3ds/PKSM/defaults/default.pk8", "wb");
        if (out)
        {
            fwrite(g8Default->rawData(), 1, g8Default->getLength(), out);
            fclose(out);
            g8Save = false;
        }
    }
    if (lgpeSave)
    {
        FILE* out = fopen("/3ds/PKSM/defaults/default.pb7", "wb");
        if (out)
        {
            fwrite(lgpeDefault->rawData(), 1, lgpeDefault->getLength(), out);
            fclose(out);
            lgpeSave = false;
        }
    }
}

std::unique_ptr<PKX> PkmUtils::getDefault(Generation gen)
{
    switch (gen)
    {
        case Generation::FOUR:
            return g4Default->clone();
        case Generation::FIVE:
            return g5Default->clone();
        case Generation::SIX:
            return g6Default->clone();
        case Generation::SEVEN:
            return g7Default->clone();
        case Generation::EIGHT:
            return g8Default->clone();
        case Generation::LGPE:
            return lgpeDefault->clone();
    }
    return nullptr;
}

void PkmUtils::setDefault(std::unique_ptr<PKX> pkm)
{
    switch (pkm->generation())
    {
        case Generation::FOUR:
            g4Default = std::unique_ptr<PK4>((PK4*)pkm.release());
            g4Save    = true;
            break;
        case Generation::FIVE:
            g5Default = std::unique_ptr<PK5>((PK5*)pkm.release());
            g5Save    = true;
            break;
        case Generation::SIX:
            g6Default = std::unique_ptr<PK6>((PK6*)pkm.release());
            g6Save    = true;
            break;
        case Generation::SEVEN:
            g7Default = std::unique_ptr<PK7>((PK7*)pkm.release());
            g7Save    = true;
            break;
        case Generation::EIGHT:
            g8Default = std::unique_ptr<PK8>((PK8*)pkm.release());
            g8Save    = true;
            break;
        case Generation::LGPE:
            lgpeDefault = std::unique_ptr<PB7>((PB7*)pkm.release());
            lgpeSave    = true;
            break;
    }
}
