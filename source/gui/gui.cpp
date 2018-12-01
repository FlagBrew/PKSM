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

#include "gui.hpp"

C3D_RenderTarget* g_renderTargetTop;
C3D_RenderTarget* g_renderTargetBottom;
C2D_TextBuf g_widthBuf;

static C2D_SpriteSheet spritesheet_ui;
static C2D_SpriteSheet spritesheet_pkm;
static C2D_SpriteSheet spritesheet_types;
static C2D_Image bgBoxes;
static C2D_TextBuf dynamicBuf;
static C2D_TextBuf staticBuf;
static std::unordered_map<std::string, C2D_Text> staticMap;

static std::stack<std::unique_ptr<Screen>> screens;
static std::function<void()> keyboardFunc;

static Tex3DS_SubTexture _select_box(const C2D_Image& image, int x, int y, int endX, int endY)
{
    Tex3DS_SubTexture tex = *image.subtex;
    if (x != endX)
    {
        int deltaX = endX - x;
        float texRL = tex.left - tex.right;
        tex.left = tex.left - (float) texRL / tex.width * x;
        tex.right = tex.left - (float) texRL / tex.width * deltaX;
        tex.width = deltaX;
    }
    if (y != endY)
    {
        float texTB = tex.top - tex.bottom;
        int deltaY = endY - y;
        tex.top = tex.top - (float) texTB / tex.height * y;
        tex.bottom = tex.top - (float) texTB / tex.height * deltaY;
        tex.height = deltaY;
    }
    return tex;
}

C2D_Image Gui::TWLIcon(void)
{
    return C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_gameselector_twlcart_idx);
}

void Gui::backgroundBottom(bool stripes)
{
    if (stripes)
    {
        for (int x = 0; x < 320; x += 7)
        {
            for (int y = 0; y < 240; y += 7)
            {
                C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_bg_stripe_bottom_idx), x, y, 0.5f);
            }
        }
    }
    else
    {
        C2D_DrawRectSolid(0, 0, 0.5f, 320, 240, C2D_Color32(40, 53, 147, 255));
    }
    C2D_DrawRectSolid(0, 220, 0.5f, 320, 20, C2D_Color32(26, 35, 126, 255));
}

void Gui::backgroundTop(bool stripes)
{
    if (stripes)
    {
        for (int x = 0; x < 400; x += 7)
        {
            for (int y = 0; y < 240; y += 7)
            {
                C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_bg_stripe_top_idx), x, y, 0.5f);
            }
        }
    }
    else
    {
        C2D_DrawRectSolid(0, 0, 0.5f, 400, 240, C2D_Color32(26, 35, 126, 255));
    }
    C2D_DrawRectSolid(0, 0, 0.5f, 400, 25, C2D_Color32(15, 22, 89, 255));
}

void Gui::backgroundAnimatedTop()
{
    static int x1 = 0;
    static int x2 = 400;
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

    C2D_DrawImageAt({bgBoxes.tex, &boxes1}, x1--, 0, 0.5f);
    C2D_DrawImageAt({bgBoxes.tex, &boxes2}, x2--, 0, 0.5f);
}

void Gui::backgroundAnimatedBottom()
{
    static int x1 = 0;
    static int x2 = 400;
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

    C2D_DrawImageAt({bgBoxes.tex, &boxes1}, x1--, 0, 0.5f);
    C2D_DrawImageAt({bgBoxes.tex, &boxes2}, x2--, 0, 0.5f);
}

void Gui::clearTextBufs(void)
{
    C2D_TextBufClear(dynamicBuf);
    C2D_TextBufClear(g_widthBuf);
}

void Gui::dynamicText(const std::string& str, int x, int y, float scaleX, float scaleY, u32 color, bool rightAligned)
{
    C2D_Text text;
    C2D_TextParse(&text, dynamicBuf, str.c_str());
    C2D_TextOptimize(&text);
    if (rightAligned)
    {
        x = x - textWidth(text, scaleX);
    }
    C2D_DrawText(&text, C2D_WithColor, x, y, 0.5f, scaleX, scaleY, color);
}

// TODO: move to StringUtils
std::string splitWord(std::string& word, float scaleX, float maxWidth)
{
    float defaultWidth = scaleX * fontGetInfo()->defaultWidth.charWidth;
    if (textWidth(word, scaleX) > maxWidth)
    {
        // TODO: maxChars is wrong:
        // with lowercase, it works with size_t maxChars = ceilf(maxWidth / defaultWidth) * 2;
        // with uppercase, it works with ceilf(maxWidth / defaultWidth * 1.25f);
        size_t maxChars = ceilf(maxWidth / defaultWidth);
        for (std::string::iterator it = word.begin() + maxChars; it < word.end(); it += maxChars)
        {
            word.insert(it++, '\n');
        }
    }
    return word;
}

