/*
*   This file is part of PKSM
*   Copyright (C) 2016-2018 Bernardo Giordano, Admiral Fish, piepie62
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

#ifndef QRSCANNER_HPP
#define QRSCANNER_HPP

#include "gui.hpp"

extern "C" {
#include "quirc/quirc.h"
#include "base64.h"
}

typedef struct {
    u16*          camera_buffer;
    Handle        mutex;
    volatile bool finished;
    Handle        cancel;
    bool          capturing;
    struct quirc* context;
    C3D_Tex*      tex;
    C2D_Image     image;
} qr_data;

enum QRMode {
    PKM4,
    PKM5,
    PKM6,
    PKM7,
    WCX4,
    WCX5,
    WCX6,
    WCX7
};

namespace QRScanner
{
    void init(QRMode mode, u8*& buff);

    // note: exposed, but not required to be called outside QRScanner.cpp
    void exit(qr_data* data);
}

#endif