#include "CameraFrame.hpp"
#include "QRGen.hpp"
#include "quirc/quirc.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <span>
#include <vector>

namespace
{
    void conversionAndPublication()
    {
        using Frame = CameraFrame<4, 4>;
        Frame frame;

        std::array<std::uint8_t, Frame::CAMERA_PIXELS> luma{};
        std::array<std::uint16_t, Frame::PREVIEW_TEXTURE_PIXELS> preview;
        preview.fill(0xFFFF);

        // There is no camera frame to process yet. The initial preview is nevertheless
        // available once so a newly allocated GPU texture starts out black.
        assert(!frame.copyLuma(luma));
        assert(frame.copyPreview(preview));
        assert(std::ranges::all_of(preview, [](auto pixel) { return pixel == 0; }));
        assert(!frame.copyPreview(preview));

        std::uint16_t* capture = frame.captureTarget();
        std::ranges::fill(std::span{capture, Frame::CAMERA_PIXELS}, 0);
        capture[0]  = 0xFFFF;
        capture[2]  = 0x001F;
        capture[8]  = 0xF800;
        capture[10] = 0x07E0;

        assert(frame.publishCapture() != capture);

        // Preview delivery has its own cursor: it does not wait for the full-resolution
        // luma consumer, and consuming it does not consume luma.
        assert(frame.copyPreview(preview));
        assert(preview[0] == capture[0]);
        assert(preview[1] == capture[2]);
        assert(preview[2] == capture[8]);
        assert(preview[3] == capture[10]);
        assert(!frame.copyPreview(preview));

        assert(frame.copyLuma(luma));
        assert(!frame.copyLuma(luma));
        assert(luma[0] == 249);
        assert(luma[2] == 82);
        assert(luma[8] == 82);
        assert(luma[10] == 84);
        assert(!frame.copyPreview(preview));
    }

    void denseQrStillDecodesAtVga()
    {
        using Frame = CameraFrame<640, 480>;
        Frame frame;

        // A version-29-or-larger binary symbol is representative of the very dense PKHeX
        // codes that motivated VGA capture. With a four-module quiet zone it cannot retain
        // even two camera samples per module in a 240-line capture.
        std::vector<std::uint8_t> payload(1200);
        for (std::size_t i = 0; i < payload.size(); i++)
        {
            payload[i] = std::uint8_t(i * 37 + 11);
        }
        const auto qr = qrcodegen::QrCode::encodeBinary(payload, qrcodegen::QrCode::Ecc::MEDIUM);
        assert(qr.getSize() >= 133);

        constexpr int QUIET_ZONE = 4;
        constexpr int SCALE      = 3;
        const int renderedSize   = (qr.getSize() + QUIET_ZONE * 2) * SCALE;
        assert(renderedSize <= int(Frame::CAMERA_HEIGHT));
        const int originX = (int(Frame::CAMERA_WIDTH) - renderedSize) / 2;
        const int originY = (int(Frame::CAMERA_HEIGHT) - renderedSize) / 2;

        std::uint16_t* capture = frame.captureTarget();
        std::ranges::fill(std::span{capture, Frame::CAMERA_PIXELS}, 0xFFFF);
        for (int moduleY = 0; moduleY < qr.getSize(); moduleY++)
        {
            for (int moduleX = 0; moduleX < qr.getSize(); moduleX++)
            {
                if (!qr.getModule(moduleX, moduleY))
                {
                    continue;
                }
                const int startX = originX + (moduleX + QUIET_ZONE) * SCALE;
                const int startY = originY + (moduleY + QUIET_ZONE) * SCALE;
                for (int y = startY; y < startY + SCALE; y++)
                {
                    std::fill_n(capture + y * Frame::CAMERA_WIDTH + startX, SCALE, 0);
                }
            }
        }
        (void)frame.publishCapture();

        quirc* decoder = quirc_new();
        assert(decoder);
        assert(quirc_resize(decoder, Frame::CAMERA_WIDTH, Frame::CAMERA_HEIGHT) == 0);
        std::uint8_t* luma = quirc_begin(decoder, nullptr, nullptr);
        assert(frame.copyLuma(
            std::span<std::uint8_t, Frame::CAMERA_PIXELS>{luma, Frame::CAMERA_PIXELS}));
        quirc_end(decoder);

        bool decoded = false;
        for (int i = 0; i < quirc_count(decoder); i++)
        {
            quirc_code code;
            quirc_data data;
            quirc_extract(decoder, i, &code);
            const auto result = quirc_decode(&code, &data);
            if (result == QUIRC_SUCCESS && data.payload_len == int(payload.size()) &&
                std::equal(payload.begin(), payload.end(), data.payload))
            {
                decoded = true;
                break;
            }
        }
        quirc_destroy(decoder);
        assert(decoded);
    }
}

int main()
{
    conversionAndPublication();
    denseQrStillDecodesAtVga();
    std::printf("CameraFrame: all checks passed\n");
    return 0;
}
