/*
*   This file is part of PKSM
*   Copyright (C) 2016-2019 Bernardo Giordano, Admiral Fish, piepie62
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

#ifndef SAVXY_HPP
#define SAVXY_HPP

#include "Sav6.hpp"

class SavXY : public Sav6
{
protected:
    static constexpr u32 chkofs[55] = {
        0x00000, 0x00400, 0x01000, 0x01200, 0x01400, 0x01600, 0x01800,
        0x01A00, 0x01C00, 0x01E00, 0x02000, 0x04200, 0x04400, 0x04A00,
        0x05000, 0x0A000, 0x0F000, 0x14000, 0x14200, 0x14A00, 0x15000,
        0x15800, 0x16000, 0x16200, 0x16400, 0x16A00, 0x16C00, 0x17000,
        0x17800, 0x17A00, 0x17C00, 0x17E00, 0x18000, 0x18200, 0x18A00,
        0x19000, 0x19400, 0x1B000, 0x1B200, 0x1B400, 0x1B800, 0x1BC00,
        0x1D800, 0x1DC00, 0x1E400, 0x1E800, 0x1F200, 0x1F600, 0x1FE00,
        0x20C00, 0x20E00, 0x21000, 0x21E00, 0x22600, 0x57200
    };

    static constexpr u32 chklen[55] = {
        0x002C8, 0x00B88, 0x0002C, 0x00038, 0x00150, 0x00004, 0x00008, 
        0x001C0, 0x000BE, 0x00024, 0x02100, 0x00140, 0x00440, 0x00574, 
        0x04E28, 0x04E28, 0x04E28, 0x00170, 0x0061C, 0x00504, 0x006A0,
        0x00644, 0x00104, 0x00004, 0x00420, 0x00064, 0x003F0, 0x0070C, 
        0x00180, 0x00004, 0x0000C, 0x00048, 0x00054, 0x00644, 0x005C8, 
        0x002F8, 0x01B40, 0x001F4, 0x001F0, 0x00216, 0x00390, 0x01A90, 
        0x00308, 0x00618, 0x0025C, 0x00834, 0x00318, 0x007D0, 0x00C48, 
        0x00078, 0x00200, 0x00C84, 0x00628, 0x34AD0, 0x0E058
    };
    
public:
    SavXY(u8* dt);
    virtual ~SavXY() { };

    void resign(void) override;

    std::map<Pouch, std::vector<int>> validItems(void) const override;
};

#endif
