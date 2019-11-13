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

#include "PKX.hpp"
#include "PB7.hpp"
#include "PK4.hpp"
#include "PK5.hpp"
#include "PK6.hpp"
#include "PK7.hpp"
#include "PK8.hpp"
#include "PKFilter.hpp"
#include "random.hpp"

u32 PKX::expTable(u8 row, u8 col) const
{
    static constexpr u32 table[100][6] = {{0, 0, 0, 0, 0, 0}, {8, 15, 4, 9, 6, 10}, {27, 52, 13, 57, 21, 33}, {64, 122, 32, 96, 51, 80},
        {125, 237, 65, 135, 100, 156}, {216, 406, 112, 179, 172, 270}, {343, 637, 178, 236, 274, 428}, {512, 942, 276, 314, 409, 640},
        {729, 1326, 393, 419, 583, 911}, {1000, 1800, 540, 560, 800, 1250}, {1331, 2369, 745, 742, 1064, 1663}, {1728, 3041, 967, 973, 1382, 2160},
        {2197, 3822, 1230, 1261, 1757, 2746}, {2744, 4719, 1591, 1612, 2195, 3430}, {3375, 5737, 1957, 2035, 2700, 4218},
        {4096, 6881, 2457, 2535, 3276, 5120}, {4913, 8155, 3046, 3120, 3930, 6141}, {5832, 9564, 3732, 3798, 4665, 7290},
        {6859, 11111, 4526, 4575, 5487, 8573}, {8000, 12800, 5440, 5460, 6400, 10000}, {9261, 14632, 6482, 6458, 7408, 11576},
        {10648, 16610, 7666, 7577, 8518, 13310}, {12167, 18737, 9003, 8825, 9733, 15208}, {13824, 21012, 10506, 10208, 11059, 17280},
        {15625, 23437, 12187, 11735, 12500, 19531}, {17576, 26012, 14060, 13411, 14060, 21970}, {19683, 28737, 16140, 15244, 15746, 24603},
        {21952, 31610, 18439, 17242, 17561, 27440}, {24389, 34632, 20974, 19411, 19511, 30486}, {27000, 37800, 23760, 21760, 21600, 33750},
        {29791, 41111, 26811, 24294, 23832, 37238}, {32768, 44564, 30146, 27021, 26214, 40960}, {35937, 48155, 33780, 29949, 28749, 44921},
        {39304, 51881, 37731, 33084, 31443, 49130}, {42875, 55737, 42017, 36435, 34300, 53593}, {46656, 59719, 46656, 40007, 37324, 58320},
        {50653, 63822, 50653, 43808, 40522, 63316}, {54872, 68041, 55969, 47846, 43897, 68590}, {59319, 72369, 60505, 52127, 47455, 74148},
        {64000, 76800, 66560, 56660, 51200, 80000}, {68921, 81326, 71677, 61450, 55136, 86151}, {74088, 85942, 78533, 66505, 59270, 92610},
        {79507, 90637, 84277, 71833, 63605, 99383}, {85184, 95406, 91998, 77440, 68147, 106480}, {91125, 100237, 98415, 83335, 72900, 113906},
        {97336, 105122, 107069, 89523, 77868, 121670}, {103823, 110052, 114205, 96012, 83058, 129778},
        {110592, 115015, 123863, 102810, 88473, 138240}, {117649, 120001, 131766, 109923, 94119, 147061},
        {125000, 125000, 142500, 117360, 100000, 156250}, {132651, 131324, 151222, 125126, 106120, 165813},
        {140608, 137795, 163105, 133229, 112486, 175760}, {148877, 144410, 172697, 141677, 119101, 186096},
        {157464, 151165, 185807, 150476, 125971, 196830}, {166375, 158056, 196322, 159635, 133100, 207968},
        {175616, 165079, 210739, 169159, 140492, 219520}, {185193, 172229, 222231, 179056, 148154, 231491},
        {195112, 179503, 238036, 189334, 156089, 243890}, {205379, 186894, 250562, 199999, 164303, 256723},
        {216000, 194400, 267840, 211060, 172800, 270000}, {226981, 202013, 281456, 222522, 181584, 283726},
        {238328, 209728, 300293, 234393, 190662, 297910}, {250047, 217540, 315059, 246681, 200037, 312558},
        {262144, 225443, 335544, 259392, 209715, 327680}, {274625, 233431, 351520, 272535, 219700, 343281},
        {287496, 241496, 373744, 286115, 229996, 359370}, {300763, 249633, 390991, 300140, 240610, 375953},
        {314432, 257834, 415050, 314618, 251545, 393040}, {328509, 267406, 433631, 329555, 262807, 410636},
        {343000, 276458, 459620, 344960, 274400, 428750}, {357911, 286328, 479600, 360838, 286328, 447388},
        {373248, 296358, 507617, 377197, 298598, 466560}, {389017, 305767, 529063, 394045, 311213, 486271},
        {405224, 316074, 559209, 411388, 324179, 506530}, {421875, 326531, 582187, 429235, 337500, 527343},
        {438976, 336255, 614566, 447591, 351180, 548720}, {456533, 346965, 639146, 466464, 365226, 570666},
        {474552, 357812, 673863, 485862, 379641, 593190}, {493039, 367807, 700115, 505791, 394431, 616298},
        {512000, 378880, 737280, 526260, 409600, 640000}, {531441, 390077, 765275, 547274, 425152, 664301},
        {551368, 400293, 804997, 568841, 441094, 689210}, {571787, 411686, 834809, 590969, 457429, 714733},
        {592704, 423190, 877201, 613664, 474163, 740880}, {614125, 433572, 908905, 636935, 491300, 767656},
        {636056, 445239, 954084, 660787, 508844, 795070}, {658503, 457001, 987754, 685228, 526802, 823128},
        {681472, 467489, 1035837, 710266, 545177, 851840}, {704969, 479378, 1071552, 735907, 563975, 881211},
        {729000, 491346, 1122660, 762160, 583200, 911250}, {753571, 501878, 1160499, 789030, 602856, 941963},
        {778688, 513934, 1214753, 816525, 622950, 973360}, {804357, 526049, 1254796, 844653, 643485, 1005446},
        {830584, 536557, 1312322, 873420, 664467, 1038230}, {857375, 548720, 1354652, 902835, 685900, 1071718},
        {884736, 560922, 1415577, 932903, 707788, 1105920}, {912673, 571333, 1460276, 963632, 730138, 1140841},
        {941192, 583539, 1524731, 995030, 752953, 1176490}, {970299, 591882, 1571884, 1027103, 776239, 1212873},
        {1000000, 600000, 1640000, 1059860, 800000, 1250000}};

    return table[row][col];
}

