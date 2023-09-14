/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2022 Bernardo Giordano, Admiral Fish, piepie62
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
#include "colors.hpp"
#include "DataMutex.hpp"
#include "gui.hpp"
#include "quirc/quirc.h"
#include "thread.hpp"
#include <3ds.h>
#include <atomic>

namespace
{
    consteval size_t camera_width_for_size(CAMU_Size camera_size)
    {
        switch (camera_size)
        {
            case SIZE_VGA:
                return 640;
            case SIZE_QVGA:
                return 320;
            case SIZE_QQVGA:
                return 160;
            case SIZE_CIF:
                return 352;
            case SIZE_QCIF:
                return 176;
            case SIZE_DS_LCD:
                return 256;
            case SIZE_DS_LCDx4:
                return 512;
            case SIZE_CTR_TOP_LCD:
                return 400;
        }
        __unreachable();
        return 0;
    }

    consteval size_t camera_height_for_size(CAMU_Size camera_size)
    {
        switch (camera_size)
        {
            case SIZE_VGA:
                return 480;
            case SIZE_QVGA:
                return 240;
            case SIZE_QQVGA:
                return 120;
            case SIZE_CIF:
                return 288;
            case SIZE_QCIF:
                return 144;
            case SIZE_DS_LCD:
                return 192;
            case SIZE_DS_LCDx4:
                return 384;
            case SIZE_CTR_TOP_LCD:
                return 240;
        }
        __unreachable();
        return 0;
    }

    constexpr float camera_scale_for_size(CAMU_Size camera_size)
    {
        switch (camera_size)
        {
            case SIZE_VGA:
                return 0.5f;
            case SIZE_QVGA:
                return 1.0f;
            case SIZE_QQVGA:
                return 2.0f;
            case SIZE_CIF:
                return 0.75f;
            case SIZE_QCIF:
                return 1.5f;
            case SIZE_DS_LCD:
                return 1.0f;
            case SIZE_DS_LCDx4:
                return 0.75f;
            case SIZE_CTR_TOP_LCD:
                return 1.0f;
        }
        __unreachable();
        return 0;
    }

    template <CAMU_Size Size>
    class QRData
    {
    public:
        QRData() : cameraBuffer(), image{&tex, &subtex}, data(quirc_new())
        {
            std::ranges::fill(cameraBuffer.lock().get(), 0);
            auto curImage = image.lock();
            C3D_TexInit(curImage->tex, image_width, image_height, GPU_RGB565);
            C3D_TexSetFilter(curImage->tex, GPU_LINEAR, GPU_LINEAR);
            curImage->tex->border = 0xFFFFFFFF;
            C3D_TexSetWrap(curImage->tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
            svcCreateEvent(&exitEvent, RESET_STICKY);
            quirc_resize(data, camera_width, camera_height);

            LightSemaphore_Init(&letHandlerRun, 10, 10);
        }

        ~QRData()
        {
            C3D_TexDelete(image.lock()->tex);
            quirc_destroy(data);
            svcCloseHandle(exitEvent);
        }

        void drawThread();
        void captureThread();
        void handler(std::vector<u8>& out);

        bool done() { return finished; }

        bool cancelled() { return cancel; }

    private:
        static constexpr CAMU_Size camera_size = Size;
        static constexpr size_t camera_width   = camera_width_for_size(camera_size);
        static constexpr size_t camera_height  = camera_height_for_size(camera_size);
        static constexpr size_t image_width    = std::bit_ceil(camera_width);
        static constexpr size_t image_height   = std::bit_ceil(camera_height);

        static constexpr float camera_scale = camera_scale_for_size(camera_size);

        static constexpr size_t image_pos_x = (400 - (camera_width * camera_scale)) / 2;
        static constexpr size_t image_pos_y = (240 - (camera_height * camera_scale)) / 2;

        static_assert(std::clamp<size_t>(image_width, 8, 1024) == image_width);
        static_assert(std::clamp<size_t>(image_height, 8, 1024) == image_height);

        void buffToImage();
        void finish();
        DataMutex<std::array<u16, camera_width * camera_height>> cameraBuffer;
        C3D_Tex tex;
        DataMutex<C2D_Image> image;
        quirc* data;
        Handle exitEvent;
        static constexpr Tex3DS_SubTexture subtex = {camera_width, camera_height, 0.0f, 1.0f,
            ((float)camera_width) / image_width, 1.0f - (((float)camera_height) / image_height)};
        LightSemaphore letHandlerRun;
        std::atomic<bool> finished = false;
        bool capturing             = false;
        bool cancel                = false;
    };

