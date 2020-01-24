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

#include "utils/utils.hpp"
#include "utils/endian.hpp"
#include "utils/g3text.hpp"
#include "utils/g4text.h"
#include <algorithm>
#include <map>
#include <queue>
#include <vector>

namespace
{
    std::string utf16DataToUtf8(const char16_t* data, size_t size, char16_t delim = 0)
    {
        std::string ret;
        ret.reserve(size);
        char addChar[4] = {0};
        for (size_t i = 0; i < size; i++)
        {
            if (data[i] == delim)
            {
                return ret;
            }
            else if (data[i] < 0x0080)
            {
                addChar[0] = data[i];
                addChar[1] = '\0';
            }
            else if (data[i] < 0x0800)
            {
                addChar[0] = 0xC0 | ((data[i] >> 6) & 0x1F);
                addChar[1] = 0x80 | (data[i] & 0x3F);
                addChar[2] = '\0';
            }
            else
            {
                addChar[0] = 0xE0 | ((data[i] >> 12) & 0x0F);
                addChar[1] = 0x80 | ((data[i] >> 6) & 0x3F);
                addChar[2] = 0x80 | (data[i] & 0x3F);
                addChar[3] = '\0';
            }
            ret.append(addChar);
        }
        return ret;
    }
    u32 swapCodepoints45(u32 codepoint)
    {
        switch (codepoint)
        {
            case u'\u2227':
                codepoint = u'\uE0A9';
                break;
            case u'\u2228':
                codepoint = u'\uE0AA';
                break;
            case u'\u2460':
                codepoint = u'\uE081';
                break;
            case u'\u2461':
                codepoint = u'\uE082';
                break;
            case u'\u2462':
                codepoint = u'\uE083';
                break;
            case u'\u2463':
                codepoint = u'\uE084';
                break;
            case u'\u2464':
                codepoint = u'\uE085';
                break;
            case u'\u2465':
                codepoint = u'\uE086';
                break;
            case u'\u2466':
                codepoint = u'\uE087';
                break;
            case u'\u2469':
                codepoint = u'\uE068';
                break;
            case u'\u246A':
                codepoint = u'\uE069';
                break;
            case u'\u246B':
                codepoint = u'\uE0AB';
                break;
            case u'\u246C':
                codepoint = u'\uE08D';
                break;
            case u'\u246D':
                codepoint = u'\uE08E';
                break;
            case u'\u246E':
                codepoint = u'\uE08F';
                break;
            case u'\u246F':
                codepoint = u'\uE090';
                break;
            case u'\u2470':
                codepoint = u'\uE091';
                break;
            case u'\u2471':
                codepoint = u'\uE092';
                break;
            case u'\u2472':
                codepoint = u'\uE093';
                break;
            case u'\u2473':
                codepoint = u'\uE094';
                break;
            case u'\u2474':
                codepoint = u'\uE095';
                break;
            case u'\u2475':
                codepoint = u'\uE096';
                break;
            case u'\u2476':
                codepoint = u'\uE097';
                break;
            case u'\u2477':
                codepoint = u'\uE098';
                break;
            case u'\u2478':
                codepoint = u'\uE099';
                break;
            case u'\u2479':
                codepoint = u'\uE09A';
                break;
            case u'\u247A':
                codepoint = u'\uE09B';
                break;
            case u'\u247B':
                codepoint = u'\uE09C';
                break;
            case u'\u247C':
                codepoint = u'\uE09D';
                break;
            case u'\u247D':
                codepoint = u'\uE09E';
                break;
            case u'\u247E':
                codepoint = u'\uE09F';
                break;
            case u'\u247F':
                codepoint = u'\uE0A0';
                break;
            case u'\u2480':
                codepoint = u'\uE0A1';
                break;
            case u'\u2481':
                codepoint = u'\uE0A2';
                break;
            case u'\u2482':
                codepoint = u'\uE0A3';
                break;
            case u'\u2483':
                codepoint = u'\uE0A4';
                break;
            case u'\u2484':
                codepoint = u'\uE0A5';
                break;
            case u'\u2485':
                codepoint = u'\uE06A';
                break;
            case u'\u2486':
                codepoint = u'\uE0A7';
                break;
            case u'\u2487':
                codepoint = u'\uE0A8';
                break;

            case u'\uE0A9':
                codepoint = u'\u2227';
                break;
            case u'\uE0AA':
                codepoint = u'\u2228';
                break;
            case u'\uE081':
                codepoint = u'\u2460';
                break;
            case u'\uE082':
                codepoint = u'\u2461';
                break;
            case u'\uE083':
                codepoint = u'\u2462';
                break;
            case u'\uE084':
                codepoint = u'\u2463';
                break;
            case u'\uE085':
                codepoint = u'\u2464';
                break;
            case u'\uE086':
                codepoint = u'\u2465';
                break;
            case u'\uE087':
                codepoint = u'\u2466';
                break;
            case u'\uE068':
                codepoint = u'\u2469';
                break;
            case u'\uE069':
                codepoint = u'\u246A';
                break;
            case u'\uE0AB':
                codepoint = u'\u246B';
                break;
            case u'\uE08D':
                codepoint = u'\u246C';
                break;
            case u'\uE08E':
                codepoint = u'\u246D';
                break;
            case u'\uE08F':
                codepoint = u'\u246E';
                break;
            case u'\uE090':
                codepoint = u'\u246F';
                break;
            case u'\uE091':
                codepoint = u'\u2470';
                break;
            case u'\uE092':
                codepoint = u'\u2471';
                break;
            case u'\uE093':
                codepoint = u'\u2472';
                break;
            case u'\uE094':
                codepoint = u'\u2473';
                break;
            case u'\uE095':
                codepoint = u'\u2474';
                break;
            case u'\uE096':
                codepoint = u'\u2475';
                break;
            case u'\uE097':
                codepoint = u'\u2476';
                break;
            case u'\uE098':
                codepoint = u'\u2477';
                break;
            case u'\uE099':
                codepoint = u'\u2478';
                break;
            case u'\uE09A':
                codepoint = u'\u2479';
                break;
            case u'\uE09B':
                codepoint = u'\u247A';
                break;
            case u'\uE09C':
                codepoint = u'\u247B';
                break;
            case u'\uE09D':
                codepoint = u'\u247C';
                break;
            case u'\uE09E':
                codepoint = u'\u247D';
                break;
            case u'\uE09F':
                codepoint = u'\u247E';
                break;
            case u'\uE0A0':
                codepoint = u'\u247F';
                break;
            case u'\uE0A1':
                codepoint = u'\u2480';
                break;
            case u'\uE0A2':
                codepoint = u'\u2481';
                break;
            case u'\uE0A3':
                codepoint = u'\u2482';
                break;
            case u'\uE0A4':
                codepoint = u'\u2483';
                break;
            case u'\uE0A5':
                codepoint = u'\u2484';
                break;
            case u'\uE06A':
                codepoint = u'\u2485';
                break;
            case u'\uE0A7':
                codepoint = u'\u2486';
                break;
            case u'\uE0A8':
                codepoint = u'\u2487';
                break;
            default:
                break;
        }
        return codepoint;
    }
    u32 swapCodepoints67(u32 codepoint)
    {
        switch (codepoint)
        {
            case u'\uE088':
                codepoint = u'\u00D7';
                break;
            case u'\uE089':
                codepoint = u'\u00F7';
                break;
            case u'\uE08A':
                codepoint = u'\uE068';
                break;
            case u'\uE08B':
                codepoint = u'\uE069';
                break;
            case u'\uE08C':
                codepoint = u'\uE0AB';
                break;
            case u'\uE0A6':
                codepoint = u'\uE06A';
                break;

            case u'\u00D7':
                codepoint = u'\uE088';
                break;
            case u'\u00F7':
                codepoint = u'\uE089';
                break;
            case u'\uE068':
                codepoint = u'\uE08A';
                break;
            case u'\uE069':
                codepoint = u'\uE08B';
                break;
            case u'\uE0AB':
                codepoint = u'\uE08C';
                break;
            case u'\uE06A':
                codepoint = u'\uE0A6';
                break;
        }
        return codepoint;
    }
}

