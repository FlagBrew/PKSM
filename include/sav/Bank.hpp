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

#ifndef BANK_HPP
#define BANK_HPP

#include "Sav.hpp"

class Bank
{
public:
    Bank();
    ~Bank()
    {
        delete[] data;
    }
    std::unique_ptr<PKX> pkm(int box, int slot) const;
    void pkm(PKX& pkm, int box, int slot);
    void resize();
    bool save() const;
    void backup() const;
    std::string boxName(int box) const;
    void boxName(std::string name, int box);
private:
    static constexpr int BANK_VERSION = 1;
    static constexpr std::string_view BANK_MAGIC = "PKSMBANK";
    void loadExtData();
    void loadSD();
    struct BankHeader {
        const char MAGIC[8];
        int version;
    };
    struct BankEntry {
        Generation gen;
        u8 data[260];
    };
    u8* data = nullptr;
    nlohmann::json boxNames;
    size_t size;
};

#endif