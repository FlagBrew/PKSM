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

#ifndef ENDIAN_HPP
#define ENDIAN_HPP

#include "coretypes.h"
#include <string.h>
#include <type_traits>

namespace Endian
{
    // Endianness checking found in https://stackoverflow.com/questions/4239993/determining-endianness-at-compile-time
    // Only works with integral types
    template <typename T>
    void convertFrom(u8* dest, const T& orig)
    {
        static_assert(std::is_integral_v<T>);
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ ||                    \
    defined(__BIG_ENDIAN__) || defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || defined(_MIBSEB) || defined(__MIBSEB) ||      \
    defined(__MIBSEB__)
        {
            // Explicitly allowed by the C++ standard
            u8* from = (u8*)&orig;
            for (size_t i = 0; i < sizeof(T); i++)
            {
                dest[i] = from[sizeof(T) - 1 - i];
            }
        }
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ||            \
    defined(__LITTLE_ENDIAN__) || defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(_MIPSEL) || defined(__MIPSEL) ||   \
    defined(__MIPSEL__)
        memcpy(dest, &orig, sizeof(T));
#else
#error "I don't know what architecture this is!"
#endif
    }

    template <typename T>
    T convertTo(u8* from)
    {
        static_assert(std::is_integral_v<T>);
        T dest;
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ ||                    \
    defined(__BIG_ENDIAN__) || defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || defined(_MIBSEB) || defined(__MIBSEB) ||      \
    defined(__MIBSEB__)
        {
            for (size_t i = 0; i < sizeof(T); i++)
            {
                dest |= T(from[i]) << (i * 8);
            }
        }
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ||            \
    defined(__LITTLE_ENDIAN__) || defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(_MIPSEL) || defined(__MIPSEL) ||   \
    defined(__MIPSEL__)
        // Already in proper format
        memcpy(&dest, from, sizeof(T));
#else
#error "I don't know what architecture this is!"
#endif
        return dest;
    }
}

#endif
