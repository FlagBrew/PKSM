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

#include "AppLegalityOverlay.hpp"
#include "ClickButton.hpp"
#include "ImageViewOverlay.hpp"
#include "PKX.hpp"
#include "QRGen.hpp"
#include "QRScanner.hpp"
#include "Sav.hpp"
#include "base64.hpp"
#include "gui.hpp"
#include "loader.hpp"

namespace
{
    // Borrowed from LOVE
    inline int nextpo2(unsigned int in)
    {
        in--;
        in |= in >> 1;
        in |= in >> 2;
        in |= in >> 4;
        in |= in >> 8;
        in |= in >> 16;
        in++;

        return std::clamp(in, 8U, 1024U); // clamp size to keep gpu from locking
    }
}

AppLegalityOverlay::AppLegalityOverlay(ReplaceableScreen& screen, std::shared_ptr<PKX> pkm)
    : ReplaceableScreen(&screen, i18n::localize("B_BACK")), pkm(pkm)
{
    std::string data = (std::string)pkm->generation() + ":" + std::to_string((u32)TitleLoader::save->version()) + ":" +
                       base64_encode(pkm->rawData(), pkm->getLength());
    qrcodegen::QrCode code = qrcodegen::QrCode::encodeText(data.c_str(), qrcodegen::QrCode::Ecc::MEDIUM);

    C2D_Image image;
    image.tex = new C3D_Tex;
    u16 po2   = nextpo2(code.getSize());
    C3D_TexInit(image.tex, po2, po2, GPU_RGB565);
    for (int y = 0; y < code.getSize(); y++)
    {
        for (int x = 0; x < code.getSize(); x++)
        {
            // Yay swizzling
            u32 dst                      = ((((y >> 3) * (po2 >> 3) + (x >> 3)) << 6) +
                       ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3)));
            ((u16*)image.tex->data)[dst] = code.getModule(x, y) ? 0 : 0xFFFF; // true indicates white, false black. However, image is inverted
        }
    }

    image.subtex = new Tex3DS_SubTexture{.width = (u16)code.getSize(),
        .height                                 = (u16)code.getSize(),
        .left                                   = 0.0f,
        .top                                    = 1.0f,
        .right                                  = (float)code.getSize() / (float)po2,
        .bottom                                 = 1.0f - (float)code.getSize() / (float)po2};

    addOverlay<ImageViewOverlay>(std::move(image));

    buttons.push_back(std::make_unique<ClickButton>(204, 171, 108, 30,
        [this]() {
            // do the socket thing
            return true;
        },
        ui_sheet_button_editor_idx, "", 0.0f, COLOR_BLACK));
    buttons.push_back(std::make_unique<ClickButton>(204, 140, 108, 30,
        [this]() {
            auto pkx = QRScanner<PKX>::scan();
            if (pkx)
            {
                pkx = TitleLoader::save->transfer(*pkx);
                if (pkx)
                {
                    std::copy(pkx->rawData(), pkx->rawData() + pkx->getLength(), this->pkm->rawData());
                }
            }
            // remove image view
            this->removeOverlay();
            // remove this
            this->parent->removeOverlay();
            return true;
        },
        ui_sheet_button_editor_idx, "", 0.0f, COLOR_BLACK));
}

void AppLegalityOverlay::drawBottom() const
{
    dim();
    for (auto& button : buttons)
    {
        button->draw();
    }

    Gui::text(i18n::localize("APP_SCAN_QR"), 258, 155, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER, TextPosY::CENTER,
        TextWidthAction::SQUISH_OR_SCROLL, 100.0f);

    Gui::text(i18n::localize("APP_USE_SOCKETS"), 258, 186, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER, TextPosY::CENTER,
        TextWidthAction::SQUISH_OR_SCROLL, 100.0f);
}

void AppLegalityOverlay::update(touchPosition* touch)
{
    for (auto& button : buttons)
    {
        if (button->update(touch))
        {
            return;
        }
    }

    if (hidKeysDown() & KEY_B)
    {
        // remove image view
        this->removeOverlay();
        // remove this
        this->parent->removeOverlay();
    }
}
