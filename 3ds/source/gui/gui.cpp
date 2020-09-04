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

#include "gui.hpp"
#include "Configuration.hpp"
#include "DecisionScreen.hpp"
#include "MessageScreen.hpp"
#include "TextParse.hpp"
#include "format.h"
#include "personal.hpp"
#include "pkx/PKX.hpp"
#include "sound.hpp"
#include "thread.hpp"
#include <stack>

namespace
{
    C3D_RenderTarget* g_renderTargetTop;
    C3D_RenderTarget* g_renderTargetBottom;

    C2D_SpriteSheet spritesheet_ui;
    C2D_SpriteSheet spritesheet_pkm;
    C2D_SpriteSheet spritesheet_types;
    C2D_Image bgBoxes;
    TextParse::TextBuf* textBuffer;
    TextParse::ScreenText topText;
    TextParse::ScreenText bottomText;
    TextParse::ScreenText* currentText = nullptr;

    std::vector<C2D_Font> fonts;

    std::stack<std::unique_ptr<Screen>> screens;

    constexpr u32 magicNumber = 0xC7D84AB9;
    float noHomeAlpha         = 0.0f;
#define NOHOMEALPHA_ACCEL 0.001f
    float dNoHomeAlpha = NOHOMEALPHA_ACCEL;

    bool textMode = false;
    bool inFrame  = false;

    struct ScrollingTextOffset
    {
        int offset;
        int pauseTime;
        bool thisFrame;
    };
    std::unordered_map<std::string, ScrollingTextOffset> scrollOffsets;

    Tex3DS_SubTexture _select_box(const C2D_Image& image, int x, int y, int endX, int endY)
    {
        Tex3DS_SubTexture tex = *image.subtex;
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

    void _draw_mirror_scale(int key, int x, int y, int off, int rep)
    {
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, key);
        // Left side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, off, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x, y);
        // Right side
        Gui::drawImageAt({sprite.tex, &tex}, x + off + rep, y, nullptr, -1.0f, 1.0f);
        // Center
        tex = _select_box(sprite, off, 0, sprite.subtex->width, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x + off, y, nullptr, rep, 1.0f);
    }

    void _draw_repeat(int key, int x, int y, u8 rows, u8 cols)
    {
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, key);
        for (u8 row = 0; row < rows; row++)
        {
            for (u8 col = 0; col < cols; col++)
            {
                Gui::drawImageAt(
                    sprite, x + col * sprite.subtex->width, y + row * sprite.subtex->height);
            }
        }
    }

    template <pksm::Species::EnumType species>
    constexpr int getSpeciesOffset()
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
        int imageOffsetFromBack = 0;
        switch (species)
        {
            // case NEXT_SPECIES_WITH_FORMS:
            // imageOffsetFromBack += 1;
            case pksm::Species::Magearna:
                imageOffsetFromBack += 3;
            case pksm::Species::Necrozma:
                imageOffsetFromBack += 1;
            case pksm::Species::Kommoo:
                imageOffsetFromBack += 1;
            case pksm::Species::Mimikyu:
                imageOffsetFromBack += 1;
            case pksm::Species::Togedemaru:
                imageOffsetFromBack += 7;
            case pksm::Species::Minior:
                imageOffsetFromBack += 1;
            case pksm::Species::Salazzle:
                imageOffsetFromBack += 1;
            case pksm::Species::Lurantis:
                imageOffsetFromBack += 1;
            case pksm::Species::Araquanid:
                imageOffsetFromBack += 1;
            case pksm::Species::Wishiwashi:
                imageOffsetFromBack += 2;
            case pksm::Species::Lycanroc:
                imageOffsetFromBack += 1;
            case pksm::Species::Rockruff:
                imageOffsetFromBack += 1;
            case pksm::Species::Ribombee:
                imageOffsetFromBack += 3;
            case pksm::Species::Oricorio:
                imageOffsetFromBack += 1;
            case pksm::Species::Vikavolt:
                imageOffsetFromBack += 1;
            case pksm::Species::Gumshoos:
                imageOffsetFromBack += 1;
            case pksm::Species::Hoopa:
                imageOffsetFromBack += 1;
            case pksm::Species::Diancie:
                imageOffsetFromBack += 4;
            case pksm::Species::Zygarde:
                imageOffsetFromBack += 1;
            case pksm::Species::Aegislash:
                imageOffsetFromBack += 1;
            case pksm::Species::Meowstic:
                imageOffsetFromBack += 9;
            case pksm::Species::Furfrou:
                imageOffsetFromBack += 4;
            case pksm::Species::Florges:
                imageOffsetFromBack += 5;
            case pksm::Species::Floette:
                imageOffsetFromBack += 4;
            case pksm::Species::Flabebe:
                imageOffsetFromBack += 19;
            case pksm::Species::Vivillon:
                imageOffsetFromBack += 2;
            case pksm::Species::Greninja:
                imageOffsetFromBack += 1;
            case pksm::Species::Meloetta:
                imageOffsetFromBack += 1;
            case pksm::Species::Keldeo:
                imageOffsetFromBack += 2;
            case pksm::Species::Kyurem:
                imageOffsetFromBack += 1;
            case pksm::Species::Landorus:
                imageOffsetFromBack += 1;
            case pksm::Species::Thundurus:
                imageOffsetFromBack += 1;
            case pksm::Species::Tornadus:
                imageOffsetFromBack += 3;
            case pksm::Species::Sawsbuck:
                imageOffsetFromBack += 3;
            case pksm::Species::Deerling:
                imageOffsetFromBack += 1;
            case pksm::Species::Darmanitan:
                imageOffsetFromBack += 1;
            case pksm::Species::Basculin:
                imageOffsetFromBack += 1;
            case pksm::Species::Audino:
                imageOffsetFromBack += 1;
            case pksm::Species::Shaymin:
                imageOffsetFromBack += 1;
            case pksm::Species::Giratina:
                imageOffsetFromBack += 5;
            case pksm::Species::Rotom:
                imageOffsetFromBack += 1;
            case pksm::Species::Gallade:
                imageOffsetFromBack += 1;
            case pksm::Species::Abomasnow:
                imageOffsetFromBack += 1;
            case pksm::Species::Lucario:
                imageOffsetFromBack += 1;
            case pksm::Species::Garchomp:
                imageOffsetFromBack += 1;
            case pksm::Species::Lopunny:
                imageOffsetFromBack += 1;
            case pksm::Species::Gastrodon:
                imageOffsetFromBack += 1;
            case pksm::Species::Shellos:
                imageOffsetFromBack += 1;
            case pksm::Species::Cherrim:
                imageOffsetFromBack += 2;
            case pksm::Species::Wormadam:
                imageOffsetFromBack += 2;
            case pksm::Species::Burmy:
                imageOffsetFromBack += 3;
            case pksm::Species::Deoxys:
                imageOffsetFromBack += 1;
            case pksm::Species::Rayquaza:
                imageOffsetFromBack += 1;
            case pksm::Species::Groudon:
                imageOffsetFromBack += 1;
            case pksm::Species::Kyogre:
                imageOffsetFromBack += 1;
            case pksm::Species::Latios:
                imageOffsetFromBack += 1;
            case pksm::Species::Latias:
                imageOffsetFromBack += 1;
            case pksm::Species::Metagross:
                imageOffsetFromBack += 1;
            case pksm::Species::Salamence:
                imageOffsetFromBack += 1;
            case pksm::Species::Glalie:
                imageOffsetFromBack += 1;
            case pksm::Species::Absol:
                imageOffsetFromBack += 1;
            case pksm::Species::Banette:
                imageOffsetFromBack += 3;
            case pksm::Species::Castform:
                imageOffsetFromBack += 1;
            case pksm::Species::Altaria:
                imageOffsetFromBack += 1;
            case pksm::Species::Camerupt:
                imageOffsetFromBack += 1;
            case pksm::Species::Sharpedo:
                imageOffsetFromBack += 1;
            case pksm::Species::Manectric:
                imageOffsetFromBack += 1;
            case pksm::Species::Medicham:
                imageOffsetFromBack += 1;
            case pksm::Species::Aggron:
                imageOffsetFromBack += 1;
            case pksm::Species::Mawile:
                imageOffsetFromBack += 1;
            case pksm::Species::Sableye:
                imageOffsetFromBack += 1;
            case pksm::Species::Gardevoir:
                imageOffsetFromBack += 1;
            case pksm::Species::Swampert:
                imageOffsetFromBack += 1;
            case pksm::Species::Blaziken:
                imageOffsetFromBack += 1;
            case pksm::Species::Sceptile:
                imageOffsetFromBack += 1;
            case pksm::Species::Tyranitar:
                imageOffsetFromBack += 1;
            case pksm::Species::Houndoom:
                imageOffsetFromBack += 1;
            case pksm::Species::Heracross:
                imageOffsetFromBack += 1;
            case pksm::Species::Scizor:
                imageOffsetFromBack += 1;
            case pksm::Species::Steelix:
                imageOffsetFromBack += 1;
            case pksm::Species::Ampharos:
                imageOffsetFromBack += 1;
            case pksm::Species::Pichu:
                imageOffsetFromBack += 2;
            case pksm::Species::Mewtwo:
                imageOffsetFromBack += 1;
            case pksm::Species::Aerodactyl:
                imageOffsetFromBack += 1;
            case pksm::Species::Gyarados:
                imageOffsetFromBack += 1;
            case pksm::Species::Pinsir:
                imageOffsetFromBack += 1;
            case pksm::Species::Kangaskhan:
                imageOffsetFromBack += 2;
            case pksm::Species::Marowak:
                imageOffsetFromBack += 1;
            case pksm::Species::Exeggutor:
                imageOffsetFromBack += 1;
            case pksm::Species::Gengar:
                imageOffsetFromBack += 1;
            case pksm::Species::Muk:
                imageOffsetFromBack += 1;
            case pksm::Species::Grimer:
                imageOffsetFromBack += 1;
            case pksm::Species::Slowbro:
                imageOffsetFromBack += 1;
            case pksm::Species::Golem:
                imageOffsetFromBack += 1;
            case pksm::Species::Graveler:
                imageOffsetFromBack += 1;
            case pksm::Species::Geodude:
                imageOffsetFromBack += 1;
            case pksm::Species::Alakazam:
                imageOffsetFromBack += 1;
            case pksm::Species::Persian:
                imageOffsetFromBack += 1;
            case pksm::Species::Meowth:
                imageOffsetFromBack += 1;
            case pksm::Species::Dugtrio:
                imageOffsetFromBack += 1;
            case pksm::Species::Diglett:
                imageOffsetFromBack += 1;
            case pksm::Species::Ninetales:
                imageOffsetFromBack += 1;
            case pksm::Species::Vulpix:
                imageOffsetFromBack += 1;
            case pksm::Species::Sandslash:
                imageOffsetFromBack += 1;
            case pksm::Species::Sandshrew:
                imageOffsetFromBack += 1;
            case pksm::Species::Raichu:
                imageOffsetFromBack += 13;
            case pksm::Species::Pikachu:
                imageOffsetFromBack += 2;
            case pksm::Species::Raticate:
                imageOffsetFromBack += 1;
            case pksm::Species::Rattata:
                imageOffsetFromBack += 1;
            case pksm::Species::Pidgeot:
                imageOffsetFromBack += 1;
            case pksm::Species::Beedrill:
                imageOffsetFromBack += 1;
            case pksm::Species::Blastoise:
                imageOffsetFromBack += 2;
            case pksm::Species::Charizard:
                imageOffsetFromBack += 1;
            case pksm::Species::Venusaur:
                imageOffsetFromBack += 0;
        }