std::u16string StringUtils::UTF8toUTF16(const std::string& src)
{
    std::u16string ret;
    ret.reserve(src.size());
    for (size_t i = 0; i < src.size(); i++)
    {
        u16 codepoint = 0xFFFD;
        int iMod      = 0;
        if (src[i] & 0x80 && src[i] & 0x40 && src[i] & 0x20 && !(src[i] & 0x10) && i + 2 < src.size())
        {
            codepoint = src[i] & 0x0F;
            codepoint = codepoint << 6 | (src[i + 1] & 0x3F);
            codepoint = codepoint << 6 | (src[i + 2] & 0x3F);
            iMod      = 2;
        }
        else if (src[i] & 0x80 && src[i] & 0x40 && !(src[i] & 0x20) && i + 1 < src.size())
        {
            codepoint = src[i] & 0x1F;
            codepoint = codepoint << 6 | (src[i + 1] & 0x3F);
            iMod      = 1;
        }
        else if (!(src[i] & 0x80))
        {
            codepoint = src[i];
        }

        ret.push_back((char16_t)codepoint);
        i += iMod;
    }
    return ret;
}

std::string StringUtils::UTF16toUTF8(const std::u16string& src)
{
    return utf16DataToUtf8(src.data(), src.size());
}

std::u16string StringUtils::getU16String(const u8* data, int ofs, int len, char16_t term)
{
    std::u16string ret;
    ret.reserve(len);
    const char16_t* buf = (char16_t*)(data + ofs);
    for (int i = 0; i < len; i++)
    {
        if (buf[i] == term)
        {
            return ret;
        }
        ret.push_back(buf[i]);
    }
    return ret;
}

