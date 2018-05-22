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

std::mt19937 g_randomNumbers;

C3D_RenderTarget* g_renderTargetTop;
C3D_RenderTarget* g_renderTargetBottom;
static C2D_SpriteSheet spritesheet_ui;
static C2D_SpriteSheet spritesheet_pkm;
static C2D_SpriteSheet spritesheet_types;
static C2D_Sprite bgCubes[7];
static C2D_Image bgBoxes;
static C2D_TextBuf dynamicBuf;
static C2D_TextBuf staticBuf;
static std::unordered_map<std::string, C2D_Text> staticMap;
C2D_TextBuf g_widthBuf;

static Tex3DS_SubTexture _select_box(const C2D_Image& image, int x, int y, int dx, int dy)
{
    Tex3DS_SubTexture tex = *image.subtex;
    if (x != dx)
    {
        int deltaX = dx - x;
        float texRL = tex.left - tex.right;
        tex.left = tex.left - (float) texRL / tex.width * x;
        tex.right = tex.left - (float) texRL / tex.width * deltaX;
        tex.width = deltaX;
    }
    if (y != dy)
    {
        float texTB = tex.top - tex.bottom;
        int deltaY = dy - y;
        tex.top = tex.top - (float) texTB / tex.height * y;
        tex.bottom = tex.top - (float) texTB / tex.height * deltaY;
        tex.height = deltaY;
    }
    return tex;
}

C2D_Image Gui::TWLIcon(void)
{
    return C2D_SpriteSheetGetImage(spritesheet_ui, ui_spritesheet_res_twlcart_idx);
}

void Gui::backgroundBottom(void)
{
    C2D_DrawRectSolid(0, 0, 0.5f, 320, 240, COLOR_PALEBLUE);
    C2D_DrawRectSolid(0, 219, 0.5f, 320, 21, COLOR_MENUBLUE);
}

void Gui::backgroundTop(void)
{
    C2D_DrawRectSolid(0, 0, 0.5f, 400, 240, COLOR_MENUBLUE);
    C2D_DrawRectSolid(0, 0, 0.5f, 400, 25, COLOR_HIGHBLUE);
}

void Gui::backgroundAnimated(gfxScreen_t screen)
{
    static bool firstRun = true;
    static std::vector<std::pair<C2D_Sprite, float>> cubes = {
        std::make_pair(bgCubes[0], randomRotation()),
        std::make_pair(bgCubes[1], randomRotation()),
        std::make_pair(bgCubes[2], randomRotation()),
        std::make_pair(bgCubes[3], randomRotation()),
        std::make_pair(bgCubes[4], randomRotation()),
        std::make_pair(bgCubes[5], randomRotation()),
        std::make_pair(bgCubes[6], randomRotation())
    };
    static int boxesX = 0;
    static const int maxCubes = 25;
    static int cooldown = 30;

    if (firstRun)
    {
        for (std::vector<std::pair<C2D_Sprite, float>>::iterator i = cubes.begin(); i != cubes.end(); i++)
        {
            i->first.params.pos.x = -1 * (int)(g_randomNumbers() % 100);
            i->first.params.pos.y = g_randomNumbers() % 200;
        }
        firstRun = false;
    }

    C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_spritesheet_res_anim_background_idx), 0, 0, 0.5f);
    
    int maxrange = screen == GFX_TOP ? 400 : 320;

    Tex3DS_SubTexture boxesPart = _select_box(bgBoxes, maxrange - boxesX / 2, 0, 800, 240);
    C2D_DrawImageAt({bgBoxes.tex, &boxesPart}, 0, 0, 0.5f);
    
    if (g_randomNumbers() % 100 < 5 && cubes.size() < maxCubes && cooldown < 0)
    {
        cubes.push_back(std::make_pair(bgCubes[g_randomNumbers() % 7], randomRotation()));
        cubes[cubes.size() - 1].first.params.pos.x = -1 * (int)((g_randomNumbers() % 50) + 50);
        cubes[cubes.size() - 1].first.params.pos.y = g_randomNumbers() % 200;
        cubes[cubes.size() - 1].first.params.depth = 0.5f;
        cubes[cubes.size() - 1].first.params.center.x = cubes[cubes.size() - 1].first.params.pos.w / 2.0f;
        cubes[cubes.size() - 1].first.params.center.y = cubes[cubes.size() - 1].first.params.pos.h / 2.0f;
        cooldown = 30;
    }

    for (std::vector<std::pair<C2D_Sprite, float>>::iterator i = cubes.begin(); i != cubes.end(); i++)
    {
        if (i->first.params.pos.x >= maxrange + 100)
        {
            cubes.erase(i);
            if (g_randomNumbers() % 100 < 50 && cubes.size() < maxCubes)
            {
                cubes.push_back(std::make_pair(bgCubes[g_randomNumbers() % 7], randomRotation()));
                cubes[cubes.size() - 1].first.params.pos.x = -1 * (int)((g_randomNumbers() % 50) + 50);
                cubes[cubes.size() - 1].first.params.pos.y = g_randomNumbers() % 200;
                cubes[cubes.size() - 1].first.params.depth = 0.5f;
                cubes[cubes.size() - 1].first.params.center.x = cubes[cubes.size() - 1].first.params.pos.w / 2.0f;
                cubes[cubes.size() - 1].first.params.center.y = cubes[cubes.size() - 1].first.params.pos.h / 2.0f;
            }
        }
        else
        {
            C2D_SpriteMove(&i->first, 1, 0);
            C2D_SpriteRotateDegrees(&i->first, i->second);
            C2D_DrawSprite(&i->first);
        }
    }

    cooldown--;
    boxesX = boxesX > (maxrange * 2) ? 0 : boxesX + 1;
}