void Gui::dynamicText(const std::string& str, int x, int y, float scaleX, float scaleY, u32 color, float maxWidth, bool fullCenter)
{
    std::string dst, line, word;
    dst = line = word = "";

    for (std::string::const_iterator it = str.begin(); it != str.end(); it++)
    {
        word += *it;
        if (*it == ' ')
        {
            // split single words that are bigger than maxWidth
            word = splitWord(word, scaleX, maxWidth);
            if (textWidth(line + word, scaleX) <= maxWidth)
            {
                line += word;
            }
            else
            {
                dst += line + '\n';
                line = word;
            }
            word = "";
        }
    }

    // "Another iteration" of the loop b/c it probably won't end with a space
    // If it does, no harm done
    word = splitWord(word, scaleX, maxWidth);
    if (textWidth(line + word, scaleX) <= maxWidth)
    {
        dst += line + word;
    }
    else
    {
        dst += line + '\n' + word;
    }

    if (!fullCenter)
    {
        C2D_Text text;
        C2D_TextParse(&text, dynamicBuf, dst.c_str());
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, x, y, 0.5f, scaleX, scaleY, color);
    }
    else
    {
        std::string tmp = dst;
        std::vector<std::string> draw;
        while (tmp.find('\n') != std::string::npos)
        {
            draw.push_back(tmp.substr(0, tmp.find('\n')));
            tmp = tmp.substr(tmp.find('\n') + 1);
        }
        draw.push_back(tmp);
        for (size_t i = 0; i < draw.size(); i++)
        {
            C2D_Text text;
            C2D_TextParse(&text, dynamicBuf, draw[i].c_str());
            C2D_TextOptimize(&text);
            float textWidth, textHeight;
            C2D_TextGetDimensions(&text, scaleX, scaleY, &textWidth, &textHeight);
            int drawX = x + ceilf((maxWidth - textWidth) / 2.0f);
            int drawY = y + ceilf(textHeight) * 1.25 * i ;
            drawY -= ceilf(textHeight) * draw.size() / 2;
            C2D_DrawText(&text, C2D_WithColor, drawX, drawY, 0.5f, scaleX, scaleY, color);
        }
    }
}

void Gui::dynamicText(gfxScreen_t screen, int y, const std::string& text, float scaleX, float scaleY, u32 color)
{
    float width = (screen == GFX_TOP) ? 400.0f : 320.0f;
    int x = ceilf((width - textWidth(text, scaleX)) / 2.0f);
    dynamicText(text, x, y, scaleX, scaleY, color);
}

void Gui::dynamicText(int x, int y, float width, const std::string& text, float scaleX, float scaleY, u32 color)
{
    int drawX = x + ceilf((width - textWidth(text, scaleX)) / 2.0f);
    dynamicText(text, drawX, y, scaleX, scaleY, color);
}

C2D_Text Gui::cacheStaticText(const std::string& strKey)
{
    C2D_Text text;
    std::unordered_map<std::string, C2D_Text>::const_iterator index = staticMap.find(strKey);
    if (index == staticMap.end())
    {
        C2D_TextParse(&text, staticBuf, strKey.c_str());
        C2D_TextOptimize(&text);
        staticMap.emplace(strKey, text);
    }
    else
    {
        return index->second;
    }

    return text;
}

void Gui::clearStaticText()
{
    C2D_TextBufClear(staticBuf);
    staticMap.clear();
}

void Gui::staticText(const std::string& strKey, int x, int y, float scaleX, float scaleY, u32 color, bool rightAligned)
{
    C2D_Text text = cacheStaticText(strKey);
    if (rightAligned)
    {
        x = x - textWidth(text, scaleX);
    }
    C2D_DrawText(&text, C2D_WithColor, x, y, 0.5f, scaleX, scaleY, color);
}

void Gui::staticText(const std::string& strKey, int x, int y, float scaleX, float scaleY, u32 color, float maxWidth)
{
    std::string print = strKey;
    C2D_Text text;
    std::unordered_map<std::string, C2D_Text>::const_iterator index = staticMap.find(strKey);
    if (index == staticMap.end())
    {
        float defaultWidth = scaleX * fontGetInfo()->defaultWidth.charWidth;
        float width = defaultWidth * print.length();
        if (width > maxWidth)
        {
            size_t maxChars = ceilf(maxWidth / defaultWidth);
            for (std::string::iterator it = print.begin() + maxChars; it < print.end(); it += maxChars)
            {
                print.insert(it++, '\n');
            }
        }
        C2D_TextParse(&text, dynamicBuf, print.c_str());
        C2D_TextOptimize(&text);
        staticMap.emplace(strKey, text);
    }
    else
    {
        text = index->second;
    }
    C2D_DrawText(&text, C2D_WithColor, x, y, 0.5f, scaleX, scaleY, color);
}

void Gui::staticText(gfxScreen_t screen, int y, const std::string& strKey, float scaleX, float scaleY, u32 color)
{
    C2D_Text text = cacheStaticText(strKey);
    float width = (screen == GFX_TOP) ? 400.0f : 320.0f;
    int x = ceilf((width - textWidth(text, scaleX)) / 2.0f);
    C2D_DrawText(&text, C2D_WithColor, x, y, 0.5f, scaleX, scaleY, color);
}

void Gui::staticText(int x, int y, float width, const std::string& strKey, float scaleX, float scaleY, u32 color)
{
    C2D_Text text = cacheStaticText(strKey);
    int drawX = x + ceilf((width - textWidth(text, scaleX)) / 2.0f);
    C2D_DrawText(&text, C2D_WithColor, drawX, y, 0.5f, scaleX, scaleY, color);
}

static void _draw_mirror_scale(int key, int x, int y, int off, int rep)
{
    C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, key);
    // Left side
    Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, off, 0);
    C2D_DrawImageAt({sprite.tex, &tex}, x, y, 0.5f);
    // Right side
    C2D_DrawImageAt({sprite.tex, &tex}, x + off + rep, y, 0.5f, nullptr, -1.0f, 1.0f);
    // Center
    tex = _select_box(sprite, off, 0, sprite.subtex->width, 0);
    C2D_DrawImageAt({sprite.tex, &tex}, x + off, y, 0.5f, nullptr, rep, 1.0f);
}

static void _draw_repeat(int key, int x, int y, u8 rows, u8 cols)
{
    C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, key);
    for (u8 row = 0; row < rows; row++)
    {
        for (u8 col = 0; col < cols; col++)
        {
            C2D_DrawImageAt(sprite, x + col * sprite.subtex->width, y + row * sprite.subtex->height, 0.5f);
        }
    }
}

