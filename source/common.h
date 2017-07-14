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

#pragma once

#include <3ds.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <netinet/in.h>
#include <sf2d.h>
#include <sfil.h>
#include <sftd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "bank.h"
#include "database.h"
#include "dex.h"
#include "editor.h"
#include "fill.h"
#include "files.h"
#include "fx.h"
#include "graphic.h"
#include "hex.h"
#include "hid.h"
#include "http.h"
#include "i18n.h"
#include "memecrypto/source/memecrypto.h"
#include "pkx.h"
#include "save.h"
#include "sha256.h"
#include "socket.h"
#include "spi.h"
#include "util.h"
#include "wcx.h"

#define DEBUG 0

#define GAME_X 		  0
#define GAME_Y 		  1
#define GAME_OR 	  2
#define GAME_AS 	  3
#define GAME_SUN 	  4
#define GAME_MOON 	  5
#define GAME_DIAMOND  6
#define GAME_PEARL 	  7
#define GAME_PLATINUM 8
#define GAME_HG 	  9
#define GAME_SS 	 10
#define GAME_B1 	 11
#define GAME_W1 	 12
#define GAME_B2 	 13
#define GAME_W2 	 14

#define ISGEN7 (game == GAME_SUN || game == GAME_MOON)
#define ISGEN6 (game == GAME_X || game == GAME_Y || game == GAME_OR || game == GAME_AS)
#define ISGEN5 (game == GAME_B1 || game == GAME_B2 || game == GAME_W1 || game == GAME_W2)
#define ISGEN4 (game == GAME_DIAMOND || game == GAME_PEARL || game == GAME_PLATINUM || game == GAME_HG || game == GAME_SS)
#define IS3DS (ISGEN6 || ISGEN7)
#define ISDS (ISGEN5 || ISGEN4)
#define ISXY (game == GAME_X || game == GAME_Y)
#define ISORAS (game == GAME_OR || game == GAME_AS)
#define ISSUMO (game == GAME_SUN || game == GAME_MOON)

#ifdef PKSV
#define VERSION "v2.0.1"
#else
#define VERSION "v4.3.1"
#endif


/**
 * OFFSETS
 */
#define SAVE_SIZE_MAX 0x80000
#define SAVE_SM_MONEY 0x4004
#define SAVE_ORAS_MONEY 0x4208
#define SAVE_XY_MONEY 0x4208
#define SAVE_SM_ITEM 0x0
#define SAVE_SM_ITEM_SIZE 0xDE0


#define WCX_SIZE 0x108
#define WCX_SIZEFULL 0x310


struct ArrayUTF32 listMoves;
struct ArrayUTF32 listItems;
struct ArrayUTF32 listSpecies;
