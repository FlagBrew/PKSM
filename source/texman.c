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

#include "texman.h"

void pp2d_draw_texture_part_blend(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, u32 color)
{
	pp2d_texture_select_part(id, x, y, xbegin, ybegin, width, height);
	pp2d_texture_blend(color);
	pp2d_texture_draw();		
}

void pp2d_draw_texture_part_scale(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY)
{
	pp2d_texture_select_part(id, x, y, xbegin, ybegin, width, height);
	pp2d_texture_scale(scaleX, scaleY);
	pp2d_texture_draw();
}

void pp2d_draw_texture_part_scale_blend(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY, u32 color)
{
	pp2d_texture_select_part(id, x, y, xbegin, ybegin, width, height);
	pp2d_texture_blend(color);
	pp2d_texture_scale(scaleX, scaleY);
	pp2d_texture_draw();		
}

void printGeneration(u8* pkmn, int x, int y)
{
	u8 version = pkx_get_version(pkmn);
	switch (version)
	{
		case 1: // sapphire
		case 2: // ruby
		case 3: // emerald
		case 4: // fire red
		case 5: // leaf green
		case 15: // colosseum/XD
			pp2d_draw_texture_part(SPRITESHEET, x, y, 29, 0, 10, 10); break;
		case 10: // diamond
		case 11: // pearl
		case 12: // platinum
		case 7: // heart gold
		case 8: // soul silver
			pp2d_draw_texture_part(SPRITESHEET, x, y, 39, 0, 10, 10); break;		
		case 20: // white
		case 21: // black
		case 22: // white2
		case 23: // black2
			pp2d_draw_texture_part(SPRITESHEET, x, y, 49, 0, 10, 10); break;
		case 24: // x
		case 25: // y
		case 26: // as
		case 27: // or
			pp2d_draw_texture_part(SPRITESHEET, x, y, 59, 0, 10, 10); break;
		case 30: // sun
		case 31: // moon
		case 32: // us
		case 33: // um
			pp2d_draw_texture_part(SPRITESHEET, x, y, 69, 0, 10, 10); break;
		case 34: // go
			pp2d_draw_texture_part(SPRITESHEET, x, y, 19, 0, 10, 10); break;
		case 35: // rd
		case 36: // gn
		case 37: // bu
		case 38: // yw
		case 39: // gd
		case 40: // sv
			pp2d_draw_texture_part(SPRITESHEET, x, y, 9, 0, 10, 10); break;
		default:
			break;
	}
}

void pksm_draw_texture(TextureID id, int x, int y)
{
	switch(id)
	{
		// not emulated
		case TEXTURE_BACK_BUTTON:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 0, 73, 34, 28); break;
		case TEXTURE_BLUE_TEXT_BOX:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 318, 20, 48, 25); break;
		case TEXTURE_BOTTOM_BAR:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 102, 73, 320, 30); break;
		case TEXTURE_BOTTOM_POPUP:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 36, 45, 318, 26); break;
		case TEXTURE_BUTTON:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 0, 103, 109, 31); break;
		case TEXTURE_DARK_BUTTON:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 0, 45, 36, 25); break;
		case TEXTURE_EVENT_MENU_BOTTOM_BAR:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 0, 137, 190, 34); break;
		case TEXTURE_EVENT_MENU_TOP_BAR_SELECTED:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 0, 175, 178, 38); break;
		case TEXTURE_EVENT_MENU_TOP_BAR_NORMAL:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 226, 137, 178, 38); break;
		case TEXTURE_FEMALE:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 79, 0, 9, 12); break;
		case TEXTURE_GENERATIONS:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 9, 0, 70, 10); break;
		case TEXTURE_HEX_BUTTON:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 291, 20, 27, 23); break;
		case TEXTURE_ICON_BANK:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 190, 137, 36, 35); break;
		case TEXTURE_ICON_CREDITS:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 245, 20, 46, 21); break;
		case TEXTURE_ICON_EDITOR:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 109, 103, 32, 32); break;
		case TEXTURE_ICON_EVENTS:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 62, 73, 40, 29); break;
		case TEXTURE_ICON_SAVE:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 141, 103, 28, 33); break;
		case TEXTURE_ICON_SETTINGS:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 34, 73, 28, 28); break;
		case TEXTURE_INCLUDE_INFO:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 385, 0, 41, 20); break;
		case TEXTURE_ITEM:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 115, 0, 10, 12); break;
		case TEXTURE_L_BUTTON:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 151, 0, 16, 16); break;
		case TEXTURE_LEFT:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 45, 20, 16, 20); break;
		case TEXTURE_LIGHT_BUTTON:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 366, 20, 36, 25); break;
		case TEXTURE_MAIN_MENU_BUTTON:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 229, 175, 140, 53); break;
		case TEXTURE_MALE:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 103, 0, 12, 12); break;
		case TEXTURE_MINI_BOX:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 209, 20, 36, 21); break;
		case TEXTURE_MINUS_BUTTON:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 183, 0, 17, 18); break;
		case TEXTURE_MINUS:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 125, 0, 13, 13); break;
		case TEXTURE_NO_MOVE:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 61, 20, 148, 20); break;
		case TEXTURE_NORMAL_BAR:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 237, 0, 148, 20); break;
		case TEXTURE_OTA_BUTTON:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 354, 45, 33, 28); break;
		case TEXTURE_PLUS_BUTTON:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 200, 0, 17, 18); break;
		case TEXTURE_PLUS:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 138, 0, 13, 13); break;
		case TEXTURE_POKEBALL:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 369, 175, 68, 69); break;
		case TEXTURE_POKEMON_BOX:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 178, 175, 51, 45); break;
		case TEXTURE_R_BUTTON:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 167, 0, 16, 16); break;
		case TEXTURE_RED_BUTTON:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 402, 20, 36, 25); break;
		case TEXTURE_SELECTOR_NORMAL:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 388, 45, 19, 28); break;
		case TEXTURE_SETTING:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 88, 0, 15, 12); break;
		case TEXTURE_SHINY:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 0, 0, 9, 9); break;
		case TEXTURE_SUB_ARROW:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 217, 0, 20, 18); break;
		case TEXTURE_TRANSFER_BUTTON:
			pp2d_draw_texture_part(SPRITESHEET, x, y, 0, 20, 45, 20); break;
			
		// emulated
		case TEXTURE_RIGHT:
			pp2d_texture_select_part(SPRITESHEET, x, y, 45, 20, 16, 20);
			pp2d_texture_flip(HORIZONTAL);
			pp2d_texture_draw();
			break;
		case TEXTURE_SUB_ARROW_FLIPPED:
			pp2d_texture_select_part(SPRITESHEET, x, y, 217, 0, 20, 18);
			pp2d_texture_flip(HORIZONTAL);
			pp2d_texture_draw();
			break;
		default:
			break;
	}
}

void pksm_draw_texture_blend(TextureID id, int x, int y, u32 color)
{
	switch(id)
	{
		case TEXTURE_BUTTON:
			pp2d_draw_texture_part_blend(SPRITESHEET, x, y, 0, 103, 109, 31, color); break;
		case TEXTURE_ITEM:
			pp2d_draw_texture_part_blend(SPRITESHEET, x, y, 115, 0, 10, 12, color); break;
		case TEXTURE_SELECTOR_NORMAL:
			pp2d_draw_texture_part_blend(SPRITESHEET, x, y, 388, 45, 19, 28, color); break;
		default:
			break;
	}
}