Result Gui::init(void)
{
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    srand(time(NULL));

    g_renderTargetTop = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    g_renderTargetBottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    dynamicBuf = C2D_TextBufNew(2048);
    g_widthBuf = C2D_TextBufNew(4096);
    staticBuf = C2D_TextBufNew(4096);

    spritesheet_ui = C2D_SpriteSheetLoad("romfs:/gfx/ui_sheet.t3x");
    spritesheet_pkm = C2D_SpriteSheetLoad("romfs:/gfx/pkm_spritesheet.t3x");
    spritesheet_types = C2D_SpriteSheetLoad("romfs:/gfx/types_spritesheet.t3x");

    bgBoxes = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_anim_squares_idx);

    return 0;
}

void Gui::mainLoop(void)
{
    bool exit = false;
    while (aptMainLoop() && !exit)
    {
        hidScanInput();
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(g_renderTargetTop, COLOR_BLACK);
        C2D_TargetClear(g_renderTargetBottom, COLOR_BLACK);

        screens.top()->draw();
        touchPosition touch;
        hidTouchRead(&touch);
        screens.top()->update(&touch);
        exit = screens.top()->type() == ScreenType::TITLELOAD && (hidKeysDown() & KEY_START);

        C3D_FrameEnd(0);
        Gui::clearTextBufs();
        if (keyboardFunc != nullptr)
        {
            keyboardFunc();
            keyboardFunc = nullptr;
        }
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
    C2D_TextBufDelete(g_widthBuf);
    C2D_TextBufDelete(dynamicBuf);
    C2D_TextBufDelete(staticBuf);
    C2D_Fini();
    C3D_Fini();
    gfxExit();
}

void Gui::sprite(int key, int x, int y)
{
    if (key == ui_sheet_res_null_idx)
    {
        return;
    }
    else if (key == ui_sheet_eventmenu_bar_selected_idx || key == ui_sheet_eventmenu_bar_unselected_idx)
    {
        u8 off = 4, rep = 174;
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, key);
        // Left side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, off, 0);
        C2D_DrawImageAt({sprite.tex, &tex}, x, y, 0.5f);
        // Center
        tex = _select_box(sprite, off, 0, sprite.subtex->width, 0);
        C2D_DrawImageAt({sprite.tex, &tex}, x + off, y, 0.5f, nullptr, rep, 1.0f);
    }
    else if (key == ui_sheet_eventmenu_page_indicator_idx)
    {
        _draw_mirror_scale(key, x, y, 4, 182);
    }
    else if (key == ui_sheet_gameselector_savebox_idx)
    {
        u8 off = 53;
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, key);
        // Top side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, 0, off);
        C2D_DrawImageAt({sprite.tex, &tex}, x, y, 0.5f);
        // Bottom side
        C2D_DrawImageAt({sprite.tex, &tex}, x, y + off - 1, 0.5f, nullptr, 1.0f, -1.0f);         
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
        C2D_DrawImageAt(sprite, x, y, 0.5f);
        // Top right
        C2D_DrawImageAt(sprite, x + width, y, 0.5f, NULL, -1.0f, 1.0f);
        // Bottom left
        C2D_DrawImageAt(sprite, x, y + height, 0.5f, NULL, 1.0f, -1.0f);
        // Bottom right
        C2D_DrawImageAt(sprite, x + width, y + height, 0.5f, NULL, -1.0f, -1.0f);
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
        C2D_DrawRectSolid(0, 225, 0.5f, 400, 15, COLOR_WHITE);
    }
    
    //emulated
    else if (key == ui_sheet_emulated_pointer_horizontal_flipped_idx)
    {
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_pointer_horizontal_idx);
        C2D_DrawImageAt(sprite, x, y, 0.5f, NULL, -1.0f, 1.0f);
    }
    else if (key == ui_sheet_emulated_bg_top_red)
    {
        C2D_ImageTint tint;
        C2D_SetImageTint(&tint, C2D_TopLeft, C2D_Color32(228, 143, 131, 255), 1);
        C2D_SetImageTint(&tint, C2D_TopRight, C2D_Color32(201, 95, 84, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotLeft, C2D_Color32(239, 163, 151, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotRight, C2D_Color32(214, 117, 106, 255), 1);
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_bg_top_greyscale_idx), x, y, 0.5f, &tint);
    }
    else if (key == ui_sheet_emulated_bg_top_blue)
    {
        C2D_ImageTint tint;
        C2D_SetImageTint(&tint, C2D_TopLeft, C2D_Color32(139, 171, 221, 255), 1);
        C2D_SetImageTint(&tint, C2D_TopRight, C2D_Color32(93, 134, 193, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotLeft, C2D_Color32(158, 186, 233, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotRight, C2D_Color32(113, 150, 205, 255), 1);
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_bg_top_greyscale_idx), x, y, 0.5f, &tint);
    }
    else if (key == ui_sheet_emulated_bg_top_green)
    {
        C2D_ImageTint tint;
        C2D_SetImageTint(&tint, C2D_TopLeft, C2D_Color32(142, 221, 138, 255), 1);
        C2D_SetImageTint(&tint, C2D_TopRight, C2D_Color32(101, 193, 93, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotLeft, C2D_Color32(161, 233, 158, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotRight, C2D_Color32(119, 205, 113, 255), 1);
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_bg_top_greyscale_idx), x, y, 0.5f, &tint);
    }
    else if (key == ui_sheet_emulated_bg_bottom_red)
    {
        C2D_ImageTint tint;
        C2D_SetImageTint(&tint, C2D_TopLeft, C2D_Color32(216, 122, 111, 255), 1);
        C2D_SetImageTint(&tint, C2D_TopRight, C2D_Color32(239, 163, 151, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotLeft, C2D_Color32(201, 95, 84, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotRight, C2D_Color32(224, 134, 123, 255), 1);
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_bg_bottom_greyscale_idx), x, y, 0.5f, &tint);        
    }
    else if (key == ui_sheet_emulated_bg_bottom_blue)
    {
        C2D_ImageTint tint;
        C2D_SetImageTint(&tint, C2D_TopLeft, C2D_Color32(120, 154, 209, 255), 1);
        C2D_SetImageTint(&tint, C2D_TopRight, C2D_Color32(158, 186, 233, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotLeft, C2D_Color32(93, 134, 193, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotRight, C2D_Color32(131, 165, 217, 255), 1);
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_bg_bottom_greyscale_idx), x, y, 0.5f, &tint);  
    }
    else if (key == ui_sheet_emulated_bg_bottom_green)
    {
        C2D_ImageTint tint;
        C2D_SetImageTint(&tint, C2D_TopLeft, C2D_Color32(125, 209, 119, 255), 1);
        C2D_SetImageTint(&tint, C2D_TopRight, C2D_Color32(161, 233, 158, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotLeft, C2D_Color32(101, 193, 93, 255), 1);
        C2D_SetImageTint(&tint, C2D_BotRight, C2D_Color32(136, 217, 131, 255), 1);
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_bg_bottom_greyscale_idx), x, y, 0.5f, &tint);        
    }
    else if (key == ui_sheet_emulated_eventmenu_bar_selected_flipped_horizontal_idx)
    {
        u8 off = 4, rep = 174;
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_eventmenu_bar_selected_idx);
        // Right side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, off, 0);
        C2D_DrawImageAt({sprite.tex, &tex}, x + rep, y, 0.5f, nullptr, -1.0f, 1.0f);
        // Center
        tex = _select_box(sprite, off, 0, sprite.subtex->width, 0);
        C2D_DrawImageAt({sprite.tex, &tex}, x, y, 0.5f, nullptr, -rep, 1.0f);
    }
    else if (key == ui_sheet_emulated_eventmenu_bar_selected_flipped_vertical_idx)
    {
        u8 off = 4, rep = 174;
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_eventmenu_bar_selected_idx);
        // Left side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, off, 0);
        C2D_DrawImageAt({sprite.tex, &tex}, x, y, 0.5f, nullptr, 1.0f, -1.0f);
        // Center
        tex = _select_box(sprite, off, 0, sprite.subtex->width, 0);
        C2D_DrawImageAt({sprite.tex, &tex}, x + off, y, 0.5f, nullptr, rep, -1.0f);
    }
    else if (key == ui_sheet_emulated_eventmenu_bar_selected_flipped_both_idx)
    {
        u8 off = 4, rep = 174;
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_eventmenu_bar_selected_idx);
        // Right side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, off, 0);
        C2D_DrawImageAt({sprite.tex, &tex}, x + rep, y, 0.5f, nullptr, -1.0f, -1.0f);
        // Center
        tex = _select_box(sprite, off, 0, sprite.subtex->width, 0);
        C2D_DrawImageAt({sprite.tex, &tex}, x, y, 0.5f, nullptr, -rep, -1.0f);
    }
    else if (key == ui_sheet_emulated_eventmenu_bar_unselected_flipped_horizontal_idx)
    {
        u8 off = 4, rep = 174;
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_eventmenu_bar_unselected_idx);
        // Right side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, off, 0);
        C2D_DrawImageAt({sprite.tex, &tex}, x + rep, y, 0.5f, nullptr, -1.0f, 1.0f);
        // Center
        tex = _select_box(sprite, off, 0, sprite.subtex->width, 0);
        C2D_DrawImageAt({sprite.tex, &tex}, x, y, 0.5f, nullptr, -rep, 1.0f);
    }
    else if (key == ui_sheet_emulated_eventmenu_bar_unselected_flipped_vertical_idx)
    {
        u8 off = 4, rep = 174;
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_eventmenu_bar_unselected_idx);
        // Left side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, off, 0);
        C2D_DrawImageAt({sprite.tex, &tex}, x, y, 0.5f, nullptr, 1.0f, -1.0f);
        // Center
        tex = _select_box(sprite, off, 0, sprite.subtex->width, 0);
        C2D_DrawImageAt({sprite.tex, &tex}, x + off, y, 0.5f, nullptr, rep, -1.0f);
    }
    else if (key == ui_sheet_emulated_eventmenu_bar_unselected_flipped_both_idx)
    {
        u8 off = 4, rep = 174;
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_eventmenu_bar_unselected_idx);
        // Right side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, off, 0);
        C2D_DrawImageAt({sprite.tex, &tex}, x + rep, y, 0.5f, nullptr, -1.0f, -1.0f);
        // Center
        tex = _select_box(sprite, off, 0, sprite.subtex->width, 0);
        C2D_DrawImageAt({sprite.tex, &tex}, x, y, 0.5f, nullptr, -rep, -1.0f);
    }
    else if (key == ui_sheet_emulated_storage_box_corner_flipped_horizontal_idx)
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_storage_box_corner_idx), x, y, 0.5f, nullptr, -1.0f, 1.0f);
    }
    else if (key == ui_sheet_emulated_storage_box_corner_flipped_vertical_idx)
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_storage_box_corner_idx), x, y, 0.5f, nullptr, 1.0f, -1.0f);
    }
    else if (key == ui_sheet_emulated_storage_box_corner_flipped_both_idx)
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_storage_box_corner_idx), x, y, 0.5f, nullptr, -1.0f, -1.0f);
    }
    else if (key == ui_sheet_emulated_toggle_green_idx)
    {
        C2D_DrawRectSolid(x, y, 0.5f, 13, 13, C2D_Color32(0x35, 0xC1, 0x3E, 0xFF));
    }
    else if (key == ui_sheet_emulated_toggle_red_idx)
    {
        C2D_DrawRectSolid(x, y, 0.5f, 13, 13, C2D_Color32(0xCC, 0x3F, 0x26, 0xFF));
    }
    else if (key == ui_sheet_emulated_gameselector_bg_idx)
    {
        u8 off = 5, rep = 197;
        /* LEFT */
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_gameselector_bg_left_idx);
        // Top side
        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, 0, off);
        C2D_DrawImageAt({sprite.tex, &tex}, x, y, 0.5f);
        // Bottom side
        C2D_DrawImageAt({sprite.tex, &tex}, x, y + off + rep, 0.5f, nullptr, 1.0f, -1.0f);
        // Center
        tex = _select_box(sprite, 0, off, 0, sprite.subtex->height);
        C2D_DrawImageAt({sprite.tex, &tex}, x, y + off, 0.5f, nullptr, 1.0f, rep);
        x += 5;
        C2D_DrawRectSolid(x, y, 0.5f, 115, rep + 10, C2D_Color32(26, 35, 126, 255));

        /* RIGHT */
        x += 119;
        C2D_DrawRectSolid(x, y, 0.5f, 263, rep + 10, C2D_Color32(26, 35, 126, 255));
        x += 263;
        sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_gameselector_bg_left_idx);
        // Top side
        tex = _select_box(sprite, 0, 0, 0, off);
        C2D_DrawImageAt({sprite.tex, &tex}, x, y, 0.5f, nullptr, -1.0f, 1.0f);
        // Bottom side
        C2D_DrawImageAt({sprite.tex, &tex}, x, y + off + rep, 0.5f, nullptr, -1.0f, -1.0f);
        // Center
        tex = _select_box(sprite, 0, off, 0, sprite.subtex->height);
        C2D_DrawImageAt({sprite.tex, &tex}, x, y + off, 0.5f, nullptr, 1.0f, rep);   
    }
    else if (key == ui_sheet_emulated_button_qr_idx)
    {
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_box_hex_value_idx);

        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, 5, 0);
        // Left
        C2D_DrawImageAt({sprite.tex, &tex}, x, y, 0.5f);
        // Right
        C2D_DrawImageAt({sprite.tex, &tex}, x + 65, y, 0.5f, nullptr, -1.0f, 1.0f);
        // Middle
        tex = _select_box(sprite, 5, 0, 6, 0);
        C2D_DrawImageAt({sprite.tex, &tex}, x + 5, y, 0.5f, nullptr, 60.0f, 1.0f);
    }
    else if (key == ui_sheet_emulated_button_item_idx)
    {
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_button_editor_idx);

        Tex3DS_SubTexture tex = _select_box(sprite, 0, 0, 16, 0);
        // Left
        C2D_DrawImageAt({sprite.tex, &tex}, x, y, 0.5f);
        // Right
        tex = _select_box(sprite, 92, 0, 108, 0);
        C2D_DrawImageAt({sprite.tex, &tex}, x + 168, y, 0.5f);
        // Center
        tex = _select_box(sprite, 16, 0, 17, 0);
        C2D_DrawImageAt({sprite.tex, &tex}, x + 16, y, 0.5f, nullptr, 152.0f, 1.0f);
    }
    // standard case
    else
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, key), x, y, 0.5f);
    }
}

