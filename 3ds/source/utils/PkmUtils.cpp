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
#include "enums/Generation.hpp"
#include "pkx/PB7.hpp"
#include "pkx/PK3.hpp"
#include "pkx/PK4.hpp"
#include "pkx/PK5.hpp"
#include "pkx/PK6.hpp"
#include "pkx/PK7.hpp"
#include "pkx/PK8.hpp"
#include "utils/genToPkx.hpp"
#include "utils/random.hpp"
#include "utils/utils.hpp"
#include <stdio.h>
#include <sys/stat.h>

namespace
{
    std::unique_ptr<pksm::PK3> g3Default   = nullptr;
    bool g3Save                            = false;
    std::unique_ptr<pksm::PK4> g4Default   = nullptr;
    bool g4Save                            = false;
    std::unique_ptr<pksm::PK5> g5Default   = nullptr;
    bool g5Save                            = false;
    std::unique_ptr<pksm::PK6> g6Default   = nullptr;
    bool g6Save                            = false;
    std::unique_ptr<pksm::PK7> g7Default   = nullptr;
    bool g7Save                            = false;
    std::unique_ptr<pksm::PK8> g8Default   = nullptr;
    bool g8Save                            = false;
    std::unique_ptr<pksm::PB7> lgpeDefault = nullptr;
    bool lgpeSave                          = false;

    template <pksm::Generation::EnumType gen>
    std::unique_ptr<typename pksm::GenToPkx<gen>::PKX> loadPkm(const std::string& fileName)
    {
        std::string pkmFile = "/3ds/PKSM/defaults/" + fileName;
        struct stat statStruct;

        if (stat(pkmFile.c_str(), &statStruct) == 0)
        {
            auto ret = pksm::PKX::getPKM<gen>(nullptr, size_t(statStruct.st_size));
            if (ret)
            {
                FILE* file = fopen(fileName.c_str(), "rb");
                if (file)
                {
                    u8* data = new u8[ret->getLength()];
                    fread(data, 1, ret->getLength(), file);
                    fclose(file);
                    ret = pksm::PKX::getPKM<gen>(data);
                    delete[] data;
                    return ret;
                }
            }
        }

        switch (gen)
        {
            case pksm::Generation::THREE:
                g3Save = true;
                break;
            case pksm::Generation::FOUR:
                g4Save = true;
                break;
            case pksm::Generation::FIVE:
                g5Save = true;
                break;
            case pksm::Generation::SIX:
                g6Save = true;
                break;
            case pksm::Generation::SEVEN:
                g7Save = true;
                break;
            case pksm::Generation::EIGHT:
                g8Save = true;
                break;
            case pksm::Generation::LGPE:
                lgpeSave = true;
                break;
        }
        auto ret = pksm::PKX::getPKM<gen>(nullptr);

        ret->otName("PKSM");
        ret->TID(12345);
        ret->SID(54321);
        ret->ball(pksm::Ball::Poke);
        ret->encryptionConstant((u32)pksm::randomNumber());
        switch (ret->generation())
        {
            case pksm::Generation::THREE:
                ret->version(pksm::GameVersion::E);
                ret->metLocation(0x0010); // Route 101
                break;
            case pksm::Generation::FOUR:
                ret->version(pksm::GameVersion::Pt);
                ret->metLocation(0x0010); // Route 201, DPPt
                break;
            case pksm::Generation::FIVE:
                ret->version(pksm::GameVersion::B);
                ret->metLocation(0x000e); // Route 1, BWB2W2
                break;
            case pksm::Generation::SIX:
                ret->version(pksm::GameVersion::Y);
                ret->metLocation(0x0008); // Route 1, XY
                break;
            case pksm::Generation::SEVEN:
                ret->version(pksm::GameVersion::SN);
                ret->metLocation(0x0006); // Route 1, SMUSUM
                break;
            case pksm::Generation::LGPE:
                ret->version(pksm::GameVersion::GP);
                ret->metLocation(0x0003); // Route 1, LGPE
                break;
            case pksm::Generation::EIGHT:
                ret->version(pksm::GameVersion::SW);
                ret->metLocation(0x000C); // Route 1, SWSH
                break;
            default:
                break;
        }
        ret->fixMoves();
        ret->PID((u32)pksm::randomNumber());
        ret->language(Configuration::getInstance().language());
        ret->metLevel(1);
        if constexpr (gen == pksm::Generation::SIX)
        {
            ((pksm::PK6*)ret.get())->otMemory(1);
            ((pksm::PK6*)ret.get())->otFeeling(0);
            ((pksm::PK6*)ret.get())->otIntensity(1);
        }

        ret->species(pksm::Species::Bulbasaur);
        std::string nick = pksm::Species::Bulbasaur.localize(ret->language());
        if (ret->generation() < pksm::Generation::FIVE)
        {
            nick = StringUtils::toUpper(nick);
        }
        ret->nickname(nick);
        ret->nicknamed(false);
        ret->alternativeForm(0);
        ret->setAbility(0);
        ret->PID(pksm::PKX::getRandomPID(ret->species(), ret->gender(), ret->version(), ret->nature(), ret->alternativeForm(), ret->abilityNumber(),
            ret->PID(), ret->generation()));

        return ret;
    }
}

