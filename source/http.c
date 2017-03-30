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

#include "http.h"

int panic = 0;
static u32			*socket_buffer = NULL;
static http_server	data;
http_server			*app_data = &data;
static char			payload[PAYLOADSIZE];

void drawIP(sftd_font *fontBold9) {
	sftd_draw_wtextf(fontBold9, 10, 220, WHITE, 9, i18n(S_HTTP_SERVER_RUNNING), inet_ntoa(data.server_addr.sin_addr));
}

void shutDownSoc() {
	close(data.server_id);
	socExit();
}

void closeOnExit() {
	if (app_data->server_id > 0) 
		close(app_data->server_id);
	if (app_data->client_id > 0) 
		close(app_data->client_id);	
}

int init() {
	socket_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);
	if (socket_buffer == NULL) {
		infoDisp(i18n(S_HTTP_BUFFER_ALLOC_FAILED));
		closeOnExit();
		return 0;
	}
	if (socInit(socket_buffer, SOC_BUFFERSIZE)) {
		infoDisp(i18n(S_HTTP_SOCINIT_FAILED));
		closeOnExit();
		return 0;
	}
	
	// Make sure the struct is clear
	memset(&data, 0, sizeof(data));
	data.client_id = -1;
	data.server_id = -1;
	data.server_id = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	// Is socket accessible?
	if (data.server_id < 0) {
		infoDisp(i18n(S_HTTP_SOCKET_UNACCESSIBLE));
		closeOnExit();
		return 0;
	}
	
	data.server_addr.sin_family = AF_INET;
	data.server_addr.sin_port = htons(9000);
	data.server_addr.sin_addr.s_addr = gethostid();
	data.client_length = sizeof(data.client_addr);
	
	if (bind(data.server_id, (struct sockaddr *) &data.server_addr, sizeof(data.server_addr))) {
		close(data.server_id);
		infoDisp(i18n(S_HTTP_BINDING_FAILED));
		closeOnExit();
		return 0;
	}
	
	// Set socket non blocking so we can still read input to exit
	fcntl(data.server_id, F_SETFL, fcntl(data.server_id, F_GETFL, 0) | O_NONBLOCK);

	if (listen(data.server_id, 5)) {
		infoDisp(i18n(S_HTTP_LISTENING_FAILED));
		closeOnExit();
		return 0;		
	}
	return 1;
}

void processing(u8* mainbuf, int game, int tempVett[]) {
	data.client_id = accept(data.server_id, (struct sockaddr *) &data.client_addr, &data.client_length);
	if (data.client_id < 0 && errno != EAGAIN) {
		infoDisp(i18n(S_HTTP_ERROR_PROCESSING_PHASE));
		closeOnExit();
		return;		
	} else {
		panic = 0;
		int boxmax = (game < 4) ? 30 : 31;
		char *dummy;
		memset(payload, 0, PAYLOADSIZE);
		// set client socket to blocking to simplify sending data back
		fcntl(data.client_id, F_SETFL, fcntl(data.client_id, F_GETFL, 0) & ~O_NONBLOCK);

        recv(data.client_id, payload, PAYLOADSIZE, 0);
        if (strstr(payload,"PKSMOTA")!=NULL && (hidKeysDown() != KEY_B)) {
            u8 pkmn[PKMNLENGTH];
            dummy = strstr(payload,"PKSMOTA");
            memcpy(pkmn, &dummy[7], PKMNLENGTH);
            setPkmn(mainbuf, tempVett[0], tempVett[1], pkmn, game);

            do {
                tempVett[1]++;
                if (tempVett[1] == 30) {
                    tempVett[0]++;
                    tempVett[1] = 0;
                }
                if (tempVett[0] > boxmax)
                    tempVett[0] = 0;

                getPkmn(mainbuf, tempVett[0], tempVett[1], pkmn, game);
                panic++;
            } while (getPokedexNumber(pkmn) && (panic < boxmax * 30));
        }
    }
    close(data.client_id);
    data.client_id = -1;
}

Result downloadFile(char* url, char* path) {
    httpcInit(0);
    httpcContext context;
    u32 statuscode = 0;
    u32 contentsize = 0;
    u8 *buf;

    if (httpcOpenContext(&context, HTTPC_METHOD_GET, url, 0)) {
        infoDisp(i18n(S_HTTP_HTTP_CONTEXT_OPEN_FAILED));
		infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
        return -1;
    }

    if (httpcAddRequestHeaderField(&context, "User-Agent", "PKSM")) {
        infoDisp(i18n(S_HTTP_ADD_REQUEST_HEADER_FIELD_FAILED));
		infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
        return -1;
    }
	
    if (httpcSetSSLOpt(&context, SSLCOPT_DisableVerify)) {
        infoDisp(i18n(S_HTTP_SET_SSLOPT_FAILED));
		infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
        return -1;
    }
	
	httpcAddRequestHeaderField(&context, "Connection", "Keep-Alive");

    if (httpcBeginRequest(&context)) {
        infoDisp(i18n(S_HTTP_BEGIN_HTTP_REQUEST_FAILED));
		infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
        return -1;
    }

    if (httpcGetResponseStatusCode(&context, &statuscode)) {
        infoDisp(i18n(S_HTTP_RECEIVE_STATUS_CODE_FAILED));
		infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
        httpcCloseContext(&context);
        return -1;
    }

    if (statuscode != 200) {
        if (statuscode >= 300 && statuscode < 400) {
            char newUrl[1024];
            if (httpcGetResponseHeader(&context, (char*)"Location", newUrl, 1024)) {
                infoDisp(i18n(S_HTTP_REDIRECTION_FAILED));
				infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
                return -1;
            }
            httpcCloseContext(&context);
            downloadFile(newUrl, path);
            return -1;
        } else {
            infoDisp(i18n(S_HTTP_REDIRECTION_FAILED));
			infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
            httpcCloseContext(&context);
            return -1;
        }
    }

    if (httpcGetDownloadSizeState(&context, NULL, &contentsize)) {
        infoDisp(i18n(S_HTTP_RECEIVE_DOWNLOAD_SIZE_FAILED));
		infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
        httpcCloseContext(&context);
        return -1;
    }

    buf = (u8*)malloc(contentsize);
    if (buf == NULL) {
		free(buf);
        infoDisp(i18n(S_HTTP_ALLOC_MEMORY_FAILED));
		infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
        return -1;
    }
    memset(buf, 0, contentsize);

    if (httpcDownloadData(&context, buf, contentsize, NULL)) {
        free(buf);
		infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
        httpcCloseContext(&context);
        return -1;
    }

    remove(path);
    FILE *fptr = fopen(path, "wb");
    fwrite(buf, 1, contentsize, fptr);
    fclose(fptr);
    free(buf);

    httpcCloseContext(&context);
    httpcExit();
    return 0;
}