void Gui::generation(PKX* pkm, int x, int y)
{
    switch (pkm->version())
    {
        case 1: // sapphire
		case 2: // ruby
		case 3: // emerald
		case 4: // fire red
		case 5: // leaf green
		case 15: // colosseum/XD
			Gui::sprite(ui_sheet_icon_generation_3_idx, x, y);
            break;
		case 10: // diamond
		case 11: // pearl
		case 12: // platinum
		case 7: // heart gold
		case 8: // soul silver
			Gui::sprite(ui_sheet_icon_generation_4_idx, x, y);
            break;		
		case 20: // white
		case 21: // black
		case 22: // white2
		case 23: // black2
			Gui::sprite(ui_sheet_icon_generation_5_idx, x, y);
            break;
		case 24: // x
		case 25: // y
		case 26: // as
		case 27: // or
			Gui::sprite(ui_sheet_icon_generation_6_idx, x, y);
            break;
		case 30: // sun
		case 31: // moon
		case 32: // us
		case 33: // um
			Gui::sprite(ui_sheet_icon_generation_7_idx, x, y);
            break;
		case 34: // go
			Gui::sprite(ui_sheet_icon_generation_go_idx, x, y);
            break;
		case 35: // rd
		case 36: // gn
		case 37: // bu
		case 38: // yw
		case 39: // gd
		case 40: // sv
			Gui::sprite(ui_sheet_icon_generation_gb_idx, x, y);
            break;
		default:
			break;
    }
}

