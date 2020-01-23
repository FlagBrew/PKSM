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

#ifndef BANKS_HPP
#define BANKS_HPP

#include "Bank.hpp"
#include "types.h"

#define BANKS_VERSION 1
#define BANK_DEFAULT_SIZE 50
#define BANK_MAX_SIZE 500

class Bank;

namespace Banks
{
    inline std::shared_ptr<Bank> bank = nullptr;
    Result init();
    Result swapSD(bool toSD);
    Result saveJson();
    bool loadBank(const std::string& name, const std::optional<int>& maxBoxes = std::nullopt);
    void removeBank(const std::string& name);
    void renameBank(const std::string& oldName, const std::string& newName);
    void setBankSize(const std::string& name, int size);
    std::vector<std::pair<std::string, int>> bankNames();
}

#endif
