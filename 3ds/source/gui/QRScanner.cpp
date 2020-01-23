/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2020 Bernardo Giordano, Admiral Fish, piepie62
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
#include "quirc/quirc.h"
#include <atomic>

namespace
{
    class QRData
    {
    public:
        QRData() : image{new C3D_Tex, &subtex}, data(quirc_new())
        {
            std::fill(cameraBuffer.begin(), cameraBuffer.end(), 0);
            C3D_TexInit(image.tex, 512, 256, GPU_RGB565);
            C3D_TexSetFilter(image.tex, GPU_LINEAR, GPU_LINEAR);
            image.tex->border = 0xFFFFFFFF;
            C3D_TexSetWrap(image.tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
            LightLock_Init(&bufferLock);
            LightLock_Init(&imageLock);
            svcCreateEvent(&exitEvent, RESET_STICKY);
            quirc_resize(data, 400, 240);
        }
        ~QRData()
        {
            C3D_TexDelete(image.tex);
            delete image.tex;
            quirc_destroy(data);
            svcCloseHandle(exitEvent);
        }
        void drawThread();
        void captureThread();
        void handler(std::vector<u8>& out);
        bool done() { return finished; }
        bool cancelled() { return cancel; }

    private:
        void buffToImage();
        void finish();
        std::array<u16, 400 * 240> cameraBuffer;
        LightLock bufferLock;
        C2D_Image image;
        LightLock imageLock;
        quirc* data;
        Handle exitEvent;
        static constexpr Tex3DS_SubTexture subtex = {512, 256, 0.0f, 1.0f, 1.0f, 0.0f};
        std::atomic<bool> finished                = false;
        bool capturing                            = false;
        bool cancel                               = false;
    };

    void drawHelp(void* arg)
    {
        QRData* data = (QRData*)arg;
        data->drawThread();
    }