void Gui::sprite(int key, int x, int y, u32 color)
{
    if (key == ui_sheet_button_editor_idx
       || key == ui_sheet_icon_item_idx
       || key == ui_sheet_pointer_arrow_idx)
    {
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, key);
        C2D_ImageTint tint;
        for (int i = 0; i < 4; i++)
        {
            tint.corners[i] = {color, 1.0f};
        }
        C2D_DrawImageAt(sprite, x, y, 0.5f, &tint);
    }
}

void Gui::pkm(PKX* pokemon, int x, int y, float scale, u32 color, float blend)
{
    if (pokemon == NULL)
    {
        return;
    }
    static C2D_ImageTint tint;
    C2D_PlainImageTint(&tint, color, blend);

    if (pokemon->egg())
    {
        if (pokemon->species() != 490)
        {
            pkm(pokemon->species(), pokemon->alternativeForm(), pokemon->generation(), x, y, scale, color, blend);
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, pkm_spritesheet_0_idx), x + 5 + 34 * (scale - 1), y + 4 + 30 * (scale - 1), 0.5f);
        }
        else
        {
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_490_e_idx), x, y, 0.5f, &tint, scale, scale);
        }
    }
    else
    {
        pkm(pokemon->species(), pokemon->alternativeForm(), pokemon->generation(), x, y, scale, color, blend);
        if (pokemon->heldItem() > 0)
        {
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_icon_item_idx), x + ceil(3 * scale), y + 21 + ceil(30 * (scale - 1)), 0.5f, &tint);
        }
    }

    if (pokemon->shiny())
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_icon_shiny_idx), x, y, 0.5f);
    }
}

