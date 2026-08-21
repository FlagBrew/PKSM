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
    long ret                  = 0;
    const std::string version = "generation: " + (std::string)mon->generation();
    const std::string pksm_version =
        "source: PKSM " +
        std::format("v{:d}.{:d}.{:d}-{:s}", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO, GIT_REV);

    struct curl_slist* headers = NULL;
    headers                    = curl_slist_append(headers, "Content-Type: multipart/form-data");
    headers                    = curl_slist_append(headers, pksm_version.c_str());
    headers                    = curl_slist_append(headers, version.c_str());

    std::string writeData;
    if (auto fetch =
            Fetch::init(Configuration::getInstance().apiUrl() + "api/v2/gpss/upload/pokemon", true,
                &writeData, headers, ""))
    {
        auto mimeThing       = fetch->mimeInit();
        curl_mimepart* field = curl_mime_addpart(mimeThing.get());
        curl_mime_name(field, "pkmn");
        curl_mime_data(field, (char*)mon->rawData().data(), mon->getLength());
        curl_mime_filename(field, "pkmn");
        fetch->setopt(CURLOPT_MIMEPOST, mimeThing.get());
        fetch->setopt(CURLOPT_TIMEOUT, 10L);

        auto res = Fetch::perform(fetch);
        if (res.index() == 1 && std::get<1>(res) == CURLE_OK)
        {
            fetch->getinfo(CURLINFO_RESPONSE_CODE, &ret);
            browser.refresh();
        }
    }
    curl_slist_free_all(headers);
    return ret;
}