u8 PKX::blockPosition(u8 index) const
{
    // clang-format off
    static constexpr u8 blocks[128] = {
        0, 1, 2, 3, 0, 1, 3, 2, 0, 2, 1, 3, 0, 3, 1, 2,
        0, 2, 3, 1, 0, 3, 2, 1, 1, 0, 2, 3, 1, 0, 3, 2,
        2, 0, 1, 3, 3, 0, 1, 2, 2, 0, 3, 1, 3, 0, 2, 1,
        1, 2, 0, 3, 1, 3, 0, 2, 2, 1, 0, 3, 3, 1, 0, 2,
        2, 3, 0, 1, 3, 2, 0, 1, 1, 2, 3, 0, 1, 3, 2, 0,
        2, 1, 3, 0, 3, 1, 2, 0, 2, 3, 1, 0, 3, 2, 1, 0,

        // duplicates of 0-7 to eliminate modulus
        0, 1, 2, 3, 0, 1, 3, 2, 0, 2, 1, 3, 0, 3, 1, 2,
        0, 2, 3, 1, 0, 3, 2, 1, 1, 0, 2, 3, 1, 0, 3, 2,
    };
    // clang-format on

    return blocks[index];
}

u8 PKX::blockPositionInvert(u8 index) const
{
    static constexpr u8 blocks[32] = {
        0, 1, 2, 4, 3, 5, 6, 7, 12, 18, 13, 19, 8, 10, 14, 20, 16, 22, 9, 11, 15, 21, 17, 23, 0, 1, 2, 4, 3, 5, 6,
        7, // duplicates of 0-7 to eliminate modulus
    };

    return blocks[index];
}

