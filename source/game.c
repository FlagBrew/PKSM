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

#include "game.h"

int game = 0;

int game_get() {
	return game;
}

void game_set(int value) {
	game = value;
}

bool game_isgen7() {
	return (game == GAME_SUN || game == GAME_MOON || game == GAME_US || game == GAME_UM);
}

bool game_isgen6() {
	return (game == GAME_X || game == GAME_Y || game == GAME_OR || game == GAME_AS);
}

bool game_isgen5() {
	return (game == GAME_B1 || game == GAME_B2 || game == GAME_W1 || game == GAME_W2);
}

bool game_isgen4() {
	return (game == GAME_DIAMOND || game == GAME_PEARL || game == GAME_PLATINUM || game == GAME_HG || game == GAME_SS);
}

bool game_is3DS() {
	return game_isgen6() || game_isgen7();
}

bool game_isDS() {
	return game_isgen5() || game_isgen4();
}

bool game_getisXY() {
	return (game == GAME_X || game == GAME_Y);
}

bool game_getisORAS() {
	return (game == GAME_OR || game == GAME_AS);
}

bool game_getisSUMO() {
	return (game == GAME_SUN || game == GAME_MOON);
}

bool game_getisUSUM() {
	return (game == GAME_US || game == GAME_UM);
}

u8 game_get_country(u8* mainbuf) {
	return *(u8*)(mainbuf + ofs.saveCountry);
}

u8 game_get_region(u8* mainbuf) {
	return *(u8*)(mainbuf + ofs.saveRegion);
}

u8 game_get_console_region(u8* mainbuf) {
	return *(u8*)(mainbuf + ofs.consoleRegion);
}

u8 game_get_language(u8* mainbuf) {
	return *(u8*)(mainbuf + ofs.saveLanguage);	
}

