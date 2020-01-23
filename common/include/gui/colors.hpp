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

#ifndef COLORS_HPP
#define COLORS_HPP

#include "types.h"

struct PKSM_Color
{
    constexpr PKSM_Color(u8 r, u8 g, u8 b, u8 a) : r(r), g(g), b(b), a(a) {}
    u8 r, g, b, a;
};

constexpr PKSM_Color COLOR_WHITE        = PKSM_Color(255, 255, 255, 255);
constexpr PKSM_Color COLOR_BLACK        = PKSM_Color(0, 0, 0, 255);
constexpr PKSM_Color COLOR_BLUE         = PKSM_Color(124, 147, 225, 255);
constexpr PKSM_Color COLOR_YELLOW       = PKSM_Color(237, 247, 157, 255);
constexpr PKSM_Color COLOR_LIGHTBLUE    = PKSM_Color(187, 208, 254, 255);
constexpr PKSM_Color COLOR_DARKBLUE     = PKSM_Color(55, 89, 187, 255);
constexpr PKSM_Color COLOR_HIGHBLUE     = PKSM_Color(48, 65, 106, 255);
constexpr PKSM_Color COLOR_MASKBLACK    = PKSM_Color(0, 0, 0, 190);
constexpr PKSM_Color COLOR_PALEBLUE     = PKSM_Color(90, 115, 164, 255);
constexpr PKSM_Color COLOR_MENUBLUE     = PKSM_Color(55, 89, 157, 255);
constexpr PKSM_Color COLOR_SELECTOR     = PKSM_Color(29, 50, 253, 255);
constexpr PKSM_Color COLOR_GREY_BLEND   = PKSM_Color(0, 0, 0, 100);
constexpr PKSM_Color COLOR_SELECTBLUE   = PKSM_Color(33, 33, 66, 255);
constexpr PKSM_Color COLOR_UNAVAILBLUE  = PKSM_Color(131, 131, 182, 255);
constexpr PKSM_Color COLOR_UNSELECTBLUE = PKSM_Color(77, 77, 254, 255);
constexpr PKSM_Color COLOR_SELECTRED    = PKSM_Color(66, 33, 33, 255);
constexpr PKSM_Color COLOR_UNAVAILRED   = PKSM_Color(182, 131, 131, 255);
constexpr PKSM_Color COLOR_UNSELECTRED  = PKSM_Color(254, 77, 77, 255);
constexpr PKSM_Color COLOR_GREY         = PKSM_Color(128, 128, 128, 255);
constexpr PKSM_Color COLOR_DARKGREY     = PKSM_Color(96, 96, 96, 255);
constexpr PKSM_Color COLOR_DARKYELLOW   = PKSM_Color(244, 170, 6, 120);
constexpr PKSM_Color COLOR_LINEBLUE     = PKSM_Color(31, 41, 139, 255);

#if defined(_3DS)
#include <citro2d.h>
typedef u32 ColorFormat;
inline constexpr ColorFormat colorToFormat(const PKSM_Color& color)
{
    return C2D_Color32(color.r, color.g, color.b, color.a);
}
#elif defined(__SWITCH__)
typedef something ColorFormat;
inline constexpr ColorFormat colorToFormat(const C2D_Color& color)
{
    return format(color.r, color.g, color.b, color.a);
}
#endif

#endif