u32 PKX::seedStep(u32 seed)
{
    return seed * 0x41C64E6D + 0x6073;
}

void PKX::reorderMoves(void)
{
    if (move(3) != 0 && move(2) == 0)
    {
        move(2, move(3));
        PP(2, PP(3));
        PPUp(2, PPUp(3));
        move(3, 0);
    }
    if (move(2) != 0 && move(1) == 0)
    {
        move(1, move(2));
        PP(1, PP(2));
        PPUp(1, PPUp(2));
        move(2, 0);
        reorderMoves();
    }
    if (move(1) != 0 && move(0) == 0)
    {
        move(0, move(1));
        PP(0, PP(1));
        PPUp(0, PPUp(1));
        move(1, 0);
        reorderMoves();
    }
    if (relearnMove(3) != 0 && relearnMove(2) == 0)
    {
        relearnMove(2, relearnMove(3));
        PP(2, PP(3));
        PPUp(2, PPUp(3));
        relearnMove(3, 0);
    }
    if (relearnMove(2) != 0 && relearnMove(1) == 0)
    {
        relearnMove(1, relearnMove(2));
        PP(1, PP(2));
        PPUp(1, PPUp(2));
        relearnMove(2, 0);
        reorderMoves();
    }
    if (relearnMove(1) != 0 && relearnMove(0) == 0)
    {
        relearnMove(0, relearnMove(1));
        PP(0, PP(1));
        PPUp(0, PPUp(1));
        relearnMove(1, 0);
        reorderMoves();
    }
}

void PKX::decrypt(void)
{
    u8 sv = (encryptionConstant() >> 13) & 31;
    crypt();
    shuffleArray(sv);
}

void PKX::encrypt(void)
{
    u8 sv = (encryptionConstant() >> 13) & 31;
    refreshChecksum();
    shuffleArray(blockPositionInvert(sv));
    crypt();
}

bool PKX::gen7(void) const
{
    return version() >= 30 && version() <= 33;
}

bool PKX::gen6(void) const
{
    return version() >= 24 && version() <= 29;
}

bool PKX::gen5(void) const
{
    return version() >= 20 && version() <= 23;
}

bool PKX::gen4(void) const
{
    return version() >= 7 && version() <= 12 && version() != 9;
}

bool PKX::gen3(void) const
{
    return (version() >= 1 && version() <= 5) || (version() == 15);
}

int PKX::genNumber(void) const
{
    if (gen7())
    {
        return 7;
    }
    else if (gen6())
    {
        return 6;
    }
    else if (gen5())
    {
        return 5;
    }
    else if (gen4())
    {
        return 4;
    }
    else if (gen3())
    {
        return 3;
    }
    else
    {
        return -1;
    }
}

void PKX::fixMoves(void)
{
    reorderMoves();

    if (move(0) == 0)
    {
        move(0, 1);
    }

    for (int i = 0; i < 4; i++)
    {
        if (move(i) == 0)
        {
            PP(i, 0);
            PPUp(i, 0);
        }
        else
        {
            PP(i, 5); // Smallest maximum
        }
    }
}

