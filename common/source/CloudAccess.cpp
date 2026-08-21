/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2025 Bernardo Giordano, Admiral Fish, piepie62
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

#include "CloudAccess.hpp"
#include "Configuration.hpp"
#include "fetch.hpp"
#include "pkx/PK7.hpp"
#include "revision.h"
#include <format>

namespace
{
    // One Pokémon going up, with the server only checking it: nothing here needs longer.
    constexpr long UPLOAD_TIMEOUT = 10;

    std::unique_ptr<pksm::PKX> emptyPkm()
    {
        return pksm::PKX::getPKM<pksm::Generation::SEVEN>(nullptr, pksm::PK7::BOX_LENGTH);
    }
}

CloudAccess::CloudAccess() : browser(GpssBrowser::Query{.kind = Gpss::Kind::Pokemon}) {}

const GpssBrowser::Mon* CloudAccess::mon(size_t slot) const
{
    const auto& entries = browser.contents().entries;
    // A Pokémon page is a page of one-Pokémon bundles, so a slot is an entry.
    if (slot >= entries.size() || entries[slot].pokemon.empty())
    {
        return nullptr;
    }
    return &entries[slot].pokemon.front();
}

std::unique_ptr<pksm::PKX> CloudAccess::pkm(size_t slot) const
{
    const auto* found = mon(slot);
    return found ? found->pkm->clone() : emptyPkm();
}

const pksm::PKX* CloudAccess::peek(size_t slot) const
{
    const auto* found = mon(slot);
    return found ? found->pkm.get() : nullptr;
}

bool CloudAccess::isLegal(size_t slot) const
{
    const auto* found = mon(slot);
    return found && found->legal;
}

std::unique_ptr<pksm::PKX> CloudAccess::fetchPkm(size_t slot) const
{
    const auto* found = mon(slot);
    if (!found)
    {
        return emptyPkm();
    }
    GpssBrowser::countDownload("pokemon", found->downloadCode);
    return found->pkm->clone();
}

void CloudAccess::sortType(SortType type)
{
    auto query    = browser.query();
    query.popular = type == POPULAR;
    browser.query(query);
}

void CloudAccess::sortDir(bool ascend)
{
    auto query      = browser.query();
    query.ascending = ascend;
    browser.query(query);
}

void CloudAccess::filterLegal(bool legal)
{
    auto query  = browser.query();
    query.legal = legal;
    browser.query(query);
}

long CloudAccess::pkm(std::unique_ptr<pksm::PKX> mon)
{
    const std::string version = "generation: " + (std::string)mon->generation();
    const std::string pksm_version =
        "source: PKSM " +
        std::format("v{:d}.{:d}.{:d}-{:s}", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO, GIT_REV);

    const Fetch::Part parts[] = {
        {"pkmn", mon->rawData()}
    };
    auto response =
        Fetch::postMultipart(Configuration::getInstance().apiUrl() + "api/v2/gpss/upload/pokemon",
            parts, {pksm_version, version}, UPLOAD_TIMEOUT);
    if (!response.ok())
    {
        return 0;
    }

    browser.refresh();
    return response.status;
}
