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
#include "PGF.hpp"
#include "PGT.hpp"
#include "PK4.hpp"
#include "PK5.hpp"
#include "PK6.hpp"
#include "PK7.hpp"
#include "WC6.hpp"
#include "WC7.hpp"
#include "loader.hpp"
#include "base64.hpp"
#include "gui.hpp"
#include "quirc/quirc.h"

typedef struct
{
    u16* camera_buffer;
    Handle mutex;
    volatile bool finished;
    Handle cancel;
    bool capturing;
    struct quirc* context;
    C3D_Tex* tex;
    C2D_Image image;
} qr_data;

class QRData
{
public:
    QRData() : image{new C3D_Tex, &subtex}, data(quirc_new())
    {
        std::fill(cameraBuffer.begin(), cameraBuffer.end(), 0);
        C3D_TexInit(image.tex, 512, 256, GPU_RGB565);
        C3D_TexSetFilter(image.tex, GPU_LINEAR, GPU_LINEAR);
        svcCreateMutex(&bufferMutex, false);
        svcCreateMutex(&imageMutex, false);
        svcCreateEvent(&exitEvent, RESET_STICKY);
        quirc_resize(data, 400, 240);
    }
    ~QRData()
    {
        C3D_TexDelete(image.tex);
        delete image.tex;
        quirc_destroy(data);
        svcCloseHandle(bufferMutex);
        svcCloseHandle(imageMutex);
        svcCloseHandle(exitEvent);
    }
    void drawThread();
    void captureThread();
    void handler(QRMode mode, std::vector<u8>& out);
    bool done() { return finished; }

private:
    void buffToImage();
    void finish();
    std::array<u16, 400 * 240> cameraBuffer;
    Handle bufferMutex;
    C2D_Image image;
    Handle imageMutex;
    quirc* data;
    Handle exitEvent;
    volatile bool finished                    = false;
    bool capturing                            = false;
    static constexpr Tex3DS_SubTexture subtex = {512, 256, 0.0f, 1.0f, 1.0f, 0.0f};
};

static void drawHelp(void* arg)
{
    QRData* data = (QRData*)arg;
    data->drawThread();
}

static void captureHelp(void* arg)
{
    QRData* data = (QRData*)arg;
    data->captureThread();
}

void QRData::buffToImage()
{
    svcWaitSynchronization(bufferMutex, U64_MAX);
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
    svcReleaseMutex(bufferMutex);
}

void QRData::finish()
{
    svcSignalEvent(exitEvent);
    while (!finished)
        svcSleepThread(1000000);
    svcWaitSynchronization(bufferMutex, U64_MAX);
    svcReleaseMutex(bufferMutex);
    svcWaitSynchronization(imageMutex, U64_MAX);
    svcReleaseMutex(imageMutex);
}

