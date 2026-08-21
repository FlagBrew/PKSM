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

#include "GpssBrowser.hpp"
#include "Configuration.hpp"
#include "fetch.hpp"
#include "nlohmann/json.hpp"
#include "pkx/PK7.hpp"
#include <algorithm>
#include <unistd.h>

namespace
{
    constexpr long PAGE_TIMEOUT = 10L;

    const GpssBrowser::Contents& emptyContents()
    {
        static const GpssBrowser::Contents empty;
        return empty;
    }

    std::unique_ptr<pksm::PKX> emptyPkm()
    {
        return pksm::PKX::getPKM<pksm::Generation::SEVEN>(nullptr, pksm::PK7::BOX_LENGTH);
    }

    // Every Pokémon on the page is built here, once, off the drawing thread. A slot the server
    // sent in a format this build cannot read becomes an empty Pokémon rather than a hole the
    // screens have to check for.
    std::shared_ptr<const GpssBrowser::Contents> decode(Gpss::Page& page)
    {
        auto contents   = std::make_shared<GpssBrowser::Contents>();
        contents->total = page.total;
        contents->pages = page.pages;
        for (auto& entry : page.entries)
        {
            GpssBrowser::Entry decoded;
            decoded.downloadCode = entry.downloadCode;
            for (auto& mon : entry.pokemon)
            {
                auto pkm = pksm::PKX::getPKM(mon.generation, mon.data.data(), mon.data.size());
                decoded.pokemon.emplace_back(GpssBrowser::Mon{
                    pkm ? std::move(pkm) : emptyPkm(), mon.legal, mon.downloadCode});
            }
            contents->entries.emplace_back(std::move(decoded));
        }
        return contents;
    }

    Fetch::Headers pageHeaders()
    {
        return {"Content-Type: application/json;charset=UTF-8", "pksm-mode: yes"};
    }
}

GpssBrowser::GpssBrowser(Query query) : searchQuery(std::move(query))
{
    refresh();
}

void GpssBrowser::query(const Query& query)
{
    if (query == searchQuery)
    {
        return;
    }
    searchQuery = query;
    refresh();
}

int GpssBrowser::errorCode() const
{
    return current ? current->errorCode.load() : 0;
}

int GpssBrowser::pages() const
{
    return std::max(1, currentContents()->pages);
}

std::shared_ptr<const GpssBrowser::Contents> GpssBrowser::currentContents() const
{
    if (current && current->contents)
    {
        return current->contents;
    }
    // Aliased at the static empty page: a caller that draws before it checks good() reads an
    // empty page rather than a dangling one.
    return {std::shared_ptr<const Contents>{}, &emptyContents()};
}

std::pair<std::string, std::string> GpssBrowser::makeURL(const Query& query, int page)
{
    nlohmann::json postData = nlohmann::json::object();
    postData.push_back({"mode", "and"});
    postData.push_back({"legal", query.legal});

    static constexpr pksm::Generation GENS_IN_ORDER[] = {
        pksm::Generation::ONE,
        pksm::Generation::TWO,
        pksm::Generation::THREE,
        pksm::Generation::FOUR,
        pksm::Generation::FIVE,
        pksm::Generation::SIX,
        pksm::Generation::SEVEN,
        pksm::Generation::EIGHT,
    };
    const auto start = std::find(std::begin(GENS_IN_ORDER), std::end(GENS_IN_ORDER), query.low);
    const auto end   = std::find(std::begin(GENS_IN_ORDER), std::end(GENS_IN_ORDER), query.high);

    nlohmann::json generations = nlohmann::json::array();
    for (auto gen = start; gen <= end; ++gen)
    {
        generations.push_back(std::string(*gen));
    }
    if (query.lgpe)
    {
        generations.push_back("LGPE");
    }
    postData.push_back({"generations", generations});

    postData.push_back({"operators",
        R"([{"operator":"=","field":"legal"},{"operator":"IN","field":"generations"}])"_json});

    postData.push_back({"sort_field", query.popular ? "popularity" : "latest"});
    postData.push_back({"sort_direction", !query.ascending});

    const std::string search = query.kind == Gpss::Kind::Pokemon ? "pokemon" : "bundles";
    return {Configuration::getInstance().apiUrl() + "api/v2/gpss/search/" + search +
                "?page=" + std::to_string(page),
        postData.dump()};
}

void GpssBrowser::countDownload(const std::string& what, const std::string& code)
{
    if (code.empty())
    {
        return;
    }
    Fetch::getAsync(
        Configuration::getInstance().apiUrl() + "api/v2/gpss/download/" + what + '/' + code);
}

