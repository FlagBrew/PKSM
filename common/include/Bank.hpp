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

#ifndef BANK_HPP
#define BANK_HPP

#include "enums/Generation.hpp"
#include "nlohmann/json_fwd.hpp"
#include "pkx/PKX.hpp"
#include "utils/crypto.hpp"

class Bank
{
public:
    Bank(const std::string& name, int maxBoxes);
    ~Bank();
    std::unique_ptr<pksm::PKX> pkm(int box, int slot) const;
    void pkm(const pksm::PKX& pkm, int box, int slot);
    void resize(int boxes);
    void load(int maxBoxes);
    bool save() const;
    bool saveWithoutBackup() const;
    bool backup() const;
    std::string boxName(int box) const;
    std::pair<std::string, std::string> paths() const;
    void boxName(std::string name, int box);
    bool hasChanged() const;
    int boxes() const;
    const std::string& name() const;
    bool setName(const std::string& name);

private:
    static constexpr int BANK_VERSION            = 3;
    static constexpr std::string_view BANK_MAGIC = "PKSMBANK";
    void createJSON();
    void createBank(int maxBoxes);
    void convertFromBankBin();
    struct BankHeader
    {
        char MAGIC[8];
        u32 version;
        u32 boxes;
    };
    static_assert(sizeof(BankHeader) == 16);
    struct BankEntry
    {
        pksm::Generation gen;
        u8 data[0x148];
        u8 padding[4]; // Pad to 8 bytes
    };
    static_assert(sizeof(BankEntry) == 0x150);
    std::unique_ptr<nlohmann::json> boxNames;
    mutable std::array<u8, 32> prevHash;
    mutable std::array<u8, 32> prevNameHash;
    std::string bankName;
    BankHeader header;
    BankEntry* entries      = nullptr;
    mutable bool needsCheck = false;
};

#endif
