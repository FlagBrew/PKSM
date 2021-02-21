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

#include "GroupCloudAccess.hpp"
#include "Configuration.hpp"
#include "base64.hpp"
#include "fetch.hpp"
#include "format.h"
#include "nlohmann/json.hpp"
#include "pkx/PB7.hpp"
#include "pkx/PK4.hpp"
#include "pkx/PK5.hpp"
#include "pkx/PK6.hpp"
#include "pkx/PK7.hpp"
#include "pkx/PK8.hpp"
#include <unistd.h>

GroupCloudAccess::Page::~Page() {}

void GroupCloudAccess::downloadGroupPage(std::shared_ptr<Page> page, int number, bool legal,
    pksm::Generation low, pksm::Generation high, bool LGPE)
{
    std::string* retData = new std::string;
    auto fetch           = Fetch::init(
        GroupCloudAccess::makeURL(number, legal, low, high, LGPE), true, retData, nullptr, "");
    Fetch::performAsync(fetch, [page, retData](CURLcode code, std::shared_ptr<Fetch> fetch) {
        if (code == CURLE_OK)
        {
            long status_code;
            fetch->getinfo(CURLINFO_RESPONSE_CODE, &status_code);
            switch (status_code)
            {
                case 200:
                    page->data = std::make_unique<nlohmann::json>(
                        nlohmann::json::parse(*retData, nullptr, false));
                    // clang-format off
                    if (!page->data || !pageIsGood(*page->data))
                    // clang-format on
                    {
                        page->data = nullptr;
                    }
                    break;
                case 401:
                {
                    nlohmann::json retJson = nlohmann::json::parse(*retData, nullptr, false);
                    if (retJson.contains("error_code") && retJson["error_code"].is_number_integer())
                    {
                        page->siteJsonErrorCode = retJson["error_code"].get<int>();
                    }
                }
                break;
                default:
                    break;
            }
        }
        delete retData;
        page->available = true;
    });
}

GroupCloudAccess::GroupCloudAccess() : pageNumber(1)
{
    refreshPages();
}

void GroupCloudAccess::refreshPages()
{
    current            = std::make_shared<Page>();
    current->data      = std::make_unique<nlohmann::json>(grabPage(pageNumber));
    current->available = true;
    if (current->data)
    {
        if (pageIsGood(*current->data))
        {
            isGood = true;
        }
        else
        {
            isGood = false;
            if (current->data->contains("error_code") &&
                (*current->data)["error_code"].is_number_integer())
            {
                current->siteJsonErrorCode = (*current->data)["error_code"].get<int>();
                current->data              = nullptr;
            }
        }
    }
    if (isGood && pageNumber >= pages())
    {
        pageNumber         = pages();
        current->data      = std::make_unique<nlohmann::json>(grabPage(pages()));
        current->available = true;
        if (current->data)
        {
            if (pageIsGood(*current->data))
            {
                isGood = true;
            }
            else
            {
                isGood = false;
                if (current->data->contains("error_code") &&
                    (*current->data)["error_code"].is_number_integer())
                {
                    current->siteJsonErrorCode = (*current->data)["error_code"].get<int>();
                    current->data              = nullptr;
                }
            }
        }
    }
    if (isGood)
    {
        if (pages() > 1)
        {
            int page = (pageNumber % pages()) + 1;
            next     = std::make_shared<Page>();
            downloadGroupPage(next, page, legal, low, high, LGPE);
            if (pages() > 2)
            {
                page = pageNumber - 1 == 0 ? pages() : pageNumber - 1;
                prev = std::make_shared<Page>();
                downloadGroupPage(prev, page, legal, low, high, LGPE);
            }
            else
            {
                prev = next;
            }
        }
        else
        {
            next = prev = current;
        }
    }
}

nlohmann::json GroupCloudAccess::grabPage(int num)
{
    std::string retData;
    auto fetch = Fetch::init(makeURL(num, legal, low, high, LGPE), true, &retData, nullptr, "");
    auto res   = Fetch::perform(fetch);
    if (res.index() == 0)
    {
        return {};
    }
    else if (std::get<1>(res) == CURLE_OK)
    {
        return nlohmann::json::parse(retData, nullptr, false);
    }
    else
    {
        return {};
    }
}

