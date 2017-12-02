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

#include "unicode.h"

ssize_t strulen(const u16 *input, ssize_t max_len)
{
    for (int i = 0; i < max_len; i++) if (input[i] == 0) return i;
    return max_len;
}

void struacat(u16 *input, const char *addition)
{
    ssize_t len = strulen(input, 256);
    for (u16 i = len; i < strlen(addition) + len; i++) 
    {
        input[i] = addition[i - len];
    }
    input[strlen(addition) + len] = 0;
}

u16 *strucat(u16 *destination, const u16 *source)
{
    ssize_t dest_len = strulen(destination, 256);
    ssize_t source_len = strulen(source, 256);
    memcpy(&destination[dest_len], source, source_len * sizeof(u16));
    return destination;
}