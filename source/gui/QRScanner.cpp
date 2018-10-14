#include "QRScanner.hpp"

static void qrHandler(qr_data*, QRMode, u8*);
static void camThread(void*);
static void uiThread(void*);

static void qrHandler(qr_data* data, QRMode mode, u8* buff)
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
    svcCloseHandle(data->mutex);
    data->finished = true;
}

static void uiThread(void* arg)
{
    qr_data* data = (qr_data*) arg;
    while (!data->finished)
    {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        for (u32 x = 0; x < 400; x++)
        {
            for (u32 y = 0; y < 240; y++)
            {
                u32 dstPos = ((((y >> 3) * (512 >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) * 2;
                u32 srcPos = (y * 400 + x) * 2;
                memcpy(&((u8*)data->image.tex->data)[dstPos], &((u8*)data->camera_buffer)[srcPos], 2);
            }
        }

        C2D_SceneBegin(g_renderTargetTop);
        C2D_DrawImageAt(data->image, 0.0f, 0.0f, 0.5f, NULL, 1.0f, 1.0f);
        C3D_FrameEnd(0);
    }
}

void QRScanner::init(QRMode mode, u8* buff)
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
    C3D_TexDelete(data->tex);
    free(data->camera_buffer);
    free(data->tex);
    quirc_destroy(data->context);
    free(data);
}