void PkmUtils::initDefaults()
{
    g3Default   = loadPkm<pksm::Generation::THREE>("default.pk3");
    g4Default   = loadPkm<pksm::Generation::FOUR>("default.pk4");
    g5Default   = loadPkm<pksm::Generation::FIVE>("default.pk5");
    g6Default   = loadPkm<pksm::Generation::SIX>("default.pk6");
    g7Default   = loadPkm<pksm::Generation::SEVEN>("default.pk7");
    g8Default   = loadPkm<pksm::Generation::EIGHT>("default.pk8");
    lgpeDefault = loadPkm<pksm::Generation::LGPE>("default.pb7");
    saveDefaults();
}

void PkmUtils::saveDefaults()
{
    if (g3Save)
    {
        FILE* out = fopen("/3ds/PKSM/defaults/default.pk3", "wb");
        if (out)
        {
            fwrite(g3Default->rawData(), 1, g3Default->getLength(), out);
            fclose(out);
            g3Save = false;
        }
    }
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

std::unique_ptr<pksm::PKX> PkmUtils::getDefault(pksm::Generation gen)
{
    switch (gen)
    {
        case pksm::Generation::THREE:
            return g3Default->clone();
        case pksm::Generation::FOUR:
            return g4Default->clone();
        case pksm::Generation::FIVE:
            return g5Default->clone();
        case pksm::Generation::SIX:
            return g6Default->clone();
        case pksm::Generation::SEVEN:
            return g7Default->clone();
        case pksm::Generation::EIGHT:
            return g8Default->clone();
        case pksm::Generation::LGPE:
            return lgpeDefault->clone();
    }
    return nullptr;
}

void PkmUtils::setDefault(std::unique_ptr<pksm::PKX> pkm)
{
    switch (pkm->generation())
    {
        case pksm::Generation::THREE:
            g3Default = std::unique_ptr<pksm::PK3>((pksm::PK3*)pkm.release());
            g3Save    = true;
            break;
        case pksm::Generation::FOUR:
            g4Default = std::unique_ptr<pksm::PK4>((pksm::PK4*)pkm.release());
            g4Save    = true;
            break;
        case pksm::Generation::FIVE:
            g5Default = std::unique_ptr<pksm::PK5>((pksm::PK5*)pkm.release());
            g5Save    = true;
            break;
        case pksm::Generation::SIX:
            g6Default = std::unique_ptr<pksm::PK6>((pksm::PK6*)pkm.release());
            g6Save    = true;
            break;
        case pksm::Generation::SEVEN:
            g7Default = std::unique_ptr<pksm::PK7>((pksm::PK7*)pkm.release());
            g7Save    = true;
            break;
        case pksm::Generation::EIGHT:
            g8Default = std::unique_ptr<pksm::PK8>((pksm::PK8*)pkm.release());
            g8Save    = true;
            break;
        case pksm::Generation::LGPE:
            lgpeDefault = std::unique_ptr<pksm::PB7>((pksm::PB7*)pkm.release());
            lgpeSave    = true;
            break;
    }
}
