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

#include "fx.h"

sf2d_texture *cubesSheet, *boxesSheet;

int movementLong = 0;
int movementSlow = 0;
int trasp = 254;
bool vanish = true;
bool up = true;

int x[11];
int y[11];
float rad = 0.0f;

void FXElementsInit() {
	srand(time(NULL));
	cubesSheet = sfil_load_PNG_file("romfs:/res/fx/Cubes.png", SF2D_PLACE_RAM);
	boxesSheet = sfil_load_PNG_file("romfs:/res/fx/Boxes.png", SF2D_PLACE_RAM);
	
	x[0] = 0;
	y[0] = 0;
	for (int i = 1; i < 11; i++) {
		x[i] = rand() % 800;
		y[i] = rand() % 200;
	}
}

void FXElementsExit() {
	sf2d_free_texture(boxesSheet);
	sf2d_free_texture(cubesSheet);
}

int movementOffsetLong(int maxrange) {
	if (up && movementLong > - maxrange * MOVEMENTSPEED) movementLong -= 1;
	if (movementLong == - maxrange * MOVEMENTSPEED) up = false;
	if (!up && movementLong < maxrange * MOVEMENTSPEED) movementLong += 1;
	if (movementLong == maxrange * MOVEMENTSPEED) up = true;
	
	return movementLong / MOVEMENTSPEED;
}

int movementOffsetSlow(int maxrange) {
	if (up && movementSlow > - maxrange * MOVEMENTSPEED) movementSlow -= 1;
	if (movementSlow == - maxrange * MOVEMENTSPEED) up = false;
	if (!up && movementSlow < maxrange * MOVEMENTSPEED) movementSlow += 1;
	if (movementSlow == maxrange * MOVEMENTSPEED) up = true;
	
	return movementSlow / MOVEMENTSPEED;
}

int giveTransparence() {
	if (vanish && trasp > 126) trasp -= 1;
	if (trasp == 126) vanish = false;
	if (!(vanish) && trasp < 254) trasp += 1;
	if (trasp == 254) vanish = true;
	
	return trasp;
}

void animateBG(bool isUp) {
	int maxrange = (isUp) ? 400 : 320;
	sf2d_draw_texture_part(boxesSheet, 0, 0, maxrange - x[0] / 2, 0, maxrange, 240);
	
	sf2d_draw_texture_part(cubesSheet, x[1] / 2, y[1], 0, 6, 66, 56); //cube1
	sf2d_draw_texture_part_rotate_scale(cubesSheet, x[2] / 2, y[2], rad, 67, 11, 56, 51, 1, 1); //cube2rotate
	sf2d_draw_texture_part(cubesSheet, x[3] / 2, y[3], 124, 0, 57, 62); //cube3
	sf2d_draw_texture_part_rotate_scale(cubesSheet, x[4] / 2, y[4], rad, 182, 6, 58, 56, 1, 1); //cube4rotate
	sf2d_draw_texture_part(cubesSheet, x[5] / 2, y[5], 241, 24, 41, 38); //cube5
	sf2d_draw_texture_part(cubesSheet, x[6] / 2, y[6], 283, 36, 27, 26); //cube6
	
	//rep
	sf2d_draw_texture_part_rotate_scale(cubesSheet, x[7] / 2, y[7], rad, 67, 11, 56, 51, 1, 1); //cube2rotate
	sf2d_draw_texture_part(cubesSheet, x[8] / 2, y[8], 283, 36, 27, 26); //cube6
	sf2d_draw_texture_part(cubesSheet, x[9] / 2, y[9], 311, 47, 14, 15); //cube7
	sf2d_draw_texture_part(cubesSheet, x[10] / 2, y[10], 311, 47, 14, 15); //cube7
	
	x[0] = (x[0] > maxrange * 2) ? 0 : (x[0] + 1);
	
	for (int i = 1; i < 11; i++) {
		if (x[i] >= maxrange * 2 + 45) {
			x[i] -= 181 + maxrange * 2 - rand() % 100;
			y[i] = rand() % 200;
		} else 
			x[i] += 1;
	}

	rad = (rad >= 360) ? 0.0f : (rad + 0.005f);
}