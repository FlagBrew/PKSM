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

// TODO: Make this work for both 3DS and Switch, as I really like the interface and it's just better
// overall than parsing strings multiple times
#ifndef TextBUF_HPP
#define TextBUF_HPP

#include "TextPos.hpp"
#include "colors.hpp"
#include "types.h"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#if defined(__3DS__)
#include <citro2d.h>
typedef float FontSize;
typedef C2D_Font FontType;
#define FONT_SIZE_18 0.72f
#define FONT_SIZE_15 0.6f
#define FONT_SIZE_14 0.56f
#define FONT_SIZE_12 0.50f
#define FONT_SIZE_11 0.46f
#define FONT_SIZE_9 0.37f
#elif defined(__SWITCH__)
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H
typedef int FontSize;
typedef FT_Face FontType;
#define FONT_SIZE_18 18
#define FONT_SIZE_15 15
#define FONT_SIZE_14 14
#define FONT_SIZE_12 12
#define FONT_SIZE_11 11
#define FONT_SIZE_9 9
#endif

namespace TextParse
{
    struct Glyph;
    struct Text;
    class TextBuf;
    class ScreenText;

#if defined(__3DS__)
    struct Glyph
    {
        Glyph(const Tex3DS_SubTexture& subtex, C3D_Tex* tex = nullptr, C2D_Font font = nullptr,
            u32 line = 0, float xPos = 0.0f, float width = 0.0f)
            : subtex(subtex), tex(tex), font(font), line(line), xPos(xPos), width(width)
        {
        }
        Tex3DS_SubTexture subtex;
        C3D_Tex* tex;
        C2D_Font font;
        u32 line;
        float xPos;
        float width;
    };
#elif defined(__SWITCH__)
    struct Glyph
    {
        Glyph(FT_Glyph ftGlyph, FTC_Node node, u32 line = 0, float xPos = 0.0f)
            : ftGlyph(ftGlyph), line(line), xPos(xPos)
        {
        }
        Glyph(const Glyph& other) = delete;
        Glyph(const Glyph&& other)
        {
            ftGlyph = other.ftGlyph;
            node    = other.node;
            line    = other.line;
            xPos    = other.xPos;
        }
        ~Glyph();
        FT_Glyph ftGlyph;
        FTC_Node node;
        u32 line;
        float xPos;
    };
#endif

    class Text
    {
        friend class TextBuf;
        friend class ScreenText;

    public:
        Text(const std::vector<Glyph>& glyphs = {}, const std::vector<float>& lineWidths = {},
            float maxLineWidth = 0.0f)
            : glyphs(glyphs), lineWidths(lineWidths), maxLineWidth(maxLineWidth)
        {
        }
        std::shared_ptr<Text> truncate(size_t lines, size_t offset = 0) const;
        std::shared_ptr<Text> slice(float maxWidth, float scrollOffset = 0.0f) const;
        // These should ONLY be used when drawing text directly instead of using ScreenText, which
        // shouldn't happen often!
        void optimize();
        void draw(float x, float y, float z, FontSize sizeX, FontSize sizeY, TextPosX textPos,
            PKSM_Color color = COLOR_BLACK) const;
        float maxWidth(float sizeX) const { return sizeX * maxLineWidth; }
        size_t lines() const { return lineWidths.size(); }

    private:
        std::vector<Glyph> glyphs;
        std::vector<float> lineWidths;
        float maxLineWidth;
        void addWord(
            std::pair<std::vector<Glyph>, std::vector<float>>&& word, float maxWidth = 0.0f);
    };

    class TextBuf
    {
    public:
        // maxChars is more of a suggestion than a limit. If it's necessary, things can extend
        // farther
        explicit TextBuf(size_t maxGlyphs, const std::vector<FontType>& fonts = {nullptr});
        std::shared_ptr<Text> parse(const std::string& str, float maxWidth = 0.0f);
        void addFont(FontType font);
        // Clears if currentGlyphs is >= maxChars
        void clear();
        // Clears unconditionally
        void clearUnconditional();

    private:
        bool fontHasChar(const FontType& font, u32 codepoint);
        FontType fontForCodepoint(u32 codepoint);
#if defined(__3DS__)
        std::unordered_map<C2D_Font, std::vector<C3D_Tex>> glyphSheets;
        void makeGlyphSheets(C2D_Font font);
#endif
        std::pair<std::vector<Glyph>, std::vector<float>> parseWord(
            std::string::const_iterator& str, float maxWidth);
        std::variant<float, size_t> parseWhitespace(std::string::const_iterator& str);
        std::vector<FontType> fonts;
        std::unordered_map<std::string, std::shared_ptr<Text>> parsedText;
        size_t maxGlyphs;
        size_t currentGlyphs;
    };

    class ScreenText
    {
    public:
        ScreenText() { glyphs.reserve(1024); }
        // y is always from baseline
        void addText(std::shared_ptr<Text> text, float x, float y, float z, FontSize sizeX,
            FontSize sizeY, TextPosX textPos, PKSM_Color color = COLOR_BLACK);
        void optimize();
        void draw() const;
        void clear();

    private:
        struct DrawableGlyph
        {
            DrawableGlyph(const Glyph& glyph, float x, float y, float z, FontSize sizeX,
                FontSize sizeY, PKSM_Color color = COLOR_BLACK)
                : x(x), y(y), z(z), sizeX(sizeX), sizeY(sizeY), color(color), glyph(glyph)
            {
            }
            float x, y, z;
            FontSize sizeX;
            FontSize sizeY;
            PKSM_Color color;
            Glyph glyph;
        };
        std::vector<DrawableGlyph> glyphs;
    };
}

#endif