#pragma GCC diagnostic pop

        return imageOffsetFromBack;
    }

    C2D_Image typeImage(pksm::Language lang, pksm::Type type)
    {
        if (type > pksm::Type::Fairy)
        {
            type = pksm::Type::Normal;
        }
        switch (lang)
        {
            case pksm::Language::SPA:
                return C2D_SpriteSheetGetImage(
                    spritesheet_types, types_spritesheet_es_00_idx + size_t(type));
            case pksm::Language::GER:
                return C2D_SpriteSheetGetImage(
                    spritesheet_types, types_spritesheet_de_00_idx + size_t(type));
            case pksm::Language::FRE:
                return C2D_SpriteSheetGetImage(
                    spritesheet_types, types_spritesheet_fr_00_idx + size_t(type));
            case pksm::Language::ITA:
                return C2D_SpriteSheetGetImage(
                    spritesheet_types, types_spritesheet_it_00_idx + size_t(type));
            case pksm::Language::JPN:
                return C2D_SpriteSheetGetImage(
                    spritesheet_types, types_spritesheet_jp_00_idx + size_t(type));
            case pksm::Language::KOR:
                return C2D_SpriteSheetGetImage(
                    spritesheet_types, types_spritesheet_ko_00_idx + size_t(type));
            case pksm::Language::CHT:
            case pksm::Language::CHS:
                return C2D_SpriteSheetGetImage(
                    spritesheet_types, types_spritesheet_zh_00_idx + size_t(type));
            case pksm::Language::ENG:
            case pksm::Language::PT:
            case pksm::Language::NL:
            default:
                return C2D_SpriteSheetGetImage(
                    spritesheet_types, types_spritesheet_en_00_idx + size_t(type));
        }
    }
}

void Gui::drawImageAt(
    const C2D_Image& img, float x, float y, const C2D_ImageTint* tint, float scaleX, float scaleY)
{
    flushText();
    C2D_DrawImageAt(img, x, y, 0.5f, tint, scaleX, scaleY);
}

void Gui::drawSolidCircle(float x, float y, float rad, PKSM_Color color)
{
    flushText();
    C2D_DrawCircleSolid(x, y, 0.5f, rad, colorToFormat(color));
}

void Gui::drawSolidRect(float x, float y, float w, float h, PKSM_Color color)
{
    flushText();
    C2D_DrawRectSolid(x, y, 0.5f, w, h, colorToFormat(color));
}

void Gui::drawSolidTriangle(
    float x1, float y1, float x2, float y2, float x3, float y3, PKSM_Color color)
{
    flushText();
    C2D_DrawTriangle(x1, y1, colorToFormat(color), x2, y2, colorToFormat(color), x3, y3,
        colorToFormat(color), 0.5f);
}

void Gui::drawLine(float x1, float y1, float x2, float y2, float width, PKSM_Color color)
{
    flushText();
    C2D_DrawLine(x1, y1, colorToFormat(color), x2, y2, colorToFormat(color), width, 0.5f);
    // float angle = atan2f(y2 - y1, x2 - x1) + C3D_Angle(.25);
    // float dy    = width / 2 * sinf(angle);
    // float dx    = width / 2 * cosf(angle);
    // drawSolidTriangle(x1 - dx, y1 - dy, x1 + dx, y1 + dy, x2 - dx, y2 - dy, color);
    // drawSolidTriangle(x2 - dx, y2 - dy, x2 + dx, y2 + dy, x1 + dx, y1 + dy, color);
}

void Gui::drawSolidPolygon(std::vector<std::pair<float, float>> points, PKSM_Color color)
{
    if (points.size() > 2)
    {
        for (size_t currentPoint = 2; currentPoint < points.size(); currentPoint++)
        {
            drawSolidTriangle(points[0].first, points[0].second, points[currentPoint - 1].first,
                points[currentPoint - 1].second, points[currentPoint].first,
                points[currentPoint].second, color);
        }
    }
}
void Gui::drawLinedPolygon(
    std::vector<std::pair<float, float>> points, float width, PKSM_Color color)
{
    if (points.size() > 2)
    {
        for (size_t startPoint = 0; startPoint < points.size(); startPoint++)
        {
            const auto& a = points[startPoint];
            const auto& b = points[(startPoint + 1) % points.size()];
            const auto& d = points[startPoint == 0 ? points.size() - 1 : startPoint - 1];
            const auto& c = points[(startPoint + 2) % points.size()];

            // Line segment angles
            float daAngle = atan2f(d.second - a.second, d.first - a.first);
            if (daAngle < 0)
            {
                daAngle = C3D_Angle(1) + daAngle;
            }
            float abAngle = atan2f(a.second - b.second, a.first - b.first);
            if (abAngle < 0)
            {
                abAngle = C3D_Angle(1) + abAngle;
            }
            float baAngle = atan2f(b.second - a.second, b.first - a.first);
            if (baAngle < 0)
            {
                baAngle = C3D_Angle(1) + baAngle;
            }
            float bcAngle = atan2f(c.second - b.second, c.first - b.first);
            if (bcAngle < 0)
            {
                bcAngle = C3D_Angle(1) + bcAngle;
            }

            // NOTE: have to handle differing signs properly

            // Angle of line segment DA with respect to AB
            float dabTheta = daAngle + baAngle;
            float dabPhi   = dabTheta / 2;
            float dabLength;
            // Should not happen; means that dab was 0 degrees
            if (sinf(dabPhi - baAngle) == 0)
            {
                dabLength = width;
            }
            else
            {
                dabLength = width / sinf(dabPhi - baAngle);
            }

            std::pair<float, float> trapPointD = {
                a.first + dabLength * cosf(dabPhi), a.second + dabLength * sinf(dabPhi)};
            drawSolidTriangle(
                a.first, a.second, b.first, b.second, trapPointD.first, trapPointD.second, color);

            // Angle of line segment AB with respect to BC
            float abcTheta = abAngle + bcAngle;
            float abcPhi   = abcTheta / 2;
            float abcLength;
            // Should not happen; means that abc was 0 degrees
            if (sinf(abcPhi - bcAngle) == 0)
            {
                abcLength = width;
            }
            else
            {
                abcLength = width / sinf(abcPhi - bcAngle);
            }

            std::pair<float, float> trapPointC = {
                b.first + abcLength * cosf(abcPhi), b.second + abcLength * sinf(abcPhi)};
            drawSolidTriangle(b.first, b.second, trapPointD.first, trapPointD.second,
                trapPointC.first, trapPointC.second, color);
        }
    }
}

void Gui::setDoHomeDraw()
{
    noHomeAlpha  = 1.0f;
    dNoHomeAlpha = NOHOMEALPHA_ACCEL;
}

void Gui::drawNoHome()
{
    static C2D_ImageTint tint;
    if (noHomeAlpha > 0.0f)
    {
        C2D_AlphaImageTint(&tint, noHomeAlpha);
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_home_blocked_idx), 130.0f,
            90.0f, &tint);
        noHomeAlpha -= dNoHomeAlpha;
        dNoHomeAlpha += NOHOMEALPHA_ACCEL;
    }
}

void Gui::target(gfxScreen_t screen)
{
    if (screen == GFX_BOTTOM)
    {
        currentText = &bottomText;
        C2D_SceneBegin(g_renderTargetBottom);
    }
    else
    {
        currentText = &topText;
        C2D_SceneBegin(g_renderTargetTop);
    }
    currentText->clear();
}

void Gui::clearScreen(gfxScreen_t screen)
{
    if (screen == GFX_BOTTOM)
    {
        C2D_TargetClear(g_renderTargetBottom, colorToFormat(COLOR_BLACK));
    }
    else
    {
        C2D_TargetClear(g_renderTargetTop, colorToFormat(COLOR_BLACK));
    }
}

void Gui::flushText()
{
    if (textMode)
    {
        currentText->optimize();
        currentText->draw();
        currentText->clear();
    }
    textMode = false;
}

void Gui::backgroundBottom(bool stripes)
{
    Gui::drawSolidRect(0, 0, 320, 240, PKSM_Color(40, 53, 147, 255));
    if (stripes)
    {
        for (int x = -240; x < 320; x += 7)
        {
            drawLine(x, 0, x + 240, 240, 2, COLOR_LINEBLUE);
        }
    }
    Gui::drawSolidRect(0, 220, 320, 20, PKSM_Color(26, 35, 126, 255));
}

