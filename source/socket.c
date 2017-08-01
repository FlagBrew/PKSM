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

#include "socket.h"

int panic = 0;
static u32 *socket_buffer = NULL;
static socket_server server;
static char	payload[PAYLOADSIZE];

static char legalityAddress[16];
bool isLegalityAddressSet = false;
bool checkingLegality = false;

char* socket_get_ip() {
	return inet_ntoa(server.server_addr.sin_addr);
}

void socket_shutdown() {
	close(server.server_id);
	socExit();
}

void socket_close() {
	if (server.server_id > 0) 
		close(server.server_id);
	if (server.client_id > 0)
		close(server.client_id);	
}

int socket_init() {
	socket_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);
	if (socket_buffer == NULL) {
		infoDisp(i18n(S_HTTP_BUFFER_ALLOC_FAILED));
		socket_close();
		return 0;
	}

	if (socInit(socket_buffer, SOC_BUFFERSIZE)) {
		infoDisp(i18n(S_HTTP_SOCINIT_FAILED));
		socket_close();
		return 0;
	}
	
	memset(&server, 0, sizeof(server));
	server.client_id = -1;
	server.server_id = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	if (server.server_id < 0) {
		infoDisp(i18n(S_HTTP_SOCKET_UNACCESSIBLE));
		socket_close();
		return 0;
	}
	
	server.server_addr.sin_family = AF_INET;
	server.server_addr.sin_port = htons(9000);
	server.server_addr.sin_addr.s_addr = gethostid();
	server.client_length = sizeof(server.client_addr);

	if (bind(server.server_id, (struct sockaddr *) &server.server_addr, sizeof(server.server_addr))) {
		close(server.server_id);
		infoDisp(i18n(S_HTTP_BINDING_FAILED));
		socket_close();
		return 0;
	}

	fcntl(server.server_id, F_SETFL, fcntl(server.server_id, F_GETFL, 0) | O_NONBLOCK);

	if (listen(server.server_id, 5)) {
		infoDisp(i18n(S_HTTP_LISTENING_FAILED));
		socket_close();
		return 0;		
	}
	return 1;
}

void process_wcx(u8* buf) {
	server.client_id = accept(server.server_id, (struct sockaddr *) &server.client_addr, &server.client_length);
	if (server.client_id < 0 && errno != EAGAIN) {
		infoDisp(i18n(S_HTTP_ERROR_PROCESSING_PHASE));
		socket_close();
		return;		
	} else {
		char *dummy;
		memset(payload, 0, PAYLOADSIZE);
		fcntl(server.client_id, F_SETFL, fcntl(server.client_id, F_GETFL, 0) & ~O_NONBLOCK);

        recv(server.client_id, payload, PAYLOADSIZE, 0);
        if (strstr(payload, "PKSMOTA") != NULL && (hidKeysDown() != KEY_B)) {
            dummy = strstr(payload, "PKSMOTA");
            memcpy(buf, &dummy[7], 264);
        }
    }
    close(server.client_id);
    server.client_id = -1;
}

void process_pkx(u8* mainbuf, int game, int tempVett[]) {
	server.client_id = accept(server.server_id, (struct sockaddr *) &server.client_addr, &server.client_length);
	if (server.client_id < 0 && errno != EAGAIN) {
		infoDisp(i18n(S_HTTP_ERROR_PROCESSING_PHASE));
		socket_close();
		return;		
	} else {
		panic = 0;
		int boxmax = ISGEN6 ? 30 : 31;
		char *dummy;
		memset(payload, 0, PAYLOADSIZE);

		fcntl(server.client_id, F_SETFL, fcntl(server.client_id, F_GETFL, 0) & ~O_NONBLOCK);
        recv(server.client_id, payload, PAYLOADSIZE, 0);
        if (strstr(payload, "PKSMOTA") != NULL && (hidKeysDown() != KEY_B)) {
            u8 pkmn[PKMNLENGTH];
            dummy = strstr(payload, "PKSMOTA");
            memcpy(pkmn, &dummy[7], PKMNLENGTH);
            pkx_set_as_it_is(mainbuf, tempVett[0], tempVett[1], pkmn, game);

			if (checkingLegality) {
				checkingLegality = false;
			} else {
				do {
					tempVett[1]++;
					if (tempVett[1] == 30) {
						tempVett[0]++;
						tempVett[1] = 0;
					}
					if (tempVett[0] > boxmax)
						tempVett[0] = 0;

					pkx_get(mainbuf, tempVett[0], tempVett[1], pkmn, game);
					panic++;
				} while (pkx_get_species(pkmn) && (panic < boxmax * 30));
			}
        }
    }
    close(server.client_id);
    server.client_id = -1;
}