u8 PKX::genFromBytes(u8* data, size_t length, bool ekx)
{
    if (length == 136)
    {
        if (*(u16*)(data + 4) == 0 && (*(u16*)(data + 0x80) >= 0x3333 || data[0x5F] >= 0x10) && *(u16*)(data + 0x46) == 0)
        {
            return 5;
        }
        return 4;
    }
    else if (length == 232)
    {
        PK6 test(data, ekx);
        if (test.species() > 721 || test.version() > 27 || test.move(0) > 621 || test.move(1) > 621 || test.move(2) > 621 || test.move(3) > 621 ||
            test.relearnMove(0) > 621 || test.relearnMove(1) > 621 || test.relearnMove(2) > 621 || test.relearnMove(3) > 621 ||
            test.ability() > 191 || test.heldItem() > 775) // Invalid values for gen 6
        {
            return 7;
        }

        int et = test.encounterType();
        if (et != 0)
        {
            if (test.level() < 100)
            {
                return 6;
            }

            switch (test.version())
            {
                case 7:
                case 8:
                case 10:
                case 11:
                case 12:
                    if (et > 24)
                    {
                        return 7;
                    }
                    break;
                default:
                    return 7;
            }
        }
        return 6;
    }
    return 0;
}

static inline u8 genderFromRatio(u32 pid, u8 gt)
{
    switch (gt)
    {
        case 0xFF:
            return 2;
        case 0xFE:
            return 1;
        case 0:
            return 0;
        default:
            return (pid & 0xFF) < gt ? 1 : 0;
    }
}

static inline u8 getUnownForm(u32 pid)
{
    u32 val = (pid & 0x3000000) >> 18 | (pid & 0x30000) >> 12 | (pid & 0x300) >> 6 | (pid & 0x3);
    return val % 28;
}

u32 PKX::getRandomPID(u16 species, u8 gender, u8 originGame, u8 nature, u8 form, u8 abilityNum, u32 oldPid, Generation gen)
{
    if (originGame >= 24) // Origin game over gen 5
    {
        return randomNumbers();
    }

    u8 (*genderTypeFinder)(u16 species);
    switch (gen)
    {
        case Generation::FOUR:
            genderTypeFinder = PersonalDPPtHGSS::gender;
            break;
        case Generation::FIVE:
            genderTypeFinder = PersonalBWB2W2::gender;
            break;
        case Generation::SIX:
            genderTypeFinder = PersonalXYORAS::gender;
            break;
        case Generation::SEVEN:
            genderTypeFinder = PersonalSMUSUM::gender;
            break;
        case Generation::LGPE:
            genderTypeFinder = PersonalLGPE::gender;
            break;
        case Generation::EIGHT:
            genderTypeFinder = PersonalSWSH::gender;
            break;
        case Generation::UNUSED:
            return 0;
    }

    u8 genderType = genderTypeFinder(species);
    bool g3unown  = originGame <= 5 && species == 201;
    while (true)
    {
        u32 possiblePID = randomNumbers();
        if (originGame <= 15 && possiblePID % 25 != nature)
        {
            continue;
        }

        if (g3unown)
        {
            if (getUnownForm(possiblePID) != form)
            {
                continue;
            }
        }
        else if (abilityNum != 4)
        {
            if (abilityNum == 1 && (possiblePID & (1 << (gen == Generation::FIVE ? 16 : 0))))
            {
                continue;
            }
            else if (abilityNum == 2 && !(possiblePID & (1 << (gen == Generation::FIVE ? 16 : 0))))
            {
                continue;
            }
        }

        if (genderType == 255 || genderType == 254 || genderType == 0 || gender == 2)
        {
            return possiblePID;
        }

        if (gender == genderFromRatio(possiblePID, genderType))
        {
            return possiblePID;
        }
    }
}

u32 PKX::versionTID() const
{
    switch (version())
    {
        default:
            return TID();
        case 30: // SM
        case 31:
        case 32: // USUM
        case 33:
        case 42: // LGPE
        case 43:
            return (u32)(SID() << 16 | TID()) % 1000000;
    }
}

u32 PKX::versionSID() const
{
    switch (version())
    {
        default:
            return SID();
        case 30: // SM
        case 31:
        case 32: // USUM
        case 33:
        case 42: // LGPE
        case 43:
            return (u32)(SID() << 16 | TID()) / 1000000;
    }
}