void Gui::backgroundTop(bool stripes)
{
    Gui::drawSolidRect(0, 0, 400, 240, PKSM_Color(26, 35, 126, 255));
    if (stripes)
    {
        for (int x = -240; x < 400; x += 7)
        {
            drawLine(x, 240, x + 240, 0, 2, COLOR_LINEBLUE);
        }
    }
    Gui::drawSolidRect(0, 0, 400, 25, PKSM_Color(15, 22, 89, 255));
}

void Gui::backgroundAnimatedTop()
{
    static int x1                         = 0;
    static int x2                         = 400;
    static const Tex3DS_SubTexture boxes1 = _select_box(bgBoxes, 0, 0, 400, 240);
    static const Tex3DS_SubTexture boxes2 = _select_box(bgBoxes, 400, 0, 800, 240);

    if (x1 < -400)
    {
        x1 = 400;
    }
    if (x2 < -400)
    {
        x2 = 400;
    }

    Gui::drawImageAt({bgBoxes.tex, &boxes1}, x1--, 0);
    Gui::drawImageAt({bgBoxes.tex, &boxes2}, x2--, 0);
}

void Gui::backgroundAnimatedBottom()
{
    static int x1                         = 0;
    static int x2                         = 400;
    static const Tex3DS_SubTexture boxes1 = _select_box(bgBoxes, 0, 0, 400, 240);
    static const Tex3DS_SubTexture boxes2 = _select_box(bgBoxes, 400, 0, 800, 240);

    if (x1 < -400)
    {
        x1 = 400;
    }
    if (x2 < -400)
    {
        x2 = 400;
    }

    Gui::drawImageAt({bgBoxes.tex, &boxes1}, x1--, 0);
    Gui::drawImageAt({bgBoxes.tex, &boxes2}, x2--, 0);
}

void Gui::clearText(void)
{
    textBuffer->clearUnconditional();
}

std::shared_ptr<TextParse::Text> Gui::parseText(
    const std::string& str, FontSize size, float maxWidth)
{
    static_assert(std::is_same<FontSize, float>::value);
    maxWidth /= size;
    return textBuffer->parse(str, maxWidth);
}

void Gui::text(std::shared_ptr<TextParse::Text> text, float x, float y, FontSize sizeX,
    FontSize sizeY, PKSM_Color color, TextPosX positionX, TextPosY positionY)
{
    static_assert(std::is_same<FontSize, float>::value);
    textMode            = true;
    const float lineMod = sizeY * C2D_FontGetInfo(fonts[1])->lineFeed;
    y -= sizeY * 6;
    switch (positionY)
    {
        case TextPosY::TOP:
            break;
        case TextPosY::CENTER:
            y -= 0.5f * (lineMod * (float)text->lines());
            break;
        case TextPosY::BOTTOM:
            y -= lineMod * (float)text->lines();
            break;
    }

    currentText->addText(text, x, y, 0.5f, sizeX, sizeY, positionX, color);
}

void Gui::text(const std::string& str, float x, float y, FontSize size, PKSM_Color color,
    TextPosX positionX, TextPosY positionY, TextWidthAction action, float maxWidth)
{
    static_assert(std::is_same<FontSize, float>::value);
    if (maxWidth == 0)
    {
        action = TextWidthAction::IGNORE;
    }
    switch (action)
    {
        case TextWidthAction::IGNORE:
        {
            auto text = parseText(str, size, 0.0f);
            Gui::text(text, x, y, size, size, color, positionX, positionY);
        }
        break;
        case TextWidthAction::WRAP:
        {
            auto text = parseText(str, size, maxWidth);
            Gui::text(text, x, y, size, size, color, positionX, positionY);
        }
        break;
        case TextWidthAction::SQUISH:
        {
            auto text   = parseText(str, size, 0.0f);
            float sizeX = std::min(size, size * (maxWidth / text->maxWidth(size)));
            Gui::text(text, x, y, sizeX, size, color, positionX, positionY);
        }
        break;
        case TextWidthAction::SLICE:
        {
            auto text = parseText(str, size, 0.0f);
            if (text->maxWidth(size) < maxWidth)
            {
                Gui::text(text, x, y, size, size, color, positionX, positionY);
                return;
            }

            text = text->slice(maxWidth / size);
            Gui::text(text, x, y, size, size, color, positionX, positionY);
        }
        break;
        case TextWidthAction::SCROLL:
        {
            auto text = parseText(str, size, 0.0f);
            if (text->maxWidth(size) < maxWidth)
            {
                Gui::text(text, x, y, size, size, color, positionX, positionY);
                return;
            }

            auto offsetIt = scrollOffsets.find(str);
            if (offsetIt == scrollOffsets.end())
            {
                offsetIt = scrollOffsets.emplace(str, ScrollingTextOffset{0, 1, true}).first;
            }

            if (!offsetIt->second.thisFrame)
            {
                offsetIt->second.thisFrame = true;
                if (offsetIt->second.pauseTime != 0)
                {
                    if (offsetIt->second.pauseTime > 30)
                    {
                        if (offsetIt->second.offset == 0)
                        {
                            offsetIt->second.pauseTime = 0;
                        }
                        else
                        {
                            offsetIt->second.pauseTime = 1;
                        }
                        offsetIt->second.offset = 0;
                    }
                    else
                    {
                        offsetIt->second.pauseTime++;
                    }
                }
                else
                {
                    offsetIt->second.offset += 1;
                    if (offsetIt->second.offset / 3 + maxWidth > (int)text->maxWidth(size) + 5)
                    {
                        offsetIt->second.pauseTime += 1;
                    }
                }
            }

            text = text->slice(maxWidth / size, (float)-offsetIt->second.offset / 3.0f / size);
            Gui::text(text, x, y, size, size, color, positionX, positionY);
        }
        break;
        case TextWidthAction::SQUISH_OR_SLICE:
        case TextWidthAction::SQUISH_OR_SCROLL:
        {
            auto text   = parseText(str, size, 0.0f);
            float sizeX = std::min(size, size * (maxWidth / text->maxWidth(size)));
            if (sizeX >= size * 0.75f)
            {
                Gui::text(text, x, y, sizeX, size, color, positionX, positionY);
            }
            else
            {
                // Won't be terribly less performant because of string caching
                TextWidthAction nextAction = action == TextWidthAction::SQUISH_OR_SCROLL
                                                 ? TextWidthAction::SCROLL
                                                 : TextWidthAction::SLICE;
                Gui::text(str, x, y, size, color, positionX, positionY, nextAction, maxWidth);
            }
        }
        break;
    }
}

Result Gui::init(void)
{
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    Sound::init();

    g_renderTargetTop    = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    g_renderTargetBottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    spritesheet_ui    = C2D_SpriteSheetLoad("romfs:/gfx/ui_sheet.t3x");
    spritesheet_pkm   = C2D_SpriteSheetLoad("/3ds/PKSM/assets/pkm_spritesheet.t3x");
    spritesheet_types = C2D_SpriteSheetLoad("/3ds/PKSM/assets/types_spritesheet.t3x");

    fonts.emplace_back(C2D_FontLoad("romfs:/gfx/pksm.bcfnt"));
    fonts.emplace_back(C2D_FontLoadSystem(CFG_REGION_USA));
    fonts.emplace_back(C2D_FontLoadSystem(CFG_REGION_KOR));
    fonts.emplace_back(C2D_FontLoadSystem(CFG_REGION_CHN));
    fonts.emplace_back(C2D_FontLoadSystem(CFG_REGION_TWN));

    textBuffer = new TextParse::TextBuf(8192, fonts);

    bgBoxes = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_anim_squares_idx);

    hidSetRepeatParameters(10, 10);

    return 0;
}

void Gui::frameClean()
{
    for (auto& i : scrollOffsets)
    {
        if (i.second.thisFrame)
        {
            i.second.thisFrame = false;
        }
        else
        {
            i.second = {0, 1, false};
        }
    }
}

void Gui::mainLoop(void)
{
    bool exit = false;
    Sound::start();
    while (aptMainLoop() && !exit)
    {
        hidScanInput();
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        inFrame = true;
        Gui::clearScreen(GFX_TOP);
        Gui::clearScreen(GFX_BOTTOM);

        u32 kHeld = hidKeysHeld();

        if (kHeld & KEY_SELECT && !screens.top()->getInstructions().empty())
        {
            target(GFX_TOP);
            screens.top()->doTopDraw();
            screens.top()->getInstructions().drawTop();
            flushText();
            target(GFX_BOTTOM);
            screens.top()->doBottomDraw();
            screens.top()->getInstructions().drawBottom();
            flushText();

            if (!aptIsHomeAllowed() && aptCheckHomePressRejected())
            {
                setDoHomeDraw();
            }
            drawNoHome();

            C3D_FrameEnd(0);
            Gui::frameClean();
            inFrame = false;
        }
        else
        {
            target(GFX_TOP);
            screens.top()->doTopDraw();
            flushText();

            target(GFX_BOTTOM);
            screens.top()->doBottomDraw();
            flushText();

            if (!aptIsHomeAllowed() && aptCheckHomePressRejected())
            {
                setDoHomeDraw();
            }
            drawNoHome();

            C3D_FrameEnd(0);
            Gui::frameClean();
            inFrame = false;

            touchPosition touch;
            hidTouchRead(&touch);
            screens.top()->doUpdate(&touch);
            exit = screens.size() == 1 && (kHeld & KEY_START);
        }

        textBuffer->clear();
    }
}

void Gui::exit(void)
{
    if (spritesheet_ui)
    {
        C2D_SpriteSheetFree(spritesheet_ui);
    }
    if (spritesheet_pkm)
    {
        C2D_SpriteSheetFree(spritesheet_pkm);
    }
    if (spritesheet_types)
    {
        C2D_SpriteSheetFree(spritesheet_types);
    }
    if (textBuffer)
    {
        delete textBuffer;
    }
    for (const auto& font : fonts)
    {
        if (font)
            C2D_FontFree(font);
    }
    C2D_Fini();
    C3D_Fini();
    Sound::exit();
}

