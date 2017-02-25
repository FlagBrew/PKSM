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
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "bank.h"
#include "database.h"
#include "dex.h"
#include "editor.h"
#include "fill.h"
#include "fx.h"
#include "graphic.h"
#include "hex.h"
#include "hid.h"
#include "http.h"
#include "memecrypto/source/memecrypto.h"
#include "save.h"
#include "sha256.h"
#include "spi.h"
#include "util.h"

#define DEBUG 1

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

#define VERSION "v4.2.0"