void game_fill_offsets()
{
	// init
	ofs.totalSpecies = 807;
	ofs.pkxLength = 232;
	ofs.totalMoves = 728;
	ofs.totalItems = 959;
	
	ofs.maxBalls = 0;
	ofs.maxBoxes = 31;
	ofs.maxAbilities = 0;
	ofs.maxSpecies = 0;
	ofs.maxMoveID = 0;
	ofs.maxItemID = 0;
	ofs.maxWondercards = 0;
	ofs.nicknameLength = 26;
	ofs.wondercardLocation = 0;
	ofs.wondercardSize = 264;
	ofs.pkmnLength = 232;
	ofs.pokedex = 0;
	ofs.battleBoxes = 0;
	ofs.saveOT = 0;
	ofs.saveGender = 0;
	ofs.saveTID = 0;
	ofs.saveSID = 0;
	ofs.saveSeed = 0;
	ofs.saveLanguage = 0;
	ofs.saveRegion = 0;
	ofs.consoleRegion = 0;
	ofs.saveCountry = 0;
	ofs.saveSize = 0;
	ofs.boxSize = 0;
	
	if (game_getisUSUM())
	{
		ofs.maxBalls = 26;
		ofs.maxBoxes = 32;
		ofs.maxAbilities = 233;
		ofs.maxSpecies = 806; // blocks zeraora, TODO: 807
		ofs.maxMoveID = 728;
		ofs.maxItemID = 959;
		ofs.maxWondercards = 48;
		ofs.nicknameLength = 26;
		ofs.wondercardLocation = 0x66300;
		ofs.wondercardSize = 264;
		ofs.pkmnLength = 232;
		ofs.pokedex = 0x2C00;
		ofs.battleBoxes = 0x50C4;
		ofs.saveOT = 0x1438;
		ofs.saveGender = 0x1405;
		ofs.saveTID = 0x1400;
		ofs.saveSID = 0x1402;
		ofs.saveSeed = 0x651DC;
		ofs.saveLanguage = 0x1435;
		ofs.saveRegion = 0x142E;
		ofs.consoleRegion = 0x1434;
		ofs.saveCountry = 0x142F;
		ofs.saveSize = 0x6CC00;
		ofs.boxSize = 0x36600;
	}
	else if (game_getisSUMO())
	{
		ofs.maxBalls = 26;
		ofs.maxBoxes = 32;
		ofs.maxAbilities = 232;
		ofs.maxSpecies = 802;
		ofs.maxMoveID = 720;
		ofs.maxItemID = 920;
		ofs.maxWondercards = 48;
		ofs.nicknameLength = 26;
		ofs.wondercardLocation = 0x65D00;
		ofs.wondercardSize = 264;
		ofs.pkmnLength = 232;
		ofs.pokedex = 0x2A00;
		ofs.battleBoxes = 0x4CC4;
		ofs.saveOT = 0x1238;
		ofs.saveGender = 0x1205;
		ofs.saveTID = 0x1200;
		ofs.saveSID = 0x1202;
		ofs.saveSeed = 0x6B5DC;
		ofs.saveLanguage = 0x1235;
		ofs.saveRegion = 0x122E;
		ofs.consoleRegion = 0x1234;
		ofs.saveCountry = 0x122F;
		ofs.saveSize = 0x6BE00;
		ofs.boxSize = 0x36600;
	}
	else if (game_getisORAS())
	{
		ofs.maxBalls = 25;
		ofs.maxBoxes = 31;
		ofs.maxAbilities = 191;
		ofs.maxSpecies = 721;
		ofs.maxMoveID = 621;
		ofs.maxItemID = 775;
		ofs.maxWondercards = 24;
		ofs.nicknameLength = 26;
		ofs.wondercardLocation = 0x1CD00;
		ofs.wondercardSize = 264;
		ofs.pkmnLength = 232;
		ofs.pokedex = 0x1500;
		//ofs.battleBoxes =
		ofs.saveOT = 0x14048;
		ofs.saveGender = 0x14005;
		ofs.saveTID = 0x14000;
		ofs.saveSID = 0x14002;
		//ofs.saveSeed = 
		ofs.saveLanguage = 0x1402D;
		ofs.saveRegion = 0x14026;
		ofs.consoleRegion = 0x1402C;
		ofs.saveCountry = 0x14027;
		ofs.saveSize = 0x76000;
		ofs.boxSize = 0x34AD0;
	}
	else if (game_getisXY())
	{
		ofs.maxBalls = 25;
		ofs.maxBoxes = 31;
		ofs.maxAbilities = 188;
		ofs.maxSpecies = 721;
		ofs.maxMoveID = 617;
		ofs.maxItemID = 717;
		ofs.maxWondercards = 24;
		ofs.nicknameLength = 26;
		ofs.wondercardLocation = 0x1BD00;
		ofs.wondercardSize = 264;
		ofs.pkmnLength = 232;
		ofs.pokedex = 0x1500;
		//ofs.battleBoxes =
		ofs.saveOT = 0x14048;
		ofs.saveGender = 0x14005;
		ofs.saveTID = 0x14000;
		ofs.saveSID = 0x14002;
		//ofs.saveSeed =
		ofs.saveLanguage = 0x1402D;
		ofs.saveRegion = 0x14026;
		ofs.consoleRegion = 0x1402C;
		ofs.saveCountry = 0x14027;
		ofs.saveSize = 0x65600;
		ofs.boxSize = 0x34AD0;
	}
	else if (game_isgen5())
	{
		ofs.maxBalls = 25;
		//ofs.maxBoxes =
		ofs.maxAbilities = 164;
		ofs.maxSpecies = 649;
		//ofs.maxMoveID = 
		//ofs.maxItemID = 
		ofs.maxWondercards = 12;
		//ofs.nicknameLength =
		ofs.wondercardLocation = 0x1C900;
		ofs.wondercardSize = 204;
		// ofs.pkmnLength =
		// ofs.pokedex =
		// ofs.battleBoxes =
		// ofs.saveOT =
		// ofs.saveGender =
		// ofs.saveTID =
		// ofs.saveSID =
		// ofs.saveSeed =
		// ofs.saveLanguage =
		// ofs.saveRegion =
		// ofs.consoleRegion =
		// ofs.saveCountry =
		// ofs.saveSize = 	
		// ofs.boxSize = 
	}
	else if (game_isgen4())
	{
		ofs.maxBalls = 25;
		//ofs.maxBoxes =
		ofs.maxAbilities = 123;
		ofs.maxSpecies = 493;
		//ofs.maxMoveID = 
		//ofs.maxItemID = 
		ofs.maxWondercards = 8;
		// ofs.nicknameLength =
		// ofs.wondercardLocation =
		ofs.wondercardSize = 260;
		// ofs.pkmnLength =
		// ofs.pokedex =
		// ofs.battleBoxes =
		// ofs.saveOT =
		// ofs.saveGender =
		// ofs.saveTID =
		// ofs.saveSID =
		// ofs.saveSeed =
		// ofs.saveLanguage =
		// ofs.saveRegion =
		// ofs.consoleRegion =
		// ofs.saveCountry =
		// ofs.saveSize =
		// ofs.boxSize = 		
	}
}