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

#include "utils.hpp"
#include <algorithm>

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
        u16 index = std::distance(G4Values, std::find(G4Values, G4Values + G4TEXT_LENGTH, temp));
        codepoint = G4Chars[index];
        if (codepoint == 0xFFFF)
            break;

        // Stupid stupid stupid
        switch (codepoint)
        {
            case 0x246E:
                codepoint = 0x2640;
                break;
            case 0x246D:
                codepoint = 0x2642;
                break;
        }
        
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
        delete[] addChar;
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
            // GAHHHHHH WHY
            switch (codepoint)
            {
                case 0x2640:
                    codepoint = 0x246E; // Female
                    break;
                case 0x2642:
                    codepoint = 0x246D; // Male
                    break;
            }
            size_t index = std::distance(G4Chars, std::find(G4Chars, G4Chars + G4TEXT_LENGTH, codepoint));
            output[outIndex] = (index < G4TEXT_LENGTH ? G4Values[index] : 0x0000); 
        }
        else
        {
            size_t index = std::distance(G4Chars, std::find(G4Chars, G4Chars + G4TEXT_LENGTH, v[charIndex]));
            output[outIndex] = (index < G4TEXT_LENGTH ? G4Values[index] : 0x0000);
        }
    }
    output[outIndex >= len ? len - 1 : outIndex] = 0xFFFF;
    memcpy(data + ofs, output, len * 2);
}

std::string& StringUtils::toLower(std::string& in)
{
    std::transform(in.begin(), in.end(), in.begin(), ::tolower);
    std::u16string otherIn = StringUtils::UTF8toUTF16(in);
    for (size_t i = 0; i < otherIn.size(); i++)
    {
        switch (otherIn[i])
        {
            case u'Í':
                otherIn[i] = u'í';
                break;
            case u'Ó':
                otherIn[i] = u'ó';
                break;
            case u'Ú':
                otherIn[i] = u'ú';
                break;
            case u'É':
                otherIn[i] = u'é';
                break;
            case u'Á':
                otherIn[i] = u'á';
                break;
            case u'Ì':
                otherIn[i] = u'ì';
                break;
            case u'Ò':
                otherIn[i] = u'ò';
                break;
            case u'Ù':
                otherIn[i] = u'ù';
                break;
            case u'È':
                otherIn[i] = u'è';
                break;
            case u'À':
                otherIn[i] = u'à';
                break;
            case u'Ñ':
                otherIn[i] = u'ñ';
                break;
            case u'Æ':
                otherIn[i] = u'æ';
                break;
        }
    }
    in = StringUtils::UTF16toUTF8(otherIn);
    return in;
}

std::string& StringUtils::splitWord(std::string& word, float scaleX, float maxWidth)
{
    float currentWidth = 0.0f;
    if (StringUtils::textWidth(word, scaleX) > maxWidth)
    {
        for (size_t i = 0; i < word.size(); i++)
        {
            u16 codepoint = 0xFFFF;
            int iMod = 0;
            if (word[i] & 0x80 && word[i] & 0x40 && word[i] & 0x20 && !(word[i] & 0x10) && i + 2 < word.size())
            {
                codepoint = word[i] & 0x0F;
                codepoint = codepoint << 6 | (word[i + 1] & 0x3F);
                codepoint = codepoint << 6 | (word[i + 2] & 0x3F);
                iMod = 2;
            }
            else if (word[i] & 0x80 && word[i] & 0x40 && !(word[i] & 0x20) && i + 1 < word.size())
            {
                codepoint = word[i] & 0x1F;
                codepoint = codepoint << 6 | (word[i + 1] & 0x3F);
                iMod = 1;
            }
            else if (!(word[i] & 0x80))
            {
                codepoint = word[i];
            }
            float charWidth = fontGetCharWidthInfo(fontGlyphIndexFromCodePoint(codepoint))->charWidth * scaleX;
            currentWidth += charWidth;
            if (currentWidth > maxWidth)
            {
                word.insert(i, 1, '\n');
                currentWidth = charWidth;
            }

            i += iMod; // Yay, variable width encodings
        }
    }
    return word;
}

float StringUtils::textWidth(const std::string& text, float scaleX)
{
    float ret = 0.0f;
    float largestRet = 0.0f;
    for (size_t i = 0; i < text.size(); i++)
    {
        if (text[i] == '\n')
        {
            largestRet = std::max(largestRet, ret);
            ret = 0.0f;
            continue;
        }
        u16 codepoint = 0xFFFF;
        if (text[i] & 0x80 && text[i] & 0x40 && text[i] & 0x20 && !(text[i] & 0x10) && i + 2 < text.size())
        {
            codepoint = text[i] & 0x0F;
            codepoint = codepoint << 6 | (text[i + 1] & 0x3F);
            codepoint = codepoint << 6 | (text[i + 2] & 0x3F);
            i += 2;
        }
        else if (text[i] & 0x80 && text[i] & 0x40 && !(text[i] & 0x20) && i + 1 < text.size())
        {
            codepoint = text[i] & 0x1F;
            codepoint = codepoint << 6 | (text[i + 1] & 0x3F);
            i += 1;
        }
        else if (!(text[i] & 0x80))
        {
            codepoint = text[i];
        }
        ret += fontGetCharWidthInfo(fontGlyphIndexFromCodePoint(codepoint))->charWidth * scaleX;
    }
    return std::max(largestRet, ret);
}

float StringUtils::textWidth(const std::u16string& text, float scaleX)
{
    float ret = 0.0f;
    float largestRet = 0.0f;
    for (size_t i = 0; i < text.size(); i++)
    {
        if (text[i] == u'\n')
        {
            largestRet = std::max(ret, largestRet);
            ret = 0.0f;
            continue;
        }
        ret += fontGetCharWidthInfo(fontGlyphIndexFromCodePoint(text[i]))->charWidth * scaleX;
    }
    return std::max(largestRet, ret);
}

float StringUtils::textWidth(const C2D_Text& text, float scaleX)
{
    return ceilf(text.width*scaleX);
}