std::string StringUtils::getString(const u8* data, int ofs, int len, char16_t term)
{
    return utf16DataToUtf8((char16_t*)(data + ofs), len, term);
}

void StringUtils::setString(u8* data, const std::u16string& v, int ofs, int len, char16_t terminator, char16_t padding)
{
    int i = 0;
    for (; i < std::min(len - 1, (int)v.size()); i++) // len includes terminator
    {
        Endian::convertFrom<char16_t>(data + ofs + i * 2, v[i]);
    }
    Endian::convertFrom<char16_t>(data + ofs + i++ * 2, terminator); // Set terminator
    for (; i < len; i++)
    {
        Endian::convertFrom<char16_t>(data + ofs + i * 2, padding); // Set final padding bytes
    }
}

void StringUtils::setString(u8* data, const std::string& v, int ofs, int len, char16_t terminator, char16_t padding)
{
    setString(data, UTF8toUTF16(v), ofs, len, terminator, padding);
}

std::string StringUtils::getString4(const u8* data, int ofs, int len)
{
    std::string output;
    len *= 2;
    char addChar[4];
    for (u8 i = 0; i < len; i += 2)
    {
        u16 temp = Endian::convertTo<u16>(data + ofs + i);
        if (temp == 0xFFFF)
            break;
        u16 index     = std::distance(G4Values, std::find(G4Values, G4Values + G4TEXT_LENGTH, temp));
        u16 codepoint = G4Chars[index];
        if (codepoint == 0xFFFF)
            break;
        if (codepoint < 0x0080)
        {
            addChar[0] = codepoint;
            addChar[1] = '\0';
        }
        else if (codepoint < 0x0800)
        {
            addChar[0] = 0xC0 | ((codepoint >> 6) & 0x1F);
            addChar[1] = 0x80 | (codepoint & 0x3F);
            addChar[2] = '\0';
        }
        else
        {
            addChar[0] = 0xE0 | ((codepoint >> 12) & 0x0F);
            addChar[1] = 0x80 | ((codepoint >> 6) & 0x3F);
            addChar[2] = 0x80 | (codepoint & 0x3F);
            addChar[3] = '\0';
        }
        output.append(addChar);
    }
    return output;
}

std::vector<u16> StringUtils::stringToG4(const std::string& v)
{
    std::vector<u16> ret;
    for (size_t charIndex = 0; charIndex < v.length(); charIndex++)
    {
        if (v[charIndex] & 0x80)
        {
            u16 codepoint = 0;
            if (v[charIndex] & 0x80 && v[charIndex] & 0x40 && v[charIndex] & 0x20)
            {
                codepoint = v[charIndex] & 0x0F;
                codepoint = codepoint << 6 | (v[charIndex + 1] & 0x3F);
                codepoint = codepoint << 6 | (v[charIndex + 2] & 0x3F);
                charIndex += 2;
            }
            else if (v[charIndex] & 0x80 && v[charIndex] & 0x40)
            {
                codepoint = v[charIndex] & 0x1F;
                codepoint = codepoint << 6 | (v[charIndex + 1] & 0x3F);
                charIndex += 1;
            }
            size_t index = std::distance(G4Chars, std::find(G4Chars, G4Chars + G4TEXT_LENGTH, codepoint));
            ret.push_back(index < G4TEXT_LENGTH ? G4Values[index] : 0x0000);
        }
        else
        {
            size_t index = std::distance(G4Chars, std::find(G4Chars, G4Chars + G4TEXT_LENGTH, v[charIndex]));
            ret.push_back(index < G4TEXT_LENGTH ? G4Values[index] : 0x0000);
        }
    }
    if (ret.back() != 0xFFFF)
    {
        ret.push_back(0xFFFF);
    }
    return ret;
}

