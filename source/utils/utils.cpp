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

static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;

std::string StringUtils::format(const std::string fmt_str, ...)
{
    va_list ap;
    char *fp = NULL;
    va_start(ap, fmt_str);
    vasprintf(&fp, fmt_str.c_str(), ap);
    va_end(ap);
    std::unique_ptr<char, decltype(free)*> formatted(fp, free);
    return std::string(formatted.get());
}

std::u16string StringUtils::UTF8toUTF16(std::string src)
{
    return convert.from_bytes(src);
}

std::string StringUtils::UTF16toUTF8(std::u16string src)
{
    return convert.to_bytes(src);
}

std::string StringUtils::getString(const u8* data, int ofs, int len)
{
    len *= 2;
    u8 buffer[len];
    std::copy(data + ofs, data + ofs + len, buffer);
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
    u16 codepoint;
    for (u8 i = 0; i < len; i += 2)
    {
        temp = *(u16*)(data + ofs + i);
        if (temp == 0xFFFF)
            break;
        codepoint = ((0 < temp && temp < G4TEXT_LENGTH) ? G4Chars[temp - 2] : G4Chars[G4TEXT_LENGTH - 1]);
        if (codepoint == 0xFFFF)
            break;
        
        char* addChar;
        if (codepoint < 0x0080)
        {
            addChar = new char[2];
            addChar[0] = codepoint;
            addChar[1] = '\0';
        }
        else if (codepoint < 0x0800)
        {
            addChar = new char[3];
            addChar[0] = 0xC0 | ((codepoint >> 6) & 0x1F);
            addChar[1] = 0x80 | (codepoint & 0x3F);
            addChar[2] = '\0';
        }
        else
        {
            addChar = new char[4];
            addChar[0] = 0xE0 | ((codepoint >> 12) & 0x0F);
            addChar[1] = 0x80 | ((codepoint >> 6) & 0x3F);
            addChar[2] = 0x80 | (codepoint & 0x3F);
            addChar[3] = '\0';
        }
        output.append(addChar);
    }
    return output;
}

void StringUtils::setString4(u8* data, const std::string v, int ofs, int len)
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
            size_t index = G4TEXT_LENGTH;
            for (size_t i = 0; i < G4TEXT_LENGTH; i++)
            {
                if (codepoint == G4Chars[i])
                {
                    index = i;
                    break;
                }
            }
            output[outIndex] = ((0 < index && index < G4TEXT_LENGTH) ? G4Values[index] : 0x0000); 
        }
        else
        {
            size_t index = G4TEXT_LENGTH;
            for (size_t i = 0; i < G4TEXT_LENGTH; i++)
            {
                if (v[charIndex] == G4Chars[i])
                {
                    index = i;
                    break;
                }
            }
            output[outIndex] = ((0 < index && index < G4TEXT_LENGTH) ? G4Values[index] : 0x0000);
        }
    }
    output[outIndex == len ? len - 1 : outIndex] = 0xFFFF;
    memcpy(data + ofs, output, len * 2);
}