void Gui::pkm(int species, int form, Generation generation, int x, int y, float scale, u32 color, float blend)
{
    static C2D_ImageTint tint;
    C2D_PlainImageTint(&tint, color, blend);
    if (species == 490 && form == -1)
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_490_e_idx), x, y, 0.5f, &tint, scale, scale);
    }
    else if (species == 201)
    {
        if (form == 0)
        {
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, species), x, y, 0.5f, &tint, scale, scale);
        }
        else
        {
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_801_1_idx + form), x, y, 0.5f, &tint, scale, scale);
        }
    }
    // For possible hex editor mishaps
    else if (species > 807)
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, pkm_spritesheet_0_idx), x, y, 0.5f, &tint, scale, scale);
    }
    else if (form == 0)
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, species), x, y, 0.5f, &tint, scale, scale);
    }
    // Mimikyu
    else if (species == 778)
    {
        if (form == 1)
        {
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, pkm_spritesheet_778_idx), x, y, 0.5f, &tint, scale, scale);
        }
        else
        {
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_778_2_idx), x, y, 0.5f, &tint, scale, scale);
        }
    }
    // Minior
    else if (species == 774)
    {
        if (form < 7)
        {
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, pkm_spritesheet_774_idx), x, y, 0.5f, &tint, scale, scale);
        }
        else
        {
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_774_7_idx + form - 7), x, y, 0.5f, &tint, scale, scale);
        }
    }
    // Pumpkaboo, Gourgeist, & Genesect
    else if (species == 710 || species == 711 || species == 649)
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, species), x, y, 0.5f, &tint, scale, scale);
    }
    // Pikachu
    else if (species == 25)
    {
        if (generation == Generation::SIX)
        {
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_20_2_idx + form), x, y, 0.5f, &tint, scale, scale);
        }
        else if (form <= 7)
        {
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_25_6_idx + form), x, y, 0.5f, &tint, scale, scale);
        }
        else // LGPE starter
        {
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, species), x, y, 0.5f, &tint, scale, scale);
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_icon_shiny_idx), x + 25 + 34 * (scale-1), y + 5, 0.5f);
        }
    }
    // LGPE starter Eevee
    else if (species == 133)
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, species), x, y, 0.5f, &tint, scale, scale);
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_sheet_icon_shiny_idx), x + 25 + 34 * (scale-1), y + 5, 0.5f);
    }
    // Arceus
    else if (species == 493)
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, species), x, y, 0.5f, &tint, scale, scale);
    }
    // Vivillon chain
    else if (species == 664 || species == 665)
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, species), x, y, 0.5f, &tint, scale, scale);
    }
    else
    {
        int imageOffsetFromBack = 0;

        switch (species)
        {
            case 801:
                imageOffsetFromBack += 3;
            case 800:
                imageOffsetFromBack += 1;
            case 784:
                imageOffsetFromBack += 1;
            case 778:
                imageOffsetFromBack += 1;
            case 777:
                imageOffsetFromBack += 7;
            case 774:
                imageOffsetFromBack += 1;
            case 758:
                imageOffsetFromBack += 1;
            case 754:
                imageOffsetFromBack += 1;
            case 752:
                imageOffsetFromBack += 1;
            case 746:
                imageOffsetFromBack += 2;
            case 745:
                imageOffsetFromBack += 1;
            case 744:
                imageOffsetFromBack += 1;
            case 743:
                imageOffsetFromBack += 3;
            case 741:
                imageOffsetFromBack += 1;
            case 738:
                imageOffsetFromBack += 1;
            case 735:
                imageOffsetFromBack += 1;
            case 720:
                imageOffsetFromBack += 1;
            case 719:
                imageOffsetFromBack += 4;
            case 718:
                imageOffsetFromBack += 1;
            case 681:
                imageOffsetFromBack += 1;
            case 678:
                imageOffsetFromBack += 9;
            case 676:
                imageOffsetFromBack += 4;
            case 671:
                imageOffsetFromBack += 5;
            case 670:
                imageOffsetFromBack += 4;
            case 669:
                imageOffsetFromBack += 19;
            case 666:
                imageOffsetFromBack += 2;
            case 658:
                imageOffsetFromBack += 1;
            case 648:
                imageOffsetFromBack += 1;
            case 647:
                imageOffsetFromBack += 2;
            case 646:
                imageOffsetFromBack += 1;
            case 645:
                imageOffsetFromBack += 1;
            case 642:
                imageOffsetFromBack += 1;
            case 641:
                imageOffsetFromBack += 3;
            case 586:
                imageOffsetFromBack += 3;
            case 585:
                imageOffsetFromBack += 1;
            case 555:
                imageOffsetFromBack += 1;
            case 550:
                imageOffsetFromBack += 1;
            case 531:
                imageOffsetFromBack += 1;
            case 492:
                imageOffsetFromBack += 1;
            case 487:
                imageOffsetFromBack += 5;
            case 479:
                imageOffsetFromBack += 1;
            case 475:
                imageOffsetFromBack += 1;
            case 460:
                imageOffsetFromBack += 1;
            case 448:
                imageOffsetFromBack += 1;
            case 445:
                imageOffsetFromBack += 1;
            case 428:
                imageOffsetFromBack += 1;
            case 423:
                imageOffsetFromBack += 1;
            case 422:
                imageOffsetFromBack += 1;
            case 421:
                imageOffsetFromBack += 2;
            case 413:
                imageOffsetFromBack += 3;
            case 386:
                imageOffsetFromBack += 1;
            case 384:
                imageOffsetFromBack += 1;
            case 383:
                imageOffsetFromBack += 1;
            case 382:
                imageOffsetFromBack += 1;
            case 381:
                imageOffsetFromBack += 1;
            case 380:
                imageOffsetFromBack += 1;
            case 376:
                imageOffsetFromBack += 1;
            case 373:
                imageOffsetFromBack += 1;
            case 362:
                imageOffsetFromBack += 1;
            case 359:
                imageOffsetFromBack += 1;
            case 354:
                imageOffsetFromBack += 3;
            case 351:
                imageOffsetFromBack += 1;
            case 334:
                imageOffsetFromBack += 1;
            case 323:
                imageOffsetFromBack += 1;
            case 319:
                imageOffsetFromBack += 1;
            case 310:
                imageOffsetFromBack += 1;
            case 308:
                imageOffsetFromBack += 1;
            case 306:
                imageOffsetFromBack += 1;
            case 303:
                imageOffsetFromBack += 1;
            case 302:
                imageOffsetFromBack += 1;
            case 282:
                imageOffsetFromBack += 1;
            case 260:
                imageOffsetFromBack += 1;
            case 257:
                imageOffsetFromBack += 1;
            case 254:
                imageOffsetFromBack += 1;
            case 248:
                imageOffsetFromBack += 1;
            case 229:
                imageOffsetFromBack += 1;
            case 214:
                imageOffsetFromBack += 1;
            case 212:
                imageOffsetFromBack += 1;
            case 208:
                imageOffsetFromBack += 1;
            case 181:
                imageOffsetFromBack += 1;
            case 172:
                imageOffsetFromBack += 2;
            case 150:
                imageOffsetFromBack += 1;
            case 142:
                imageOffsetFromBack += 1;
            case 130:
                imageOffsetFromBack += 1;
            case 127:
                imageOffsetFromBack += 1;
            case 115:
                imageOffsetFromBack += 2;
            case 105:
                imageOffsetFromBack += 1;
            case 103:
                imageOffsetFromBack += 1;
            case 94:
                imageOffsetFromBack += 1;
            case 89:
                imageOffsetFromBack += 1;
            case 88:
                imageOffsetFromBack += 1;
            case 80:
                imageOffsetFromBack += 1;
            case 76:
                imageOffsetFromBack += 1;
            case 75:
                imageOffsetFromBack += 1;
            case 74:
                imageOffsetFromBack += 1;
            case 65:
                imageOffsetFromBack += 1;
            case 53:
                imageOffsetFromBack += 1;
            case 52:
                imageOffsetFromBack += 1;
            case 51:
                imageOffsetFromBack += 1;
            case 50:
                imageOffsetFromBack += 1;
            case 38:
                imageOffsetFromBack += 1;
            case 37:
                imageOffsetFromBack += 1;
            case 28:
                imageOffsetFromBack += 1;
            case 27:
                imageOffsetFromBack += 1;
            case 26:
                imageOffsetFromBack += 13;
            case 25:
                imageOffsetFromBack += 2;
            case 20:
                imageOffsetFromBack += 1;
            case 19:
                imageOffsetFromBack += 1;
            case 18:
                imageOffsetFromBack += 1;
            case 15:
                imageOffsetFromBack += 1;
            case 9:
                imageOffsetFromBack += 2;
            case 6:
                imageOffsetFromBack += 1;
        }
        int drawIndex = types_spritesheet_beast_idx + imageOffsetFromBack + form;
        if (drawIndex < types_spritesheet_201_1_idx)
        { 
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_types, drawIndex), x, y, 0.5f, &tint, scale, scale);
        }
        else
        {
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, pkm_spritesheet_0_idx), x, y, 0.5f, &tint, scale, scale);
        }
    }
}