void Gui::clearTextBufs(void)
{
    C2D_TextBufClear(dynamicBuf);
    C2D_TextBufClear(g_widthBuf);
}

void Gui::dynamicText(const std::string& str, int x, int y, float scaleX, float scaleY, u32 color)
{
    C2D_Text text;
    C2D_TextParse(&text, dynamicBuf, str.c_str());
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, x, y, 0.5f, scaleX, scaleY, color);
}

void Gui::dynamicText(const std::string& str, int x, int y, float scaleX, float scaleY, u32 color, float maxWidth)
{
    std::string print = str;
    int nextSpace = -1, lastSplit = 0, position = 0, length = print.length();
    C2D_Text text;

    while (position < length)
    {
        nextSpace = print.find(' ', position);
        if (textWidth(print.substr(lastSplit + 1, (nextSpace == std::string::npos ? nextSpace : nextSpace - (lastSplit + 1))), scaleX) > maxWidth)
        {
            // Can't figure out the exact reason this doesn
            // if (lastSplit == position - 1)
            // {
            //     // split without regard for words
            //     for (int i = nextSpace; i > position; i--)
            //     {
            //         if (textWidth(print.substr(position, i - position), scaleX) < maxWidth)
            //         {
            //             i++;
            //             print.insert(i, "\n");
            //             lastSplit = i;
            //             if (textWidth(print.substr(i + 1, nextSpace), scaleX) > maxWidth)
            //             {
            //                 position = i + 1;
            //                 i = nextSpace;
            //             }
            //             else
            //                 break;
            //         }
            //     }
            // }
            // else
            // {
                print[position - 1] = '\n';
                lastSplit = nextSpace - 1;
            // }
        }
        if (nextSpace == std::string::npos)
            break;
        position = nextSpace + 1;
    }
    // float defaultWidth = scaleX * fontGetInfo()->defaultWidth.charWidth;
    // float width = defaultWidth * print.length();
    
    // if (width > maxWidth)
    // {
    //     size_t maxChars = (size_t)(maxWidth / defaultWidth);
    //     for (size_t i = 1; i * maxChars < str.length(); i++)
    //     {
    //         print.insert(i * maxChars + i - 1, "\n");
    //     }
    // }
    C2D_TextParse(&text, dynamicBuf, print.c_str());
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, x, y, 0.5f, scaleX, scaleY, color);
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
    std::unordered_map<std::string, C2D_Text>::iterator index = staticMap.find(strKey);
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