std::string GroupCloudAccess::makeURL(
    int num, bool legal, pksm::Generation low, pksm::Generation high, bool LGPE)
{
    return WEBSITE_URL "api/v1/gpss/bundles/all?count=" + std::to_string(NUM_GROUPS) +
           "&min_gen=" + (std::string)low + "&max_gen=" + (std::string)high +
           "&lgpe=" + (LGPE ? std::string("yes") : std::string("no")) +
           "&page=" + std::to_string(num) + (legal ? "&legal_only=yes" : "");
}

std::optional<int> GroupCloudAccess::nextPage()
{
    while (!next->available)
    {
        constexpr timespec sleepTime = {0, 100000};
        nanosleep(&sleepTime, nullptr);
    }
    if (!next->data || next->data->is_discarded())
    {
        isGood = false;
        return next->siteJsonErrorCode;
    }
    // Update data
    pageNumber = (pageNumber % pages()) + 1;
    prev       = current;
    current    = next;
    next       = std::make_shared<Page>();

    // Download next page in the background
    int nextPage = (pageNumber % pages()) + 1;
    downloadGroupPage(next, nextPage, legal, low, high, LGPE);

    // If there's a mon number desync, also download the previous page again
    if ((*current->data)["total_bundles"] != (*prev->data)["total_bundles"])
    {
        int prevPage = pageNumber - 1 == 0 ? pages() : pageNumber - 1;
        downloadGroupPage(prev, prevPage, legal, low, high, LGPE);
    }

    return std::nullopt;
}

std::optional<int> GroupCloudAccess::prevPage()
{
    while (!prev->available)
    {
        constexpr timespec sleepTime = {0, 100000};
        nanosleep(&sleepTime, nullptr);
    }
    if (!prev->data || prev->data->is_discarded())
    {
        isGood = false;
        return prev->siteJsonErrorCode;
    }
    // Update data
    pageNumber = pageNumber - 1 == 0 ? pages() : pageNumber - 1;
    next       = current;
    current    = prev;
    prev       = std::make_shared<Page>();

    // Download the next page in the background
    int prevPage = pageNumber - 1 == 0 ? pages() : pageNumber - 1;
    downloadGroupPage(prev, prevPage, legal, low, high, LGPE);

    // If there's a mon number desync, also download the next page again
    if ((*current->data)["total_bundles"] != (*next->data)["total_bundles"])
    {
        int nextPage = (pageNumber % pages()) + 1;
        downloadGroupPage(next, nextPage, legal, low, high, LGPE);
    }

    return std::nullopt;
}

int GroupCloudAccess::pages() const
{
    return (*current->data)["pages"].get<int>();
}

std::unique_ptr<pksm::PKX> GroupCloudAccess::pkm(size_t groupIndex, size_t pokeIndex) const
{
    if (groupIndex < (*current->data)["results"].size())
    {
        auto& group = (*current->data)["results"][groupIndex];
        if (pokeIndex < group["pokemon"].size())
        {
            auto& poke           = group["pokemon"][pokeIndex];
            std::vector<u8> data = base64_decode(poke["base64"].get<std::string>());
            pksm::Generation gen =
                pksm::Generation::fromString(poke["generation"].get<std::string>());

            auto ret = pksm::PKX::getPKM(gen, data.data(), data.size());
            if (ret)
            {
                return ret;
            }
        }
    }
    return pksm::PKX::getPKM<pksm::Generation::SEVEN>(nullptr);
}

bool GroupCloudAccess::isLegal(size_t groupIndex, size_t pokeIndex) const
{
    if (groupIndex < (*current->data)["results"].size())
    {
        auto& group = (*current->data)["results"][groupIndex];
        if (pokeIndex < group["pokemon"].size())
        {
            return group["pokemon"][pokeIndex]["legal"];
        }
    }
    return false;
}

std::unique_ptr<pksm::PKX> GroupCloudAccess::fetchPkm(size_t groupIndex, size_t pokeIndex) const
{
    if (groupIndex < (*current->data)["results"].size())
    {
        auto& group = (*current->data)["results"][groupIndex];
        if (pokeIndex < group["pokemon"].size())
        {
            auto& poke = group["pokemon"][pokeIndex];
            auto ret   = pkm(groupIndex, pokeIndex);

            if (auto fetch = Fetch::init(
                    WEBSITE_URL "gpss/download/" + poke["code"].get<std::string>(), true,
                    nullptr, nullptr, ""))
            {
                Fetch::performAsync(fetch);
            }

            return ret;
        }
    }
    return pksm::PKX::getPKM<pksm::Generation::SEVEN>(nullptr);
}

