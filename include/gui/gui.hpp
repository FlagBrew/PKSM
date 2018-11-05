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
#include <stack>
#include "json.hpp"
#include "i18n.hpp"
#include "PKX.hpp"
#include "Sav.hpp"
#include "Optional.hpp"

#include "ui_sheet.h"
#include "pkm_spritesheet.h"
#include "types_spritesheet.h"
#include "WCX.hpp"

#include "Screen.hpp"

// emulated
#define ui_sheet_res_null_idx 500
#define ui_sheet_emulated_pointer_horizontal_flipped_idx 501
#define ui_sheet_emulated_bg_top_red 502
#define ui_sheet_emulated_bg_top_blue 503
#define ui_sheet_emulated_bg_top_green 504
#define ui_sheet_emulated_bg_bottom_red 505
#define ui_sheet_emulated_bg_bottom_blue 506
#define ui_sheet_emulated_bg_bottom_green 507
#define ui_sheet_emulated_eventmenu_bar_selected_flipped_horizontal_idx 508
#define ui_sheet_emulated_eventmenu_bar_unselected_flipped_horizontal_idx 509
#define ui_sheet_emulated_eventmenu_bar_selected_flipped_vertical_idx 510
#define ui_sheet_emulated_eventmenu_bar_unselected_flipped_vertical_idx 511
#define ui_sheet_emulated_eventmenu_bar_selected_flipped_both_idx 512
#define ui_sheet_emulated_eventmenu_bar_unselected_flipped_both_idx 513
#define ui_sheet_emulated_storage_box_corner_flipped_horizontal_idx 514
#define ui_sheet_emulated_storage_box_corner_flipped_vertical_idx 515
#define ui_sheet_emulated_storage_box_corner_flipped_both_idx 516
#define ui_sheet_emulated_toggle_green_idx 517
#define ui_sheet_emulated_toggle_red_idx 518
#define ui_sheet_emulated_gameselector_bg_idx 519

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

// TODO: all the colors to be defined here
#define COLOR_SELECTOR  C2D_Color32( 29,  50, 253, 255)

#define FONT_SIZE_18 0.72f
#define FONT_SIZE_15 0.6f
#define FONT_SIZE_14 0.56f
#define FONT_SIZE_12 0.50f
#define FONT_SIZE_11 0.46f
#define FONT_SIZE_9 0.37f

namespace Gui
{
    Result init(void);
    void mainLoop(void);
    void exit(void);

    C3D_RenderTarget* target(gfxScreen_t t);
    C2D_Image TWLIcon(void);

    void ball(size_t index, int x, int y);
    void type(Language lang, u8 type, int x, int y);
    void generation(PKX* pkm, int x, int y);
    void sprite(int key, int x, int y);
    void sprite(int key, int x, int y, u32 color);
    void pkm(PKX* pkm, int x, int y, float scale = 1.0f, u32 color = C2D_Color32(0, 0, 0, 255), float blend = 0.0f);
    void pkm(int species, int form, int generation, int x, int y, float scale = 1.0f, u32 color = C2D_Color32(0, 0, 0, 255), float blend = 0.0f);
    void pkmInfoViewer(PKX* pkm);

    void backgroundTop(bool stripes);
    void backgroundBottom(bool stripes);
    void backgroundAnimatedTop(void);
    void backgroundAnimatedBottom(void);

    void clearTextBufs(void);
    void dynamicText(const std::string& str, int x, int y, float scaleX, float scaleY, u32 color, bool rightAligned = false);
    void dynamicText(const std::string& text, int x, int y, float scaleX, float scaleY, u32 color, float maxWidth, bool fullCenter = false);
    void dynamicText(gfxScreen_t screen, int y, const std::string& text, float scaleX, float scaleY, u32 color);
    void dynamicText(int x, int y, float width, const std::string& text, float scaleX, float scaleY, u32 color);

    C2D_Text cacheStaticText(const std::string& strKey);
    void clearStaticText(void);
    void staticText(const std::string& strKey, int x, int y, float scaleX, float scaleY, u32 color, bool rightAligned = false);
    void staticText(const std::string& strKey, int x, int y, float scaleX, float scaleY, u32 color, float maxWidth);
    void staticText(gfxScreen_t screen, int y, const std::string& strKey, float scaleX, float scaleY, u32 color);
    void staticText(int x, int y, float width, const std::string& strKey, float scaleX, float scaleY, u32 color);

    void setScreen(std::unique_ptr<Screen> screen);
    void screenBack(void);
    bool showChoiceMessage(const std::string& message, Optional<std::string> message2 = Optional<std::string>(nullptr));
    void showRestoreProgress(u32 partial, u32 total);
    void warn(const std::string& message, Optional<std::string> message2 = Optional<std::string>(nullptr));
    void setNextKeyboardFunc(std::function<void()> callback);
    void showResizeStorage(void);
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