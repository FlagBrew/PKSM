/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2022 Bernardo Giordano, Admiral Fish, piepie62
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

#include "TextParse.hpp"
#include <algorithm>
#include <iterator>
#include <type_traits>

namespace
{
    Tex3DS_SubTexture _select_box(const Tex3DS_SubTexture& texIn, int x, int y, int endX, int endY)
    {
        Tex3DS_SubTexture tex = texIn;
        if (x != endX)
        {
            int deltaX  = endX - x;
            float texRL = tex.left - tex.right;
            tex.left    = tex.left - (float)texRL / tex.width * x;
            tex.right   = tex.left - (float)texRL / tex.width * deltaX;
            tex.width   = deltaX;
        }
        if (y != endY)
        {
            float texTB = tex.top - tex.bottom;
            int deltaY  = endY - y;
            tex.top     = tex.top - (float)texTB / tex.height * y;
            tex.bottom  = tex.top - (float)texTB / tex.height * deltaY;
            tex.height  = deltaY;
        }
        return tex;
    }
}

namespace TextParse
{
    void Text::addWord(std::pair<std::vector<Glyph>, std::vector<float>>& word, float maxWidth)
    {
        if (word.second.size() > 1 ||
            (maxWidth > 0.0f && lineWidths.back() + word.second.front() > maxWidth))
        {
            for (auto& glyph : word.first)
            {
                glyph.line += lines() + 1;
            }
            for (size_t i = 0; i < word.second.size(); i++)
            {
                lineWidths.push_back(word.second[i]);
            }
        }
        else
        {
            for (auto& glyph : word.first)
            {
                glyph.xPos += lineWidths.back();
                glyph.line  = lines();
            }
            lineWidths[lines() - 1] += word.second.front();
        }
        glyphs.insert(glyphs.end(), word.first.begin(), word.first.end());
        word.first.clear();
        word.second.clear();
    }

    void Text::optimize()
    {
        std::sort(glyphs.begin(), glyphs.end(),
            [](const Glyph& g1, const Glyph& g2)
            {
                if (g1.font != g2.font)
                {
                    // These are arbitrarily ordered
                    return g1.font < g2.font;
                }
                else
                {
                    // Also arbitrary, but keeps those of the same type together
                    return g1.tex < g2.tex;
                }
            });
    }

    void Text::draw(float x, float y, float z, FontSize sizeX, FontSize sizeY, TextPosX textPos,
        PKSM_Color color) const
    {
        static_assert(std::is_same<FontSize, float>::value);
        static const u8 lineFeed = fontGetInfo(nullptr)->lineFeed;
        C2D_ImageTint tint;
        C2D_PlainImageTint(&tint, colorToFormat(color), 1.0f);
        for (const auto& glyph : glyphs)
        {
            float drawY = y + sizeY * (lineFeed * glyph.line -
                                          C2D_FontGetInfo(glyph.font)->tglp->baselinePos);
            // The one exception to using Gui::drawImageAt: we want to control depth here
            float drawX = x + (float)glyph.xPos * sizeX;
            switch (textPos)
            {
                case TextPosX::LEFT:
                    break;
                case TextPosX::CENTER:
                    drawX -= sizeX * lineWidths[glyph.line - 1] / 2;
                    break;
                case TextPosX::RIGHT:
                    drawX -= sizeX * lineWidths[glyph.line - 1];
                    break;
            }
            C2D_DrawImageAt({glyph.tex, &glyph.subtex}, drawX, drawY, z, &tint, sizeX, sizeY);
        }
    }

    std::shared_ptr<Text> Text::truncate(size_t lines, size_t offset) const
    {
        std::shared_ptr<Text> ret = std::make_shared<Text>();
        for (auto i = glyphs.begin(); i != glyphs.end(); i++)
        {
            if (i->line > offset && i->line - offset <= lines)
            {
                ret->glyphs.emplace_back(
                    i->subtex, i->tex, i->font, i->line - offset, i->xPos, i->width);
            }
        }
        for (size_t i = offset; i < lines; i++)
        {
            ret->lineWidths.emplace_back(lineWidths[i]);
        }
        ret->maxLineWidth = *std::max_element(ret->lineWidths.begin(), ret->lineWidths.end());

        return ret;
    }