void Gui::staticText(const std::string& strKey, int x, int y, float scaleX, float scaleY, u32 color)
{
    C2D_Text text = cacheStaticText(strKey);
    C2D_DrawText(&text, C2D_WithColor, x, y, 0.5f, scaleX, scaleY, color);
}

void Gui::staticText(const std::string& strKey, int x, int y, float scaleX, float scaleY, u32 color, float maxWidth)
{
    std::string print = strKey;
    C2D_Text text;
    std::unordered_map<std::string, C2D_Text>::iterator index = staticMap.find(strKey);
    if (index == staticMap.end())
    {
        float defaultWidth = scaleX * fontGetInfo()->defaultWidth.charWidth;
        float width = defaultWidth * print.length();
        if (width > maxWidth)
        {
            size_t maxChars = (size_t)(maxWidth / defaultWidth);
            for (size_t i = 1; i * maxChars < strKey.length(); i++)
            {
                print.insert(i * maxChars + i - 1, "\n");
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
    float right = tex.right;
    tex.right = tex.left;
    tex.left = right;
    C2D_DrawImageAt({sprite.tex, &tex}, x + off + rep, y, 0.5f);
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

// TODO: this is just a test, remove
static void drawMenuTop()
{
    static const std::string version = StringUtils::format("v%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);
    Gui::clearTextBufs();
    Gui::backgroundTop();
    Gui::staticText(GFX_TOP, 4, "PKSM", FONT_SIZE_14, FONT_SIZE_14, COLOR_BLUE);
    C2D_ImageTint tint;
    for(int i = 0; i < 4; i++)
    {
        tint.corners[i] = {COLOR_YELLOW, 1.0f};
    }
    float width = textWidth(version, FONT_SIZE_9);
    Gui::staticText(version, 398 - width, 229, FONT_SIZE_9, FONT_SIZE_9, COLOR_LIGHTBLUE);
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

    dynamicBuf = C2D_TextBufNew(1024);
    g_widthBuf = C2D_TextBufNew(1024);
    staticBuf = C2D_TextBufNew(1024);

    spritesheet_ui = C2D_SpriteSheetLoad("romfs:/gfx/ui_spritesheet.t3x");
    spritesheet_pkm = C2D_SpriteSheetLoad("romfs:/gfx/pkm_spritesheet.t3x");
    spritesheet_types = C2D_SpriteSheetLoad("romfs:/gfx/types_spritesheet.t3x");

    bgBoxes = C2D_SpriteSheetGetImage(spritesheet_ui, ui_spritesheet_res_anim_squares_idx);

    C2D_SpriteFromSheet(&bgCubes[0], spritesheet_ui, ui_spritesheet_res_anim_cubes_1_idx);
    C2D_SpriteFromSheet(&bgCubes[1], spritesheet_ui, ui_spritesheet_res_anim_cubes_2_idx);
    C2D_SpriteFromSheet(&bgCubes[2], spritesheet_ui, ui_spritesheet_res_anim_cubes_3_idx);
    C2D_SpriteFromSheet(&bgCubes[3], spritesheet_ui, ui_spritesheet_res_anim_cubes_4_idx);
    C2D_SpriteFromSheet(&bgCubes[4], spritesheet_ui, ui_spritesheet_res_anim_cubes_5_idx);
    C2D_SpriteFromSheet(&bgCubes[5], spritesheet_ui, ui_spritesheet_res_anim_cubes_6_idx);
    C2D_SpriteFromSheet(&bgCubes[6], spritesheet_ui, ui_spritesheet_res_anim_cubes_7_idx);

    //test
    // while (aptMainLoop() && !(hidKeysDown() & KEY_START))
    // {
    //     hidScanInput();

    //     C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    //     C2D_TargetClear(g_renderTargetTop, C2D_Color32(0, 0, 0, 0));
    //     C2D_TargetClear(g_renderTargetBottom, C2D_Color32(0x23, 0x45, 0x89, 0));
    //     C2D_SceneBegin(g_renderTargetTop);
    //     //backgroundAnimated(GFX_TOP);
    //     drawMenuTop();
    //     //sprite(ui_spritesheet_res_info_top_idx, 0, 0);
    //     C2D_SceneBegin(g_renderTargetBottom);
    //     backgroundAnimated(GFX_BOTTOM);
    //     C3D_FrameEnd(0);
    //     C2D_TextBufClear(g_widthBuf);
    // }
    
    return 0;
}

void Gui::exit(void)
{
    C2D_SpriteSheetFree(spritesheet_ui);
    C2D_SpriteSheetFree(spritesheet_pkm);
    C2D_SpriteSheetFree(spritesheet_types);
    C2D_TextBufDelete(g_widthBuf);
    C2D_TextBufDelete(dynamicBuf);
    C2D_TextBufDelete(staticBuf);
    C2D_Fini();
    C3D_Fini();
    gfxExit();
}

void Gui::sprite(int key, int x, int y)
{
    if (key == ui_spritesheet_res_bar_editor_idx)
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, key), x, y, 0.5f, nullptr, 320.0f, 1.0f);
    }
    else if (key == ui_spritesheet_res_bar_event_bottom_idx)
    {
        _draw_mirror_scale(key, x, y, 3, 184);
    }
    else if (key == ui_spritesheet_res_bar_event_top_normal_idx)
    {
        _draw_mirror_scale(key, x, y, 3, 172);
    }
    else if (key == ui_spritesheet_res_bar_event_top_selected_idx)
    {
        _draw_mirror_scale(key, x, y, 3, 172);
    }
    else if (key == ui_spritesheet_res_bar_selected_idx)
    {
        _draw_mirror_scale(key, x, y, 8, 302);
    }
    else if (key == ui_spritesheet_res_box_no_shadow_idx)
    {
        _draw_mirror_scale(key, x, y, 2, 32);
    }
    else if (key == ui_spritesheet_res_button_menu_idx)
    {
        _draw_mirror_scale(key, x, y, 3, 134);
    }
    else if (key == ui_spritesheet_res_editor_20x2_idx)
    {
        _draw_repeat(key, x, y, 20, 1);
    }
    else if (key == ui_spritesheet_res_info_bottom_idx || key == ui_spritesheet_res_info_top_idx)
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
    else if (key == ui_spritesheet_res_mtx_4x4_idx)
    {
        _draw_repeat(key, x, y, 4, 4);
    }
    else if (key == ui_spritesheet_res_mtx_5x6_idx)
    {
        _draw_repeat(key, x, y, 5, 6);
    }
    else if (key == ui_spritesheet_res_mtx_5x8_idx)
    {
        _draw_repeat(key, x, y, 5, 8);
    }
    else if (key == ui_spritesheet_res_mtx_15x16_idx)
    {
        _draw_repeat(key, x, y, 15, 16);
        C2D_DrawRectSolid(0, 225, 0.5f, 400, 15, COLOR_WHITE);
        //_draw_rectangle(0, 225, 400, 15, COLOR_WHITE);
    }
    else if (key == ui_spritesheet_res_wondercard_top_idx)
    {
        // top bar
        C2D_Image image = C2D_SpriteSheetGetImage(spritesheet_ui, key);
        Tex3DS_SubTexture tex = _select_box(image, 0, 0, 373, 20);
        C2D_DrawImageAt({image.tex, &tex}, x, y, 0.5f);
        // body
        tex = _select_box(image, 0, 20, 220, image.subtex->height);
        C2D_DrawImageAt({image.tex, &tex}, x, y + 22, 0.5f);
        // moves
        tex = _select_box(image, 221, 23, 373, 128);
        C2D_DrawImageAt({image.tex, &tex}, x + 248, y + 135, 0.5f);
    }
    
    //emulated
    else if (key == ui_spritesheet_res_emulated_button_arrow_right_idx)
    {
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_spritesheet_res_button_arrow_idx);
        C2D_DrawImageAt(sprite, x, y, 0.5f, NULL, -1.0f, 1.0f);
    }
    else if (key == ui_spritesheet_res_emulated_selector_menu_left_idx)
    {
        C2D_Image sprite = C2D_SpriteSheetGetImage(spritesheet_ui, ui_spritesheet_res_selector_menu_idx);
        C2D_DrawImageAt(sprite, x, y, 0.5f, NULL, -1.0f, 1.0f);
    }
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
			Gui::sprite(ui_spritesheet_res_generation_3_idx, x, y);
            break;
		case 10: // diamond
		case 11: // pearl
		case 12: // platinum
		case 7: // heart gold
		case 8: // soul silver
			Gui::sprite(ui_spritesheet_res_generation_4_idx, x, y);
            break;		
		case 20: // white
		case 21: // black
		case 22: // white2
		case 23: // black2
			Gui::sprite(ui_spritesheet_res_generation_5_idx, x, y);
            break;
		case 24: // x
		case 25: // y
		case 26: // as
		case 27: // or
			Gui::sprite(ui_spritesheet_res_generation_6_idx, x, y);
            break;
		case 30: // sun
		case 31: // moon
		case 32: // us
		case 33: // um
			Gui::sprite(ui_spritesheet_res_generation_7_idx, x, y);
            break;
		case 34: // go
			Gui::sprite(ui_spritesheet_res_generation_go_idx, x, y);
            break;
		case 35: // rd
		case 36: // gn
		case 37: // bu
		case 38: // yw
		case 39: // gd
		case 40: // sv
			Gui::sprite(ui_spritesheet_res_generation_gb_idx, x, y);
            break;
		default:
			break;
    }
}