    template <CAMU_Size Size>
    void captureHelp(void* arg)
    {
        QRData<Size>* data = (QRData<Size>*)arg;
        data->captureThread();
    }
}

template <CAMU_Size Size>
void QRData<Size>::buffToImage()
{
    auto lockedImage  = image.lock();
    auto lockedBuffer = cameraBuffer.lock();
    u32 size;
    void* imageData = C3D_Tex2DGetImagePtr(lockedImage->tex, 0, &size);
    for (u32 x = 0; x < camera_width; x++)
    {
        for (u32 y = 0; y < camera_height; y++)
        {
            u32 dstPos = ((((y >> 3) * (image_width >> 3) + (x >> 3)) << 6) +
                             ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) |
                                 ((x & 4) << 2) | ((y & 4) << 3))) *
                         2;
            u32 srcPos = (y * camera_width + x) * 2;
            memcpy(((u8*)imageData) + dstPos, ((u8*)lockedBuffer->data()) + srcPos, 2);
        }
    }
    GSPGPU_FlushDataCache(imageData, size);
}

template <CAMU_Size Size>
void QRData<Size>::finish()
{
    svcSignalEvent(exitEvent);
    while (!done())
    {
        LightSemaphore_Release(&letHandlerRun, 10);
        svcSleepThread(1000000);
    }
}

template <CAMU_Size Size>
void QRData<Size>::drawThread()
{
    while (aptMainLoop() && !done())
    {
        LightSemaphore_Acquire(&letHandlerRun, 1);
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        buffToImage();

        Gui::target(GFX_TOP);
        Gui::drawSolidRect(0, 0, 400, 240, COLOR_BLACK);
        Gui::drawImageAt(
            image.lock().get(), image_pos_x, image_pos_y, nullptr, camera_scale, camera_scale);

        Gui::target(GFX_BOTTOM);
        Gui::backgroundBottom(false);
        Gui::backgroundAnimatedBottom();
        Gui::drawSolidRect(0, 0, 320.0f, 240.0f, COLOR_MASKBLACK);
        Gui::text(i18n::localize("SCANNER_EXIT"), 160, 115, FONT_SIZE_18, COLOR_WHITE,
            TextPosX::CENTER, TextPosY::TOP);
        Gui::flushText();

        if (!aptIsHomeAllowed() && aptCheckHomePressRejected())
        {
            Gui::setDoHomeDraw();
        }

        Gui::drawNoHome();

        C3D_FrameEnd(0);
        Gui::frameClean();
    }
}

template <CAMU_Size Size>
void QRData<Size>::captureThread()
{
    Handle events[3] = {0};
    events[0]        = exitEvent;
    u32 transferUnit;

    std::unique_ptr<u16[]> buffer = std::unique_ptr<u16[]>(new u16[camera_width * camera_height]);
    camInit();
    CAMU_SetSize(SELECT_OUT1, camera_size, CONTEXT_A);
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
    CAMU_GetMaxBytes(&transferUnit, camera_width, camera_height);
    CAMU_SetTransferBytes(PORT_CAM1, transferUnit, camera_width, camera_height);
    CAMU_ClearBuffer(PORT_CAM1);
    CAMU_SetReceiving(&events[1], buffer.get(), PORT_CAM1,
        camera_width * camera_height * sizeof(u16), (s16)transferUnit);
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
                LightSemaphore_Acquire(&letHandlerRun, 1);
                svcCloseHandle(events[1]);
                events[1] = 0;
                {
                    auto lockedBuffer = cameraBuffer.lock();
                    memcpy(lockedBuffer->data(), buffer.get(),
                        camera_width * camera_height * sizeof(u16));
                    GSPGPU_FlushDataCache(
                        lockedBuffer->data(), camera_width * camera_height * sizeof(u16));
                }
                CAMU_SetReceiving(&events[1], buffer.get(), PORT_CAM1,
                    camera_width * camera_height * sizeof(u16), transferUnit);
                break;
            case 2:
                svcCloseHandle(events[1]);
                events[1] = 0;
                CAMU_ClearBuffer(PORT_CAM1);
                CAMU_SetReceiving(&events[1], buffer.get(), PORT_CAM1,
                    camera_width * camera_height * sizeof(u16), transferUnit);
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

template <CAMU_Size Size>
void QRData<Size>::handler(std::vector<u8>& out)
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
        if (threadCreate((ThreadFunc)&captureHelp<Size>, this, 0x10000, 0x1A, 1, true) != NULL)
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
    {
        auto lockedBuffer = cameraBuffer.lock();
        for (ssize_t x = 0; x < w; x++)
        {
            for (ssize_t y = 0; y < h; y++)
            {
                u16 px           = lockedBuffer.get()[y * camera_width + x];
                image[y * w + x] = (u8)(((((px >> 11) & 0x1F) << 3) + (((px >> 5) & 0x3F) << 2) +
                                            ((px & 0x1F) << 3)) /
                                        3);
            }
        }
    }
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

    LightSemaphore_Release(&letHandlerRun, 10);
}

std::vector<u8> QR_Internal::scan()
{
    static constexpr CAMU_Size CAMERA_SIZE    = SIZE_VGA;
    std::vector<u8> out                       = {};
    std::unique_ptr<QRData<CAMERA_SIZE>> data = std::make_unique<QRData<CAMERA_SIZE>>();
    aptSetHomeAllowed(false);
    Threads::create<&QRData<CAMERA_SIZE>::drawThread>(0x10000, data.get());
    while (!data->done())
    {
        data->handler(out);
    }
    aptSetHomeAllowed(true);
    return out;
}
