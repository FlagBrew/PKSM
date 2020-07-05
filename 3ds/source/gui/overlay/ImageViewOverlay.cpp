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

#include "ImageViewOverlay.hpp"
#include "gui.hpp"

ImageViewOverlay::ImageViewOverlay(
    ReplaceableScreen& screen, C2D_Image& image, PKSM_Color background)
    : ReplaceableScreen(&screen, i18n::localize("B_BACK")),
      image(image),
      bg(background),
      deleteImage(false)
{
}

ImageViewOverlay::ImageViewOverlay(
    ReplaceableScreen& screen, C2D_Image&& image, PKSM_Color background)
    : ReplaceableScreen(&screen, i18n::localize("B_BACK")),
      image(image),
      bg(background),
      deleteImage(true)
{
}

ImageViewOverlay::~ImageViewOverlay()
{
    if (deleteImage)
    {
        C3D_TexDelete(image.tex);
        delete image.tex;
        delete image.subtex;
    }
}

void ImageViewOverlay::drawTop() const
{
    Gui::drawSolidRect(0, 0, 400, 240, bg);
    float scale = std::min(390.0f / image.subtex->width, 230.0f / image.subtex->height);
    float x     = (400.0f - scale * image.subtex->width) / 2;
    float y     = (240.0f - scale * image.subtex->width) / 2;
    Gui::drawImageAt(image, x, y, nullptr, scale, scale);
}