void Gui::sprite(int key, int x, int y)
{
    if (key == ui_sheet_res_null_idx)
    {
        return;
    }
    else if (key == ui_sheet_eventmenu_bar_selected_idx ||
             key == ui_sheet_eventmenu_bar_unselected_idx)
    {
        u8 off = 4, rep = 174;
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, key);
        // Left side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, off, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x, y);
        // Center
        tex = _select_box(sprite, off, 0, sprite.subtex->width, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x + off, y, nullptr, rep, 1.0f);
    }
    else if (key == ui_sheet_eventmenu_page_indicator_idx)
    {
        _draw_mirror_scale(key, x, y, 4, 182);
    }
    else if (key == ui_sheet_gameselector_savebox_idx)
    {
        u8 off           = 53;
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, key);
        // Top side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, 0, off);
        Gui::drawImageAt({sprite.tex, &tex}, x, y);
        // Bottom side
        Gui::drawImageAt({sprite.tex, &tex}, x, y + off - 1, nullptr, 1.0f, -1.0f);
    }
    else if (key == ui_sheet_mainmenu_button_idx)
    {
        _draw_mirror_scale(key, x, y, 4, 132);
    }
    else if (key == ui_sheet_part_editor_20x2_idx)
    {
        _draw_repeat(key, x, y, 20, 1);
    }
    else if (key == ui_sheet_part_info_bottom_idx || key == ui_sheet_part_info_top_idx)
    {
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, key);
        int width = sprite.subtex->width, height = sprite.subtex->height;
        // Top left
        Gui::drawImageAt(sprite, x, y);
        // Top right
        Gui::drawImageAt(sprite, x + width, y, NULL, -1.0f, 1.0f);
        // Bottom left
        Gui::drawImageAt(sprite, x, y + height, NULL, 1.0f, -1.0f);
        // Bottom right
        Gui::drawImageAt(sprite, x + width, y + height, NULL, -1.0f, -1.0f);
    }
    else if (key == ui_sheet_part_mtx_4x4_idx)
    {
        _draw_repeat(key, x, y, 4, 4);
    }
    else if (key == ui_sheet_part_mtx_5x6_idx)
    {
        _draw_repeat(key, x, y, 5, 6);
    }
    else if (key == ui_sheet_part_mtx_5x8_idx)
    {
        _draw_repeat(key, x, y, 5, 8);
    }
    else if (key == ui_sheet_part_mtx_15x16_idx)
    {
        _draw_repeat(key, x, y, 15, 16);
        Gui::drawSolidRect(0, 225, 400, 15, COLOR_WHITE);
    }

    // emulated
    else if (key == ui_sheet_emulated_pointer_horizontal_flipped_idx)
    {
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_pointer_horizontal_idx);
        Gui::drawImageAt(sprite, x, y, NULL, -1.0f, 1.0f);
    }
    else if (key == ui_sheet_emulated_bg_top_red)
    {
        C2D_ImageTint tint;
        C2D_SetImageTint(&tint, C2D_TopLeft, C2D_Color32(228, 143, 131, 255), 1);
        C2D_SetImageTint(&tint, C2D_TopRight, C2D_Color32(201, 95, 84, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotLeft, C2D_Color32(239, 163, 151, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotRight, C2D_Color32(214, 117, 106, 255), 1);
        Gui::drawImageAt(
            C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_bg_top_greyscale_idx), x, y, &tint);
    }
    else if (key == ui_sheet_emulated_bg_top_blue)
    {
        C2D_ImageTint tint;
        C2D_SetImageTint(&tint, C2D_TopLeft, C2D_Color32(139, 171, 221, 255), 1);
        C2D_SetImageTint(&tint, C2D_TopRight, C2D_Color32(93, 134, 193, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotLeft, C2D_Color32(158, 186, 233, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotRight, C2D_Color32(113, 150, 205, 255), 1);
        Gui::drawImageAt(
            C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_bg_top_greyscale_idx), x, y, &tint);
    }
    else if (key == ui_sheet_emulated_bg_top_green)
    {
        C2D_ImageTint tint;
        C2D_SetImageTint(&tint, C2D_TopLeft, C2D_Color32(142, 221, 138, 255), 1);
        C2D_SetImageTint(&tint, C2D_TopRight, C2D_Color32(101, 193, 93, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotLeft, C2D_Color32(161, 233, 158, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotRight, C2D_Color32(119, 205, 113, 255), 1);
        Gui::drawImageAt(
            C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_bg_top_greyscale_idx), x, y, &tint);
    }
    else if (key == ui_sheet_emulated_bg_bottom_red)
    {
        C2D_ImageTint tint;
        C2D_SetImageTint(&tint, C2D_TopLeft, C2D_Color32(216, 122, 111, 255), 1);
        C2D_SetImageTint(&tint, C2D_TopRight, C2D_Color32(239, 163, 151, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotLeft, C2D_Color32(201, 95, 84, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotRight, C2D_Color32(224, 134, 123, 255), 1);
        Gui::drawImageAt(
            C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_bg_bottom_greyscale_idx), x, y, &tint);
    }
    else if (key == ui_sheet_emulated_bg_bottom_blue)
    {
        C2D_ImageTint tint;
        C2D_SetImageTint(&tint, C2D_TopLeft, C2D_Color32(120, 154, 209, 255), 1);
        C2D_SetImageTint(&tint, C2D_TopRight, C2D_Color32(158, 186, 233, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotLeft, C2D_Color32(93, 134, 193, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotRight, C2D_Color32(131, 165, 217, 255), 1);
        Gui::drawImageAt(
            C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_bg_bottom_greyscale_idx), x, y, &tint);
    }
    else if (key == ui_sheet_emulated_bg_bottom_green)
    {
        C2D_ImageTint tint;
        C2D_SetImageTint(&tint, C2D_TopLeft, C2D_Color32(125, 209, 119, 255), 1);
        C2D_SetImageTint(&tint, C2D_TopRight, C2D_Color32(161, 233, 158, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotLeft, C2D_Color32(101, 193, 93, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotRight, C2D_Color32(136, 217, 131, 255), 1);
        Gui::drawImageAt(
            C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_bg_bottom_greyscale_idx), x, y, &tint);
    }
    else if (key == ui_sheet_emulated_eventmenu_bar_selected_flipped_horizontal_idx)
    {
        u8 off = 4, rep = 174;
        C2D_Image sprite =
            C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_eventmenu_bar_selected_idx);
        // Right side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, off, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x + rep, y, nullptr, -1.0f, 1.0f);
        // Center
        tex = _select_box(sprite, off, 0, sprite.subtex->width, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x, y, nullptr, -rep, 1.0f);
    }
    else if (key == ui_sheet_emulated_eventmenu_bar_selected_flipped_vertical_idx)
    {
        u8 off = 4, rep = 174;
        C2D_Image sprite =
            C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_eventmenu_bar_selected_idx);
        // Left side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, off, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x, y, nullptr, 1.0f, -1.0f);
        // Center
        tex = _select_box(sprite, off, 0, sprite.subtex->width, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x + off, y, nullptr, rep, -1.0f);
    }
    else if (key == ui_sheet_emulated_eventmenu_bar_selected_flipped_both_idx)
    {
        u8 off = 4, rep = 174;
        C2D_Image sprite =
            C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_eventmenu_bar_selected_idx);
        // Right side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, off, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x + rep, y, nullptr, -1.0f, -1.0f);
        // Center
        tex = _select_box(sprite, off, 0, sprite.subtex->width, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x, y, nullptr, -rep, -1.0f);
    }
    else if (key == ui_sheet_emulated_eventmenu_bar_unselected_flipped_horizontal_idx)
    {
        u8 off = 4, rep = 174;
        C2D_Image sprite =
            C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_eventmenu_bar_unselected_idx);
        // Right side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, off, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x + rep, y, nullptr, -1.0f, 1.0f);
        // Center
        tex = _select_box(sprite, off, 0, sprite.subtex->width, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x, y, nullptr, -rep, 1.0f);
    }
    else if (key == ui_sheet_emulated_eventmenu_bar_unselected_flipped_vertical_idx)
    {
        u8 off = 4, rep = 174;
        C2D_Image sprite =
            C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_eventmenu_bar_unselected_idx);
        // Left side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, off, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x, y, nullptr, 1.0f, -1.0f);
        // Center
        tex = _select_box(sprite, off, 0, sprite.subtex->width, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x + off, y, nullptr, rep, -1.0f);
    }
    else if (key == ui_sheet_emulated_eventmenu_bar_unselected_flipped_both_idx)
    {
        u8 off = 4, rep = 174;
        C2D_Image sprite =
            C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_eventmenu_bar_unselected_idx);
        // Right side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, off, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x + rep, y, nullptr, -1.0f, -1.0f);
        // Center
        tex = _select_box(sprite, off, 0, sprite.subtex->width, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x, y, nullptr, -rep, -1.0f);
    }
    else if (key == ui_sheet_emulated_storage_box_corner_flipped_horizontal_idx)
    {
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_storage_box_corner_idx),
            x, y, nullptr, -1.0f, 1.0f);
    }
    else if (key == ui_sheet_emulated_storage_box_corner_flipped_vertical_idx)
    {
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_storage_box_corner_idx),
            x, y, nullptr, 1.0f, -1.0f);
    }
    else if (key == ui_sheet_emulated_storage_box_corner_flipped_both_idx)
    {
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_storage_box_corner_idx),
            x, y, nullptr, -1.0f, -1.0f);
    }
    else if (key == ui_sheet_emulated_toggle_green_idx)
    {
        Gui::drawSolidRect(x, y, 13, 13, PKSM_Color(0x35, 0xC1, 0x3E, 0xFF));
    }
    else if (key == ui_sheet_emulated_toggle_red_idx)
    {
        Gui::drawSolidRect(x, y, 13, 13, PKSM_Color(0xCC, 0x3F, 0x26, 0xFF));
    }
    else if (key == ui_sheet_emulated_gameselector_bg_idx)
    {
        u8 off = 5, rep = 197;
        /* LEFT */
        C2D_Image sprite =
            C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_gameselector_bg_left_idx);
        // Top side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, 0, off);
        Gui::drawImageAt({sprite.tex, &tex}, x, y);
        // Bottom side
        Gui::drawImageAt({sprite.tex, &tex}, x, y + off + rep, nullptr, 1.0f, -1.0f);
        // Center
        tex = _select_box(sprite, 0, off, 0, sprite.subtex->height);
        Gui::drawImageAt({sprite.tex, &tex}, x, y + off, nullptr, 1.0f, rep);
        x += 5;
        Gui::drawSolidRect(x, y, 115, rep + 10, PKSM_Color(26, 35, 126, 255));

        /* RIGHT */
        x += 119;
        Gui::drawSolidRect(x, y, 263, rep + 10, PKSM_Color(26, 35, 126, 255));
        x += 263;
        sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_gameselector_bg_left_idx);
        // Top side
        tex = _select_box(sprite, 0, 0, 0, off);
        Gui::drawImageAt({sprite.tex, &tex}, x, y, nullptr, -1.0f, 1.0f);
        // Bottom side
        Gui::drawImageAt({sprite.tex, &tex}, x, y + off + rep, nullptr, -1.0f, -1.0f);
        // Center
        tex = _select_box(sprite, 0, off, 0, sprite.subtex->height);
        Gui::drawImageAt({sprite.tex, &tex}, x, y + off, nullptr, 1.0f, rep);
    }
    else if (key == ui_sheet_emulated_button_qr_idx)
    {
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_button_greyscale_idx);
        C2D_ImageTint tint;
        C2D_PlainImageTint(&tint, colorToFormat(COLOR_SELECTBLUE), 1.0f);

        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, 5, 0);
        // Left
        Gui::drawImageAt({sprite.tex, &tex}, x, y, &tint, 1.0f, 1.0f);
        // Right
        Gui::drawImageAt({sprite.tex, &tex}, x + 65, y, &tint, -1.0f, 1.0f);
        // Middle
        tex = _select_box(sprite, 5, 0, 6, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x + 5, y, &tint, 60.0f, 1.0f);
    }
    else if (key == ui_sheet_emulated_button_item_idx)
    {
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_button_editor_idx);

        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, 16, 0);
        // Left
        Gui::drawImageAt({sprite.tex, &tex}, x, y);
        // Right
        tex = _select_box(sprite, 92, 0, 108, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x + 182, y);
        // Center
        tex = _select_box(sprite, 16, 0, 17, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x + 16, y, nullptr, 166.0f, 1.0f);
    }
    else if (key == ui_sheet_emulated_button_plus_small_black_idx)
    {
        C2D_ImageTint tint;
        C2D_PlainImageTint(&tint, colorToFormat(COLOR_BLACK), 1.0f);
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_button_plus_small_idx), x,
            y, &tint, 1.0f, 1.0f);
    }
    else if (key == ui_sheet_emulated_button_minus_small_black_idx)
    {
        C2D_ImageTint tint;
        C2D_PlainImageTint(&tint, colorToFormat(COLOR_BLACK), 1.0f);
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_button_minus_small_idx),
            x, y, &tint, 1.0f, 1.0f);
    }
    else if (key == ui_sheet_emulated_box_search_idx)
    {
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_button_greyscale_idx);
        C2D_ImageTint tint;
        C2D_PlainImageTint(&tint, colorToFormat(COLOR_SELECTBLUE), 1.0f);

        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, 5, 0);
        // Left
        Gui::drawImageAt({sprite.tex, &tex}, x, y, &tint, 1.0f, 1.0f);
        // Right
        Gui::drawImageAt({sprite.tex, &tex}, x + 165, y, &tint, -1.0f, 1.0f);
        // Middle
        tex = _select_box(sprite, 5, 0, 6, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x + 5, y, &tint, 160.0f, 1.0f);

        Gui::drawSolidRect(x + 20, y + 17, 144, 1, COLOR_WHITE);
    }
    else if (key == ui_sheet_emulated_toggle_gray_idx)
    {
        Gui::drawSolidRect(x, y, 13, 13, PKSM_Color(0x80, 0x80, 0x80, 0xFF));
    }
    else if (key == ui_sheet_emulated_toggle_blue_idx)
    {
        Gui::drawSolidRect(x, y, 13, 13, PKSM_Color(0x00, 0x00, 0xFF, 0xFF));
    }
    else if (key == ui_sheet_emulated_party_indicator_1_idx)
    {
        text("\u2460", x, y - 3, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    }
    else if (key == ui_sheet_emulated_party_indicator_2_idx)
    {
        text("\u2461", x, y - 3, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    }
    else if (key == ui_sheet_emulated_party_indicator_3_idx)
    {
        text("\u2462", x, y - 3, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    }
    else if (key == ui_sheet_emulated_party_indicator_4_idx)
    {
        text("\u2463", x, y - 3, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    }
    else if (key == ui_sheet_emulated_party_indicator_5_idx)
    {
        text("\u2464", x, y - 3, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    }
    else if (key == ui_sheet_emulated_party_indicator_6_idx)
    {
        text("\u2465", x, y - 3, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    }
    else if (key == ui_sheet_emulated_button_selected_blue_idx)
    {
        C2D_ImageTint tint;
        C2D_PlainImageTint(&tint, colorToFormat(COLOR_SELECTBLUE), 1.0f);
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_button_greyscale_idx), x,
            y, &tint, 1.0f, 1.0f);
    }
    else if (key == ui_sheet_emulated_button_unselected_blue_idx)
    {
        C2D_ImageTint tint;
        C2D_PlainImageTint(&tint, colorToFormat(COLOR_UNSELECTBLUE), 1.0f);
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_button_greyscale_idx), x,
            y, &tint, 1.0f, 1.0f);
    }
    else if (key == ui_sheet_emulated_button_unavailable_blue_idx)
    {
        C2D_ImageTint tint;
        C2D_PlainImageTint(&tint, colorToFormat(COLOR_UNAVAILBLUE), 1.0f);
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_button_greyscale_idx), x,
            y, &tint, 1.0f, 1.0f);
    }
    else if (key == ui_sheet_emulated_button_selected_red_idx)
    {
        C2D_ImageTint tint;
        C2D_PlainImageTint(&tint, colorToFormat(COLOR_SELECTRED), 1.0f);
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_button_greyscale_idx), x,
            y, &tint, 1.0f, 1.0f);
    }
    else if (key == ui_sheet_emulated_button_unselected_red_idx)
    {
        C2D_ImageTint tint;
        C2D_PlainImageTint(&tint, colorToFormat(COLOR_UNSELECTRED), 1.0f);
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_button_greyscale_idx), x,
            y, &tint, 1.0f, 1.0f);
    }
    else if (key == ui_sheet_emulated_button_unavailable_red_idx)
    {
        C2D_ImageTint tint;
        C2D_PlainImageTint(&tint, colorToFormat(COLOR_UNAVAILRED), 1.0f);
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_button_greyscale_idx), x,
            y, &tint, 1.0f, 1.0f);
    }
    else if (key == ui_sheet_emulated_button_pouch_idx)
    {
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_button_editor_idx);

        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, 16, 0);
        // Left
        Gui::drawImageAt({sprite.tex, &tex}, x, y);
        // Right
        tex = _select_box(sprite, 92, 0, 108, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x + 84, y);
        // Center
        tex = _select_box(sprite, 16, 0, 17, 0);
        Gui::drawImageAt({sprite.tex, &tex}, x + 16, y, nullptr, 68.0f, 1.0f);
    }
    else if (key == ui_sheet_emulated_textbox_illegal_idx)
    {
        C2D_ImageTint tint;
        C2D_PlainImageTint(&tint, C2D_Color32(0xFF, 0, 0, 0xFF), 0.1f);
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_textbox_pksm_idx), x, y,
            &tint, 1.0f, 1.0f);
    }
    else if (key == ui_sheet_emulated_bg_top_yellow_idx)
    {
        C2D_ImageTint tint;
        C2D_SetImageTint(&tint, C2D_TopLeft, C2D_Color32(242, 221, 131, 255), 1);
        C2D_SetImageTint(&tint, C2D_TopRight, C2D_Color32(239, 202, 43, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotLeft, C2D_Color32(246, 230, 158, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotRight, C2D_Color32(244, 212, 81, 255), 1);
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_bg_top_greyscale_idx), x,
            y, 0.5f, &tint);
    }
    else if (key == ui_sheet_emulated_bg_bottom_yellow_idx)
    {
        C2D_ImageTint tint;
        C2D_SetImageTint(&tint, C2D_TopLeft, C2D_Color32(244, 220, 118, 255), 1);
        C2D_SetImageTint(&tint, C2D_TopRight, C2D_Color32(246, 230, 158, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotLeft, C2D_Color32(242, 211, 78, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotRight, C2D_Color32(242, 221, 131, 255), 1);
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_bg_bottom_greyscale_idx),
            x, y, 0.5f, &tint);
    }
    else if (key == ui_sheet_emulated_button_lang_disabled_idx)
    {
        drawSolidRect(x, y, 8, 8, COLOR_MENUBLUE);
    }
    else if (key == ui_sheet_emulated_button_lang_enabled_idx)
    {
        drawSolidRect(x - 1, y - 1, 10, 10, COLOR_HIGHBLUE);
    }
    else if (key == ui_sheet_emulated_stripe_move_grey_idx)
    {
        C2D_ImageTint tint;
        C2D_PlainImageTint(&tint, colorToFormat(COLOR_DARKGREY), 1.0f);
        C2D_DrawImageAt(
            C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_stripe_move_editor_row_idx), x, y,
            0.5f, &tint);
    }
    else if (key == ui_sheet_emulated_button_filter_positive_idx)
    {
        C2D_ImageTint tint;
        C2D_PlainImageTint(&tint, C2D_Color32(0x10, 0x87, 0x1e, 255), 1.0f);
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_button_plus_small_idx), x,
            y, 0.5f, &tint);
    }
    else if (key == ui_sheet_emulated_button_filter_negative_idx)
    {
        C2D_ImageTint tint;
        C2D_PlainImageTint(&tint, C2D_Color32(0xbd, 0x30, 0x26, 255), 1.0f);
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_button_minus_small_idx), x,
            y, 0.5f, &tint);
    }
    else if (key == ui_sheet_emulated_button_tabs_3_unselected_idx)
    {
        drawSolidRect(x, y, 104, 17, COLOR_DARKBLUE);
    }
    else if (key == ui_sheet_emulated_checkbox_disabled_idx)
    {
        C2D_ImageTint tint;
        C2D_PlainImageTint(&tint, colorToFormat(COLOR_DARKGREY), 1.0f);
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_checkbox_blank_idx), x, y,
            0.5f, &tint);
    }
    else if (key == ui_sheet_emulated_button_tabs_2_unselected_idx)
    {
        drawSolidRect(x, y, 158, 17, COLOR_DARKBLUE);
    }
    else if (key == ui_sheet_emulated_gameselector_bg_solid_idx)
    {
        u8 off = 5, rep = 197;
        /* LEFT */
        C2D_Image sprite =
            C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_gameselector_bg_left_idx);
        // Top side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, 0, off);
        Gui::drawImageAt({sprite.tex, &tex}, x, y);
        // Bottom side
        Gui::drawImageAt({sprite.tex, &tex}, x, y + off + rep, nullptr, 1.0f, -1.0f);
        // Center
        tex = _select_box(sprite, 0, off, 0, sprite.subtex->height);
        Gui::drawImageAt({sprite.tex, &tex}, x, y + off, nullptr, 1.0f, rep);
        x += 5;
        Gui::drawSolidRect(x, y, 382, rep + 10, PKSM_Color(26, 35, 126, 255));
        /* RIGHT */
        x += 382;
        sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_gameselector_bg_left_idx);
        // Top side
        tex = _select_box(sprite, 0, 0, 0, off);
        Gui::drawImageAt({sprite.tex, &tex}, x, y, nullptr, -1.0f, 1.0f);
        // Bottom side
        Gui::drawImageAt({sprite.tex, &tex}, x, y + off + rep, nullptr, -1.0f, -1.0f);
        // Center
        tex = _select_box(sprite, 0, off, 0, sprite.subtex->height);
        Gui::drawImageAt({sprite.tex, &tex}, x, y + off, nullptr, 1.0f, rep);
    }
    // standard case
    else
    {
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, key), x, y);
    }
}

