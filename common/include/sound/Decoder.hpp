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

#ifndef DECODER_HPP
#define DECODER_HPP

#include "types.h"
#include <memory>
#include <string>

class Decoder
{
public:
    virtual ~Decoder() = default;

    bool good() { return initialized; }

    virtual u32 pos()                                   = 0;
    virtual u32 length()                                = 0;
    virtual u32 decode(void* buffer, size_t bufferSize) = 0;
    virtual bool stereo()                               = 0;
    virtual u32 sampleRate()                            = 0;
    static std::unique_ptr<Decoder> get(const std::string& fileName);
    static bool init();
    static void exit();

protected:
    bool initialized = false;
};

#endif