    std::shared_ptr<Text> Text::slice(float maxWidth, float offset) const
    {
        std::shared_ptr<Text> ret = std::make_shared<Text>();
        for (const auto& width : lineWidths)
        {
            ret->lineWidths.push_back(width > maxWidth ? maxWidth : width);
        }
        ret->maxLineWidth = *std::max_element(ret->lineWidths.begin(), ret->lineWidths.end());

        for (auto i = glyphs.begin(); i != glyphs.end(); i++)
        {
            if (lineWidths[i->line - 1] > maxWidth)
            {
                // Completely out of bounds to the left
                if (i->xPos + i->width + offset <= 0)
                {
                    continue;
                }
                // Completely out of bounds to the right
                else if (i->xPos + offset >= maxWidth)
                {
                    continue;
                }
                // Partially out of bounds to the left, so xPos will be < 0 and xPos + width will be
                // > 0
                else if (i->xPos + offset < 0)
                {
                    float newXPos               = i->xPos + offset;
                    float targetWidth           = (float)i->width + newXPos;
                    Tex3DS_SubTexture newSubtex = _select_box(
                        i->subtex, i->width - static_cast<u16>(ceilf(targetWidth)), 0, i->width, 0);
                    ret->glyphs.emplace_back(
                        newSubtex, i->tex, i->font, i->line, 0, newSubtex.width);
                }
                // Partially out of bounds to the right, so xPos will be < maxWidth and xPos + width
                // will be > maxWidth
                else if (i->xPos + i->width + offset > maxWidth)
                {
                    float newXPos     = i->xPos + offset;
                    float targetWidth = (float)maxWidth - newXPos;
                    Tex3DS_SubTexture newSubtex =
                        _select_box(i->subtex, 0, 0, static_cast<u16>(ceilf(targetWidth)), 0);
                    ret->glyphs.emplace_back(
                        newSubtex, i->tex, i->font, i->line, newXPos, newSubtex.width);
                }
                // Fully in-bounds: just move it the amount of the offset
                else
                {
                    ret->glyphs.emplace_back(
                        i->subtex, i->tex, i->font, i->line, i->xPos + offset, i->width);
                }
            }
            else
            {
                ret->glyphs.emplace_back(*i);
            }
        }
        return ret;
    }