void Gui::format(const pksm::PKX& pkm, int x, int y)
{
    switch (pkm.generation())
    {
        // If this ever happens
        // case Generation::ONE:
        // case Generation::TWO: // if this ever happens
        //     Gui::sprite(ui_sheet_icon_generation_gb_idx, x, y);
        //     break;
        case pksm::Generation::THREE:
            Gui::sprite(ui_sheet_icon_generation_3_idx, x, y);
            break;
        case pksm::Generation::FOUR:
            Gui::sprite(ui_sheet_icon_generation_4_idx, x, y);
            break;
        case pksm::Generation::FIVE:
            Gui::sprite(ui_sheet_icon_generation_5_idx, x, y);
            break;
        case pksm::Generation::SIX:
            Gui::sprite(ui_sheet_icon_generation_6_idx, x, y);
            break;
        case pksm::Generation::SEVEN:
            Gui::sprite(ui_sheet_icon_generation_7_idx, x, y);
            break;
        case pksm::Generation::LGPE:
            Gui::sprite(ui_sheet_icon_generation_go_idx, x, y);
            break;
        case pksm::Generation::UNUSED:
            break;
    }
}

void Gui::generation(const pksm::PKX& pkm, int x, int y)
{
    switch (pkm.version())
    {
        case pksm::GameVersion::S:
        case pksm::GameVersion::R:
        case pksm::GameVersion::E:
        case pksm::GameVersion::FR:
        case pksm::GameVersion::LG:
        case pksm::GameVersion::CXD:
            Gui::sprite(ui_sheet_icon_generation_3_idx, x, y);
            break;
        case pksm::GameVersion::D:
        case pksm::GameVersion::P:
        case pksm::GameVersion::Pt:
        case pksm::GameVersion::HG:
        case pksm::GameVersion::SS:
            Gui::sprite(ui_sheet_icon_generation_4_idx, x, y);
            break;
        case pksm::GameVersion::W:
        case pksm::GameVersion::B:
        case pksm::GameVersion::W2:
        case pksm::GameVersion::B2:
            Gui::sprite(ui_sheet_icon_generation_5_idx, x, y);
            break;
        case pksm::GameVersion::X:
        case pksm::GameVersion::Y:
        case pksm::GameVersion::AS:
        case pksm::GameVersion::OR:
            Gui::sprite(ui_sheet_icon_generation_6_idx, x, y);
            break;
        case pksm::GameVersion::SN:
        case pksm::GameVersion::MN:
        case pksm::GameVersion::US:
        case pksm::GameVersion::UM:
            Gui::sprite(ui_sheet_icon_generation_7_idx, x, y);
            break;
        case pksm::GameVersion::GO:
            Gui::sprite(ui_sheet_icon_generation_go_idx, x, y);
            break;
        case pksm::GameVersion::RD:
        case pksm::GameVersion::GN:
        case pksm::GameVersion::BU:
        case pksm::GameVersion::YW:
        case pksm::GameVersion::GD:
        case pksm::GameVersion::SV:
        case pksm::GameVersion::C:
            Gui::sprite(ui_sheet_icon_generation_gb_idx, x, y);
            break;
        case pksm::GameVersion::GP:
        case pksm::GameVersion::GE:
        case pksm::GameVersion::SW:
        case pksm::GameVersion::SH:
        default:
            break;
    }
}

