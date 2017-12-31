/*  This file is part of PKSM
>	Copyright (C) 2016/2017 Bernardo Giordano
>
>   This program is free software: you can redistribute it and/or modify
>   it under the terms of the GNU General Public License as published by
>   the Free Software Foundation, either version 3 of the License, or
>   (at your option) any later version.
>
>   This program is distributed in the hope that it will be useful,
>   but WITHOUT ANY WARRANTY; without even the implied warranty of
>   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>   GNU General Public License for more details.
>
>   You should have received a copy of the GNU General Public License
>   along with this program.  If not, see <http://www.gnu.org/licenses/>.
>   See LICENSE for information.
*/

/*
*   This file is part of Anemone3DS
*   Copyright (C) 2016-2017 Alex Taber ("astronautlevel"), Dawid Eckert ("daedreth")
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

#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"

#define MODE_PKX 1
#define MODE_WCX 2

// anatomy of a QR7:
// u32 magic; // POKE
// u32 _0xFF;
// u32 box;
// u32 slot;
// u32 num_copies;
// u8  reserved[0x1C];
// u8  ek7[0x104];
// u8  dex_data[0x60];
// u16 crc16
// sizeof(QR7) == 0x1A2

typedef struct {
    u16 *camera_buffer;
    u32 *texture_buffer;
    Handle mutex;
    volatile bool finished;
    volatile bool success;
    Handle cancel;

    bool capturing;
    struct quirc* context;
} qr_data;

void capture_cam_thread(void *arg);
bool start_capture_cam(qr_data *data);
void update_qr(qr_data *data, u8* buf, int mode);
void exit_qr(qr_data *data);
bool init_qr(u8* buf, int mode);

#endif