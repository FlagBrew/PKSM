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

#ifndef TextBUF_HPP
#define TextBUF_HPP

#include "TextPos.hpp"
#include "types.h"
#include <citro2d.h>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace TextParse
{
    struct Glyph;
    struct Text;
    class TextBuf;
    class ScreenText;

    struct Glyph
    {
        Glyph(const Tex3DS_SubTexture& subtex, C3D_Tex* tex = nullptr, C2D_Font font = nullptr, u32 line = 0, float xPos = 0.0f, float width = 0.0f)
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

    struct Text
    {
        Text(const std::vector<Glyph>& glyphs = {}, const std::vector<float>& lineWidths = {}, float maxLineWidth = 0.0f)
            : glyphs(glyphs), lineWidths(lineWidths), maxLineWidth(maxLineWidth)
        {
        }
        void addWord(std::pair<std::vector<Glyph>, float>&& word, float maxWidth = 0.0f);
        std::vector<Glyph> glyphs;
        std::vector<float> lineWidths;
        float maxLineWidth;
        float maxWidth(float sizeX) { return sizeX * maxLineWidth; }
    };

    class TextBuf
    {
    public:
        // maxChars is more of a suggestion than a limit. If it's necessary, things can extend farther
        TextBuf(size_t maxGlyphs, const std::vector<C2D_Font>& fonts = {nullptr});
        std::shared_ptr<Text> parse(const std::string& str, float maxWidth = 0.0f);
        void addFont(C2D_Font font);
        // Clears if currentGlyphs is >= maxChars
        void clear();
        // Clears unconditionally
        void clearUnconditional();

    private:
        bool fontHasChar(const C2D_Font& font, u32 codepoint);
        C2D_Font fontForCodepoint(u32 codepoint);
        void makeGlyphSheets(C2D_Font font);
        std::pair<std::vector<Glyph>, float> parseWord(std::string::const_iterator& str, float maxWidth);
        std::variant<float, size_t> parseWhitespace(std::string::const_iterator& str);
        std::vector<C2D_Font> fonts;
        std::unordered_map<std::string, std::shared_ptr<Text>> parsedText;
        size_t maxGlyphs;
        size_t currentGlyphs;
        std::unordered_map<C2D_Font, std::vector<C3D_Tex>> glyphSheets;
    };

    class ScreenText
    {
    public:
        ScreenText() { glyphs.reserve(1024); }
        // y is always from baseline
        void addText(std::shared_ptr<Text> text, float x, float y, float z, float scaleX, float scaleY, TextPosX textPos,
            u32 color = C2D_Color32(0, 0, 0, 255));
        void optimize();
        void draw() const;
        void clear();

    private:
        struct PositionedGlyph
        {
            PositionedGlyph(const Glyph& glyph, float x = 0.0f, float y = 0.0f, float z = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f,
                u32 color = C2D_Color32(0, 0, 0, 255))
                : x(x), y(y), z(z), scaleX(scaleX), scaleY(scaleY), color(color), glyph(glyph)
            {
            }
            float x, y, z;
            float scaleX, scaleY;
            u32 color;
            Glyph glyph;
        };
        std::vector<PositionedGlyph> glyphs;
    };
}

#endif