void Gui::sprite(int key, int x, int y, PKSM_Color color)
{
    if (key == ui_sheet_button_editor_idx || key == ui_sheet_icon_item_idx ||
        key == ui_sheet_pointer_arrow_idx)
    {
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, key);
        C2D_ImageTint tint;
        C2D_PlainImageTint(&tint, colorToFormat(color), 1.0f);
        Gui::drawImageAt(sprite, x, y, &tint);
    }
}

void Gui::pkm(const pksm::PKX& pokemon, int x, int y, float scale, PKSM_Color color, float blend)
{
    C2D_ImageTint tint;
    C2D_PlainImageTint(&tint, colorToFormat(color), blend);

    if (pokemon.egg())
    {
        if (pokemon.species() != pksm::Species::Manaphy)
        {
            pkm(pokemon.species(), pokemon.alternativeForm(), pokemon.generation(),
                pokemon.gender(), x, y, scale, color, blend);
            Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, pkm_spritesheet_0_idx),
                x - 13 + ceilf(3 * scale), y + 4 + 30 * (scale - 1), &tint);
        }
        else
        {
            Gui::drawImageAt(
                C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_490_e_idx), x, y,
                &tint, scale, scale);
        }
    }
    else
    {
        pkm(pokemon.species(), pokemon.alternativeForm(), pokemon.generation(), pokemon.gender(), x,
            y, scale, color, blend);
        if (pokemon.heldItem() > 0)
        {
            Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_icon_item_idx),
                x + ceilf(3 * scale), y + 21 + ceilf(30 * (scale - 1)), &tint);
        }
    }

    if (pokemon.shiny())
    {
        Gui::drawImageAt(
            C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_icon_shiny_idx), x, y, &tint);
    }
}