void StringUtils::setString4(u8* data, const std::string& v, int ofs, int len)
{
    u16 output[len] = {0};
    u16 outIndex = 0, charIndex = 0;
    for (; outIndex < len && charIndex < v.length(); charIndex++, outIndex++)
    {
        if (v[charIndex] & 0x80)
        {
            u16 codepoint = 0;
            if (v[charIndex] & 0x80 && v[charIndex] & 0x40 && v[charIndex] & 0x20)
            {
                codepoint = v[charIndex] & 0x0F;
                codepoint = codepoint << 6 | (v[charIndex + 1] & 0x3F);
                codepoint = codepoint << 6 | (v[charIndex + 2] & 0x3F);
                charIndex += 2;
            }
            else if (v[charIndex] & 0x80 && v[charIndex] & 0x40)
            {
                codepoint = v[charIndex] & 0x1F;
                codepoint = codepoint << 6 | (v[charIndex + 1] & 0x3F);
                charIndex += 1;
            }
            size_t index     = std::distance(G4Chars, std::find(G4Chars, G4Chars + G4TEXT_LENGTH, codepoint));
            output[outIndex] = (index < G4TEXT_LENGTH ? G4Values[index] : 0x0000);
        }
        else
        {
            size_t index     = std::distance(G4Chars, std::find(G4Chars, G4Chars + G4TEXT_LENGTH, v[charIndex]));
            output[outIndex] = (index < G4TEXT_LENGTH ? G4Values[index] : 0x0000);
        }
    }
    output[outIndex >= len ? len - 1 : outIndex] = 0xFFFF;
    memcpy(data + ofs, output, len * 2);
}

std::string& StringUtils::toUpper(std::string& in)
{
    std::transform(in.begin(), in.end(), in.begin(), ::toupper);
    // Just saying, I have NO clue why two outer braces levels are necessary
    static constexpr std::array<std::pair<std::string_view, std::string_view>, 12> transStrings = {{{"í", "Í"}, {"ó", "Ó"}, {"ú", "Ú"}, {"é", "É"},
        {"á", "Á"}, {"ì", "Ì"}, {"ò", "Ò"}, {"ù", "Ù"}, {"è", "È"}, {"à", "À"}, {"ñ", "Ñ"}, {"æ", "Æ"}}};
    for (auto& str : transStrings)
    {
        size_t found;
        while ((found = in.find(str.first)) != std::string::npos)
        {
            in.replace(found, str.first.size(), str.second);
        }
    }
    return in;
}

std::string& StringUtils::toLower(std::string& in)
{
    std::transform(in.begin(), in.end(), in.begin(), ::tolower);
    // Just saying, I have NO clue why two outer braces levels are necessary
    static constexpr std::array<std::pair<std::string_view, std::string_view>, 12> transStrings = {{{"Í", "í"}, {"Ó", "ó"}, {"Ú", "ú"}, {"É", "é"},
        {"Á", "á"}, {"Ì", "ì"}, {"Ò", "ò"}, {"Ù", "ù"}, {"È", "è"}, {"À", "à"}, {"Ñ", "ñ"}, {"Æ", "æ"}}};
    for (auto& str : transStrings)
    {
        size_t found;
        while ((found = in.find(str.first)) != std::string::npos)
        {
            in.replace(found, str.first.size(), str.second);
        }
    }
    return in;
}

std::string StringUtils::transString45(const std::string& str)
{
    return UTF16toUTF8(transString45(UTF8toUTF16(str)));
}

std::u16string StringUtils::transString45(const std::u16string& str)
{
    std::u16string ret = str;
    std::transform(str.begin(), str.end(), ret.begin(), [](const char16_t& chr) { return (char16_t)swapCodepoints45(chr); });
    return ret;
}

std::string StringUtils::transString67(const std::string& str)
{
    return UTF16toUTF8(transString67(UTF8toUTF16(str)));
}

std::u16string StringUtils::transString67(const std::u16string& str)
{
    std::u16string ret = str;
    std::transform(str.begin(), str.end(), ret.begin(), [](const char16_t& chr) { return (char16_t)swapCodepoints67(chr); });
    return ret;
}

std::string StringUtils::getString3(const u8* data, int ofs, int len, bool jp)
{
    auto& characters = jp ? G3_JP : G3_EN;
    std::u16string outString;

    for (size_t i = 0; i < len; i++)
    {
        if (data[ofs + i] < characters.size())
        {
            outString += characters[data[ofs + i]];
        }
        else
        {
            break;
        }
    }

    return StringUtils::UTF16toUTF8(outString);
}

void StringUtils::setString3(u8* data, const std::string& v, int ofs, int len, bool jp)
{
    auto& characters   = jp ? G3_JP : G3_EN;
    std::u16string str = StringUtils::UTF8toUTF16(v);

    size_t outPos;
    for (outPos = 0; outPos < std::min((size_t)len, str.size()); outPos++)
    {
        auto it = std::find(characters.begin(), characters.end(), str[outPos]);
        if (it != characters.end())
        {
            data[ofs + outPos] = (u8)std::distance(characters.begin(), it);
        }
        else
        {
            break;
        }
    }

    data[outPos >= len ? len - 1 : outPos] = 0xFF;
}
