/*
*   This file is part of PKSM
*   Copyright (C) 2016-2018 Bernardo Giordano, Admiral Fish, piepie62
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

#include "utils.hpp"

// https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
std::string StringUtils::format(const std::string fmt_str, ...)
{
    int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
    std::unique_ptr<char[]> formatted;
    va_list ap;
    while(1)
    {
        formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
        strcpy(&formatted[0], fmt_str.c_str());
        va_start(ap, fmt_str);
        final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
        va_end(ap);
        if (final_n < 0 || final_n >= n)
            n += abs(final_n - n + 1);
        else
            break;
    }
    return std::string(formatted.get());
}

std::string StringUtils::UTF16toUTF8(std::u16string src)
{
    static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
    std::string dst = convert.to_bytes(src);
    return dst;
}

std::string StringUtils::getString(const u8* data, int ofs, int len)
{
    len *= 2;
    u8 buffer[len];
    std::copy(data + ofs, data + ofs + len, buffer);
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
    std::string dst = convert.to_bytes((char16_t*)buffer);
    return dst;
}

std::string StringUtils::getTrimmedString(const u8* data, int ofs, int len, char* substr)
{
    std::string str = getString(data, ofs, len);
    size_t found = str.find(substr);
    return found != std::string::npos ? str.substr(0, found) : str;
}

void StringUtils::setString(u8* data, const char* v, int ofs, int len)
{
    len *= 2;
    u8 toinsert[len] = {0};
    if (!memcmp(v, toinsert, len)) return;
    
    char buf;
    int nicklen = strlen(v), r = 0, w = 0, i = 0;
    while (r < nicklen || w > len)
    {
        buf = *(v + r++);
        if ((buf & 0x80) == 0)
        {
            toinsert[w] = buf & 0x7f;
            i = 0;
        }
        else if ((buf & 0xe0) == 0xc0)
        {
            toinsert[w] = buf & 0x1f;
            i = 1;
        }
        else if ((buf & 0xf0) == 0xe0)
        {
            toinsert[w] = buf & 0x0f;
            i = 2;
        }
        else break;
        
        for (int j = 0; j < i; j++)
        {
            buf = *(v + r++);
            if (toinsert[w] > 0x04)
            {
                toinsert[w + 1] = (toinsert[w + 1] << 6) | (((toinsert[w] & 0xfc) >> 2) & 0x3f);
                toinsert[w] &= 0x03;
            }
            toinsert[w] = (toinsert[w] << 6) | (buf & 0x3f);
        }
        w += 2;
    }
    memcpy(data + ofs, toinsert, len);
}

void StringUtils::setStringWithBytes(u8* data, const char* v, int ofs, int len, char* padding)
{
    std::string toSet = v;
    toSet.append(padding);
    setString(data, toSet.c_str(), ofs, len);
}

std::string StringUtils::getString4(const u8* data, int ofs, int len)
{
    std::string output;
    len *= 2;
    u16 temp;
    char tempChar[2];
    for (u8 i = 0; i < len; i += 2)
    {
        temp = *(u16*)(data + ofs + i);
        if (temp == 0xFFFF)
            break;
        *(u16*)(tempChar) = ((0 < temp && temp < G4TEXT_LENGTH) ? G4Chars[temp - 2] : G4Chars[G4TEXT_LENGTH - 1]);
        if (*(u16*)(tempChar) == 0xFFFF)
            break;
        output.append((char*)tempChar);
    }
    return output;
}

void StringUtils::setString4(u8* data, const std::string v, int ofs, int len)
{
    u16 output[len] = {0};
    for (int tLen = 0; tLen < len; tLen++)
    {
        size_t index = G4TEXT_LENGTH;
        for (size_t i = 0; i < G4TEXT_LENGTH; i++)
        {
            if (v[tLen] == G4Chars[i])
            {
                index = i;
                break;
            }
        }
        output[tLen] = ((0 < index && index < G4TEXT_LENGTH) ? G4Values[index] : 0x0000);
    }
    output[v.length()] = 0xFFFF;
    memcpy(data + ofs, output, len * 2);
}