void Gui::sprite(int key, int x, int y, u32 color)
{
    if (key == ui_spritesheet_res_button_entry_idx
       || key == ui_spritesheet_res_item_idx
       || key == ui_spritesheet_res_selector_box_idx)
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

void Gui::pkm(PKX* pokemon, int x, int y, u32 color)
{
    if (pokemon == NULL)
    {
        return;
    }
    static C2D_ImageTint tint;
    for (int i = 0; i < 4; i++)
    {
        tint.corners[i] = {color, 1.0f};
    }

    if (pokemon->egg())
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, pkm_spritesheet_0_idx), x, y, 0.5f, &tint);
    }
    else
    {
        if (pokemon->alternativeForm() == 0 || pokemon->species() != 201)
        {
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, pokemon->formSpecies()), x, y, 0.5f, &tint);
        }
        else
        {
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, pkm_spritesheet_744_1_idx + pokemon->alternativeForm()), x, y, 0.5f, &tint);
        }
        if (pokemon->heldItem() > 0)
        {
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_spritesheet_res_item_idx), x + 3, y + 21, 0.5f, &tint);
        }
    }
}

void Gui::pkm(int formSpecies, int x, int y, u32 color)
{
    static C2D_ImageTint tint;
    for (int i = 0; i < 4; i++)
    {
        tint.corners[i] = {color, 1.0f};
    }
    C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, formSpecies), x, y, 0.5f, &tint);
}

