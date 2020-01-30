/*
 *   This file is part of PKSM-Core
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

#include "utils/ValueConverter.hpp"
#include "utils/g3values.h"
#include <algorithm>

u16 SpeciesConverter::g3ToNational(u16 v)
{
    if (v < g3ToSpecies.size())
    {
        return g3ToSpecies[v];
    }
    return 0;
}

u16 SpeciesConverter::nationalToG3(u16 v)
{
    if (v < speciesToG3.size())
    {
        return speciesToG3[v];
    }
    return 0;
}

u16 ItemConverter::g3ToNational(u16 v)
{
    if (v < g3ToItem.size())
    {
        return g3ToItem[v];
    }
    return 0;
}

u16 ItemConverter::nationalToG3(u16 v)
{
    if (v == 0)
    {
        return 0;
    }

    auto it = std::find(g3ToItem.begin(), g3ToItem.end(), v);
    if (it == g3ToItem.end())
    {
        return 0;
    }
    return *it;
}
