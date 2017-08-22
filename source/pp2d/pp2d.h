/**
 * @file pp2d.h
 * @author Bernardo Giordano
 * @date 18 August 2017
 * @brief pp2d header
 */

#pragma once

#include <3ds.h>
#include <citro3d.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "lodepng.h"
#include "vshader_shbin.h"

#define TOP_WIDTH 400
#define BOTTOM_WIDTH 320
#define SCREEN_HEIGHT 240

/**
 * @brief Used to transfer the final rendered display to the framebuffer
 */
#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

/**
 * @brief Creates a 8 byte RGBA color
 * @param r red component of the color
 * @param g green component of the color
 * @param b blue component of the color
 * @param a alpha component of the color
 */
#define RGBA8(r, g, b, a) ((((r)&0xFF)<<0) | (((g)&0xFF)<<8) | (((b)&0xFF)<<16) | (((a)&0xFF)<<24))

/**
 * @brief Creates a 8 byte ABGR color
 * @param a alpha component of the color
 * @param b blue component of the color
 * @param g green component of the color
 * @param r red component of the color
 */
#define ABGR8(a, b, g, r) ((((a)&0xFF)<<0) | (((b)&0xFF)<<8) | (((g)&0xFF)<<16) | (((r)&0xFF)<<24))

#define BACKGROUND_COLOR ABGR8(255, 0, 0, 0)
#define PP2D_NEUTRAL RGBA8(255, 255, 255, 255)

#define MAX_TEXTURES 256
#define MAX_SPRITES 1600

#define TEXT_VTX_ARRAY_COUNT (4*1024)

typedef enum {
	NONE,
	HORIZONTAL,
	VERTICAL,
	BOTH
} flipType;

typedef struct { 
	float position[3]; 
	float texcoord[2]; 
} textVertex_s;

/**
 * @brief Starts a new frame on the specified screen
 * @param target GFX_TOP or GFX_BOTTOM
 */
void pp2d_begin_draw(gfxScreen_t target);

/**
 * @brief Changes target screen to the specified target
 * @param target GFX_TOP or GFX_BOTTOM
 */
void pp2d_draw_on(gfxScreen_t target);

/**
 * @brief Draw a rectangle
 * @param x of the top left corner
 * @param y of the top left corner
 * @param width on the rectangle
 * @param height of the rectangle
 * @param color RGBA8 to fill the rectangle
 */
void pp2d_draw_rectangle(int x, int y, int width, int height, u32 color);

/**
 * @brief Prints a char pointer
 * @param x position to start drawing
 * @param y position to start drawing
 * @param scaleX multiplier for the text width
 * @param scaleY multiplier for the text height
 * @param color RGBA8 the text will be drawn
 * @param text to be printed on the screen
 */
void pp2d_draw_text(float x, float y, float scaleX, float scaleY, u32 color, const char* text);

/**
 * @brief Prints a char pointer in the middle of the target screen
 * @param target screen to draw the text to
 * @param y position to start drawing
 * @param scaleX multiplier for the text width
 * @param scaleY multiplier for the text height
 * @param color RGBA8 the text will be drawn
 * @param text to be printed on the screen
 */
void pp2d_draw_text_center(gfxScreen_t target, float y, float scaleX, float scaleY, u32 color, const char* text);

/**
 * @brief Prints a char pointer with arguments
 * @param x position to start drawing
 * @param y position to start drawing
 * @param scaleX multiplier for the text width
 * @param scaleY multiplier for the text height
 * @param color RGBA8 the text will be drawn
 * @param text to be printed on the screen
 * @param ... arguments
 */
void pp2d_draw_textf(float x, float y, float scaleX, float scaleY, u32 color, const char* text, ...); 

/**
 * @brief Prints a texture
 * @param id of the texture 
 * @param x position on the screen to draw the texture
 * @param y position on the screen to draw the texture
 */
void pp2d_draw_texture(size_t id, int x, int y);

