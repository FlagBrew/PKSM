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

#include "Sav8.hpp"
#include "PK8.hpp"
#include <algorithm>

Sav8::Sav8(std::shared_ptr<u8[]> dt, size_t length) : Sav(dt, length)
{
    swshcrypto_applyXor(dt, length);
    blocks = swshcrypto_getBlockList(dt, length);
}

std::shared_ptr<SCBlock> Sav8::getBlock(u32 key) const
{
    int min = 0, mid = 0, max = blocks.size();
    while (min <= max)
    {
        mid = min + (max - min) / 2;
        if (blocks[mid]->key() == key)
        {
            return blocks[mid];
        }
        if (blocks[mid]->key() < key)
        {
            min = mid + 1;
        }
        else
        {
            max = mid - 1;
        }
    }

    return nullptr;
}

std::shared_ptr<PKX> Sav8::emptyPkm() const
{
    return std::make_shared<PK8>();
}

const std::set<int>& Sav8::availableItems(void) const
{
    if (items.empty())
    {
        fill_set(items, 0, maxItem());
    }
    return items;
}

const std::set<int>& Sav8::availableMoves(void) const
{
    if (moves.empty())
    {
        fill_set(moves, 0, maxMove());
    }
    return moves;
}

const std::set<int>& Sav8::availableSpecies(void) const
{
    if (species.empty())
    {
        fill_set(species, 1, maxSpecies());
    }
    return species;
}

const std::set<int>& Sav8::availableAbilities(void) const
{
    if (abilities.empty())
    {
        fill_set(abilities, 1, maxAbility());
    }
    return abilities;
}

const std::set<int>& Sav8::availableBalls(void) const
{
    if (balls.empty())
    {
        fill_set(balls, 1, maxBall());
    }
    return balls;
}

void Sav8::trade(std::shared_ptr<PKX> pk)
{
    if (pk->egg())
    {
        if (pk->otName() != otName() || pk->TID() != TID() || pk->SID() != SID() || pk->gender() != gender())
        {
            pk->metLocation(30002);
        }
    }
    else
    {
        if (pk->otName() != otName() || pk->TID() != TID() || pk->SID() != SID() || pk->gender() != gender())
        {
            pk->currentHandler(0);
        }
        else
        {
            pk->currentHandler(1);
            ((PK8*)pk.get())->htName(otName());
            ((PK8*)pk.get())->currentFriendship(pk->baseFriendship());
            ((PK8*)pk.get())->htGender(gender());
            ((PK8*)pk.get())->htLanguage(language());
        }
    }
}

void Sav8::encrypt()
{
    for (auto& block : blocks)
    {
        block->encrypt();
    }

    swshcrypto_applyXor(data, length);
    swshcrypto_sign(data, length);
}

void Sav8::decrypt()
{
    swshcrypto_applyXor(data, length);

    // I could decrypt every block here, but why not just let them be done on the fly via the functions that need them?
}