void Gui::ball(size_t index, int x, int y)
{
    if (index < 27)
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, index + pkm_spritesheet_empty_idx), x, y, 0.5f);
    }
    else
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_pkm, pkm_spritesheet_empty_idx), x, y, 0.5f);
    }
}

// TODO:remove
/*
void Gui::pkmInfoViewer(PKX* pkm)
{
    C2D_SceneBegin(g_renderTargetTop);
    std::vector<std::string> labels = {"Localize me"}; // TODO: entries & values; i18n::localize
    int y_desc = 29;
    
    backgroundAnimated(GFX_TOP);
    sprite(ui_spritesheet_res_info_top_idx, 0, 2);

    sprite(pkm->move(0) ? ui_spritesheet_res_bar_move_idx : ui_spritesheet_res_bar_no_move_idx, 252, 155);
    sprite(pkm->move(1) ? ui_spritesheet_res_bar_move_idx : ui_spritesheet_res_bar_no_move_idx, 252, 155);
    sprite(pkm->move(2) ? ui_spritesheet_res_bar_move_idx : ui_spritesheet_res_bar_no_move_idx, 252, 155);
    sprite(pkm->move(3) ? ui_spritesheet_res_bar_move_idx : ui_spritesheet_res_bar_no_move_idx, 252, 155);
    
    dynamicText("Moves", 251, 138, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLUE); // TODO: i18n::localize
    for (int i = 0; i < 10; i++)
    {
        if (i == 8 && pkm->egg())
        {
            dynamicText("Egg Cycle", 2, y_desc, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLUE); // TODO: i18n::localize
        }
        else
        {
            dynamicText(labels[i], 2, y_desc, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLUE);
        }
        y_desc += 20;
        if (i == 2) y_desc += 5;
        if (i == 5) y_desc += 6;
    }

    y_desc = 8;
    for (int i = 10; i < 16; i++)
    {
        dynamicText(labels[i], 225, y_desc, FONT_SIZE_12, FONT_SIZE_12, COLOR_LIGHTBLUE);
        y_desc += 20;
    }

    if (pkm != NULL && pkm->species() > 0)
    {
        ball(pkm->ball(), -2, -5);
        dynamicText(i18n::species(pkm->language(), pkm->species()), 30, 6, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        
        printGen(pkm, 134, 8);

        if (pkm->gender() == 0)
        {
            sprite(ui_spritesheet_res_male_idx, 146, 7);
        }
        else if (pkm->gender() == 1)
        {
            sprite(ui_spritesheet_res_female_idx, 148, 7);
        }

        std::string toDraw;

        toDraw = "Level " + std::to_string(pkm->level()); // TODO: i18n::localize
        dynamicText(toDraw, 160, 6, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);

        toDraw = pkm->nickname();
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 29, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        toDraw = pkm->otName();
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 49, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);

        toDraw = pkm->pkrsStrain() ? "Yes" : "No"; // TODO: i18n::localize
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 69, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        toDraw = i18n::nature(pkm->language(), pkm->nature());
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 94, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        toDraw = i18n::ability(pkm->language(), pkm->ability());
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 114, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        toDraw = i18n::item(pkm->language(), pkm->heldItem());
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 134, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);

        if (pkm->shiny())
        {
            sprite(ui_spritesheet_res_shiny_idx, 205, 9);
        }

        if (pkm->egg())
        {
            toDraw = std::to_string(pkm->otFriendship());
        }
        else
        {
            toDraw = std::to_string(pkm->currentFriendship()) + " / " + std::to_string(pkm->otFriendship());
        }
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 200, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);

        toDraw = std::to_string(pkm->PSV()) + " / " + std::to_string(pkm->TSV());
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 160, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        toDraw = std::to_string(pkm->TID()) + " / " + std::to_string(pkm->SID());
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 180, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);

        toDraw = i18n::hp(pkm->language(), pkm->hpType());
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 220, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        
        int max = textWidth("252", FONT_SIZE_12);
        int y_moves = 159;
        for (int i = 0; i < 4; i++)
        {
            if (pkm->move(i))
            {
                toDraw = i18n::move(pkm->language(), pkm->move(i));
                dynamicText(toDraw, 396 - textWidth(toDraw, FONT_SIZE_12), y_moves, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
            }
            y_moves += 21;
        }

        for (int i = 0; i < 6; i++)
        {
            toDraw = std::to_string(pkm->iv(i));
            dynamicText(toDraw, 289 + (max - textWidth(toDraw, FONT_SIZE_12)) / 2, 8 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
            toDraw = std::to_string(pkm->ev(i));
            dynamicText(toDraw, 328 + (max - textWidth(toDraw, FONT_SIZE_12)) / 2, 8 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
            toDraw = std::to_string(pkm->stat(i));
            dynamicText(toDraw, 369 + (max - textWidth(toDraw, FONT_SIZE_12)) / 2, 8 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        }
    }
}
*/

