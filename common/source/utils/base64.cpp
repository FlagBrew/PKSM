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

#include "base64.hpp"
#include <array>
#include <functional>
#include <string>

namespace
{
    // clang-format off
    constexpr std::array<char, 64> encoding_table = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', '+', '/'
    };
    // clang-format on
    constexpr std::array<char, 256> decoding_table = std::invoke(
        []()
        {
            std::array<char, 256> ret = {0};
            for (size_t i = 0; i < encoding_table.size(); i++)
            {
                ret[encoding_table[i]] = i;
            }
            return ret;
        });
}

std::vector<unsigned char> base64_decode(const char* data, size_t input_length)
{
    if (input_length % 4 != 0)
        return {};

    size_t output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=')
        output_length--;
    if (data[input_length - 2] == '=')
        output_length--;

    std::vector<unsigned char> ret(output_length);

    for (size_t i = 0, j = 0; i < input_length;)
    {
        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[(size_t)data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[(size_t)data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[(size_t)data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[(size_t)data[i++]];

        uint32_t triple =
            (sextet_a << 3 * 6) + (sextet_b << 2 * 6) + (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

        if (j < output_length)
            ret[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < output_length)
            ret[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < output_length)
            ret[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return ret;
}

std::string base64_encode(const char* data, size_t input_length)
{
    std::string ret(4 * ((input_length + 2) / 3), '=');
    size_t out_index = 0;

    for (size_t i = 0, j = 0; i < input_length; j += 4)
    {
        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        ret[out_index++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        ret[out_index++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        ret[out_index++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        ret[out_index++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    return ret;
}
