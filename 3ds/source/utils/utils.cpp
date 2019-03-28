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

#include "3dsutils.hpp"
#include <algorithm>
#include <vector>
#include <map>
#include <queue>
#include "g4text.h"

static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
namespace Gui
{
    extern std::vector<C2D_Font> fonts;
}

std::string StringUtils::format(const std::string& fmt_str, ...)
{
    va_list ap;
    char *fp = NULL;
    va_start(ap, fmt_str);
    vasprintf(&fp, fmt_str.c_str(), ap);
    va_end(ap);
    std::unique_ptr<char, decltype(free)*> formatted(fp, free);
    return std::string(formatted.get());
}

std::u16string StringUtils::UTF8toUTF16(const std::string& src)
{
    return convert.from_bytes(src);
}

std::string StringUtils::UTF16toUTF8(const std::u16string& src)
{
    return convert.to_bytes(src);
}

std::u16string StringUtils::getU16String(const u8* data, int ofs, int len)
{
    len *= 2;
    u8 buffer[len];
    std::copy(data + ofs, data + ofs + len, buffer);
    return std::u16string{(char16_t*)buffer};
}

std::string StringUtils::getString(const u8* data, int ofs, int len)
{
    len *= 2;
    u8 buffer[len];
    std::copy(data + ofs, data + ofs + len, buffer);
    std::string dst = convert.to_bytes((char16_t*)buffer);
    return dst;
}

std::u16string StringUtils::getTrimmedU16String(const u8* data, int ofs, int len, char16_t* substr)
{
    std::u16string str = getU16String(data, ofs, len);
    size_t found = str.find(substr);
    return found != std::string::npos ? str.substr(0, found) : str;
}

std::string StringUtils::getTrimmedString(const u8* data, int ofs, int len, char* substr)
{
    std::string str = getString(data, ofs, len);
    size_t found = str.find(substr);
    return found != std::string::npos ? str.substr(0, found) : str;
}