std::shared_ptr<GpssBrowser::Slot> GpssBrowser::fetch(int number)
{
    auto slot                  = std::make_shared<Slot>();
    const auto [url, postData] = makeURL(searchQuery, number);
    const Gpss::Kind kind      = searchQuery.kind;

    // Fetch runs the callback exactly once whatever becomes of the request, so a page that was
    // never even sent still becomes available and whoever waits for it stops waiting.
    Fetch::postJsonAsync(
        url, postData,
        [slot, kind](Fetch::Response response)
        {
            if (response.ok())
            {
                if (response.status == 200)
                {
                    auto parsed = Gpss::parse(response.body, kind);
                    if (auto* page = std::get_if<Gpss::Page>(&parsed))
                    {
                        slot->contents = decode(*page);
                    }
                }
                if (!slot->contents)
                {
                    if (auto error = Gpss::errorCode(response.body, kind))
                    {
                        slot->errorCode = *error;
                    }
                }
            }
            // Last, and only once: everything above happened before the page was published.
            slot->ready = true;
        },
        pageHeaders(), PAGE_TIMEOUT);

    return slot;
}

std::shared_ptr<GpssBrowser::Slot> GpssBrowser::fetchNow(int number)
{
    auto slot                  = std::make_shared<Slot>();
    const auto [url, postData] = makeURL(searchQuery, number);

    auto response = Fetch::postJson(url, postData, pageHeaders(), PAGE_TIMEOUT);
    if (response.ok())
    {
        auto parsed = Gpss::parse(response.body, searchQuery.kind);
        if (auto* page = std::get_if<Gpss::Page>(&parsed))
        {
            slot->contents = decode(*page);
        }
        else if (auto error = Gpss::errorCode(response.body, searchQuery.kind))
        {
            slot->errorCode = *error;
        }
    }

    slot->ready = true;
    return slot;
}

std::optional<int> GpssBrowser::adopt(std::shared_ptr<Slot> slot, int number)
{
    while (!slot->ready.load())
    {
        static constexpr timespec sleepTime = {0, 1000000}; // 1 ms
        nanosleep(&sleepTime, nullptr);
    }

    if (!slot->contents)
    {
        isGood  = false;
        current = std::move(slot);
        return errorCode();
    }

    current    = std::move(slot);
    pageNumber = number;
    isGood     = true;
    return std::nullopt;
}

std::optional<int> GpssBrowser::nextPage()
{
    const int number = (pageNumber % pages()) + 1;
    auto arriving    = next;
    auto departing   = current;

    if (auto error = adopt(std::move(arriving), number))
    {
        return error;
    }

    // The page we came from is already in hand, so it becomes the one behind us for free.
    prev = departing;
    if (pages() > 1)
    {
        next = fetch((pageNumber % pages()) + 1);
    }
    else
    {
        next = prev = current;
    }
    if (pages() == 2)
    {
        prev = next;
    }
    // Someone uploaded while we were paging, so the page behind us is not the one we came from
    // any more. It comes down again into a page of its own: the one we came from is what the
    // caller was reading a moment ago.
    else if (pages() > 2 && departing->contents && contents().total != departing->contents->total)
    {
        prev = fetch(pageNumber - 1 == 0 ? pages() : pageNumber - 1);
    }

    return std::nullopt;
}

std::optional<int> GpssBrowser::prevPage()
{
    const int number = pageNumber - 1 == 0 ? pages() : pageNumber - 1;
    auto arriving    = prev;
    auto departing   = current;

    if (auto error = adopt(std::move(arriving), number))
    {
        return error;
    }

    next = departing;
    if (pages() > 1)
    {
        prev = fetch(pageNumber - 1 == 0 ? pages() : pageNumber - 1);
    }
    else
    {
        next = prev = current;
    }
    if (pages() == 2)
    {
        next = prev;
    }
    else if (pages() > 2 && departing->contents && contents().total != departing->contents->total)
    {
        next = fetch((pageNumber % pages()) + 1);
    }

    return std::nullopt;
}

std::optional<int> GpssBrowser::jumpPage(int page)
{
    if (!isGood)
    {
        return errorCode();
    }

    page = std::clamp(page, 1, pages());
    if (page == pageNumber)
    {
        return std::nullopt;
    }

    pageNumber = page;
    refresh();
    return isGood ? std::optional<int>{} : std::optional<int>{errorCode()};
}

void GpssBrowser::refresh()
{
    auto loaded = fetchNow(pageNumber);
    if (adopt(loaded, pageNumber))
    {
        return;
    }

    // A search can shrink under a page number that was valid when it was chosen.
    if (pageNumber > pages())
    {
        const int last = pages();
        if (adopt(fetchNow(last), last))
        {
            return;
        }
    }

    if (pages() > 1)
    {
        next = fetch((pageNumber % pages()) + 1);
        prev = pages() > 2 ? fetch(pageNumber - 1 == 0 ? pages() : pageNumber - 1) : next;
    }
    else
    {
        next = prev = current;
    }
}
