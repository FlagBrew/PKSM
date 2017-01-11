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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include "editor.h"
#include "http.h"
#include "graphic.h"
#include "certs/cybertrust.h"
#include "certs/digicert.h"

int panic = 0;
static u32			*socket_buffer = NULL;
static http_server	data;
http_server			*app_data = &data;
static char			payload[PAYLOADSIZE];

void drawIP(sftd_font *fontBold9) {
	sftd_draw_textf(fontBold9, 16, 220, WHITE, 9, "Server running at http://%s:9000", inet_ntoa(data.server_addr.sin_addr));	
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
		infoDisp("Buffer allocation failed!");
		closeOnExit();
		return 0;
	}
	if (socInit(socket_buffer, SOC_BUFFERSIZE)) {
		infoDisp("socInit failed!");
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
		infoDisp("Socket unaccesible!");
		closeOnExit();
		return 0;
	}
	
	data.server_addr.sin_family = AF_INET;
	data.server_addr.sin_port = htons(9000);
	data.server_addr.sin_addr.s_addr = gethostid();
	data.client_length = sizeof(data.client_addr);
	
	if (bind(data.server_id, (struct sockaddr *) &data.server_addr, sizeof(data.server_addr))) {
		close(data.server_id);
		infoDisp("Binding failed!");
		closeOnExit();
		return 0;
	}
	
	// Set socket non blocking so we can still read input to exit
	fcntl(data.server_id, F_SETFL, fcntl(data.server_id, F_GETFL, 0) | O_NONBLOCK);

	if (listen(data.server_id, 5)) {
		infoDisp("Listening failed!");
		closeOnExit();
		return 0;		
	}
	return 1;
}

void processing(u8* mainbuf, int game, int tempVett[]) {
	data.client_id = accept(data.server_id, (struct sockaddr *) &data.client_addr, &data.client_length);
	if (data.client_id < 0 && errno != EAGAIN) {
		infoDisp("Error during processing phase!");
		closeOnExit();
		return;		
	} else {
		panic = 0;
		int boxmax = (game < 4) ? 30 : 31;
		char dummy[PAYLOADSIZE];
		memset(dummy, 0, PAYLOADSIZE);
		memset(payload, 0, PAYLOADSIZE);
		// set client socket to blocking to simplify sending data back
		fcntl(data.client_id, F_SETFL, fcntl(data.client_id, F_GETFL, 0) & ~O_NONBLOCK);

		recv(data.client_id, payload, PAYLOADSIZE, 0);
		if (memcmp(dummy, payload, PAYLOADSIZE)) {
			u8 pkmn[PKMNLENGTH];
			memcpy(pkmn, payload, PKMNLENGTH);
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

		close(data.client_id);
		data.client_id = -1;
	}
}

Result downloadFile(char* url, char* path, bool install) {
    httpcInit(0);
    httpcContext context;
    u32 statuscode = 0;
    u32 contentsize = 0;
    u8 *buf;

    if (httpcOpenContext(&context, HTTPC_METHOD_GET, url, 0)) {
        infoDisp("Failed to open http context!");
        return -1;
    }

    if (httpcAddRequestHeaderField(&context, "User-Agent", "PKSM")) {
        infoDisp("Failed to add a request header field!");
        return -1;
    }
	
    if (httpcSetSSLOpt(&context, 1 << 9)) {
        infoDisp("Failed to set SSLOpt!");
        return -1;
    }

    httpcAddTrustedRootCA(&context, cybertrust_cer, cybertrust_cer_len);
    httpcAddTrustedRootCA(&context, digicert_cer, digicert_cer_len);

    if (httpcBeginRequest(&context)) {
        infoDisp("Failed to begin a http request!");
        return -1;
    }

    if (httpcGetResponseStatusCode(&context, &statuscode)) {
        infoDisp("Failed to receive a status code!");
        httpcCloseContext(&context);
        return -1;
    }

    if (statuscode != 200) {
        if (statuscode >= 300 && statuscode < 400) {
            char newUrl[1024];
            if (httpcGetResponseHeader(&context, (char*)"Location", newUrl, 1024)) {
                infoDisp("Redirection failed!");
                return -1;
            }
            httpcCloseContext(&context);
            downloadFile(newUrl, path, install);
            return -1;
        } else {
            infoDisp("Redirection failed!");
            httpcCloseContext(&context);
            return -1;
        }
    }

    if (httpcGetDownloadSizeState(&context, NULL, &contentsize)) {
        infoDisp("Failed to receive download size!");
        httpcCloseContext(&context);
        return -1;
    }

    buf = (u8*)malloc(contentsize);
    if (buf == NULL) {
		free(buf);
        infoDisp("Failed to alloc memory!");
        return -1;
    }
    memset(buf, 0, contentsize);

    if (httpcDownloadData(&context, buf, contentsize, NULL)) {
        free(buf);
        infoDisp("Failed to download file!");
        httpcCloseContext(&context);
        return -1;
    }
	
	if (install) {
		amInit();
		Handle handle;
		AM_QueryAvailableExternalTitleDatabase(NULL);
		AM_StartCiaInstall(MEDIATYPE_SD, &handle);
		FSFILE_Write(handle, NULL, 0, buf, contentsize, 0);
		AM_FinishCiaInstall(handle);
		amExit();
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