u32 PKX::formatTID() const
{
    switch (generation())
    {
        case Generation::FOUR:
        case Generation::FIVE:
        case Generation::SIX:
            return TID();
        case Generation::SEVEN:
        case Generation::LGPE:
        case Generation::EIGHT:
            return (u32)(SID() << 16 | TID()) % 1000000;
        case Generation::UNUSED:
            return 0;
    }
    return 0;
}
u32 PKX::formatSID() const
{
    switch (generation())
    {
        case Generation::FOUR:
        case Generation::FIVE:
        case Generation::SIX:
            return SID();
        case Generation::SEVEN:
        case Generation::LGPE:
        case Generation::EIGHT:
            return (u32)(SID() << 16 | TID()) / 1000000;
        case Generation::UNUSED:
            return 0;
    }
    return 0;
}

std::unique_ptr<PKX> PKX::getPKM(Generation gen, u8* data, bool ekx, bool party, bool directAccess)
{
    switch (gen)
    {
        case Generation::FOUR:
            return std::make_unique<PK4>(data, ekx, party, directAccess);
        case Generation::FIVE:
            return std::make_unique<PK5>(data, ekx, party, directAccess);
        case Generation::SIX:
            return std::make_unique<PK6>(data, ekx, party, directAccess);
        case Generation::SEVEN:
            return std::make_unique<PK7>(data, ekx, party, directAccess);
        case Generation::LGPE:
            return std::make_unique<PB7>(data, ekx, directAccess);
        case Generation::EIGHT:
            return std::make_unique<PK8>(data, ekx, party, directAccess);
        case Generation::UNUSED:
            return nullptr;
    }
    return nullptr;
}

bool PKX::operator==(const PKFilter& filter) const
{
    if (filter.generationEnabled() && (filter.generationInversed() != (generation() != filter.generation())))
    {
        return false;
    }
    if (filter.speciesEnabled() && (filter.speciesInversed() != (species() != filter.species())))
    {
        return false;
    }
    if (filter.heldItemEnabled() && (filter.heldItemInversed() != (heldItem() != filter.heldItem())))
    {
        return false;
    }
    if (filter.levelEnabled() && (filter.levelInversed() != (level() != filter.level())))
    {
        return false;
    }
    if (filter.abilityEnabled() && (filter.abilityInversed() != (ability() != filter.ability())))
    {
        return false;
    }
    if (filter.TSVEnabled() && (filter.TSVInversed() != (TSV() != filter.TSV())))
    {
        return false;
    }
    if (filter.natureEnabled() && (filter.natureInversed() != (nature() != filter.nature())))
    {
        return false;
    }
    if (filter.genderEnabled() && (filter.genderInversed() != (gender() != filter.gender())))
    {
        return false;
    }
    if (filter.ballEnabled() && (filter.ballInversed() != (ball() != filter.ball())))
    {
        return false;
    }
    if (filter.languageEnabled() && (filter.languageInversed() != (language() != filter.language())))
    {
        return false;
    }
    if (filter.eggEnabled() && (filter.eggInversed() != (egg() != filter.egg())))
    {
        return false;
    }
    for (int i = 0; i < 4; i++)
    {
        if (filter.moveEnabled(i) && (filter.moveInversed(i) != (move(i) != filter.move(i))))
        {
            return false;
        }
        if (filter.relearnMoveEnabled(i))
        {
            if (generation() == Generation::FOUR || generation() == Generation::FIVE)
            {
                return false;
            }
            else if (filter.relearnMoveInversed(i) != (filter.relearnMove(i) != relearnMove(i)))
            {
                return false;
            }
        }
    }
    for (int i = 0; i < 6; i++)
    {
        if (filter.ivEnabled(Stat(i)) && (filter.ivInversed(Stat(i)) != (iv(Stat(i)) < filter.iv(Stat(i)))))
        {
            return false;
        }
    }
    if (filter.shinyEnabled() && (filter.shinyInversed() != (filter.shiny() != shiny())))
    {
        return false;
    }
    if (filter.alternativeFormEnabled() && (filter.alternativeFormInversed() != (filter.alternativeForm() != alternativeForm())))
    {
        return false;
    }
    return true;
}
