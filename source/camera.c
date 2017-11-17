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

/*
*   This file is part of Anemone3DS
*   Copyright (C) 2016-2017 Alex Taber ("astronautlevel"), Dawid Eckert ("daedreth")
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include "camera.h"

static bool qr_mode = false;
static u32 transfer_size;
static Handle event;
static struct quirc* context;

bool camera_get_qrmode(void) { return qr_mode; }
void camera_set_qrmode(bool mode) { qr_mode = mode; }

void camera_init(void)
{
    camInit();
    CAMU_SetSize(SELECT_OUT1_OUT2, SIZE_CTR_TOP_LCD, CONTEXT_A);
    CAMU_SetOutputFormat(SELECT_OUT1_OUT2, OUTPUT_RGB_565, CONTEXT_A);
    CAMU_SetFrameRate(SELECT_OUT1_OUT2, FRAME_RATE_10);

    CAMU_SetNoiseFilter(SELECT_OUT1_OUT2, true);
    CAMU_SetAutoExposure(SELECT_OUT1_OUT2, true);
    CAMU_SetAutoWhiteBalance(SELECT_OUT1_OUT2, true);
    CAMU_SetTrimming(PORT_CAM1, false);
    CAMU_SetTrimming(PORT_CAM2, false);

    CAMU_GetMaxBytes(&transfer_size, 400, 240);
    CAMU_SetTransferBytes(PORT_BOTH, transfer_size, 400, 240);

    CAMU_Activate(SELECT_OUT1_OUT2);
    event = 0;

    CAMU_ClearBuffer(PORT_BOTH);
    CAMU_SynchronizeVsyncTiming(SELECT_OUT1, SELECT_OUT2);
    CAMU_StartCapture(PORT_BOTH);

    context = quirc_new();
    quirc_resize(context, 400, 240);
}

void camera_exit(void)
{
    CAMU_StopCapture(PORT_BOTH);
    CAMU_Activate(SELECT_NONE);
    camExit();
    quirc_destroy(context);
}

void camera_scan_qr(u16 *buf, u8* payload, int mode)
{
    int w;
    int h;

    u8 *image = (u8*)quirc_begin(context, &w, &h);

    for (ssize_t x = 0; x < w; x++)
    {
        for (ssize_t y = 0; y < h; y++)
        {
            u16 px = buf[y * 400 + x];
            image[y * w + x] = (u8)(((((px >> 11) & 0x1F) << 3) + (((px >> 5) & 0x3F) << 2) + ((px & 0x1F) << 3)) / 3);
        }
    }

    quirc_end(context);

    if (quirc_count(context) > 0)
    {
        struct quirc_code code;
        struct quirc_data data;
        quirc_extract(context, 0, &code);
        if (!quirc_decode(&code, &data))
        {
			if (mode == MODE_WCX)
			{
				size_t outlen;
				static const int headerSize = strlen("http://lunarcookies.github.io/wc.html#");
				unsigned char* out = base64_decode((char*)data.payload + headerSize, data.payload_len - headerSize, &outlen);
				
				if (outlen == ofs.wondercardSize)
				{
					memcpy(payload, out, outlen);
				}
			}
			else if (mode == MODE_PKX)
			{
				if (data.payload_len != 0x1A2)
				{
					return;
				}
				
				int box = *(u32*)(data.payload + 8);
				int slot = *(u32*)(data.payload + 12);
				int copies = *(u32*)(data.payload + 16);
				int startaddress = pkx_get_save_address(box, slot);
				
				// TODO: check checksums
				for (int i = 0; i < copies; i++)
				{
					if (startaddress < ofs.boxSize)
					{
						memcpy(payload + startaddress + i*ofs.pkxLength, data.payload + 0x30, ofs.pkxLength);
					}
				}
			}
			qr_mode = false;
        }
    }
}

static void draw_qr_rect(int x, int y, int w, u32 color)
{
	int center = w/3;
	int bord = 0.4*center;
	
	pp2d_draw_rectangle(           x,            y,    w,       bord, color); // top
	pp2d_draw_rectangle(           x,     y + bord, bord, w - 2*bord, color); // left
	pp2d_draw_rectangle(x + w - bord,     y + bord, bord, w - 2*bord, color); // right
	pp2d_draw_rectangle(           x, y + w - bord,    w,       bord, color); // bottom
	
	pp2d_draw_rectangle(x + center, y + center, center, center, color);
}

void camera_take_qr(u8* payload, int mode)
{
    u16 *buf = malloc(sizeof(u16) * 400 * 240 * 4);
    if (buf == NULL) return;
    CAMU_SetReceiving(&event, buf, PORT_CAM1, 240 * 400 * 2, transfer_size);
    svcWaitSynchronization(event, U64_MAX);
    svcCloseHandle(event);
	
    pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		u32 *rgba8_buf = malloc(240 * 400 * sizeof(u32));
		if (rgba8_buf == NULL) return;
		for (int i = 0; i < 240 * 400; i++)
		{
			rgba8_buf[i] = RGB565_TO_ABGR8(buf[i]);
		}
		pp2d_load_texture_memory(TEXTURE_QR, rgba8_buf, 400, 240);
		pp2d_draw_texture(TEXTURE_QR, 0, 0);
		
		if (mode == MODE_WCX)
		{
			draw_qr_rect(102, 18, 24, WHITE);
			draw_qr_rect(102, 198, 24, WHITE);
			draw_qr_rect(284, 18, 24, WHITE);
		}
		else if (mode == MODE_PKX)
		{
			draw_qr_rect(97, 12, 19, WHITE);
			draw_qr_rect(289, 12, 19, WHITE);
			draw_qr_rect(97, 206, 19, WHITE);			
		}

		pp2d_draw_text_center(GFX_TOP, 101, 1.3f, 1.3f, WHITE, "\uE01E \uE004");
    pp2d_end_draw();
    
	free(rgba8_buf);
    pp2d_free_texture(TEXTURE_QR);
	
    hidScanInput();
    u32 kDown = hidKeysDown();
    if (kDown & KEY_L)
    {
        CAMU_StopCapture(PORT_BOTH);
        CAMU_Activate(SELECT_NONE);
        camera_scan_qr(buf, payload, mode);
        CAMU_Activate(SELECT_OUT1_OUT2);
        CAMU_StartCapture(PORT_BOTH);
    }
    if (kDown & KEY_B)
    {
        camera_exit();
        qr_mode = false;
    }

    free(buf);
}