void StringUtils::setString(u8* data, const std::string& v, int ofs, int len)
{
    len *= 2;
    u8 toinsert[len] = {0};
    if (v.empty()) return;
    
    char buf;
    int nicklen = v.length(), r = 0, w = 0, i = 0;
    while (r < nicklen || w > len)
    {
        buf = v[r++];
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
            buf = v[r++];
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

void StringUtils::setStringWithBytes(u8* data, const std::string& v, int ofs, int len, char* padding)
{
    setString(data, v + padding, ofs, len);
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

std::string& StringUtils::toUpper(std::string& in)
{
    std::transform(in.begin(), in.end(), in.begin(), ::toupper);
    std::u16string otherIn = StringUtils::UTF8toUTF16(in);
    for (size_t i = 0; i < otherIn.size(); i++)
    {
        switch (otherIn[i])
        {
            case u'í':
                otherIn[i] = u'Í';
                break;
            case u'ó':
                otherIn[i] = u'Ó';
                break;
            case u'ú':
                otherIn[i] = u'Ú';
                break;
            case u'é':
                otherIn[i] = u'É';
                break;
            case u'á':
                otherIn[i] = u'Á';
                break;
            case u'ì':
                otherIn[i] = u'Ì';
                break;
            case u'ò':
                otherIn[i] = u'Ò';
                break;
            case u'ù':
                otherIn[i] = u'Ù';
                break;
            case u'è':
                otherIn[i] = u'È';
                break;
            case u'à':
                otherIn[i] = u'À';
                break;
            case u'ñ':
                otherIn[i] = u'Ñ';
                break;
            case u'æ':
                otherIn[i] = u'Æ';
                break;
        }
    }
    in = StringUtils::UTF16toUTF8(otherIn);
    return in;
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

static std::map<u16, charWidthInfo_s*> widthCache;
static std::queue<u16> widthCacheOrder;

std::string StringUtils::splitWord(const std::string& text, float scaleX, float maxWidth)
{
    std::string word = text;
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
            float charWidth;
            auto width = widthCache.find(codepoint);
            if (width != widthCache.end())
            {
                charWidth = width->second->charWidth * scaleX;
            }
            else
            {
                std::string tmpString = word.substr(i, iMod + 1); // The character
                widthCache.insert_or_assign(codepoint, C2D_FontGetCharWidthInfo(fontForCodepoint(codepoint), C2D_FontGlyphIndexFromCodePoint(fontForCodepoint(codepoint), codepoint)));
                widthCacheOrder.push(codepoint);
                if (widthCache.size() > 1024)
                {
                    widthCache.erase(widthCacheOrder.front());
                    widthCacheOrder.pop();
                }
                charWidth = widthCache[codepoint]->charWidth * scaleX;
            }
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
    int iMod = 0;
    for (size_t i = 0; i < text.size(); i++)
    {
        if (text[i] == '\n')
        {
            largestRet = std::max(largestRet, ret);
            ret = 0.0f;
            continue;
        }
        u16 codepoint = 0xFFFD;
        if (text[i] & 0x80 && text[i] & 0x40 && text[i] & 0x20 && !(text[i] & 0x10) && i + 2 < text.size())
        {
            codepoint = text[i] & 0x0F;
            codepoint = codepoint << 6 | (text[i + 1] & 0x3F);
            codepoint = codepoint << 6 | (text[i + 2] & 0x3F);
            iMod = 2;
        }
        else if (text[i] & 0x80 && text[i] & 0x40 && !(text[i] & 0x20) && i + 1 < text.size())
        {
            codepoint = text[i] & 0x1F;
            codepoint = codepoint << 6 | (text[i + 1] & 0x3F);
            iMod = 1;
        }
        else if (!(text[i] & 0x80))
        {
            codepoint = text[i];
            iMod = 0;
        }
        float charWidth;
        auto width = widthCache.find(codepoint);
        if (width != widthCache.end())
        {
            charWidth = width->second->charWidth * scaleX;
        }
        else
        {
            widthCache.insert_or_assign(codepoint, C2D_FontGetCharWidthInfo(fontForCodepoint(codepoint), C2D_FontGlyphIndexFromCodePoint(fontForCodepoint(codepoint), codepoint)));
            widthCacheOrder.push(codepoint);
            if (widthCache.size() > 1024)
            {
                widthCache.erase(widthCacheOrder.front());
                widthCacheOrder.pop();
            }
            charWidth = widthCache[codepoint]->charWidth * scaleX;
        }
        ret += charWidth;
        i += iMod;
        iMod = 0;
    }
    return std::max(largestRet, ret);
}

float StringUtils::textWidth(const std::u16string& text, float scaleX)
{
    return textWidth(UTF16toUTF8(text), scaleX);
}

float StringUtils::textWidth(const C2D_Text& text, float scaleX)
{
    return ceilf(text.width*scaleX);
}

std::string StringUtils::wrap(const std::string& text, float scaleX, float maxWidth)
{
    if (textWidth(text, scaleX) <= maxWidth)
    {
        return text;
    }
    std::string dst, line, word;
    dst = line = word = "";

    for (std::string::const_iterator it = text.begin(); it != text.end(); it++)
    {
        word += *it;
        if (*it == ' ')
        {
            // split single words that are bigger than maxWidth
            if (StringUtils::textWidth(line + word, scaleX) <= maxWidth)
            {
                line += word;
            }
            else
            {
                if (StringUtils::textWidth(word, scaleX) > maxWidth)
                {
                    line += word;
                    line = StringUtils::splitWord(line, scaleX, maxWidth);
                    word = line.substr(line.find('\n')+1, std::string::npos);
                    line = line.substr(0, line.find('\n')); // Split line on first newLine; assign second part to word and first to line
                }
                if (line[line.size() - 1] == ' ')
                {
                    dst += line.substr(0, line.size() - 1) + '\n';
                }
                else
                {
                    dst += line + '\n';
                }
                line = word;
            }
            word = "";
        }
    }

    // "Another iteration" of the loop b/c it probably won't end with a space
    // If it does, no harm done
    // word = StringUtils::splitWord(word, scaleX, maxWidth);
    if (StringUtils::textWidth(line + word, scaleX) <= maxWidth)
    {
        dst += line + word;
    }
    else
    {
        if (StringUtils::textWidth(word, scaleX) > maxWidth)
        {
            line += word;
            line = StringUtils::splitWord(line, scaleX, maxWidth);
            word = line.substr(line.find('\n')+1, std::string::npos);
            line = line.substr(0, line.find('\n'));
        }
        if (line[line.size() - 1] == ' ')
        {
            dst += line.substr(0, line.size() - 1) + '\n' + word;
        }
        else
        {
            dst += line + '\n' + word;
        }
    }
    return dst;
}

std::string StringUtils::wrap(const std::string& text, float scaleX, float maxWidth, size_t lines)
{
    if (textWidth(text, scaleX) <= maxWidth)
    {
        return text;
    }

    // Get the wrapped string
    std::string wrapped = wrap(text, scaleX, maxWidth);
    if (lines == 0)
    {
        return wrapped;
    }

    // string.split('\n')
    std::vector<std::string> split;
    for (size_t i = 0; i < wrapped.size(); i++)
    {
        if (wrapped[i] == '\n')
        {
            split.push_back(wrapped.substr(0, i));
            wrapped = wrapped.substr(i+1, std::string::npos);
            i = 0;
        }
    }
    if (!wrapped.empty())
    {
        split.push_back(wrapped);
    }

    // If it's already the correct amount of lines, return it
    if (split.size() <= lines)
    {
        wrapped = split[0];
        for (size_t i = 1; i < split.size(); i++)
        {
            wrapped += '\n' + split[i];
        }
        return wrapped;
    }

    // Otherwise truncate it to the correct amount
    for (size_t i = split.size(); i > lines; i--)
    {
        split.pop_back();
    }

    const float ellipsis = C2D_FontGetCharWidthInfo(Gui::fonts[1], C2D_FontGlyphIndexFromCodePoint(Gui::fonts[1], '.'))->charWidth * 3 * scaleX;

    // If there's space for the ellipsis, add it
    if (textWidth(split[lines - 1], scaleX) + ellipsis <= maxWidth)
    {
        split[lines - 1] += "...";
    }
    // Otherwise do some sort of magic
    else
    {
        std::string& finalLine = split[lines - 1];
        // If there's a long enough word and a large enough space on the top line, move stuff up & add ellipsis to the end
        if (lines > 1 && textWidth(split[lines - 2], scaleX) <= maxWidth / 2 && textWidth(finalLine.substr(0, finalLine.find(' ')), scaleX) > maxWidth * 0.75f)
        {
            std::string sliced = wrap(finalLine, scaleX, maxWidth * 0.4f);
            split[lines - 2] += ' ' + sliced.substr(0, sliced.find('\n'));
            sliced = sliced.substr(sliced.find('\n')+1);
            for (size_t i = sliced.size(); i > 0; i--)
            {
                if (sliced[i - 1] == '\n')
                {
                    sliced.erase(i - 1, 1);
                }
            }
            finalLine = sliced + "...";
        }
        // Or get rid of enough characters for it to fit
        else
        {
            for (size_t i = finalLine.size(); i > 0; i--)
            {
                if ((finalLine[i-1] & 0x80 && finalLine[i-1] & 0x40) || !(finalLine[i-1] & 0x80)) // Beginning UTF-8 byte
                {
                    if (textWidth(finalLine.substr(0, i-1), scaleX) + ellipsis <= maxWidth)
                    {
                        finalLine = finalLine.substr(0, i-1) + "...";
                    }
                }
            }
        }
    }

    // Concatenate them and return
    wrapped = split[0];
    for (size_t i = 1; i < split.size(); i++)
    {
        wrapped += '\n' + split[i];
    }

    return wrapped;
}

bool StringUtils::fontHasChar(const C2D_Font& font, u16 codepoint)
{
    if (C2D_FontGlyphIndexFromCodePoint(font, codepoint) == C2D_FontGetInfo(font)->alterCharIndex)
    {
        return false;
    }
    return true;
}

std::vector<FontString> StringUtils::fontSplit(const std::string& str)
{
    std::vector<FontString> ret;
    std::string parseMe = "", currentChar = "";
    C2D_Font prevFont = nullptr;
    for (size_t i = 0; i < str.size() + 1; i++)
    {
        u16 codepoint = 0xFFFD;
        if (str[i] & 0x80 && str[i] & 0x40 && str[i] & 0x20 && !(str[i] & 0x10) && i + 2 < str.size())
        {
            codepoint = str[i] & 0x0F;
            currentChar += str[i];
            codepoint = codepoint << 6 | (str[i + 1] & 0x3F);
            currentChar += str[i + 1];
            codepoint = codepoint << 6 | (str[i + 2] & 0x3F);
            currentChar += str[i + 2];
            i += 2;
        }
        else if (str[i] & 0x80 && str[i] & 0x40 && !(str[i] & 0x20) && i + 1 < str.size())
        {
            codepoint = str[i] & 0x1F;
            currentChar += str[i];
            codepoint = codepoint << 6 | (str[i + 1] & 0x3F);
            currentChar += str[i + 1];
            i += 1;
        }
        else if (!(str[i] & 0x80))
        {
            codepoint = str[i];
            currentChar += str[i];
        }
        if (codepoint == 0xFFFD)
        {
            parseMe += "\uFFFD";
            currentChar = "";
            continue;
        }
        else if (codepoint == 0)
        {
            ret.push_back({prevFont, parseMe});
            return ret;
        }

        C2D_Font font = fontForCodepoint(codepoint);
        if (prevFont == font)
        {
            parseMe += currentChar;
        }
        else
        {
            ret.push_back({prevFont, parseMe});
            parseMe = currentChar;
            prevFont = font;
        }
        currentChar = "";
    }
    return ret;
}

C2D_Font StringUtils::fontForCodepoint(u16 codepoint)
{
    if (codepoint != u' ')
    {
        for (size_t font = 0; font < Gui::fonts.size(); font++)
        {
            if (StringUtils::fontHasChar(Gui::fonts[font], codepoint))
            {
                return Gui::fonts[font];
            }
        }
    }
    // Either space, in which case we want to use the system font, or not found, in which case we want to use the system font
    return Gui::fonts[1];
}

// TODO Implement this without using two intermediate strings
std::string StringUtils::transString45(const std::string& str)
{
    return StringUtils::UTF16toUTF8(transString45(StringUtils::UTF8toUTF16(str)));
}

std::u16string StringUtils::transString45(const std::u16string& str)
{
    std::u16string ret = str;
    for (auto& codepoint : ret)
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
    }
    return ret;
}

// TODO Make this not require two intermediate strings
std::string StringUtils::transString67(const std::string& str)
{
    return StringUtils::UTF16toUTF8(StringUtils::transString67(StringUtils::UTF8toUTF16(str)));
}

std::u16string StringUtils::transString67(const std::u16string& str)
{
    std::u16string ret = str;
    for (auto& codepoint : ret)
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
    }
    return ret;
}
