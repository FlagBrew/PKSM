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

#include "GroupCloudAccess.hpp"
#include "Configuration.hpp"
#include "fetch.hpp"
#include "nlohmann/json.hpp"
#include "pkx/PK7.hpp"
#include "Presenter.hpp"
#include "revision.h"
#include <format>

namespace
{
    // The server legality-checks every Pokemon in the bundle before it answers, and says nothing
    // while it does. Over Fetch::STALL_GUARD_LIMIT on purpose: that is what keeps the stall guard
    // from tearing the request down mid-processing.
    constexpr long BUNDLE_TIMEOUT = 60;

    std::unique_ptr<pksm::PKX> emptyPkm()
    {
        return pksm::PKX::getPKM<pksm::Generation::SEVEN>(nullptr, pksm::PK7::BOX_LENGTH);
    }
}

GroupCloudAccess::GroupCloudAccess() : browser(GpssBrowser::Query{.kind = Gpss::Kind::Bundles}) {}

const GpssBrowser::Entry* GroupCloudAccess::entry(size_t groupIndex) const
{
    const auto& entries = browser.contents().entries;
    return groupIndex < entries.size() ? &entries[groupIndex] : nullptr;
}

const GpssBrowser::Mon* GroupCloudAccess::mon(size_t groupIndex, size_t pokeIndex) const
{
    const auto* group = entry(groupIndex);
    if (!group || pokeIndex >= group->pokemon.size())
    {
        return nullptr;
    }
    return &group->pokemon[pokeIndex];
}

std::unique_ptr<pksm::PKX> GroupCloudAccess::pkm(size_t groupIndex, size_t pokeIndex) const
{
    const auto* found = mon(groupIndex, pokeIndex);
    return found ? found->pkm->clone() : emptyPkm();
}

const pksm::PKX* GroupCloudAccess::peek(size_t groupIndex, size_t pokeIndex) const
{
    const auto* found = mon(groupIndex, pokeIndex);
    return found ? found->pkm.get() : nullptr;
}

bool GroupCloudAccess::isLegal(size_t groupIndex, size_t pokeIndex) const
{
    const auto* found = mon(groupIndex, pokeIndex);
    return found && found->legal;
}

std::vector<std::unique_ptr<pksm::PKX>> GroupCloudAccess::group(size_t groupIndex) const
{
    std::vector<std::unique_ptr<pksm::PKX>> ret;
    if (const auto* group = entry(groupIndex))
    {
        for (const auto& mon : group->pokemon)
        {
            ret.emplace_back(mon.pkm->clone());
        }
    }
    return ret;
}

std::vector<std::unique_ptr<pksm::PKX>> GroupCloudAccess::fetchGroup(size_t groupIndex) const
{
    auto ret = group(groupIndex);
    if (const auto* group = entry(groupIndex))
    {
        // The whole bundle counts as one download; the server credits every Pokémon in it.
        GpssBrowser::countDownload("bundles", group->downloadCode);
    }
    return ret;
}

void GroupCloudAccess::filterLegal(bool legal)
{
    auto query  = browser.query();
    query.legal = legal;
    browser.query(query);
}

long GroupCloudAccess::group(std::vector<std::unique_ptr<pksm::PKX>> sendMe)
{
    long ret = 0;

    const std::string pksm_version =
        "source: PKSM " +
        std::format("v{:d}.{:d}.{:d}-{:s}", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO, GIT_REV);

    std::string amount     = "count: " + std::to_string(sendMe.size());
    std::string generation = "generations: ";
    for (const auto& mon : sendMe)
    {
        generation += (std::string)mon->generation() + ',';
    }
    // Remove trailing comma
    generation.pop_back();

    std::vector<Fetch::Part> parts;
    parts.reserve(sendMe.size());
    for (size_t i = 0; i < sendMe.size(); i++)
    {
        parts.push_back({std::format("pkmn{:d}", i + 1), sendMe[i]->rawData()});
    }

    auto response =
        Fetch::postMultipart(Configuration::getInstance().apiUrl() + "api/v2/gpss/upload/bundle",
            parts, {amount, pksm_version, generation}, BUNDLE_TIMEOUT);
    if (!response.ok())
    {
        return ret;
    }

    ret = response.status;
    // Only the success responses carry a download code. Error statuses are reported by the
    // caller based on the returned status code, so don't try to parse them here (the body may
    // not even be JSON, which would crash).
    if (ret == 200 || ret == 201)
    {
        nlohmann::json retJson = nlohmann::json::parse(response.body, nullptr, false);
        if (retJson.is_object() && retJson.contains("code") && retJson["code"].is_string())
        {
            pksm::present::show(
                pksm::Notice::GpssDownloadCode, 0, retJson["code"].get<std::string>());
            browser.refresh();
        }
        else
        {
            pksm::present::show(pksm::Notice::GpssCommunicationError, ret);
        }
    }
    return ret;
}
