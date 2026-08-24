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

#include "LocationOverlay.hpp"
#include "Configuration.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "pkx/PKX.hpp"
#include "utils.hpp"
#include <algorithm>
#include <map>

LocationOverlay::LocationOverlay(ReplaceableScreen& screen, pksm::PKX& pkm, bool met)
    : ListPickerOverlay(screen, i18n::localize("A_SELECT") + '\n' + i18n::localize("B_BACK"),
          i18n::localize(met ? "MET_LOCATION" : "EGG_LOCATION")),
      pkm(pkm),
      met(met)
{
    const std::map<u16, std::string>& rawLocations = i18n::rawLocations(
        Configuration::getInstance().language(), (pksm::Generation)pkm.version());
    validLocations.reserve(rawLocations.size());
    for (const auto& location : rawLocations)
    {
        validLocations.emplace_back(location.first, &location.second);
    }
    locations = validLocations;

    hid.update(locations.size());
    u16 current = met ? pkm.metLocation() : pkm.eggLocation();
    hid.select(std::distance(
        locations.begin(), std::find_if(locations.begin(), locations.end(),
                               [current](const std::pair<u16, const std::string*>& location)
                               { return location.first == current; })));
}

std::string LocationOverlay::entryLine(size_t index) const
{
    return std::to_string(locations[index].first) + " - " + *locations[index].second;
}

void LocationOverlay::filter(const std::string& search)
{
    if (search.empty())
    {
        locations = validLocations;
        return;
    }
    locations.clear();
    for (const auto& location : validLocations)
    {
        std::string locName = location.second->substr(0, search.size());
        StringUtils::toLower(locName);
        if (locName == search)
        {
            locations.emplace_back(location);
        }
    }
}

bool LocationOverlay::commit()
{
    if (!locations.empty())
    {
        if (met)
        {
            pkm.metLocation(locations[hid.fullIndex()].first);
        }
        else
        {
            pkm.eggLocation(locations[hid.fullIndex()].first);
        }
    }
    return true;
}
