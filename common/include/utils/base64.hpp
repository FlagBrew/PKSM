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

#ifndef BASE64_HPP
#define BASE64_HPP

#include <string>
#include <vector>

std::vector<unsigned char> base64_decode(const char* data, size_t input_length);
inline std::vector<unsigned char> base64_decode(const std::string& data)
{
    return base64_decode(data.data(), data.size());
}
inline std::vector<unsigned char> base64_decode(const uint8_t* data, size_t input_length)
{
    return base64_decode((char*)data, input_length);
}
std::string base64_encode(const char* data, size_t input_length);
inline std::string base64_encode(const unsigned char* data, size_t input_length)
{
    return base64_encode((char*)data, input_length);
}
inline std::string base64_encode(const std::vector<char>& data)
{
    return base64_encode(data.data(), data.size());
}
inline std::string base64_encode(const std::vector<unsigned char>& data)
{
    return base64_encode(data.data(), data.size());
}

#endif
