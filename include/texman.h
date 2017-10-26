/*  This file is part of PKSM
>	Copyright (C) 2016/2017 Bernardo Giordano
>
>   This program is free software: you can redistribute it and/or modify
>   it under the terms of the GNU General Public License as published by
>   the Free Software Foundation, either version 3 of the License, or
>   (at your option) any later version.
>
>   This program is distributed in the hope that it will be useful,
>   but WITHOUT ANY WARRANTY; without even the implied warranty of
>   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>   GNU General Public License for more details.
>
>   You should have received a copy of the GNU General Public License
>   along with this program.  If not, see <http://www.gnu.org/licenses/>.
>   See LICENSE for information.
*/

#ifndef TEXMAN_H
#define TEXMAN_H

#include "common.h"

typedef enum {
	// not emulated
	TEXTURE_BACK_BUTTON,
	TEXTURE_BLUE_TEXT_BOX,
	TEXTURE_BOTTOM_BAR,
	TEXTURE_BOTTOM_POPUP,
	TEXTURE_BUTTON,
	TEXTURE_DARK_BUTTON,
	TEXTURE_EVENT_MENU_BOTTOM_BAR,
	TEXTURE_EVENT_MENU_TOP_BAR_SELECTED,
	TEXTURE_EVENT_MENU_TOP_BAR_NORMAL,
	TEXTURE_FEMALE,
	TEXTURE_GENERATIONS,
	TEXTURE_HEX_BUTTON,
	TEXTURE_ICON_BANK,
	TEXTURE_ICON_CREDITS,
	TEXTURE_ICON_EDITOR,
	TEXTURE_ICON_EVENTS,
	TEXTURE_ICON_SAVE,
	TEXTURE_ICON_SETTINGS,
	TEXTURE_INCLUDE_INFO,
	TEXTURE_ITEM,
	TEXTURE_L_BUTTON,
	TEXTURE_LEFT,
	TEXTURE_LIGHT_BUTTON,
	TEXTURE_MAIN_MENU_BUTTON,
	TEXTURE_MALE,
	TEXTURE_MINI_BOX,
	TEXTURE_MINUS_BUTTON,
	TEXTURE_MINUS,
	TEXTURE_NO_MOVE,
	TEXTURE_NORMAL_BAR,
	TEXTURE_OTA_BUTTON,
	TEXTURE_PLUS_BUTTON,
	TEXTURE_PLUS,
	TEXTURE_POKEBALL,
	TEXTURE_POKEMON_BOX,
	TEXTURE_R_BUTTON,
	TEXTURE_RED_BUTTON,
	TEXTURE_SELECTOR_NORMAL,
	TEXTURE_SETTING,
	TEXTURE_SHINY,
	TEXTURE_SUB_ARROW,
	TEXTURE_TRANSFER_BUTTON,
	
	// emulated
	TEXTURE_RIGHT,
	TEXTURE_SUB_ARROW_FLIPPED
} TextureID;

void pp2d_draw_texture_part_blend(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, u32 color);
void pp2d_draw_texture_part_scale(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY);
void pp2d_draw_texture_part_scale_blend(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY, u32 color);

void printGeneration(u8* pkmn, int x, int y);

void pksm_draw_texture(TextureID id, int x, int y);
void pksm_draw_texture_blend(TextureID id, int x, int y, u32 color);

#endif