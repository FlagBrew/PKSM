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

#ifndef BASE64_HPP
#define BASE64_HPP

#include "utils/coretypes.h"
#include <span>
#include <string>
#include <string_view>
#include <vector>

std::vector<u8> base64_decode(std::span<const u8> data);
inline std::vector<u8> base64_decode(const std::string_view& data)
{
    return base64_decode(std::span<const u8>{(const u8*)data.data(), data.size()});
}
std::string base64_encode(std::span<const u8> data);

#endif