/*void Gui::drawBox(int box, const Sav* save)
{
    C2D_SceneBegin(g_renderTargetBottom);
    C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet_ui, ui_spritesheet_res_box_bottom_idx), 0, 0, 0.5f);
    sprite(ui_spritesheet_res_bar_editor_idx, 0, 210);
    sprite(ui_spritesheet_res_button_arrow_idx, 7, 17);
    sprite(ui_spritesheet_res_emulated_button_arrow_right_idx, 185, 17);
    sprite(ui_spritesheet_res_button_swap_idx, 242, 5);
    sprite(ui_spritesheet_res_button_back_idx, 283, 211);
    float textWidth;
    std::string text;
    // buttons
    sprite(ui_spritesheet_res_button_entry_idx, 208, 43);
    textWidth = _text_width(text = "button");
    _draw_text(208.0f + ((109.0f - textWidth) / 2.0f), 49, text, COLOR_BLACK);

    sprite(ui_spritesheet_res_button_entry_idx, 208, 72);
    textWidth = _text_width(text = "Button");
    _draw_text(208.0f + ((109.0f - textWidth) / 2.0f), 78, text, COLOR_BLACK);

    sprite(ui_spritesheet_res_button_entry_idx, 208, 101);
    textWidth = _text_width(text = "BUTTON");
    _draw_text(208.0f + ((109.0f - textWidth) / 2.0f), 107, text, COLOR_BLACK);

    sprite(ui_spritesheet_res_button_entry_idx, 208, 130);
    textWidth = _text_width(text = "butón");
    _draw_text(208.0f + ((109.0f - textWidth) / 2.0f), 136, text, COLOR_BLACK);

    sprite(ui_spritesheet_res_button_wireless_idx, 240, 211);

    int y = 45;
    for (int row = 0; row < 5; row++)
    {
        int x = 44;
        for (int column = 0; column < 6; column++)
        {
            int index = row * 6 + column;
            pkm(save->pkm(box, index).get(), x, y);
            x += 34;
        }
        y += 30;
    }
}*/

