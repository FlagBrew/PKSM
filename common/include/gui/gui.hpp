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

#ifndef GUI_HPP
#define GUI_HPP

#include "PKX.hpp"
#include "Sav.hpp"
#include "Screen.hpp"
#include "TextParse.hpp"
#include "TextPos.hpp"
#include "WCX.hpp"
#include "colors.hpp"
#include "i18n.hpp"
#include "json.hpp"
#include "random.hpp"
#include "sound.hpp"
#include "spritesheets.h"
#include "thread.hpp"
#include "types.h"
#include "utils.hpp"
#include <citro2d.h>
#include <stack>
#include <string.h>

namespace Gui
{
    Result init(void);
    void mainLoop(void);
    void exit(void);

#if defined(_3DS)
    void target(gfxScreen_t t);
    void clearScreen(gfxScreen_t t);
    void flushText();
    C2D_Image TWLIcon(void);
#elif defined(__SWITCH__)
    // Dunno what specific things might be necessary
#endif

    void ball(size_t index, int x, int y);
    void type(Language lang, u8 type, int x, int y);
    void generation(const PKX& pkm, int x, int y);
    void format(const PKX& pkm, int x, int y);
    void sprite(int key, int x, int y);
    void sprite(int key, int x, int y, PKSM_Color color);
    void pkm(const PKX& pkm, int x, int y, float scale = 1.0f, PKSM_Color color = COLOR_BLACK, float blend = 0.0f);
    void pkm(int species, int form, Generation generation, int gender, int x, int y, float scale = 1.0f, PKSM_Color color = COLOR_BLACK,
        float blend = 0.0f);

    int pointerBob();
#if defined(_3DS)
    void drawImageAt(const C2D_Image& img, float x, float y, const C2D_ImageTint* tint = nullptr, float scaleX = 1.0f, float scaleY = 1.0f);
#elif defined(__SWITCH__)
    // Not sure what the image format is
    void drawImageAt(const Image& img, float x, float y, const Tint* tint = nullptr, float scaleX = 1.0f, float scaleY = 1.0f);
#endif
    void drawSolidRect(float x, float y, float w, float h, PKSM_Color color);
    void drawSolidCircle(float x, float y, float radius, PKSM_Color color);
    void drawSolidTriangle(float x1, float y1, float x2, float y2, float x3, float y3, PKSM_Color color);
    void drawLine(float x1, float y1, float x2, float y2, float thickness, PKSM_Color color);

    void backgroundTop(bool stripes);
    void backgroundBottom(bool stripes);
    void backgroundAnimatedTop(void);
    void backgroundAnimatedBottom(void);
    void setDoHomeDraw(void);
    void drawNoHome(void);
    void drawSelector(float x, float y);

    // Used to get text width/number of lines
    std::shared_ptr<TextParse::Text> parseText(const std::string& str, FontSize size, float maxWidth = 0.0f);
    void clearText(void);
    void text(const std::shared_ptr<TextParse::Text> text, float x, float y, FontSize size, PKSM_Color color, TextPosX positionX, TextPosY positionY);
    void text(
        const std::string& str, float x, float y, FontSize size, PKSM_Color color, TextPosX positionX, TextPosY positionY, float maxWidth = 0.0f);
    // Only works with single lines, because why the fuck would you wrap AND scroll?
    void scrollingText(const std::string& str, float x, float y, FontSize size, PKSM_Color color, TextPosX positionX, TextPosY positionY, int width);
    void slicedText(const std::string& str, float x, float y, FontSize size, PKSM_Color color, TextPosX positionX, TextPosY positionY, int width);

    void setScreen(std::unique_ptr<Screen> screen);
    void screenBack(void);
    bool showChoiceMessage(const std::string& message, int timer = 0);
    void showRestoreProgress(u32 partial, u32 total);
    void showDownloadProgress(const std::string& path, u32 partial, u32 total);
    void waitFrame(const std::string& message);
    void warn(const std::string& message, std::optional<Language> forceLang = std::nullopt);
    void error(const std::string& message, Result errorCode);
    void showResizeStorage(void);
}

#endif
