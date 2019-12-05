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

#include "Sav8.hpp"
#include "PK8.hpp"

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