void Gui::ball(size_t index, int x, int y)
{
    if (index < 27)
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_types, index + types_spritesheet_empty_idx), x, y, 0.5f);
    }
    else
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_empty_idx), x, y, 0.5f);
    }
}

static C2D_Image typeImage(Language lang, u8 type)
{
    if (type > 17)
    {
        type = 0;
    }
    switch (lang)
    {
        case Language::ES:
            return C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_es_00_idx + type);
        case Language::DE:
            return C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_de_00_idx + type);
        case Language::FR:
            return C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_fr_00_idx + type);
        case Language::IT:
            return C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_it_00_idx + type);
        case Language::JP:
            return C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_jp_00_idx + type);
        case Language::KO:
            return C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_ko_00_idx + type);
        case Language::TW:
        case Language::ZH:
            return C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_zh_00_idx + type);
        case Language::EN:
        case Language::PT:
        case Language::NL:
        default:
            return C2D_SpriteSheetGetImage(spritesheet_types, types_spritesheet_en_00_idx + type);
    }
}

void Gui::type(Language lang, u8 type, int x, int y)
{
    C2D_DrawImageAt(typeImage(lang, type), x, y, 0.5f);
}

void Gui::setScreen(std::unique_ptr<Screen> screen)
{
    if (!screens.empty() && screens.top()->type() == screen->type())
    {
        if (screen == screens.top())
            return;
    }
    screens.push(std::move(screen));
}

