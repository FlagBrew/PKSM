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
#include "common.h"

#define PAYLOADSIZE    0x400
#define SOC_ALIGN      0x1000
#define SOC_BUFFERSIZE 0x100000

typedef struct {
	struct sockaddr_in client_addr;
	struct sockaddr_in server_addr;
	u32	client_length;
	s32	server_id;
	s32	client_id;
} socket_server;

char* socket_get_ip();
void socket_shutdown();
int socket_init();
void process_pkx(u8* mainbuf, int game, int tempVett[]);
void process_wcx(u8* buf);
void processLegality(u8* pkmn);

bool socket_check_valid_ip_address(char *ipAddress);
bool socket_is_legality_address_set();
void socket_set_legality_address(bool value);