/**
 * @brief Prints a texture with color modulation
 * @param id of the texture 
 * @param x position on the screen to draw the texture
 * @param y position on the screen to draw the texture
 * @param color RGBA8 to modulate the texture with
 */
void pp2d_draw_texture_blend(size_t id, int x, int y, u32 color);

/**
 * @brief Prints a texture with color modulation
 * @param id of the texture 
 * @param x position on the screen to draw the texture
 * @param y position on the screen to draw the texture
 * @param color RGBA8 to modulate the texture with
 * @param fliptype HORIZONTAL, VERTICAL or BOTH
 */
void pp2d_draw_texture_blend_flip(size_t id, int x, int y, u32 color, flipType fliptype);

/**
 * @brief Prints a texture with color modulation
 * @param id of the texture 
 * @param x position on the screen to draw the texture
 * @param y position on the screen to draw the texture
 * @param color RGBA8 to modulate the texture with
 * @param fliptype HORIZONTAL, VERTICAL or BOTH
 */
void pp2d_draw_texture_flip(size_t id, int x, int y, flipType fliptype);

/**
 * @brief Prints a texture with a scale factor
 * @param id of the texture 
 * @param x position on the screen to draw the texture
 * @param y position on the screen to draw the texture
 * @param scaleX width scale factor
 * @param scaleY height scale factor
 */
void pp2d_draw_texture_scale(size_t id, int x, int y, float scaleX, float scaleY);

/**
 * @brief Prints a texture with a scale factor and color modulation
 * @param id of the texture 
 * @param x position on the screen to draw the texture
 * @param y position on the screen to draw the texture
 * @param scaleX width scale factor
 * @param scaleY height scale factor
 * @param color RGBA8 to modulate the texture with
 */
void pp2d_draw_texture_scale_blend(size_t id, int x, int y, float scaleX, float scaleY, u32 color);

/**
 * @brief Prints a portion of a texture
 * @param id of the texture 
 * @param x position on the screen to draw the texture
 * @param y position on the screen to draw the texture
 * @param xbegin position to start drawing
 * @param ybegin position to start drawing
 * @param width to draw from the xbegin position 
 * @param height to draw from the ybegin position
 */
void pp2d_draw_texture_part(size_t id, int x, int y, int xbegin, int ybegin, int width, int height);

/**
 * @brief Prints a portion of a texture with color modulation
 * @param id of the texture 
 * @param x position on the screen to draw the texture
 * @param y position on the screen to draw the texture
 * @param xbegin position to start drawing
 * @param ybegin position to start drawing
 * @param width to draw from the xbegin position 
 * @param height to draw from the ybegin position
 * @param color RGBA8 to modulate the texture with
 */
void pp2d_draw_texture_part_blend(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, u32 color);

/**
 * @brief Prints a portion of a texture with a scale factor
 * @param id of the texture 
 * @param x position on the screen to draw the texture
 * @param y position on the screen to draw the texture
 * @param xbegin position to start drawing
 * @param ybegin position to start drawing
 * @param width to draw from the xbegin position 
 * @param height to draw from the ybegin position
 * @param scaleX width scale factor
 * @param scaleY height scale factor
 */
void pp2d_draw_texture_part_scale(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY);

/**
 * @brief Prints a portion of a texture with a scale factor and color modulation
 * @param id of the texture 
 * @param x position on the screen to draw the texture
 * @param y position on the screen to draw the texture
 * @param xbegin position to start drawing
 * @param ybegin position to start drawing
 * @param width to draw from the xbegin position 
 * @param height to draw from the ybegin position
 * @param scaleX width scale factor
 * @param scaleY height scale factor
 * @param color RGBA8 to modulate the texture with
 */
void pp2d_draw_texture_part_scale_blend(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY, u32 color);

/**
 * @brief Prints a wchar_t pointer
 * @param x position to start drawing
 * @param y position to start drawing
 * @param scaleX multiplier for the text width
 * @param scaleY multiplier for the text height
 * @param color RGBA8 the text will be drawn
 * @param text to be printed on the screen
 */