    TextBuf::TextBuf(size_t maxGlyphs, const std::vector<C2D_Font>& fonts)
        : fonts(fonts), maxGlyphs(maxGlyphs), currentGlyphs(0)
    {
        for (const auto& font : this->fonts)
        {
            TGLP_s* glyphInfo = C2D_FontGetInfo(font)->tglp;
            std::vector<C3D_Tex> fontSheets(glyphInfo->nSheets);
            for (size_t i = 0; i < fontSheets.size(); i++)
            {
                fontSheets[i].data   = glyphInfo->sheetData + glyphInfo->sheetSize * i;
                fontSheets[i].fmt    = (GPU_TEXCOLOR)glyphInfo->sheetFmt;
                fontSheets[i].size   = glyphInfo->sheetSize;
                fontSheets[i].width  = glyphInfo->sheetWidth;
                fontSheets[i].height = glyphInfo->sheetHeight;
                fontSheets[i].param  = GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) |
                                      GPU_TEXTURE_MIN_FILTER(GPU_LINEAR) |
                                      GPU_TEXTURE_WRAP_S(GPU_CLAMP_TO_BORDER) |
                                      GPU_TEXTURE_WRAP_T(GPU_CLAMP_TO_BORDER);
                fontSheets[i].border   = 0;
                fontSheets[i].lodParam = 0;
            }
            glyphSheets.emplace(font, std::move(fontSheets));
        }
    }

    void TextBuf::addFont(C2D_Font font)
    {
        fonts.emplace_back(font);
        TGLP_s* glyphInfo = C2D_FontGetInfo(font)->tglp;
        std::vector<C3D_Tex> fontSheets(glyphInfo->nSheets);
        for (size_t i = 0; i < fontSheets.size(); i++)
        {
            fontSheets[i].data   = glyphInfo->sheetData + glyphInfo->sheetSize * i;
            fontSheets[i].fmt    = (GPU_TEXCOLOR)glyphInfo->sheetFmt;
            fontSheets[i].size   = glyphInfo->sheetSize;
            fontSheets[i].width  = glyphInfo->sheetWidth;
            fontSheets[i].height = glyphInfo->sheetHeight;
            fontSheets[i].param =
                GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) | GPU_TEXTURE_MIN_FILTER(GPU_LINEAR) |
                GPU_TEXTURE_WRAP_S(GPU_CLAMP_TO_BORDER) | GPU_TEXTURE_WRAP_T(GPU_CLAMP_TO_BORDER);
            fontSheets[i].border   = 0;
            fontSheets[i].lodParam = 0;
        }
        glyphSheets.emplace(font, std::move(fontSheets));
    }

    void TextBuf::clear()
    {
        if (currentGlyphs >= maxGlyphs)
        {
            clearUnconditional();
        }
    }

    void TextBuf::clearUnconditional()
    {
        parsedText.clear();
    }

    bool TextBuf::fontHasChar(const C2D_Font& font, u32 codepoint)
    {
        if (C2D_FontGlyphIndexFromCodePoint(font, codepoint) ==
            C2D_FontGetInfo(font)->alterCharIndex)
        {
            return false;
        }
        return true;
    }

    C2D_Font TextBuf::fontForCodepoint(u32 codepoint)
    {
        if (codepoint != ' ')
        {
            for (size_t font = 0; font < fonts.size(); font++)
            {
                if (TextBuf::fontHasChar(fonts[font], codepoint))
                {
                    return fonts[font];
                }
            }
        }
        // Not found, so use the system font
        return nullptr;
    }

    void TextBuf::parseWord(std::pair<std::vector<Glyph>, std::vector<float>>& output,
        std::string::const_iterator& str, float maxWidth)
    {
        auto& [ret, lineWidths] = output;
        ret.clear();
        lineWidths   = {0.0f};
        ssize_t iMod = 0;
        for (; *str != '\0'; str += iMod)
        {
            u32 chr;
            iMod = decode_utf8(&chr, (u8*)str.base());
            if (iMod == -1)
            {
                chr  = 0xFFFD;
                iMod = 1;
            }
            if (chr == '\n')
            {
                break;
            }
            C2D_Font font = fontForCodepoint(chr);
            fontGlyphPos_s glyphPos;
            C2D_FontCalcGlyphPos(
                font, &glyphPos, C2D_FontGlyphIndexFromCodePoint(font, chr), 0, 1.0f, 1.0f);
            if (glyphPos.width > 0.0f)
            {
                if (maxWidth == 0.0f || lineWidths.back() + glyphPos.xAdvance <= maxWidth)
                {
                    ret.emplace_back(
                        Tex3DS_SubTexture{.width = static_cast<u16>(ceilf(glyphPos.width)),
                            .height              = (u16)C2D_FontGetInfo(font)->tglp->cellHeight,
                            .left                = glyphPos.texcoord.left,
                            .top                 = glyphPos.texcoord.top,
                            .right               = glyphPos.texcoord.right,
                            .bottom              = glyphPos.texcoord.bottom},
                        &glyphSheets[font][glyphPos.sheetIndex], font, lineWidths.size() - 1,
                        lineWidths.back() + glyphPos.xOffset, glyphPos.width);
                    lineWidths.back() += glyphPos.xAdvance;
                }
                else
                {
                    ret.emplace_back(
                        Tex3DS_SubTexture{.width = static_cast<u16>(ceilf(glyphPos.width)),
                            .height              = (u16)C2D_FontGetInfo(font)->tglp->cellHeight,
                            .left                = glyphPos.texcoord.left,
                            .top                 = glyphPos.texcoord.top,
                            .right               = glyphPos.texcoord.right,
                            .bottom              = glyphPos.texcoord.bottom},
                        &glyphSheets[font][glyphPos.sheetIndex], font, lineWidths.size(), 0,
                        glyphPos.width);
                    lineWidths.push_back(glyphPos.xAdvance);
                }
            }
            else
            {
                break;
            }
        }
    }

    std::pair<float, size_t> TextBuf::parseWhitespace(std::string::const_iterator& str)
    {
        float width  = 0.0f;
        size_t lines = 0;
        ssize_t iMod = 0;
        for (; *str != '\0'; str += iMod)
        {
            u32 chr;
            iMod = decode_utf8(&chr, (u8*)str.base());
            if (iMod == -1)
            {
                chr  = 0xFFFD;
                iMod = 1;
            }
            if (chr == '\n')
            {
                lines++;
                width = 0;
                continue;
            }
            C2D_Font font = fontForCodepoint(chr);
            fontGlyphPos_s glyphPos;
            C2D_FontCalcGlyphPos(
                font, &glyphPos, C2D_FontGlyphIndexFromCodePoint(font, chr), 0, 1.0f, 1.0f);
            if (glyphPos.width <= 0.0f)
            {
                width += glyphPos.xAdvance;
            }
            else
            {
                break;
            }
        }
        return {width, lines};
    }

    std::shared_ptr<Text> TextBuf::parse(const std::string& str, float maxWidth)
    {
        auto it = parsedText.find(str);
        if (it != parsedText.end())
        {
            return it->second;
        }
        else
        {
            std::shared_ptr<Text> tmp = std::make_shared<Text>();
            tmp->lineWidths.push_back(0.0f);
            auto strIt = str.begin();
            std::pair<std::vector<Glyph>, std::vector<float>> inProgressWord;
            do
            {
                parseWord(inProgressWord, strIt, maxWidth);
                tmp->addWord(inProgressWord, maxWidth);
                auto whitespace = parseWhitespace(strIt);
                for (size_t i = 0; i < whitespace.second; i++)
                {
                    tmp->lineWidths.push_back(0.0f);
                }
                tmp->lineWidths.back() += whitespace.first;
            }
            while (strIt != str.end());

            tmp->maxLineWidth = *std::max_element(tmp->lineWidths.begin(), tmp->lineWidths.end());

            auto ret = parsedText.emplace(str, std::move(tmp));
            return ret.first->second;
        }
    }

    void ScreenText::addText(const std::shared_ptr<Text>& text, float x, float y, float z,
        FontSize sizeX, FontSize sizeY, TextPosX textPos, PKSM_Color color)
    {
        static_assert(std::is_same<FontSize, float>::value);
        static const u8 lineFeed = fontGetInfo(nullptr)->lineFeed;
        if (!text || text->glyphs.empty())
        {
            return;
        }

        for (const auto& glyph : text->glyphs)
        {
            float glyphX = x + sizeX * glyph.xPos;
            switch (textPos)
            {
                case TextPosX::LEFT:
                    break;
                case TextPosX::CENTER:
                    glyphX -= sizeX * text->lineWidths[glyph.line - 1] / 2;
                    break;
                case TextPosX::RIGHT:
                    glyphX -= sizeX * text->lineWidths[glyph.line - 1];
                    break;
            }
            float glyphY = y + sizeY * (lineFeed * glyph.line -
                                           C2D_FontGetInfo(glyph.font)->tglp->baselinePos);
            glyphs.emplace_back(glyph, glyphX, glyphY, z, sizeX, sizeY, color);
        }
    }

    void ScreenText::optimize()
    {
        std::sort(glyphs.begin(), glyphs.end(),
            [](const DrawableGlyph& g1, const DrawableGlyph& g2)
            {
                if (g1.glyph.font != g2.glyph.font)
                {
                    // These are arbitrarily ordered
                    return g1.glyph.font < g2.glyph.font;
                }
                else
                {
                    // Also arbitrary, but keeps those of the same type together
                    return g1.glyph.tex < g2.glyph.tex;
                }
            });
    }

    void ScreenText::draw() const
    {
        static_assert(std::is_same<FontSize, float>::value);
        C2D_ImageTint tint;
        for (const auto& glyph : glyphs)
        {
            C2D_PlainImageTint(&tint, colorToFormat(glyph.color), 1.0f);
            // The one exception to using Gui::drawImageAt: we want to control depth here, and not
            // cause a pretty infinite loop of death
            C2D_DrawImageAt({glyph.glyph.tex, &glyph.glyph.subtex}, glyph.x, glyph.y, glyph.z,
                &tint, glyph.sizeX, glyph.sizeY);
        }
    }

    void ScreenText::clear()
    {
        glyphs.clear();
    }
}
