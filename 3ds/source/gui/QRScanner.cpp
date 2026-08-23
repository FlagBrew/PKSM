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
#include "CameraFrame.hpp"
#include "colors.hpp"
#include "DataMutex.hpp"
#include "gui.hpp"
#include "quirc/quirc.h"
#include "thread.hpp"
#include "utils/logging.hpp"
#include <3ds.h>
#include <atomic>

namespace
{
    // Dense PKHeX codes need more than the 240 vertical samples of SIZE_CTR_TOP_LCD.
    // Keep quirc at VGA and only reduce the display-only preview.
    constexpr CAMU_Size CAMERA_SIZE = SIZE_VGA;
    using ScannerFrame              = CameraFrame<640, 480>;

    class QRData
    {
    public:
        QRData() : frame(), image{&tex, &subtex}, data(quirc_new())
        {
            C3D_TexInit(image.tex, ScannerFrame::PREVIEW_TEXTURE_WIDTH,
                ScannerFrame::PREVIEW_TEXTURE_HEIGHT, GPU_RGB565);
            C3D_TexSetFilter(image.tex, GPU_LINEAR, GPU_LINEAR);
            image.tex->border = 0xFFFFFFFF;
            C3D_TexSetWrap(image.tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
            svcCreateEvent(&exitEvent, RESET_STICKY);
            LightEvent_Init(&drawThreadDone, RESET_STICKY);
            LightEvent_Init(&captureThreadDone, RESET_STICKY);
            quirc_resize(data, ScannerFrame::CAMERA_WIDTH, ScannerFrame::CAMERA_HEIGHT);
        }

        ~QRData()
        {
            C3D_TexDelete(image.tex);
            quirc_destroy(data);
            svcCloseHandle(exitEvent);
        }

        void drawThread();
        void captureThread();
        void handler(std::vector<u8>& out);
        // Blocks until neither thread can touch this object again
        void joinThreads();

        bool done() { return finished; }

    private:
        void buffToImage();
        void finish();
        DataMutex<ScannerFrame> frame;
        C3D_Tex tex;
        C2D_Image image;
        quirc* data;
        Handle exitEvent;
        static constexpr Tex3DS_SubTexture subtex = {ScannerFrame::PREVIEW_WIDTH,
            ScannerFrame::PREVIEW_HEIGHT, 0.0f, 1.0f,
            float(ScannerFrame::PREVIEW_WIDTH) / ScannerFrame::PREVIEW_TEXTURE_WIDTH,
            1.0f - float(ScannerFrame::PREVIEW_HEIGHT) / ScannerFrame::PREVIEW_TEXTURE_HEIGHT};
        LightEvent drawThreadDone;
        LightEvent captureThreadDone;
        std::atomic<bool> finished = false;
        bool capturing             = false;
    };

    void captureHelp(void* arg)
    {
        QRData* data = (QRData*)arg;
        data->captureThread();
    }
}

void QRData::buffToImage()
{
    u32 size;
    auto* imageData = (u16*)C3D_Tex2DGetImagePtr(image.tex, 0, &size);
    {
        auto lockedFrame = frame.lock();
        if (!lockedFrame->copyPreview(std::span<u16, ScannerFrame::PREVIEW_TEXTURE_PIXELS>{
                imageData, ScannerFrame::PREVIEW_TEXTURE_PIXELS}))
        {
            return;
        }
    }
    GSPGPU_FlushDataCache(imageData, size);
}

void QRData::finish()
{
    svcSignalEvent(exitEvent);
    while (!done())
    {
        svcSleepThread(1000000);
    }
}

void QRData::drawThread()
{
    while (aptMainLoop() && !done())
    {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        buffToImage();

        Gui::target(GFX_TOP);
        Gui::drawSolidRect(0, 0, 400, 240, COLOR_BLACK);
        Gui::drawImageAt(image, 40, 0);

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

    LightEvent_Signal(&drawThreadDone);
}

void QRData::captureThread()
{
    Handle events[3] = {0};
    events[0]        = exitEvent;
    u32 transferUnit;

    u16* buffer;
    {
        buffer = frame.lock()->captureTarget();
    }
    camInit();
    CAMU_SetSize(SELECT_OUT1, CAMERA_SIZE, CONTEXT_A);
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
    CAMU_GetMaxBytes(&transferUnit, ScannerFrame::CAMERA_WIDTH, ScannerFrame::CAMERA_HEIGHT);
    CAMU_SetTransferBytes(
        PORT_CAM1, transferUnit, ScannerFrame::CAMERA_WIDTH, ScannerFrame::CAMERA_HEIGHT);
    CAMU_ClearBuffer(PORT_CAM1);
    CAMU_SetReceiving(&events[1], buffer, PORT_CAM1, ScannerFrame::CAMERA_BYTES, (s16)transferUnit);
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
                // The camera wrote this buffer without going through the CPU cache. It becomes
                // the immutable published frame as soon as its stale cache lines are discarded.
                GSPGPU_InvalidateDataCache(buffer, ScannerFrame::CAMERA_BYTES);
                {
                    buffer = frame.lock()->publishCapture();
                }
                CAMU_SetReceiving(
                    &events[1], buffer, PORT_CAM1, ScannerFrame::CAMERA_BYTES, (s16)transferUnit);
                break;
            case 2:
                svcCloseHandle(events[1]);
                events[1] = 0;
                CAMU_ClearBuffer(PORT_CAM1);
                CAMU_SetReceiving(
                    &events[1], buffer, PORT_CAM1, ScannerFrame::CAMERA_BYTES, (s16)transferUnit);
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
    LightEvent_Signal(&captureThreadDone);
}

void QRData::joinThreads()
{
    LightEvent_Wait(&drawThreadDone);
    if (capturing)
    {
        LightEvent_Wait(&captureThreadDone);
    }
}

void QRData::handler(std::vector<u8>& out)
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
        if (Threads::atPriority(&captureHelp, this, 0x10000, 0x1A))
        {
            capturing = true;
        }
        else
        {
            finished = true;
            return;
        }
    }

    if (done())
    {
        return;
    }

    u8* image = (u8*)quirc_begin(data, nullptr, nullptr);
    bool processed;
    {
        auto lockedFrame = frame.lock();
        processed        = lockedFrame->copyLuma(
            std::span<u8, ScannerFrame::CAMERA_PIXELS>{image, ScannerFrame::CAMERA_PIXELS});
    }
    if (!processed)
    {
        // The camera produces 30 frames per second. Do not spin or ask quirc to identify the
        // same frame repeatedly while waiting for the next one.
        svcSleepThread(1000000);
        return;
    }

    quirc_end(data);
    for (int i = 0; i < quirc_count(data); i++)
    {
        struct quirc_code code;
        struct quirc_data scan_data;
        quirc_extract(data, i, &code);
        if (!quirc_decode(&code, &scan_data))
        {
            finish();
            out.resize(scan_data.payload_len);
            std::copy(scan_data.payload, scan_data.payload + scan_data.payload_len, out.begin());
            return;
        }
    }
}

std::vector<u8> QR_Internal::scan()
{
    std::vector<u8> out          = {};
    std::unique_ptr<QRData> data = std::make_unique<QRData>();
    aptSetHomeAllowed(false);
    if (Threads::background<&QRData::drawThread>(0x10000, data.get()))
    {
        while (!data->done())
        {
            data->handler(out);
        }
        // data dies at the end of this scope, so nothing may still be inside it
        data->joinThreads();
    }
    else
    {
        Logging::warning("Could not start the QR scanner draw thread");
    }
    aptSetHomeAllowed(true);
    return out;
}