void pp2d_draw_wtext(float x, float y, float scaleX, float scaleY, u32 color, const wchar_t* text); 

/**
 * @brief Prints a wchar_t pointer in the middle of the target screen
 * @param target screen to draw the text to
 * @param y position to start drawing
 * @param scaleX multiplier for the text width
 * @param scaleY multiplier for the text height
 * @param color RGBA8 the text will be drawn
 * @param text to be printed on the screen
 */
void pp2d_draw_wtext_center(gfxScreen_t target, float y, float scaleX, float scaleY, u32 color, const wchar_t* text);

/**
 * @brief Prints a wchar_t pointer with arguments
 * @param x position to start drawing
 * @param y position to start drawing
 * @param scaleX multiplier for the text width
 * @param scaleY multiplier for the text height
 * @param color RGBA8 the text will be drawn
 * @param text to be printed on the screen
 * @param ... arguments
 */
void pp2d_draw_wtextf(float x, float y, float scaleX, float scaleY, u32 color, const wchar_t* text, ...); 

/**
 * @brief Ends a frame
 */
void pp2d_end_draw(void);

/**
 * @brief Frees the pp2d environment
 */
void pp2d_exit(void);

/**
 * @brief Initializes the fast draw buffer
 * @param id of the texture to bind
 */
void pp2d_fast_draw_init(size_t id);

/**
 * @brief Bufferizes a texture for fast drawing
 * @param x to draw the texture to
 * @param y to draw the texture to
 */
void pp2d_fast_draw_texture(int x, int y);

/**
 * @brief Bufferizes a portion of a texture for fast drawing
 * @param x position on the screen to draw the texture
 * @param y position on the screen to draw the texture
 * @param xbegin position to start drawing
 * @param ybegin position to start drawing
 * @param width to draw from the xbegin position 
 * @param height to draw from the ybegin position
 */
void pp2d_fast_draw_texture_part(int x, int y, int xbegin, int ybegin, int width, int height);

/**
 * @brief Renders the bufferized textures for fast drawing
 */
void pp2d_fast_render(void);

/**
 * @brief Inits the pp2d environment
 * @return 0 if everything went correctly, otherwise returns Result code
 * @note This will trigger gfxInitDefault by default 
 */
Result pp2d_init(void);

/**
 * @brief Calculates a char pointer height
 * @param text char pointer to calculate the height of
 * @param scaleX multiplier for the text width 
 * @param scaleY multiplier for the text height
 * @return height the text will have if rendered in the supplied conditions
 */
float pp2d_get_text_height(const char* text, float scaleX, float scaleY);

/**
 * @brief Calculates a char pointer width
 * @param text char pointer to calculate the width of
 * @param scaleX multiplier for the text width 
 * @param scaleY multiplier for the text height
 * @return width the text will have if rendered in the supplied conditions
 */
float pp2d_get_text_width(const char* text, float scaleX, float scaleY);

/**
 * @brief Calculates a wchar_t pointer height
 * @param text wchar_t pointer to calculate the height of
 * @param scaleX multiplier for the text width 
 * @param scaleY multiplier for the text height
 * @return height the text will have if rendered in the supplied conditions
 */
float pp2d_get_wtext_height(const wchar_t* text, float scaleX, float scaleY);

/**
 * @brief Calculates a wchar_t pointer width
 * @param text wchar_t pointer to calculate the width of
 * @param scaleX multiplier for the text width 
 * @param scaleY multiplier for the text height
 * @return width the text will have if rendered in the supplied conditions
 */
float pp2d_get_wtext_width(const wchar_t* text, float scaleX, float scaleY);

/**
 * @brief Loads a texture from a png file
 * @param id of the texture 
 * @param path where the ong file is located 
 */
void pp2d_load_texture_png(size_t id, const char* path);

/**
 * @brief Sets a background color for the specified screen
 * @param target GFX_TOP or GFX_BOTTOM
 * @param color ABGR8 which will be the background one
 */
void pp2d_set_screen_color(gfxScreen_t target, u32 color);