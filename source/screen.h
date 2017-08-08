#ifndef SCREEN_H
#define SCREEN_H

#include <3ds.h>
#include <citro3d.h>

#include <errno.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#pragma once

#define TOP_SCREEN_WIDTH			400
#define BOTTOM_SCREEN_WIDTH			320

#define SCREEN_HEIGHT				240

#define MAX_TEXTURES 				1024

#define RGBA8(r, g, b, a) 			((((a)&0xFF)<<24) | (((b)&0xFF)<<16) | (((g)&0xFF)<<8) | (((r)&0xFF)<<0))

#define CLEAR_COLOR 				0x000000FF

void screen_reset_text_vtx_array_pos();
void screen_init(void);
void screen_exit(void);
void screen_clear(gfxScreen_t screen, u32 color);
void screen_set_base_alpha(u8 alpha);
u32 screen_allocate_free_texture(void);
void screen_load_texture_untiled(u32 id, void* data, u32 size, u32 width, u32 height, GPU_TEXCOLOR format, bool linearFilter);
void screen_load_texture_png(u32 id, const char* path, bool linearFilter);
void screen_load_texture_tiled(u32 id, void* data, u32 size, u32 width, u32 height, GPU_TEXCOLOR format, bool linearFilter);
void screen_unload_texture(u32 id);
void screen_get_texture_size(u32* width, u32* height, u32 id);
void screen_begin_frame(void);
void screen_end_frame(void);
void screen_select(gfxScreen_t screen);
void screen_draw_texture(u32 id, float x, float y) ;
void screen_draw_texture_crop(u32 id, float x, float y, float width, float height);
int screen_get_texture_width(u32 id);
int screen_get_texture_height(u32 id);
void screen_get_string_size(float* width, float* height, const char* text, float scaleX, float scaleY);
void screen_get_string_size_wrap(float* width, float* height, const char* text, float scaleX, float scaleY, float wrapWidth);
float screen_get_string_width(const char * text, float scaleX, float scaleY);
float screen_get_string_height(const char * text, float scaleX, float scaleY);
void screen_draw_string(float x, float y, float scaleX, float scaleY, u32 color, const char * text);
void screen_draw_stringf(float x, float y, float scaleX, float scaleY, u32 color, const char * text, ...);
void screen_draw_string_wrap(float x, float y, float scaleX, float scaleY, u32 color, float wrapX, const char * text);
void screen_draw_rect(float x, float y, float width, float height, u32 color);
void screen_set_text_vertex_array_pos(u32 pos);
void screen_get_wstring_size(float * width, float * height, const wchar_t * text, float scaleX, float scaleY);
void screen_draw_wstring(float x, float y, float scaleX, float scaleY, u32 color, const wchar_t * text);
void screen_draw_wstring_center(bool isTop, float y, float scaleX, float scaleY, u32 color, const wchar_t * wtext);
void screen_draw_texture_part(u32 id, float x, float y, float xbegin, float ybegin, float w, float h);
void screen_draw_wstringf(float x, float y, float scaleX, float scaleY, u32 color, const wchar_t * text, ...);
float screen_get_wstring_width(const wchar_t * text, float scaleX, float scaleY);
float screen_get_wstring_height(const wchar_t * text, float scaleX, float scaleY);
void screen_draw_string_center(bool isTop, float y, float scaleX, float scaleY, u32 color, const char * text);

#endif