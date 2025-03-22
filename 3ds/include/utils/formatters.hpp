/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2025 Bernardo Giordano, Admiral Fish, piepie62
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

#ifndef FORMATTERS_HPP
#define FORMATTERS_HPP

#include <3ds.h>
#include <format>

template <>
struct std::formatter<CFG_Region> : std::formatter<int>
{
    auto format(CFG_Region region, std::format_context& ctx) const
    {
        const char* regionStr;
        switch (region)
        {
            case CFG_REGION_JPN:
                regionStr = "JPN";
                break;
            case CFG_REGION_USA:
                regionStr = "USA";
                break;
            case CFG_REGION_EUR:
                regionStr = "EUR";
                break;
            case CFG_REGION_AUS:
                regionStr = "AUS";
                break;
            case CFG_REGION_CHN:
                regionStr = "CHN";
                break;
            case CFG_REGION_KOR:
                regionStr = "KOR";
                break;
            case CFG_REGION_TWN:
                regionStr = "TWN";
                break;
            default:
                regionStr = "Unknown";
                break;
        }

        std::formatter<std::string_view> fmt;
        return fmt.format(regionStr, ctx);
    }
};

#endif