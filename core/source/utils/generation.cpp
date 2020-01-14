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

#include "generation.hpp"

std::string genToString(Generation gen)
{
    return genToCstring(gen);
}

const char* genToCstring(Generation gen)
{
    switch (gen)
    {
        case Generation::LGPE:
            return "LGPE";
        case Generation::THREE:
            return "3";
        case Generation::FOUR:
            return "4";
        case Generation::FIVE:
            return "5";
        case Generation::SIX:
            return "6";
        case Generation::SEVEN:
            return "7";
        case Generation::EIGHT:
            return "8";
        case Generation::UNUSED:
            return "INVALID";
    }
    return "INVALID";
}

Generation stringToGen(const std::string& str)
{
    if (str == "3")
    {
        return Generation::THREE;
    }
    else if (str == "4")
    {
        return Generation::FOUR;
    }
    else if (str == "5")
    {
        return Generation::FIVE;
    }
    else if (str == "6")
    {
        return Generation::SIX;
    }
    else if (str == "7")
    {
        return Generation::SEVEN;
    }
    else if (str == "LGPE")
    {
        return Generation::LGPE;
    }
    else if (str == "8")
    {
        return Generation::EIGHT;
    }
    return Generation::UNUSED;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
bool operator<(Generation g1, Generation g2)
{
    switch (g1)
    {
        case Generation::THREE:
            if (g2 == Generation::FOUR)
            {
                return true;
            }
        case Generation::FOUR:
            if (g2 == Generation::FIVE)
            {
                return true;
            }
        case Generation::FIVE:
            if (g2 == Generation::SIX)
            {
                return true;
            }
        case Generation::SIX:
            if (g2 == Generation::SEVEN)
            {
                return true;
            }
        case Generation::SEVEN:
            if (g2 == Generation::LGPE)
            {
                return true;
            }
        case Generation::LGPE:
            if (g2 == Generation::EIGHT)
            {
                return true;
            }
        case Generation::EIGHT:
            if (g2 == Generation::UNUSED)
            {
                return true;
            }
        case Generation::UNUSED:
        default:
            return false;
    }
}
#pragma GCC diagnostic pop

bool operator>(Generation g1, Generation g2)
{
    if (g1 == g2)
    {
        return false;
    }
    else
    {
        return !(g1 < g2);
    }
}