std::vector<std::unique_ptr<pksm::PKX>> GroupCloudAccess::group(size_t groupIndex) const
{
    std::vector<std::unique_ptr<pksm::PKX>> ret;
    if (groupIndex < (*current->data)["results"].size())
    {
        auto& group = (*current->data)["results"][groupIndex];
        for (size_t i = 0; i < group["pokemon"].size(); i++)
        {
            ret.emplace_back(pkm(groupIndex, i));
        }
    }
    return ret;
}

std::vector<std::unique_ptr<pksm::PKX>> GroupCloudAccess::fetchGroup(size_t groupIndex) const
{
    std::vector<std::unique_ptr<pksm::PKX>> ret;
    if (groupIndex < (*current->data)["results"].size())
    {
        auto& group = (*current->data)["results"][groupIndex];
        for (size_t i = 0; i < group["pokemon"].size(); i++)
        {
            // When the full group is downloaded, all the individual download counters will be
            // incremented
            ret.emplace_back(pkm(groupIndex, i));
        }
        if (auto fetch = Fetch::init(
                "https://github.com/gpss/download/bundle/" + group["code"].get<std::string>(), true,
                nullptr, nullptr, ""))
        {
            Fetch::performAsync(fetch);
        }
    }

    return ret;
}

long GroupCloudAccess::group(std::vector<std::unique_ptr<pksm::PKX>> sendMe)
{
    long ret               = 0;
    std::string amount     = "amount: " + std::to_string(sendMe.size());
    std::string code       = Configuration::getInstance().patronCode();
    std::string generation = "Generations: ";
    for (const auto& mon : sendMe)
    {
        generation += (std::string)mon->generation() + ',';
    }
    // Remove trailing comma
    generation.pop_back();
    if (!code.empty())
    {
        code = "PC: " + code;
    }
    struct curl_slist* headers = NULL;
    headers                    = curl_slist_append(headers, amount.c_str());
    headers                    = curl_slist_append(headers, "bundle: yes");
    headers                    = curl_slist_append(headers, generation.c_str());
    if (!code.empty())
    {
        headers = curl_slist_append(headers, code.c_str());
    }

    std::string writeData;
    if (auto fetch = Fetch::init(WEBSITE_URL "gpss/share", true, &writeData, headers, ""))
    {
        auto mimeThing = fetch->mimeInit();
        for (size_t i = 0; i < sendMe.size(); i++)
        {
            curl_mimepart* field  = curl_mime_addpart(mimeThing.get());
            std::string fieldName = fmt::format("pkmn{:d}", i + 1);
            curl_mime_name(field, fieldName.c_str());
            curl_mime_data(field, (char*)sendMe[i]->rawData(), sendMe[i]->getLength());
            curl_mime_filename(field, fieldName.c_str());
        }
        fetch->setopt(CURLOPT_MIMEPOST, mimeThing.get());

        auto res = Fetch::perform(fetch);
        if (res.index() == 1 && std::get<1>(res) == CURLE_OK)
        {
            fetch->getinfo(CURLINFO_RESPONSE_CODE, &ret);
            if (ret == 201)
            {
                refreshPages();
            }
        }
    }
    curl_slist_free_all(headers);
    return ret;
}

bool GroupCloudAccess::pageIsGood(const nlohmann::json& page)
{
    // clang-format off
    if (!page.is_object() ||
        !page.contains("total_bundles") || !page["total_bundles"].is_number_integer() ||
        !page.contains("pages") || !page["pages"].is_number_integer() ||
        !page.contains("results") || !page["results"].is_array())
    // clang-format on
    {
        return false;
    }
    else
    {
        for (const auto& group : page["results"])
        {
            // clang-format off
            if (!group.is_object() ||
                !group.contains("pokemon") || !group["pokemon"].is_array() ||
                !group.contains("code") || !group["code"].is_string())
            // clang-format on
            {
                return false;
            }
            else
            {
                for (const auto& pkm : group["pokemon"])
                {
                    // clang-format off
                    if (!pkm.is_object() ||
                        !pkm.contains("base64") || !pkm["base64"].is_string() ||
                        !pkm.contains("generation") || !pkm["generation"].is_string() ||
                        !pkm.contains("legal") || !pkm["legal"].is_boolean() ||
                        !pkm.contains("code") || !pkm["code"].is_string())
                    // clang-format on
                    {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}