    void captureHelp(void* arg)
    {
        QRData* data = (QRData*)arg;
        data->captureThread();
    }
}

void QRData::buffToImage()
{
    LightLock_Lock(&bufferLock);
    for (u32 x = 0; x < 400; x++)
    {
        for (u32 y = 0; y < 240; y++)
        {
            u32 dstPos = ((((y >> 3) * (512 >> 3) + (x >> 3)) << 6) +
                             ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) *
                         2;
            u32 srcPos = (y * 400 + x) * 2;
            memcpy(((u8*)image.tex->data) + dstPos, ((u8*)cameraBuffer.data()) + srcPos, 2);
        }
    }
    LightLock_Unlock(&bufferLock);
}

void QRData::finish()
{
    svcSignalEvent(exitEvent);
    while (!done())
        svcSleepThread(1000000);
    LightLock_Lock(&bufferLock);
    LightLock_Unlock(&bufferLock);
    LightLock_Lock(&imageLock);
    LightLock_Unlock(&imageLock);
}

void QRData::drawThread()
{
    LightLock_Lock(&imageLock);
    while (aptMainLoop() && !done())
    {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        buffToImage();

        Gui::target(GFX_TOP);
        Gui::drawImageAt(image, 0, 0, nullptr, 1.0f, 1.0f);

        Gui::target(GFX_BOTTOM);
        Gui::backgroundBottom(false);
        Gui::backgroundAnimatedBottom();
        Gui::drawSolidRect(0, 0, 320.0f, 240.0f, COLOR_MASKBLACK);
        Gui::text(i18n::localize("SCANNER_EXIT"), 160, 115, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
        Gui::flushText();

        if (!aptIsHomeAllowed() && aptIsHomePressed())
        {
            Gui::setDoHomeDraw();
        }

        Gui::drawNoHome();

        C3D_FrameEnd(0);
        Gui::frameClean();
    }
    LightLock_Unlock(&imageLock);
}

void QRData::captureThread()
{
    Handle events[3] = {0};
    events[0]        = exitEvent;
    u32 transferUnit;

    u16* buffer = new u16[400 * 240];
    camInit();
    CAMU_SetSize(SELECT_OUT1, SIZE_CTR_TOP_LCD, CONTEXT_A);
    CAMU_SetOutputFormat(SELECT_OUT1, OUTPUT_RGB_565, CONTEXT_A);
    CAMU_SetFrameRate(SELECT_OUT1, FRAME_RATE_30);
    CAMU_SetNoiseFilter(SELECT_OUT1, true);
    CAMU_SetAutoExposure(SELECT_OUT1, true);
    CAMU_SetAutoWhiteBalance(SELECT_OUT1, true);
    CAMU_SetPhotoMode(SELECT_OUT1, PHOTO_MODE_LETTER);
    // No clue if this is actually effective or if it's just a placebo effect, but it seems to help?
    CAMU_SetSharpness(SELECT_OUT1, 127);
    CAMU_Activate(SELECT_OUT1);
    CAMU_GetBufferErrorInterruptEvent(&events[2], PORT_CAM1);
    CAMU_SetTrimming(PORT_CAM1, false);
    CAMU_GetMaxBytes(&transferUnit, 400, 240);
    CAMU_SetTransferBytes(PORT_CAM1, transferUnit, 400, 240);
    CAMU_ClearBuffer(PORT_CAM1);
    CAMU_SetReceiving(&events[1], buffer, PORT_CAM1, 400 * 240 * sizeof(u16), (s16)transferUnit);
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
                LightLock_Lock(&bufferLock);
                memcpy(cameraBuffer.data(), buffer, 400 * 240 * sizeof(u16));
                GSPGPU_FlushDataCache(cameraBuffer.data(), 400 * 240 * sizeof(u16));
                LightLock_Unlock(&bufferLock);
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
    delete[] buffer;
    for (int i = 1; i < 3; i++)
    {
        if (events[i] != 0)
        {
            svcCloseHandle(events[i]);
            events[i] = 0;
        }
    }
    finished = true;
}

void QRData::handler(std::vector<u8>& out)
{
    hidScanInput();
    if (hidKeysDown() & KEY_B)
    {
        cancel = true;
        finish();
        return;
    }

    if (!capturing)
    {
        // create cam thread
        if (threadCreate((ThreadFunc)&captureHelp, this, 0x10000, 0x1A, 1, true) != NULL)
        {
            capturing = true;
        }
        else
        {
            finish();
            return;
        }
    }

    if (done())
    {
        return;
    }

    int w, h;
    u8* image = (u8*)quirc_begin(data, &w, &h);
    LightLock_Lock(&bufferLock);
    for (ssize_t x = 0; x < w; x++)
    {
        for (ssize_t y = 0; y < h; y++)
        {
            u16 px           = cameraBuffer[y * 400 + x];
            image[y * w + x] = (u8)(((((px >> 11) & 0x1F) << 3) + (((px >> 5) & 0x3F) << 2) + ((px & 0x1F) << 3)) / 3);
        }
    }
    LightLock_Unlock(&bufferLock);
    quirc_end(data);
    if (quirc_count(data) > 0)
    {
        struct quirc_code code;
        struct quirc_data scan_data;
        quirc_extract(data, 0, &code);
        if (!quirc_decode(&code, &scan_data))
        {
            finish();
            out.resize(scan_data.payload_len);
            std::copy(scan_data.payload, scan_data.payload + scan_data.payload_len, out.begin());
        }
    }
}

std::vector<u8> QR_Internal::scan()
{
    std::vector<u8> out          = {};
    std::unique_ptr<QRData> data = std::make_unique<QRData>();
    aptSetHomeAllowed(false);
    threadCreate((ThreadFunc)&drawHelp, data.get(), 0x10000, 0x1A, 1, true);
    while (!data->done())
    {
        data->handler(out);
    }
    aptSetHomeAllowed(true);
    return out;
}
