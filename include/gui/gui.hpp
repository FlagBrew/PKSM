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

#ifndef GUI_HPP
#define GUI_HPP

#include <3ds.h>
#include <citro2d.h>
#include <unordered_map>
#include <fstream>
#include <string.h>
#include <random>
#include "json.hpp"
#include "i18n.hpp"
#include "PKX.hpp"
#include "Sav.hpp"

#include "ui_spritesheet.h"
#include "pkm_spritesheet.h"
#include "types_spritesheet.h"
#include "WCX.hpp"

// emulated
#define ui_spritesheet_res_emulated_button_arrow_right_idx 501
#define ui_spritesheet_res_emulated_selector_menu_left_idx 502
#define ui_spritesheet_res_null 503

// colors
#define COLOR_WHITE     C2D_Color32(255, 255, 255, 255)
#define COLOR_BLACK     C2D_Color32(  0,   0,   0, 255)
#define COLOR_BLUE      C2D_Color32(124, 147, 225, 255)
#define COLOR_YELLOW    C2D_Color32(237, 247, 157, 255)
#define COLOR_LIGHTBLUE C2D_Color32(187, 208, 254, 255)
#define COLOR_DARKBLUE  C2D_Color32( 55,  89, 187, 255)
#define COLOR_HIGHBLUE  C2D_Color32( 48,  65, 106, 255)
#define COLOR_MASKBLACK C2D_Color32(  0,   0,   0, 190)
#define COLOR_PALEBLUE  C2D_Color32( 90, 115, 164, 255)
#define COLOR_MENUBLUE  C2D_Color32( 55,  89, 157, 255)

#define FONT_SIZE_18 0.72f
#define FONT_SIZE_15 0.6f
#define FONT_SIZE_14 0.56f
#define FONT_SIZE_12 0.50f
#define FONT_SIZE_11 0.46f
#define FONT_SIZE_9 0.37f

namespace Gui
{
    Result init(void);
    void exit(void);

    C3D_RenderTarget* target(gfxScreen_t t);
    C2D_Image TWLIcon(void);
    C2D_Image type(Language lang, u8 type);

    void ball(size_t index, int x, int y);
    void generation(PKX* pkm, int x, int y);
    void sprite(int key, int x, int y);
    void sprite(int key, int x, int y, u32 color);
    void pkm(PKX* pkm, int x, int y, u32 color = 0);
    void pkm(int formSpecies, int x, int y, u32 color = 0);
    void pkmInfoViewer(PKX* pkm);

    void backgroundTop(void);
    void backgroundBottom(void);
    void backgroundAnimated(gfxScreen_t screen);

    void clearTextBufs(void);    
    void dynamicText(const std::string& str, int x, int y, float scaleX, float scaleY, u32 color);
    void dynamicText(const std::string& text, int x, int y, float scaleX, float scaleY, u32 color, float maxWidth);
    void dynamicText(gfxScreen_t screen, int y, const std::string& text, float scaleX, float scaleY, u32 color);
    void dynamicText(int x, int y, float width, const std::string& text, float scaleX, float scaleY, u32 color);

    C2D_Text cacheStaticText(const std::string& strKey);
    void staticText(const std::string& strKey, int x, int y, float scaleX, float scaleY, u32 color);
    void staticText(const std::string& strKey, int x, int y, float scaleX, float scaleY, u32 color, float maxWidth);
    void staticText(gfxScreen_t screen, int y, const std::string& strKey, float scaleX, float scaleY, u32 color);
    void staticText(int x, int y, float width, const std::string& strKey, float scaleX, float scaleY, u32 color);

    // TODO: remove
    //void drawBox(int box, const Sav* save);
    //void eventList(WCX* database[], int currentEntry, int page);
    //void menu(Language lang);
}

extern std::mt19937 g_randomNumbers;
static inline float randomRotation(void)
{
    return (((float)(g_randomNumbers() % 1000)) / 500.0f) - 1.0f;
}

extern C2D_TextBuf g_widthBuf;
static inline float textWidth(const std::string& str, float scaleX)
{
    C2D_Text text;
    C2D_TextParse(&text, g_widthBuf, str.c_str());
    return ceilf(text.width*scaleX);
}

static inline float textWidth(const C2D_Text& str, float scaleX)
{
    return ceilf(str.width*scaleX);
}

#endif