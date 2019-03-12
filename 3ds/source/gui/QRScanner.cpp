/*
*   This file is part of PKSM
*   Copyright (C) 2016-2019 Bernardo Giordano, Admiral Fish, piepie62
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

#include "QRScanner.hpp"
#include "WC7.hpp"
#include "WC6.hpp"
#include "PGT.hpp"
#include "PGF.hpp"
#include "PK4.hpp"
#include "PK5.hpp"
#include "PK6.hpp"
#include "PK7.hpp"
#include "loader.hpp"

static void qrHandler(qr_data*, QRMode, u8*&);
static void camThread(void*);
static void uiThread(void*);

static void qrHandler(qr_data* data, QRMode mode, u8*& buff)
{
    hidScanInput();
    if (hidKeysDown() & KEY_B)
    {
        QRScanner::exit(data);
        return;
    }

    if (!data->capturing)
    {
        // create cam thread
        data->mutex = 0;
        data->cancel = 0;
        svcCreateEvent(&data->cancel, RESET_STICKY);
        svcCreateMutex(&data->mutex, false);
        if (threadCreate(camThread, data, 0x10000, 0x1A, 1, true) != NULL)
        {
            data->capturing = true;
        }
        else
        {
            QRScanner::exit(data);
            return;
        }
    }

    if (data->finished)
    {
        QRScanner::exit(data);
        return;
    }

    int w, h;
    u8* image = (u8*)quirc_begin(data->context, &w, &h);
    svcWaitSynchronization(data->mutex, U64_MAX);
    for (ssize_t x = 0; x < w; x++)
    {
        for (ssize_t y = 0; y < h; y++)
        {
            u16 px = data->camera_buffer[y * 400 + x];
            image[y * w + x] = (u8)(((((px >> 11) & 0x1F) << 3) + (((px >> 5) & 0x3F) << 2) + ((px & 0x1F) << 3)) / 3);
        }
    }
    svcReleaseMutex(data->mutex);
    quirc_end(data->context);
    if (quirc_count(data->context) > 0)
    {
        struct quirc_code code;
        struct quirc_data scan_data;
        quirc_extract(data->context, 0, &code);  
        if (!quirc_decode(&code, &scan_data))
        {
            QRScanner::exit(data);
            if (mode == WCX4)
            {
                size_t outSize;
                static constexpr int wcHeader = 38; // strlen("http://lunarcookies.github.io/wc.html#)
                u8* out = base64_decode((const char*)scan_data.payload + wcHeader, scan_data.payload_len - wcHeader, &outSize);

                if (outSize == PGT::length || outSize == WC4::length)
                {
                    buff = new u8[outSize];
                    std::copy(out, out + outSize, buff);
                }

                free(out);
            }
            else if (mode == WCX5)
            {
                size_t outSize;
                static constexpr int wcHeader = 38; // strlen("http://lunarcookies.github.io/wc.html#)
                u8* out = base64_decode((const char*)scan_data.payload + wcHeader, scan_data.payload_len - wcHeader, &outSize);

                if (outSize == PGF::length)
                {
                    buff = new u8[outSize];
                    std::copy(out, out + outSize, buff);
                }

                free(out);
            }
            else if (mode == WCX6 || mode == WCX7)
            {
                size_t outSize;
                static constexpr int wcHeader = 38; // strlen("http://lunarcookies.github.io/wc.html#)
                u8* out = base64_decode((const char*)scan_data.payload + wcHeader, scan_data.payload_len - wcHeader, &outSize);

                if (outSize == WC6::length || outSize == WC6::lengthFull)
                {
                    buff = new u8[WC6::length];
                    int ofs = outSize == WC6::lengthFull ? 0x206 : 0;

                    std::copy(out + ofs, out + ofs + WC6::length, buff);
                }

                free(out);
            }
            else if (mode == PKM4)
            {
                size_t outSize;
                static constexpr int pkHeader = 6; // strlen("null/#")
                u8* out = base64_decode((const char*)scan_data.payload + pkHeader, scan_data.payload_len - pkHeader, &outSize);

                if (outSize == 136) // PK4/5 length
                {
                    buff = new u8[outSize];
                    std::copy(out, out + outSize, buff);
                }

                free(out);
            }
            else if (mode == PKM5)
            {
                size_t outSize;
                static constexpr int pkHeader = 6; // strlen("null/#")
                u8* out = base64_decode((const char*)scan_data.payload + pkHeader, scan_data.payload_len - pkHeader, &outSize);

                Gui::warn(std::to_string(outSize));

                if (outSize == 136) // PK4/5 length
                {
                    buff = new u8[outSize];
                    std::copy(out, out + outSize, buff);
                }

                free(out);
            }
            else if (mode == PKM6)
            {
                size_t outSize;
                static constexpr int pkHeader = 40; // strlen("http://lunarcookies.github.io/b1s1.html#")
                u8* out = base64_decode((const char*)scan_data.payload + pkHeader, scan_data.payload_len - pkHeader, &outSize);

                if (PKX::genFromBytes(out, outSize, true) == 6) // PK6 length
                {
                    buff = new u8[outSize];
                    std::copy(out, out + outSize, buff);
                }

                free(out);
            }
            else if (mode == PKM7)
            {
                if (scan_data.payload_len != 0x1A2)
                {
                    return;
                }

                u32 box = *(u32*)(scan_data.payload + 8);
                u32 slot = *(u32*)(scan_data.payload + 12);
                u32 copies = *(u32*)(scan_data.payload + 16);

                if (copies > 1)
                {
                    if ((int) box < TitleLoader::save->maxBoxes() && slot < 30)
                    {
                        std::shared_ptr<PKX> pkx;
                        if (mode == PKM6)
                        {
                            pkx = std::make_shared<PK6>(scan_data.payload + 0x30, true);
                        }
                        else
                        {
                            pkx = std::make_shared<PK7>(scan_data.payload + 0x30, true);
                        }
                        for (u32 i = 0; i < copies; i++)
                        {
                            u32 tmpSlot = (slot + i) % 30;
                            u32 tmpBox = box + (slot + i) / 30;
                            if ((int) tmpBox < TitleLoader::save->maxBoxes() && tmpSlot < 30)
                            {
                                TitleLoader::save->pkm(*pkx, tmpBox, tmpSlot);
                            }
                        }
                    }
                }
                else
                {
                    buff = new u8[232]; // PK7 size
                    std::copy(scan_data.payload + 0x30, scan_data.payload + 0x30 + 232, buff);
                }
            }
        }
    }
}

static void camThread(void *arg) 
{
    qr_data* data = (qr_data*) arg;
    Handle events[3] = {0};
    events[0] = data->cancel;
    u32 transferUnit;

    u16* buffer = (u16*)malloc(400 * 240 * sizeof(u16));
    camInit();
    CAMU_SetSize(SELECT_OUT1, SIZE_CTR_TOP_LCD, CONTEXT_A);
    CAMU_SetOutputFormat(SELECT_OUT1, OUTPUT_RGB_565, CONTEXT_A);
    CAMU_SetFrameRate(SELECT_OUT1, FRAME_RATE_30);
    CAMU_SetNoiseFilter(SELECT_OUT1, true);
    CAMU_SetAutoExposure(SELECT_OUT1, true);
    CAMU_SetAutoWhiteBalance(SELECT_OUT1, true);
    CAMU_Activate(SELECT_OUT1);
    CAMU_GetBufferErrorInterruptEvent(&events[2], PORT_CAM1);
    CAMU_SetTrimming(PORT_CAM1, false);
    CAMU_GetMaxBytes(&transferUnit, 400, 240);
    CAMU_SetTransferBytes(PORT_CAM1, transferUnit, 400, 240);
    CAMU_ClearBuffer(PORT_CAM1);
    CAMU_SetReceiving(&events[1], buffer, PORT_CAM1, 400 * 240 * sizeof(u16), (s16) transferUnit);
    CAMU_StartCapture(PORT_CAM1);
    bool cancel = false;
    while (!cancel) 
    {
        s32 index = 0;
        svcWaitSynchronizationN(&index, events, 3, false, U64_MAX);
        switch (index)
        {
            case 0:
                cancel = true;
                break;
            case 1:
                svcCloseHandle(events[1]);
                events[1] = 0;
                svcWaitSynchronization(data->mutex, U64_MAX);
                memcpy(data->camera_buffer, buffer, 400 * 240 * sizeof(u16));
                GSPGPU_FlushDataCache(data->camera_buffer, 400 * 240 * sizeof(u16));
                svcReleaseMutex(data->mutex);
                CAMU_SetReceiving(&events[1], buffer, PORT_CAM1, 400 * 240 * sizeof(u16), transferUnit);
                break;
            case 2:
                svcCloseHandle(events[1]);
                events[1] = 0;
                CAMU_ClearBuffer(PORT_CAM1);
                CAMU_SetReceiving(&events[1], buffer, PORT_CAM1, 400 * 240 * sizeof(u16), transferUnit);
                CAMU_StartCapture(PORT_CAM1);
                break;
            default:
                break;
        }
    }

    CAMU_StopCapture(PORT_CAM1);

    bool busy = false;
    while (R_SUCCEEDED(CAMU_IsBusy(&busy, PORT_CAM1)) && busy)
    {
        svcSleepThread(1000000);
    }

    CAMU_ClearBuffer(PORT_CAM1);
    CAMU_Activate(SELECT_NONE);
    camExit();
    free(buffer);
    for (int i = 0; i < 3; i++)
    {
        if (events[i] != 0)
        {
            svcCloseHandle(events[i]);
            events[i] = 0;
        }
    }
    data->finished = true;
}

static void uiThread(void* arg)
{
    static bool first = true;
    qr_data* data = (qr_data*) arg;
    while (true)
    {
        svcWaitSynchronization(data->mutex, U64_MAX);
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        for (u32 x = 0; x < 400; x++)
        {
            for (u32 y = 0; y < 240; y++)
            {
                if (data->finished || data->tex == NULL || data->image.tex->data == NULL)
                {
                    svcReleaseMutex(data->mutex);
                    first = true;
                    return;
                }
                u32 dstPos = ((((y >> 3) * (512 >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) * 2;
                u32 srcPos = (y * 400 + x) * 2;
                memcpy((u8*)data->image.tex->data + dstPos, (u8*)data->camera_buffer + srcPos, 2);
            }
        }
        svcReleaseMutex(data->mutex);

        C2D_SceneBegin(g_renderTargetTop);
        C2D_DrawImageAt(data->image, 0.0f, 0.0f, 0.5f, NULL, 1.0f, 1.0f);
        if (first)
        {
            C2D_SceneBegin(g_renderTargetBottom);
            C2D_DrawRectSolid(0, 0, 0.5f, 320.0f, 240.0f, COLOR_MASKBLACK);
            Gui::staticText(i18n::localize("SCANNER_EXIT"), 160, 115, FONT_SIZE_18, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
            first = false;
        }
        C3D_FrameEnd(0);
    }
}

void QRScanner::init(QRMode mode, u8*& buff)
{
    // init qr_data struct variables
    qr_data* data = (qr_data*)malloc(sizeof(qr_data));
    data->capturing = false;
    data->finished = false;
    data->context = quirc_new();
    quirc_resize(data->context, 400, 240);
    data->camera_buffer = (u16*)calloc(1, 400 * 240 * sizeof(u16));
    data->tex = (C3D_Tex*)malloc(sizeof(C3D_Tex));
    static const Tex3DS_SubTexture subt3x = { 512, 256, 0.0f, 1.0f, 1.0f, 0.0f };
    data->image = (C2D_Image){ data->tex, &subt3x };
    C3D_TexInit(data->image.tex, 512, 256, GPU_RGB565);
    C3D_TexSetFilter(data->image.tex, GPU_LINEAR, GPU_LINEAR);

    threadCreate(uiThread, data, 0x10000, 0x1A, 1, true);
    while (!data->finished)
    {
        qrHandler(data, mode, buff);
    }
}

void QRScanner::exit(qr_data *data)
{
    svcSignalEvent(data->cancel);
    while (!data->finished)
    {
        svcSleepThread(1000000);
    }
    data->capturing = false;
    svcWaitSynchronization(data->mutex, U64_MAX); // Wait for the end of the 
    svcReleaseMutex(data->mutex);
    svcCloseHandle(data->mutex);
    C3D_TexDelete(data->tex);
    free(data->camera_buffer);
    free(data->tex);
    data->tex = NULL;
    quirc_destroy(data->context);
    free(data);
}