/*void Gui::eventList(WCX* database[], int currentEntry, int page) // This database could be an STL class, too
{
    C2D_SceneBegin(g_renderTargetTop);
    printMenuTop();
    // do title text
    int y = 41;
    std::string pages = std::to_string(page + 1) + "/" + "WHEE"; // + mysteryGiftTotal() / 10;
    C2D_TextBufClear(dynamicBuf);
    for (int i = 0; i < 5; i++)
    {
        if (i == currentEntry)
        {
            sprite(ui_spritesheet_res_bar_event_top_selected_idx, 18, y);
        }
        else
        {
            sprite(ui_spritesheet_res_bar_event_top_normal_idx, 18, y);
        }

        if (database[page * 10 + i]->pokemon()) // Or just use the value?
        {
            if (database[page * 10 + i]->alternativeForm() == 0 || database[page * 10 + i]->species() != 201)
            {
                pkm(database[page * 10 + i]->formSpecies(), 20, y - ((i == currentEntry) ? movementOffsetSlow(2) : 0));
            }
            else
            {
                pkm(pkm_spritesheet_744_1_idx + database[page * 10 + i]->alternativeForm(), 20, y - ((i == currentEntry) ? movementOffsetSlow(2) : 0));
            }
        }

        float textWidth = _text_width(database[page * 10 + i]->title(), FONT_SIZE_9);
        if (textWidth < 140.0f)
        {
            _draw_text(54, y + 12, database[page * 10 + i]->title(), (i == currentEntry) ? COLOR_HIGHBLUE : COLOR_YELLOW, FONT_SIZE_9, FONT_SIZE_9);
        }
        else
        {
            _draw_text_wrap(54, y + 3, database[page * 10 + i]->title(), 140.0f, (i == currentEntry) ? COLOR_HIGHBLUE : COLOR_YELLOW, FONT_SIZE_9, FONT_SIZE_9);
        }
        y += 37;
    }

    y = 41;
    for (int i = 5; i < 10; i++)
    {
        if (i == currentEntry)
        {
            sprite(ui_spritesheet_res_bar_event_top_selected_idx, 200, y);
        }
        else
        {
            sprite(ui_spritesheet_res_bar_event_top_normal_idx, 200, y);
        }

        if (database[page * 10 + i]->pokemon()) // Or just use the value?
        {
            if (database[page * 10 + i]->alternativeForm() == 0 || database[page * 10 + i]->species() != 201)
            {
                pkm(database[page * 10 + i]->formSpecies(), 20, y - ((i == currentEntry) ? movementOffsetSlow(2) : 0));
            }
            else
            {
                pkm(pkm_spritesheet_744_1_idx + database[page * 10 + i]->alternativeForm(), 20, y - ((i == currentEntry) ? movementOffsetSlow(2) : 0));
            }
        }

        float textWidth = _text_width(database[page * 10 + i]->title(), FONT_SIZE_9);
        if (textWidth < 140.0f)
        {
            _draw_text(235, y + 14, database[page * 10 + i]->title(), (i == currentEntry) ? COLOR_HIGHBLUE : COLOR_YELLOW, FONT_SIZE_9, FONT_SIZE_9);
        }
        else
        {
            _draw_text_wrap(235, y + 3, database[page * 10 + i]->title(), 140.0f, (i == currentEntry) ? COLOR_HIGHBLUE : COLOR_YELLOW, FONT_SIZE_9, FONT_SIZE_9);
        }
        y += 37;
    }
    
    C2D_SceneBegin(g_renderTargetBottom);
    printMenuBottom();
    sprite(ui_spritesheet_res_bar_event_bottom_idx, 65, 45);
    C2D_DrawText(&LButton, C2D_WithColor, 83, 52, 0.5f, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
    C2D_DrawText(&RButton, C2D_WithColor, 221, 52, 0.5f, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
    _draw_text_center(g_renderTargetBottom, 52, pages);
}*/