void Gui::pkm(pksm::Species species, int form, pksm::Generation generation, pksm::Gender gender,
    int x, int y, float scale, PKSM_Color color, float blend)
{
    static C2D_ImageTint tint;
    C2D_PlainImageTint(&tint, colorToFormat(color), blend);
    Date date = Date::today();
    if (date.day() == ((u16)(~magicNumber >> 16) ^ 0x3826) &&
        date.month() == ((u16)(~magicNumber) ^ 0xB542))
    {
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, (u8)(~magicNumber >> 13) ^ 184),
            x, y, &tint, scale, scale);
        return;
    }
    if (species == pksm::Species::Manaphy && form == -1)
    {
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_490_e_idx), x,
            y, &tint, scale, scale);
    }
    else if (species == pksm::Species::Unown)
    {
        if (form == 0 || form > 27)
        {
            Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, size_t(species)), x, y, &tint,
                scale, scale);
        }
        else
        {
            Gui::drawImageAt(
                C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_801_1_idx + form), x,
                y, &tint, scale, scale);
        }
    }
    // For possible hex editor mishaps
    else if (species > pksm::Species::Melmetal)
    {
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, pkm_spritesheet_0_idx), x, y,
            &tint, scale, scale);
    }
    else if (species == pksm::Species::Unfezant && gender == pksm::Gender::Female)
    {
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_521_1_idx), x,
            y, &tint, scale, scale);
    }
    else if (species == pksm::Species::Frillish && gender == pksm::Gender::Female)
    {
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_592_1_idx), x,
            y, &tint, scale, scale);
    }
    else if (species == pksm::Species::Jellicent && gender == pksm::Gender::Female)
    {
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_593_1_idx), x,
            y, &tint, scale, scale);
    }
    else if (species == pksm::Species::Pyroar && gender == pksm::Gender::Female)
    {
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_668_1_idx), x,
            y, &tint, scale, scale);
    }
    else if (form == 0)
    {
        // This max will change over time!
        if (species > pksm::Species::Melmetal)
        {
            species = pksm::Species::Melmetal;
        }
        if (species < pksm::Species::None)
        {
            species = pksm::Species::None;
        }
        Gui::drawImageAt(
            C2D_SpriteSheetGetImage(spritesheet_pkm, size_t(species)), x, y, &tint, scale, scale);
    }
    else if (species == pksm::Species::Mimikyu)
    {
        if (form == 1 || form > pksm::PersonalSMUSUM::formCount(778))
        {
            Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, pkm_spritesheet_778_idx), x,
                y, &tint, scale, scale);
        }
        else
        {
            Gui::drawImageAt(
                C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_778_2_idx), x, y,
                &tint, scale, scale);
        }
    }
    else if (species == pksm::Species::Minior)
    {
        if (form < 7 || form > pksm::PersonalSMUSUM::formCount(774))
        {
            Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, pkm_spritesheet_774_idx), x,
                y, &tint, scale, scale);
        }
        else
        {
            Gui::drawImageAt(
                C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_774_7_idx + form - 7),
                x, y, &tint, scale, scale);
        }
    }
    else if (species == pksm::Species::Pumpkaboo || species == pksm::Species::Gourgeist ||
             species == pksm::Species::Genesect)
    {
        Gui::drawImageAt(
            C2D_SpriteSheetGetImage(spritesheet_pkm, size_t(species)), x, y, &tint, scale, scale);
    }
    // Pikachu
    else if (species == pksm::Species::Pikachu)
    {
        if (generation == pksm::Generation::SIX &&
            form < pksm::PersonalXYORAS::formCount(size_t(species)))
        {
            Gui::drawImageAt(
                C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_20_2_idx + form), x, y,
                &tint, scale, scale);
        }
        else if (form < pksm::PersonalSMUSUM::formCount(size_t(species)))
        {
            Gui::drawImageAt(
                C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_25_6_idx + form), x, y,
                &tint, scale, scale);
        }
        else if (form == pksm::PersonalLGPE::formCount(size_t(species)) - 1)
        {
            Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, size_t(species)), x, y, &tint,
                scale, scale);
            Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_icon_shiny_idx),
                x + 25 + 34 * (scale - 1), y + 5);
        }
        else
        {
            Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, size_t(species)), x, y, &tint,
                scale, scale);
        }
    }
    else if (species == pksm::Species::Eevee)
    {
        Gui::drawImageAt(
            C2D_SpriteSheetGetImage(spritesheet_pkm, size_t(species)), x, y, &tint, scale, scale);
        Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_icon_shiny_idx),
            x + 25 + 34 * (scale - 1), y + 5);
    }
    else if (species == pksm::Species::Arceus)
    {
        Gui::drawImageAt(
            C2D_SpriteSheetGetImage(spritesheet_pkm, size_t(species)), x, y, &tint, scale, scale);
    }
    else if (species == pksm::Species::Scatterbug || species == pksm::Species::Spewpa)
    {
        Gui::drawImageAt(
            C2D_SpriteSheetGetImage(spritesheet_pkm, size_t(species)), x, y, &tint, scale, scale);
    }
    else if (species == pksm::Species::Silvally)
    {
        Gui::drawImageAt(
            C2D_SpriteSheetGetImage(spritesheet_pkm, size_t(species)), x, y, &tint, scale, scale);
    }
    else
    {
        decltype(pksm::PersonalSWSH::formCount)* formCountGetter;
        switch (generation)
        {
            case pksm::Generation::FOUR:
                formCountGetter = pksm::PersonalDPPtHGSS::formCount;
                break;
            case pksm::Generation::FIVE:
                formCountGetter = pksm::PersonalBWB2W2::formCount;
                break;
            case pksm::Generation::SIX:
                formCountGetter = pksm::PersonalXYORAS::formCount;
                break;
            case pksm::Generation::EIGHT:
                formCountGetter = pksm::PersonalSWSH::formCount;
                break;
            case pksm::Generation::LGPE:
                formCountGetter = pksm::PersonalLGPE::formCount;
                break;
            case pksm::Generation::SEVEN:
            default:
                formCountGetter = pksm::PersonalSMUSUM::formCount;
                break;
        }
        if (form > formCountGetter(size_t(species)))
        {
            Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, size_t(species)), x, y, &tint,
                scale, scale);
            return;
        }
        int imageOffsetFromBack = 0;

        // In switch for constexpr evaluation
        switch (species)
        {
            case pksm::Species::Magearna:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Magearna>();
                break;
            case pksm::Species::Necrozma:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Necrozma>();
                break;
            case pksm::Species::Kommoo:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Kommoo>();
                break;
            case pksm::Species::Mimikyu:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Mimikyu>();
                break;
            case pksm::Species::Togedemaru:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Togedemaru>();
                break;
            case pksm::Species::Minior:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Minior>();
                break;
            case pksm::Species::Salazzle:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Salazzle>();
                break;
            case pksm::Species::Lurantis:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Lurantis>();
                break;
            case pksm::Species::Araquanid:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Araquanid>();
                break;
            case pksm::Species::Wishiwashi:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Wishiwashi>();
                break;
            case pksm::Species::Lycanroc:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Lycanroc>();
                break;
            case pksm::Species::Rockruff:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Rockruff>();
                break;
            case pksm::Species::Ribombee:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Ribombee>();
                break;
            case pksm::Species::Oricorio:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Oricorio>();
                break;
            case pksm::Species::Vikavolt:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Vikavolt>();
                break;
            case pksm::Species::Gumshoos:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Gumshoos>();
                break;
            case pksm::Species::Hoopa:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Hoopa>();
                break;
            case pksm::Species::Diancie:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Diancie>();
                break;
            case pksm::Species::Zygarde:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Zygarde>();
                break;
            case pksm::Species::Aegislash:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Aegislash>();
                break;
            case pksm::Species::Meowstic:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Meowstic>();
                break;
            case pksm::Species::Furfrou:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Furfrou>();
                break;
            case pksm::Species::Florges:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Florges>();
                break;
            case pksm::Species::Floette:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Floette>();
                break;
            case pksm::Species::Flabebe:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Flabebe>();
                break;
            case pksm::Species::Vivillon:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Vivillon>();
                break;
            case pksm::Species::Greninja:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Greninja>();
                break;
            case pksm::Species::Meloetta:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Meloetta>();
                break;
            case pksm::Species::Keldeo:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Keldeo>();
                break;
            case pksm::Species::Kyurem:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Kyurem>();
                break;
            case pksm::Species::Landorus:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Landorus>();
                break;
            case pksm::Species::Thundurus:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Thundurus>();
                break;
            case pksm::Species::Tornadus:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Tornadus>();
                break;
            case pksm::Species::Sawsbuck:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Sawsbuck>();
                break;
            case pksm::Species::Deerling:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Deerling>();
                break;
            case pksm::Species::Darmanitan:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Darmanitan>();
                break;
            case pksm::Species::Basculin:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Basculin>();
                break;
            case pksm::Species::Audino:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Audino>();
                break;
            case pksm::Species::Shaymin:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Shaymin>();
                break;
            case pksm::Species::Giratina:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Giratina>();
                break;
            case pksm::Species::Rotom:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Rotom>();
                break;
            case pksm::Species::Gallade:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Gallade>();
                break;
            case pksm::Species::Abomasnow:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Abomasnow>();
                break;
            case pksm::Species::Lucario:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Lucario>();
                break;
            case pksm::Species::Garchomp:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Garchomp>();
                break;
            case pksm::Species::Lopunny:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Lopunny>();
                break;
            case pksm::Species::Gastrodon:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Gastrodon>();
                break;
            case pksm::Species::Shellos:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Shellos>();
                break;
            case pksm::Species::Cherrim:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Cherrim>();
                break;
            case pksm::Species::Wormadam:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Wormadam>();
                break;
            case pksm::Species::Burmy:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Burmy>();
                break;
            case pksm::Species::Deoxys:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Deoxys>();
                break;
            case pksm::Species::Rayquaza:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Rayquaza>();
                break;
            case pksm::Species::Groudon:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Groudon>();
                break;
            case pksm::Species::Kyogre:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Kyogre>();
                break;
            case pksm::Species::Latios:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Latios>();
                break;
            case pksm::Species::Latias:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Latias>();
                break;
            case pksm::Species::Metagross:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Metagross>();
                break;
            case pksm::Species::Salamence:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Salamence>();
                break;
            case pksm::Species::Glalie:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Glalie>();
                break;
            case pksm::Species::Absol:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Absol>();
                break;
            case pksm::Species::Banette:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Banette>();
                break;
            case pksm::Species::Castform:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Castform>();
                break;
            case pksm::Species::Altaria:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Altaria>();
                break;
            case pksm::Species::Camerupt:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Camerupt>();
                break;
            case pksm::Species::Sharpedo:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Sharpedo>();
                break;
            case pksm::Species::Manectric:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Manectric>();
                break;
            case pksm::Species::Medicham:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Medicham>();
                break;
            case pksm::Species::Aggron:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Aggron>();
                break;
            case pksm::Species::Mawile:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Mawile>();
                break;
            case pksm::Species::Sableye:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Sableye>();
                break;
            case pksm::Species::Gardevoir:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Gardevoir>();
                break;
            case pksm::Species::Swampert:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Swampert>();
                break;
            case pksm::Species::Blaziken:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Blaziken>();
                break;
            case pksm::Species::Sceptile:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Sceptile>();
                break;
            case pksm::Species::Tyranitar:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Tyranitar>();
                break;
            case pksm::Species::Houndoom:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Houndoom>();
                break;
            case pksm::Species::Heracross:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Heracross>();
                break;
            case pksm::Species::Scizor:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Scizor>();
                break;
            case pksm::Species::Steelix:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Steelix>();
                break;
            case pksm::Species::Ampharos:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Ampharos>();
                break;
            case pksm::Species::Pichu:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Pichu>();
                break;
            case pksm::Species::Mewtwo:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Mewtwo>();
                break;
            case pksm::Species::Aerodactyl:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Aerodactyl>();
                break;
            case pksm::Species::Gyarados:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Gyarados>();
                break;
            case pksm::Species::Pinsir:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Pinsir>();
                break;
            case pksm::Species::Kangaskhan:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Kangaskhan>();
                break;
            case pksm::Species::Marowak:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Marowak>();
                break;
            case pksm::Species::Exeggutor:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Exeggutor>();
                break;
            case pksm::Species::Gengar:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Gengar>();
                break;
            case pksm::Species::Muk:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Muk>();
                break;
            case pksm::Species::Grimer:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Grimer>();
                break;
            case pksm::Species::Slowbro:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Slowbro>();
                break;
            case pksm::Species::Golem:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Golem>();
                break;
            case pksm::Species::Graveler:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Graveler>();
                break;
            case pksm::Species::Geodude:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Geodude>();
                break;
            case pksm::Species::Alakazam:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Alakazam>();
                break;
            case pksm::Species::Persian:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Persian>();
                break;
            case pksm::Species::Meowth:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Meowth>();
                break;
            case pksm::Species::Dugtrio:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Dugtrio>();
                break;
            case pksm::Species::Diglett:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Diglett>();
                break;
            case pksm::Species::Ninetales:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Ninetales>();
                break;
            case pksm::Species::Vulpix:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Vulpix>();
                break;
            case pksm::Species::Sandslash:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Sandslash>();
                break;
            case pksm::Species::Sandshrew:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Sandshrew>();
                break;
            case pksm::Species::Raichu:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Raichu>();
                break;
            case pksm::Species::Pikachu:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Pikachu>();
                break;
            case pksm::Species::Raticate:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Raticate>();
                break;
            case pksm::Species::Rattata:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Rattata>();
                break;
            case pksm::Species::Pidgeot:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Pidgeot>();
                break;
            case pksm::Species::Beedrill:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Beedrill>();
                break;
            case pksm::Species::Blastoise:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Blastoise>();
                break;
            case pksm::Species::Charizard:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Charizard>();
                break;
            case pksm::Species::Venusaur:
                imageOffsetFromBack = getSpeciesOffset<pksm::Species::Venusaur>();
                break;
            default:
                break;
        }
        int drawIndex = types_spritesheet_beast_idx + imageOffsetFromBack + form;
        if (drawIndex < types_spritesheet_201_1_idx)
        {
            Gui::drawImageAt(
                C2D_SpriteSheetGetImage(spritesheet_types, drawIndex), x, y, &tint, scale, scale);
        }
        else
        {
            Gui::drawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, pkm_spritesheet_0_idx), x, y,
                &tint, scale, scale);
        }
    }
}

