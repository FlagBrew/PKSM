/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2022 Bernardo Giordano, Admiral Fish, piepie62
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

#ifndef SAVEGROUPS_HPP
#define SAVEGROUPS_HPP

#include "types.h"

enum class SystemGroup : u8
{
    GB_GBC_GBA,
    DS_3DS,
    SWITCH
};

enum class SaveGroup : u8
{
    // GB_GBC_GBA
    RGB,
    Y,
    GS,
    C,
    RS,
    E,
    FRLG,
    // DS_3DS
    DP,
    HGSS,
    Pt,
    BW,
    B2W2,
    XY,
    ORAS,
    SM,
    USUM,
    // SWITCH
    LGPE,
    SwSh
};

inline SaveGroup calcSaveGroupFromSystem(SystemGroup system, u8 offset)
{
    switch (system)
    {
        case SystemGroup::GB_GBC_GBA:
            switch (offset)
            {
                default:
                case 0:
                    return SaveGroup::RGB;
                case 1:
                    return SaveGroup::Y;
                case 2:
                    return SaveGroup::GS;
                case 3:
                    return SaveGroup::C;
                case 4:
                    return SaveGroup::RS;
                case 5:
                    return SaveGroup::E;
                case 6:
                    return SaveGroup::FRLG;
            }
        default:
        case SystemGroup::DS_3DS:
            switch (offset)
            {
                default:
                case 0:
                    return SaveGroup::DP;
                case 1:
                    return SaveGroup::HGSS;
                case 2:
                    return SaveGroup::Pt;
                case 3:
                    return SaveGroup::BW;
                case 4:
                    return SaveGroup::B2W2;
                case 5:
                    return SaveGroup::XY;
                case 6:
                    return SaveGroup::ORAS;
                case 7:
                    return SaveGroup::SM;
                case 8:
                    return SaveGroup::USUM;
            }
        case SystemGroup::SWITCH:
            switch (offset)
            {
                default:
                case 0:
                    return SaveGroup::LGPE;
                case 1:
                    return SaveGroup::SwSh;
            }
    }
}

inline SystemGroup& operator++(SystemGroup& g)
{
    switch (g)
    {
        using enum SystemGroup;
        case GB_GBC_GBA:
        default:
            return g = DS_3DS;
        case DS_3DS:
            return g = SWITCH;
        case SWITCH:
            return g = GB_GBC_GBA;
    }
}

inline SystemGroup operator++(SystemGroup& g, int)
{
    SystemGroup ret = g;
    switch (g)
    {
        using enum SystemGroup;
        case GB_GBC_GBA:
        default:
            g = DS_3DS;
            break;
        case DS_3DS:
            g = SWITCH;
            break;
        case SWITCH:
            g = GB_GBC_GBA;
            break;
    }
    return ret;
}

inline SystemGroup& operator--(SystemGroup& g)
{
    switch (g)
    {
        using enum SystemGroup;
        case GB_GBC_GBA:
            return g = SWITCH;
        case DS_3DS:
            return g = GB_GBC_GBA;
        case SWITCH:
        default:
            return g = DS_3DS;
    }
}

inline SystemGroup operator--(SystemGroup& g, int)
{
    SystemGroup ret = g;
    switch (g)
    {
        using enum SystemGroup;
        case GB_GBC_GBA:
            g = SWITCH;
            break;
        case DS_3DS:
            g = GB_GBC_GBA;
            break;
        case SWITCH:
        default:
            g = DS_3DS;
            break;
    }
    return ret;
}

#endif
