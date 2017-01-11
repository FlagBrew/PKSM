/* This file is part of PKSM

Copyright (C) 2016 Bernardo Giordano

>    This program is free software: you can redistribute it and/or modify
>    it under the terms of the GNU General Public License as published by
>    the Free Software Foundation, either version 3 of the License, or
>    (at your option) any later version.
>
>    This program is distributed in the hope that it will be useful,
>    but WITHOUT ANY WARRANTY; without even the implied warranty of
>    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>    GNU General Public License for more details.
>
>    You should have received a copy of the GNU General Public License
>    along with this program.  If not, see <http://www.gnu.org/licenses/>.
>    See LICENSE for information.
*/

#pragma once

#include <stdlib.h>
#include <3ds.h>
#include <malloc.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include "graphic.h"

#define PAYLOADSIZE 	232
#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

typedef struct {
	u8					running;
	struct sockaddr_in	client_addr;
	struct sockaddr_in	server_addr;
	u32					client_length;
	s32					server_id;
	s32					client_id;
} http_server;

void drawIP(sftd_font *fontBold9);
void shutDownSoc();
void closeOnExit();
int init();
void processing(u8* mainbuf, int game, int tempVett[]);
Result downloadFile(char* url, char* path, bool install);