/*
void Gui::menu(Language lang)
{
    C2D_SceneBegin(g_renderTargetTop);

    drawMenuTop();

    C2D_SceneBegin(g_renderTargetBottom);
    backgroundBottom();
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            sprite(ui_spritesheet_res_button_menu_idx, 15 + j*150, 20 + i*63);
            std::string toDraw = "Menu Item " + std::to_string(i*2 + j); // TODO: i18n::localize
            dynamicText(toDraw, 15 + j*150 + 52 + (84 - textWidth(toDraw, FONT_SIZE_15)) / 2, 17 + i*63 + 19, FONT_SIZE_15, FONT_SIZE_15, COLOR_LIGHTBLUE);
            switch (i*2 + j)
            {
                case 0:
                    sprite(ui_spritesheet_res_icon_storage_idx, 25, 27);
                    break;
                case 1:
                    sprite(ui_spritesheet_res_icon_editor_idx, 175, 28);
                    break;
                case 2:
                    sprite(ui_spritesheet_res_icon_events_idx, 25, 93);
                    break;
                case 3:
                    sprite(ui_spritesheet_res_icon_scripts_idx, 175, 91);
                    break;
                case 4:
                    sprite(ui_spritesheet_res_icon_settings_idx, 25, 157);
                    break;
                case 5:
                    sprite(ui_spritesheet_res_icon_credits_idx, 175, 160);
                    break;
            }
        }
    }
    dynamicText(GFX_BOTTOM, 225, "Bottom Indications", FONT_SIZE_9, FONT_SIZE_9, COLOR_LIGHTBLUE); // TODO: i18n::localize
}
*/

C2D_Image Gui::type(Language lang, u8 type)
{
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