void Gui::ball(pksm::Ball ball, int x, int y)
{
    if (ball <= pksm::Ball::Beast)
    {
        Gui::drawImageAt(
            C2D_SpriteSheetGetImage(spritesheet_types, size_t(ball) + types_spritesheet_empty_idx),
            x, y);
    }
    else
    {
        Gui::drawImageAt(
            C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_empty_idx), x, y);
    }
}

void Gui::type(pksm::Language lang, pksm::Type type, int x, int y)
{
    Gui::drawImageAt(typeImage(lang, type), x, y);
}

void Gui::setScreen(std::unique_ptr<Screen> screen)
{
    screens.push(std::move(screen));
}

int Gui::pointerBob()
{
    static int currentBob = 0;
    static bool up        = true;
    if (up)
    {
        currentBob++;
        if (currentBob >= 12)
        {
            up = false;
        }
    }
    else
    {
        currentBob--;
        if (currentBob <= 0)
        {
            up = true;
        }
    }
    return currentBob / 4;
}

u8 Gui::transparencyWaver()
{
    static u8 currentAmount = 255;
    static bool dir         = true;
    if (!dir)
    {
        currentAmount++;
        if (currentAmount == 255)
            dir = true;
    }
    else
    {
        currentAmount--;
        if (currentAmount < 155)
            dir = false;
    }
    return currentAmount;
}

bool Gui::showChoiceMessage(const std::string& message, int timer)
{
    DecisionScreen screen(message);
    return runScreen(screen);
}

void Gui::waitFrame(const std::string& message)
{
    if (inFrame)
    {
        C3D_FrameEnd(0);
        Gui::frameClean();
    }

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    Gui::clearScreen(GFX_TOP);
    Gui::clearScreen(GFX_BOTTOM);

    target(GFX_TOP);
    sprite(ui_sheet_part_info_top_idx, 0, 0);

    auto parsed   = parseText(message, FONT_SIZE_15);
    float lineMod = fontGetInfo(nullptr)->lineFeed * FONT_SIZE_15;

    text(parsed, 200, 110, FONT_SIZE_15, FONT_SIZE_15,
        PKSM_Color(255, 255, 255, transparencyWaver()), TextPosX::CENTER, TextPosY::CENTER);

    float continueY = 110 + (lineMod / 2) * parsed->lines();

    text(i18n::localize("PLEASE_WAIT"), 200, continueY + 3, FONT_SIZE_11, COLOR_WHITE,
        TextPosX::CENTER, TextPosY::TOP);

    flushText();

    target(GFX_BOTTOM);
    sprite(ui_sheet_part_info_bottom_idx, 0, 0);

    C3D_FrameEnd(0);
    Gui::frameClean();

    if (inFrame)
    {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    }
}

void Gui::warn(const std::string& message, std::optional<pksm::Language> lang)
{
    MessageScreen screen(message, lang.value_or(pksm::Language::ENG));
    runScreen(screen);
}

void Gui::screenBack()
{
    scrollOffsets.clear();
    screens.pop();
}

void Gui::showRestoreProgress(u32 partial, u32 total)
{
    if (inFrame)
    {
        C3D_FrameEnd(0);
        Gui::frameClean();
    }

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    Gui::clearScreen(GFX_TOP);
    Gui::clearScreen(GFX_BOTTOM);
    target(GFX_TOP);
    sprite(ui_sheet_part_info_top_idx, 0, 0);
    text(i18n::localize("SAVING"), 200, 95, FONT_SIZE_15, COLOR_WHITE, TextPosX::CENTER,
        TextPosY::TOP);
    text(fmt::format(i18n::localize("SAVE_PROGRESS"), partial, total), 200, 130, FONT_SIZE_12,
        COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
    flushText();

    target(GFX_BOTTOM);
    sprite(ui_sheet_part_info_bottom_idx, 0, 0);

    C3D_FrameEnd(0);
    Gui::frameClean();

    if (inFrame)
    {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    }
}

void Gui::showDownloadProgress(const std::string& path, u32 partial, u32 total)
{
    if (inFrame)
    {
        C3D_FrameEnd(0);
        Gui::frameClean();
    }

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    Gui::clearScreen(GFX_TOP);
    Gui::clearScreen(GFX_BOTTOM);
    target(GFX_TOP);
    sprite(ui_sheet_part_info_top_idx, 0, 0);
    text(fmt::format(i18n::localize("DOWNLOADING_FILE"), path), 200, 95, FONT_SIZE_15, COLOR_WHITE,
        TextPosX::CENTER, TextPosY::TOP);
    text(fmt::format(i18n::localize("SAVE_PROGRESS"), partial, total), 200, 130, FONT_SIZE_12,
        COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
    flushText();

    target(GFX_BOTTOM);
    sprite(ui_sheet_part_info_bottom_idx, 0, 0);

    C3D_FrameEnd(0);
    Gui::frameClean();

    if (inFrame)
    {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    }
}

void Gui::showResizeStorage()
{
    if (inFrame)
    {
        C3D_FrameEnd(0);
        Gui::frameClean();
    }

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    Gui::clearScreen(GFX_TOP);
    Gui::clearScreen(GFX_BOTTOM);
    target(GFX_TOP);
    sprite(ui_sheet_part_info_top_idx, 0, 0);
    text(i18n::localize("STORAGE_RESIZE"), 200, 95, FONT_SIZE_15, COLOR_WHITE, TextPosX::CENTER,
        TextPosY::TOP);
    flushText();

    target(GFX_BOTTOM);
    sprite(ui_sheet_part_info_bottom_idx, 0, 0);

    C3D_FrameEnd(0);
    Gui::frameClean();

    if (inFrame)
    {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    }
}

void Gui::error(const std::string& message, Result errorCode)
{
    u32 keys = 0;
    if (inFrame)
    {
        C3D_FrameEnd(0);
        Gui::frameClean();
    }
    hidScanInput();
    while (aptMainLoop() && !((keys = hidKeysDown()) & KEY_A))
    {
        hidScanInput();
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        Gui::clearScreen(GFX_TOP);
        Gui::clearScreen(GFX_BOTTOM);

        target(GFX_TOP);
        sprite(ui_sheet_part_info_top_idx, 0, 0);
        u8 transparency = transparencyWaver();
        text(message, 200, 85, FONT_SIZE_15, PKSM_Color(255, 255, 255, transparency),
            TextPosX::CENTER, TextPosY::TOP);
        text(fmt::format(i18n::localize("ERROR_CODE"), (u32)errorCode), 200, 105, FONT_SIZE_15,
            PKSM_Color(255, 255, 255, transparency), TextPosX::CENTER, TextPosY::TOP);

        text(i18n::localize("CONTINUE"), 200, 130, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER,
            TextPosY::TOP);

        flushText();

        target(GFX_BOTTOM);
        sprite(ui_sheet_part_info_bottom_idx, 0, 0);

        if (!aptIsHomeAllowed() && aptCheckHomePressRejected())
        {
            setDoHomeDraw();
        }

        drawNoHome();

        C3D_FrameEnd(0);
        Gui::frameClean();
    }
    hidScanInput();
    if (inFrame)
    {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    }
}

void Gui::drawSelector(float x, float y)
{
    static constexpr int w     = 2;
    static float timer         = 0.0f;
    float highlight_multiplier = fmax(0.0, fabs(fmod(timer, 1.0) - 0.5) / 0.5);
    u8 r                       = COLOR_SELECTOR.r;
    u8 g                       = COLOR_SELECTOR.g;
    u8 b                       = COLOR_SELECTOR.b;
    PKSM_Color color           = PKSM_Color(r + (255 - r) * highlight_multiplier,
        g + (255 - g) * highlight_multiplier, b + (255 - b) * highlight_multiplier, 255);

    Gui::drawSolidRect(x, y, 50, 50, PKSM_Color(255, 255, 255, 100));
    Gui::drawSolidRect(x, y, 50, w, color);                      // top
    Gui::drawSolidRect(x, y + w, w, 50 - 2 * w, color);          // left
    Gui::drawSolidRect(x + 50 - w, y + w, w, 50 - 2 * w, color); // right
    Gui::drawSolidRect(x, y + 50 - w, 50, w, color);             // bottom

    timer += .025f;
}