void QRData::drawThread()
{
    bool first = true;
    svcWaitSynchronization(imageMutex, U64_MAX);
    while (aptMainLoop() && !finished)
    {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        buffToImage();

        Gui::target(GFX_TOP);
        Gui::drawImageAt(image, 0, 0, nullptr, 1.0f, 1.0f);
        if (first)
        {
            Gui::target(GFX_BOTTOM);
            Gui::drawSolidRect(0, 0, 320.0f, 240.0f, COLOR_MASKBLACK);
            Gui::text(i18n::localize("SCANNER_EXIT"), 160, 115, FONT_SIZE_18, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
            first = false;
            Gui::flushText();
        }
        C3D_FrameEnd(0);
    }
    svcReleaseMutex(imageMutex);
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
                svcWaitSynchronization(bufferMutex, U64_MAX);
                memcpy(cameraBuffer.data(), buffer, 400 * 240 * sizeof(u16));
                GSPGPU_FlushDataCache(cameraBuffer.data(), 400 * 240 * sizeof(u16));
                svcReleaseMutex(bufferMutex);
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

void QRData::handler(QRMode mode, std::vector<u8>& out)
{
    hidScanInput();
    if (hidKeysDown() & KEY_B)
    {
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

    if (finished)
    {
        return;
    }

    int w, h;
    u8* image = (u8*)quirc_begin(data, &w, &h);
    svcWaitSynchronization(bufferMutex, U64_MAX);
    for (ssize_t x = 0; x < w; x++)
    {
        for (ssize_t y = 0; y < h; y++)
        {
            u16 px           = cameraBuffer[y * 400 + x];
            image[y * w + x] = (u8)(((((px >> 11) & 0x1F) << 3) + (((px >> 5) & 0x3F) << 2) + ((px & 0x1F) << 3)) / 3);
        }
    }
    svcReleaseMutex(bufferMutex);
    quirc_end(data);
    if (quirc_count(data) > 0)
    {
        struct quirc_code code;
        struct quirc_data scan_data;
        quirc_extract(data, 0, &code);
        if (!quirc_decode(&code, &scan_data))
        {
            finish();
            if (mode == WCX4)
            {
                static constexpr int wcHeader = 38; // strlen("http://lunarcookies.github.io/wc.html#)
                out                           = base64_decode(scan_data.payload + wcHeader, scan_data.payload_len - wcHeader);

                if (out.size() != PGT::length && out.size() != WC4::length)
                {
                    out.clear();
                }
            }
            else if (mode == WCX5)
            {
                static constexpr int wcHeader = 38; // strlen("http://lunarcookies.github.io/wc.html#)
                out                           = base64_decode((const char*)scan_data.payload + wcHeader, scan_data.payload_len - wcHeader);

                if (out.size() != PGF::length)
                {
                    out.clear();
                }
            }
            else if (mode == WCX6 || mode == WCX7)
            {
                static constexpr int wcHeader = 38; // strlen("http://lunarcookies.github.io/wc.html#)
                out                           = base64_decode((const char*)scan_data.payload + wcHeader, scan_data.payload_len - wcHeader);

                if (out.size() != WC6::length && out.size() != WC6::lengthFull)
                {
                    out.clear();
                }
            }
            else if (mode == PKM4)
            {
                static constexpr int pkHeader = 6; // strlen("null/#")
                out                           = base64_decode((const char*)scan_data.payload + pkHeader, scan_data.payload_len - pkHeader);

                if (out.size() != 136) // PK4/5 length
                {
                    out.clear();
                }
            }
            else if (mode == PKM5)
            {
                static constexpr int pkHeader = 6; // strlen("null/#")
                out                           = base64_decode((const char*)scan_data.payload + pkHeader, scan_data.payload_len - pkHeader);

                if (out.size() != 136) // PK4/5 length
                {
                    out.clear();
                }
            }
            else if (mode == PKM6)
            {
                static constexpr int pkHeader = 40; // strlen("http://lunarcookies.github.io/b1s1.html#")
                out                           = base64_decode((const char*)scan_data.payload + pkHeader, scan_data.payload_len - pkHeader);

                if (PKX::genFromBytes(out.data(), out.size(), true) != 6) // PK6 length
                {
                    out.clear();
                }
            }
            else if (mode == PKM7)
            {
                if (scan_data.payload_len != 0x1A2)
                {
                    return;
                }

                u32 box    = *(u32*)(scan_data.payload + 8);
                u32 slot   = *(u32*)(scan_data.payload + 12);
                u32 copies = *(u32*)(scan_data.payload + 16);

                if (box > 31)
                {
                    box = 31;
                }
                if (slot > 29)
                {
                    slot = 29;
                }

                if (copies > 1)
                {
                    if ((int)box < TitleLoader::save->maxBoxes() && slot < 30)
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
                            u32 tmpBox  = box + (slot + i) / 30;
                            if ((int)tmpBox < TitleLoader::save->maxBoxes() && tmpSlot < 30)
                            {
                                TitleLoader::save->pkm(pkx, tmpBox, tmpSlot, false);
                            }
                        }
                    }
                }
                else
                {
                    out.resize(232);
                    std::copy(scan_data.payload + 0x30, scan_data.payload + 0x30 + 232, out.begin());
                }
            }
            else if (mode == NUMBER)
            {
                out.resize(scan_data.payload_len + 1);
                out[scan_data.payload_len] = '\0';
                std::copy(scan_data.payload, scan_data.payload + scan_data.payload_len, out.begin());
            }
        }
    }
}

std::vector<u8> QRScanner::scan(QRMode mode)
{
    C3D_FrameEnd(0);
    std::vector<u8> out          = {};
    std::unique_ptr<QRData> data = std::make_unique<QRData>();
    threadCreate((ThreadFunc)&drawHelp, data.get(), 0x10000, 0x1A, 1, true);
    while (!data->done())
    {
        data->handler(mode, out);
    }
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    return out;
}