void processLegality(u8* pkmn) {
	char message[PKMNLENGTH + 7];
	const char* prefix = "PKSMOTA";
	memcpy(message, prefix, 7);
	memcpy(message + 7, pkmn, PKMNLENGTH);

	struct sockaddr_in legalityServer;
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		close(sock);
		infoDisp(i18n(S_HTTP_SOCKET_UNACCESSIBLE));
		return;
	}
	
	legalityServer.sin_addr.s_addr = inet_addr(legalityAddress);
	legalityServer.sin_family = AF_INET;
	legalityServer.sin_port = htons(9000);
 
	if (connect(sock, (struct sockaddr *)&legalityServer, sizeof(legalityServer)) < 0) {
		close(sock);
		infoDisp(i18n(S_SOCKET_CONNECT_FAILED));
		return;		
	}
	
	fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);
	if (send(sock, message, 239, 0) < 0) {
		close(sock);
		infoDisp(i18n(S_SOCKET_SEND_FAILED));
		return;
	}

	checkingLegality = true;
	
	close(sock);
}

void process_bank(u8* buf, int game) {
	server.client_id = accept(server.server_id, (struct sockaddr *) &server.client_addr, &server.client_length);
	if (server.client_id < 0 && errno != EAGAIN) {
		infoDisp(i18n(S_HTTP_ERROR_PROCESSING_PHASE));
		socket_close();
		return;		
	} else {
		panic = 0;
		int boxmax = ISGEN6 ? 30 : 31;
		int box = 0, slot = 0;
		char *dummy;
		memset(payload, 0, PAYLOADSIZE);
		
		u8 tmp[PKMNLENGTH];
		memcpy(tmp, &buf[box*30*PKMNLENGTH + slot*PKMNLENGTH], PKMNLENGTH);
		while(pkx_get_species(tmp) && (panic < boxmax * 30)) {
			slot++;
			if (slot == 30) {
				box++;
				slot = 0;
			}
			if (box > boxmax)
				box = 0;
			panic++;
			
			memcpy(tmp, &buf[box*30*PKMNLENGTH + slot*PKMNLENGTH], PKMNLENGTH);
		}

		fcntl(server.client_id, F_SETFL, fcntl(server.client_id, F_GETFL, 0) & ~O_NONBLOCK);
        recv(server.client_id, payload, PAYLOADSIZE, 0);
        if (strstr(payload, "PKSMOTA") != NULL && (hidKeysDown() != KEY_B)) {
            u8 pkmn[PKMNLENGTH];
            dummy = strstr(payload, "PKSMOTA");
            memcpy(pkmn, &dummy[7], PKMNLENGTH);
			memcpy(&buf[box*30*PKMNLENGTH + slot*PKMNLENGTH], pkmn, PKMNLENGTH);
        }
    }
    close(server.client_id);
    server.client_id = -1;
}

bool socket_check_valid_ip_address(char *ipAddress) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result != 0;
}

bool socket_is_legality_address_set() {
	return isLegalityAddressSet;
}

void socket_set_legality_address(bool value) {
	if (isLegalityAddressSet)
		isLegalityAddressSet = value;
	else {
		SwkbdState swkbd;
		memset(legalityAddress, 0, 16);

		swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 2, 15);
		swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, SWKBD_FILTER_DIGITS | SWKBD_FILTER_AT | SWKBD_FILTER_PERCENT | SWKBD_FILTER_BACKSLASH | SWKBD_FILTER_PROFANITY, 12);
		i18n_initTextSwkbd(&swkbd, S_EDITOR_TEXT_CANCEL, S_EDITOR_TEXT_SET, S_EDITOR_TEXT_ENTER_LEGALITY_ADDRESS);

		swkbdInputText(&swkbd, legalityAddress, 16);
		legalityAddress[15] = '\0';	

		if (!socket_check_valid_ip_address(legalityAddress)) {
			memset(legalityAddress, 0, 16);
		} else {
			isLegalityAddressSet = true;
		}
	}
}