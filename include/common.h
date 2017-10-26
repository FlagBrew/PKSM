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

#ifndef COMMON_H
#define COMMON_H

#include <3ds.h>
#include <citro3d.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "bank.h"
#include "config.h"
#include "database.h"
#include "dex.h"
#include "editor.h"
#include "fill.h"
#include "files.h"
#include "fx.h"
#include "game.h"
#include "graphic.h"
#include "hex.h"
#include "hid.h"
#include "http.h"
#include "i18n.h"
#include "../source/memecrypto/source/memecrypto.h"
#include "../source/pp2d/pp2d.h"
#include "pkx.h"
#include "save.h"
#include "sha256.h"
#include "socket.h"
#include "spi.h"
#include "texman.h"
#include "util.h"
#include "wcx.h"

struct ArrayUTF32 listMoves;
struct ArrayUTF32 listItems;
struct ArrayUTF32 listSpecies;

#endif
