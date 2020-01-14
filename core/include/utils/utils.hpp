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

#ifndef UTILS_HPP
#define UTILS_HPP

#include "coretypes.h"
#include <codecvt>
#include <locale>
#include <memory>
#include <optional>
#include <stdarg.h>
#include <string.h>
#include <string>
#include <vector>

namespace StringUtils
{
    std::string format(std::string fmt_str, ...);
    std::u16string UTF8toUTF16(const std::string& src);
    std::string UTF16toUTF8(const std::u16string& src);
    std::u16string getU16String(const u8* data, int ofs, int len, char16_t term);
    std::string getString(const u8* data, int ofs, int len, char16_t term = 0);
    void setString(u8* data, const std::u16string& v, int ofs, int len, char16_t terminator = 0, char16_t padding = 0);
    void setString(u8* data, const std::string& v, int ofs, int len, char16_t terminator = 0, char16_t padding = 0);
    std::string getString4(const u8* data, int ofs, int len);
    void setString4(u8* data, const std::string& v, int ofs, int len);
    std::string getString3(const u8* data, int ofs, int len, bool jp);
    void setString3(u8* data, const std::string& v, int ofs, int len, bool jp);
    std::vector<u16> stringToG4(const std::string& v);
    std::string& toLower(std::string& in);
    std::string& toUpper(std::string& in);
    /** Swaps and reswaps Pokemon specific characters, namely:
     * 0x2227, 0x2228, 0x2460, 0x2461, 0x2462,
     * 0x2463, 0x2464, 0x2465, 0x2466, 0x2469,
     * 0x246A, 0x246B, 0x246C, 0x246D, 0x246E,
     * 0x246F, 0x2470, 0x2471, 0x2472, 0x2473,
     * 0x2474, 0x2475, 0x2476, 0x2477, 0x2478,
     * 0x2479, 0x247A, 0x247B, 0x247C, 0x247D,
     * 0x247E, 0x247F, 0x2480, 0x2481, 0x2482,
     * 0x2483, 0x2484, 0x2485, 0x2486, and 0x2487
     */
    std::string transString45(const std::string& str);
    std::u16string transString45(const std::u16string& str);
    /** Swaps and reswaps Pokemon specific characters, namely:
     * 0xE088, 0xE089, 0xE08A, 0xE08B, 0xE08C, 0xE0A6
     */
    std::string transString67(const std::string& str);
    std::u16string transString67(const std::u16string& str);
}

#endif
