/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2026 Bernardo Giordano, Admiral Fish, piepie62
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

#ifndef CAMERAFRAME_HPP
#define CAMERAFRAME_HPP

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <span>
#include <utility>

// One captured RGB565 frame and both products the QR scanner needs from it.
//
// The camera writes into one VGA buffer while consumers use the other. Publishing swaps
// the two, so accepting a frame does not copy it. Luma and preview consumers then run at
// independent cadences: quirc can spend hundreds of milliseconds identifying one full-VGA
// frame without holding the 320x240 preview at the same frame.
//
// CameraFrame deliberately does not choose a locking primitive. The 3DS adapter wraps it in
// DataMutex; host tests can drive the same interface without libctru.
template <std::size_t Width, std::size_t Height, std::size_t PreviewScale = 2>
class CameraFrame
{
public:
    using Pixel = std::uint16_t;

    static_assert(Width > 0 && Height > 0);
    static_assert(PreviewScale > 0);
    static_assert(Width % PreviewScale == 0 && Height % PreviewScale == 0);

    static constexpr std::size_t CAMERA_WIDTH  = Width;
    static constexpr std::size_t CAMERA_HEIGHT = Height;
    static constexpr std::size_t CAMERA_PIXELS = CAMERA_WIDTH * CAMERA_HEIGHT;
    static constexpr std::size_t CAMERA_BYTES  = CAMERA_PIXELS * sizeof(Pixel);

    static constexpr std::size_t PREVIEW_WIDTH  = CAMERA_WIDTH / PreviewScale;
    static constexpr std::size_t PREVIEW_HEIGHT = CAMERA_HEIGHT / PreviewScale;
    static constexpr std::size_t PREVIEW_TEXTURE_WIDTH =
        std::max<std::size_t>(8, std::bit_ceil(PREVIEW_WIDTH));
    static constexpr std::size_t PREVIEW_TEXTURE_HEIGHT =
        std::max<std::size_t>(8, std::bit_ceil(PREVIEW_HEIGHT));
    static constexpr std::size_t PREVIEW_TEXTURE_PIXELS =
        PREVIEW_TEXTURE_WIDTH * PREVIEW_TEXTURE_HEIGHT;

    CameraFrame()
        : published(std::make_unique<Pixel[]>(CAMERA_PIXELS)),
          receiving(std::make_unique<Pixel[]>(CAMERA_PIXELS))
    {
    }

    CameraFrame(const CameraFrame&)            = delete;
    CameraFrame& operator=(const CameraFrame&) = delete;

    // The camera may write CAMERA_BYTES here until it reports that reception completed.
    [[nodiscard]] Pixel* captureTarget() { return receiving.get(); }

    // Make the completed capture current and return the buffer for the next reception.
    // The caller must ensure the camera has stopped touching captureTarget() first.
    [[nodiscard]] Pixel* publishCapture()
    {
        std::swap(published, receiving);
        capturedGeneration++;
        return receiving.get();
    }

    // Derive full-resolution luma once for the newest published frame. Returns false when no
    // new frame has arrived, so quirc does not repeatedly identify the same image.
    [[nodiscard]] bool copyLuma(std::span<std::uint8_t, CAMERA_PIXELS> destination)
    {
        if (processedGeneration == capturedGeneration)
        {
            return false;
        }

        const Pixel* source = published.get();
        for (std::size_t i = 0; i < CAMERA_PIXELS; i++)
        {
            destination[i] = toLuma(source[i]);
        }

        processedGeneration = capturedGeneration;
        return true;
    }

    // Downsample the newest published frame straight into a tiled RGB565 texture. This has its
    // own generation cursor, so drawing keeps up with camera capture instead of copyLuma() and
    // quirc. Only one pixel in each PreviewScale square is touched.
    [[nodiscard]] bool copyPreview(std::span<Pixel, PREVIEW_TEXTURE_PIXELS> destination)
    {
        if (displayedGeneration == capturedGeneration)
        {
            return false;
        }

        if (displayedGeneration == std::numeric_limits<std::uint64_t>::max())
        {
            // Only the visible subtexture is rewritten below. Clear its power-of-two padding
            // on the first upload so texture filtering can never sample uninitialized memory.
            std::ranges::fill(destination, Pixel{});
        }

        const Pixel* source = published.get();
        for (std::size_t y = 0; y < PREVIEW_HEIGHT; y++)
        {
            const std::size_t sourceRow = y * PreviewScale * CAMERA_WIDTH;
            for (std::size_t x = 0; x < PREVIEW_WIDTH; x++)
            {
                destination[tiledIndex(x, y)] = source[sourceRow + x * PreviewScale];
            }
        }

        displayedGeneration = capturedGeneration;
        return true;
    }

private:
    [[nodiscard]] static constexpr std::uint8_t toLuma(Pixel pixel)
    {
        // Keep the scanner's established RGB565 conversion exactly: camera output names the
        // red and blue fields oppositely on different paths, but their equal weights make the
        // luma independent of that naming.
        return std::uint8_t(
            ((((pixel >> 11) & 0x1F) << 3) + (((pixel >> 5) & 0x3F) << 2) + ((pixel & 0x1F) << 3)) /
            3);
    }

    [[nodiscard]] static constexpr std::size_t tiledIndex(std::size_t x, std::size_t y)
    {
        return (((y >> 3) * (PREVIEW_TEXTURE_WIDTH >> 3) + (x >> 3)) << 6) +
               ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) |
                   ((y & 4) << 3));
    }

    std::unique_ptr<Pixel[]> published;
    std::unique_ptr<Pixel[]> receiving;
    std::uint64_t capturedGeneration  = 0;
    std::uint64_t processedGeneration = 0;
    // Make the first copyPreview() upload the initial black frame.
    std::uint64_t displayedGeneration = std::numeric_limits<std::uint64_t>::max();
};

#endif