u8 transparencyWaver()
{
    static u8 currentAmount = 255;
    static bool dir = true;
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

bool Gui::showChoiceMessage(const std::string& message, std::optional<std::string> message2)
{
    u32 keys = 0;
    C3D_FrameEnd(0);
    clearTextBufs();
    hidScanInput();
    while (aptMainLoop() && !((keys = hidKeysDown()) & KEY_B))
    {
        hidScanInput();
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(g_renderTargetTop, COLOR_BLACK);
        C2D_TargetClear(g_renderTargetBottom, COLOR_BLACK);

        C2D_SceneBegin(g_renderTargetTop);
        sprite(ui_sheet_part_info_top_idx, 0, 0);
        if (!message2)
        {
            dynamicText(GFX_TOP, 95, message, FONT_SIZE_15, FONT_SIZE_15, C2D_Color32(255, 255, 255, transparencyWaver()));
        }
        else
        {
            u8 transparency = transparencyWaver();
            dynamicText(GFX_TOP, 85, message, FONT_SIZE_15, FONT_SIZE_15, C2D_Color32(255, 255, 255, transparency));
            dynamicText(GFX_TOP, 105, message2.value(), FONT_SIZE_15, FONT_SIZE_15, C2D_Color32(255, 255, 255, transparency));
        }

        dynamicText(GFX_TOP, 130, "Press A to continue, B to cancel.", FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);

        C2D_SceneBegin(g_renderTargetBottom);
        sprite(ui_sheet_part_info_bottom_idx, 0, 0);

        C3D_FrameEnd(0);
        Gui::clearTextBufs();
        if (keys & KEY_A)
        {
            hidScanInput();
            return true;
        }
    }
    hidScanInput();
    return false;
}

void Gui::waitFrame(const std::string& message, std::optional<std::string> message2)
{
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(g_renderTargetTop, COLOR_BLACK);
    C2D_TargetClear(g_renderTargetBottom, COLOR_BLACK);

    C2D_SceneBegin(g_renderTargetTop);
    sprite(ui_sheet_part_info_top_idx, 0, 0);
    if (!message2)
    {
        dynamicText(GFX_TOP, 95, message, FONT_SIZE_15, FONT_SIZE_15, COLOR_WHITE);
    }
    else
    {
        dynamicText(GFX_TOP, 85, message, FONT_SIZE_15, FONT_SIZE_15, COLOR_WHITE);
        dynamicText(GFX_TOP, 105, message2.value(), FONT_SIZE_15, FONT_SIZE_15, COLOR_WHITE);
    }

    dynamicText(GFX_TOP, 130, "Please wait.", FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);

    C2D_SceneBegin(g_renderTargetBottom);
    sprite(ui_sheet_part_info_bottom_idx, 0, 0);

    C3D_FrameEnd(0);
    Gui::clearTextBufs();
}

void Gui::warn(const std::string& message, std::optional<std::string> message2)
{
    u32 keys = 0;
    C3D_FrameEnd(0);
    clearTextBufs();
    hidScanInput();
    while (aptMainLoop() && !((keys = hidKeysDown()) & KEY_A))
    {
        hidScanInput();
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(g_renderTargetTop, COLOR_BLACK);
        C2D_TargetClear(g_renderTargetBottom, COLOR_BLACK);

        C2D_SceneBegin(g_renderTargetTop);
        sprite(ui_sheet_part_info_top_idx, 0, 0);
        if (!message2)
        {
            dynamicText(GFX_TOP, 95, message, FONT_SIZE_15, FONT_SIZE_15, C2D_Color32(255, 255, 255, transparencyWaver()));
        }
        else
        {
            u8 transparency = transparencyWaver();
            dynamicText(GFX_TOP, 85, message, FONT_SIZE_15, FONT_SIZE_15, C2D_Color32(255, 255, 255, transparency));
            dynamicText(GFX_TOP, 105, message2.value(), FONT_SIZE_15, FONT_SIZE_15, C2D_Color32(255, 255, 255, transparency));
        }

        dynamicText(GFX_TOP, 130, "Press A to continue.", FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);

        C2D_SceneBegin(g_renderTargetBottom);
        sprite(ui_sheet_part_info_bottom_idx, 0, 0);

        C3D_FrameEnd(0);
        Gui::clearTextBufs();
    }
    hidScanInput();
}

void Gui::screenBack()
{
    screens.pop();
}

void Gui::setNextKeyboardFunc(std::function<void()> func)
{
    keyboardFunc = func;
}

void Gui::showRestoreProgress(u32 partial, u32 total)
{
    clearTextBufs();
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(g_renderTargetTop, COLOR_BLACK);
    C2D_TargetClear(g_renderTargetBottom, COLOR_BLACK);
    C2D_SceneBegin(g_renderTargetTop);
    sprite(ui_sheet_part_info_top_idx, 0, 0);
    staticText(GFX_TOP, 95, "Saving...", FONT_SIZE_15, FONT_SIZE_15, COLOR_WHITE);
    dynamicText(GFX_TOP, 130, StringUtils::format("%lu KB of %lu KB...", partial, total), FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
    C2D_SceneBegin(g_renderTargetBottom);
    sprite(ui_sheet_part_info_bottom_idx, 0, 0);
    C3D_FrameEnd(0);
}

void Gui::showResizeStorage()
{
    clearTextBufs();
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(g_renderTargetTop, COLOR_BLACK);
    C2D_TargetClear(g_renderTargetBottom, COLOR_BLACK);
    C2D_SceneBegin(g_renderTargetTop);
    sprite(ui_sheet_part_info_top_idx, 0, 0);
    staticText(GFX_TOP, 95, "Resizing Storage...", FONT_SIZE_15, FONT_SIZE_15, COLOR_WHITE);
    // dynamicText(GFX_TOP, 130, StringUtils::format("%lu KB of %lu KB...", partial, total), FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
    C2D_SceneBegin(g_renderTargetBottom);
    sprite(ui_sheet_part_info_bottom_idx, 0, 0);
    C3D_FrameEnd(0);
}