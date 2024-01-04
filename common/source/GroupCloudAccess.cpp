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

#include "GroupCloudAccess.hpp"
#include "base64.hpp"
#include "Configuration.hpp"
#include "fetch.hpp"
#include "gui.hpp"
#include "nlohmann/json.hpp"
#include "pkx/PB7.hpp"
#include "pkx/PK4.hpp"
#include "pkx/PK5.hpp"
#include "pkx/PK6.hpp"
#include "pkx/PK7.hpp"
#include "pkx/PK8.hpp"
#include "revision.h"
#include "website.h"
#include <format>
#include <unistd.h>

GroupCloudAccess::Page::~Page() {}

void GroupCloudAccess::downloadGroupPage(std::shared_ptr<Page> page, int number, bool legal,
    pksm::Generation low, pksm::Generation high, bool LGPE)
{
    std::string* retData       = new std::string;
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json;charset=UTF-8");
    headers = curl_slist_append(headers, "pksm-mode: yes");

    const auto [url, postData] = GroupCloudAccess::makeURL(number, legal, low, high, LGPE);
    auto fetch                 = Fetch::init(url, true, retData, headers, postData);
    Fetch::performAsync(fetch,
        [page, retData, headers](CURLcode code, std::shared_ptr<Fetch> fetch)
        {
            if (code == CURLE_OK)
            {
                long status_code;
                fetch->getinfo(CURLINFO_RESPONSE_CODE, &status_code);
                switch (status_code)
                {
                    case 200:
                        page->data = std::make_unique<nlohmann::json>(
                            nlohmann::json::parse(*retData, nullptr, false));
                        if (!page->data || !pageIsGood(*page->data))
                        {
                            page->data = nullptr;
                        }
                        break;
                    case 401:
                    {
                        nlohmann::json retJson = nlohmann::json::parse(*retData, nullptr, false);
                        if (retJson.contains("error_code") &&
                            retJson["error_code"].is_number_integer())
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
            curl_slist_free_all(headers);
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

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json;charset=UTF-8");
    headers = curl_slist_append(headers, "pksm-mode: yes");

    const auto [url, postData] = GroupCloudAccess::makeURL(num, legal, low, high, LGPE);
    auto fetch                 = Fetch::init(url, true, &retData, headers, postData);
    auto res                   = Fetch::perform(fetch);
    curl_slist_free_all(headers);

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

std::pair<std::string, std::string> GroupCloudAccess::makeURL(
    int num, bool legal, pksm::Generation low, pksm::Generation high, bool LGPE)
{
    nlohmann::json post_data = nlohmann::json::object();
    post_data.push_back({"mode", "and"});
    post_data.push_back({"legal", legal});

    const pksm::Generation gens_in_order[] = {
        pksm::Generation::ONE,
        pksm::Generation::TWO,
        pksm::Generation::THREE,
        pksm::Generation::FOUR,
        pksm::Generation::FIVE,
        pksm::Generation::SIX,
        pksm::Generation::SEVEN,
        pksm::Generation::EIGHT,
    };
    const auto start_idx = std::find(std::begin(gens_in_order), std::end(gens_in_order), low);
    const auto end_idx   = std::find(std::begin(gens_in_order), std::end(gens_in_order), high);

    nlohmann::json generations_data = nlohmann::json::array();
    for (auto idx = start_idx; idx <= end_idx; ++idx)
    {
        generations_data.push_back(std::string(*idx));
    }

    if (LGPE)
    {
        generations_data.push_back("LGPE");
    }

    post_data.push_back({"generations", generations_data});

    nlohmann::json operators_data =
        R"([{"operator":"=","field":"legal"},{"operator":"IN","field":"generations"}])"_json;
    post_data.push_back({"operators", operators_data});

    post_data.push_back({"sort_field", "latest"});
    post_data.push_back({"sort_direction", false});

    return {WEBSITE_URL "api/v2/gpss/search/bundles?page=" + std::to_string(num), post_data.dump()};
}

std::optional<int> GroupCloudAccess::nextPage()
{
    next->available.wait(false);
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
    if ((*current->data)["total"] != (*prev->data)["total"])
    {
        int prevPage = pageNumber - 1 == 0 ? pages() : pageNumber - 1;
        downloadGroupPage(prev, prevPage, legal, low, high, LGPE);
    }

    return std::nullopt;
}

std::optional<int> GroupCloudAccess::prevPage()
{
    prev->available.wait(false);
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
    if ((*current->data)["total"] != (*next->data)["total"])
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
    if (groupIndex < (*current->data)["bundles"].size())
    {
        auto& group = (*current->data)["bundles"][groupIndex];
        if (pokeIndex < group["count"].get<size_t>())
        {
            std::vector<u8> data =
                base64_decode(group["pokemons"][pokeIndex]["base_64"].get<std::string>());
            pksm::Generation gen = pksm::Generation::fromString(
                group["pokemons"][pokeIndex]["generation"].get<std::string>());

            auto ret = pksm::PKX::getPKM(gen, data.data(), data.size());
            if (ret)
            {
                return ret;
            }
        }
    }
    return pksm::PKX::getPKM<pksm::Generation::SEVEN>(nullptr, pksm::PK7::BOX_LENGTH);
}

bool GroupCloudAccess::isLegal(size_t groupIndex, size_t pokeIndex) const
{
    if (groupIndex < (*current->data)["bundles"].size())
    {
        auto& group = (*current->data)["bundles"][groupIndex];
        if (pokeIndex < group["count"].get<size_t>())
        {
            return group["pokemons"][pokeIndex]["legality"].get<bool>();
        }
    }
    return false;
}

std::unique_ptr<pksm::PKX> GroupCloudAccess::fetchPkm(size_t groupIndex, size_t pokeIndex) const
{
    if (groupIndex < (*current->data)["bundles"].size())
    {
        auto& group = (*current->data)["bundles"][groupIndex];
        if (pokeIndex < group["count"].get<size_t>())
        {
            auto ret = pkm(groupIndex, pokeIndex);

            if (auto fetch = Fetch::init(WEBSITE_URL "api/v2/gpss/download/pokemon/" +
                                             group["download_codes"][pokeIndex].get<std::string>(),
                    true, nullptr, nullptr, ""))
            {
                Fetch::performAsync(fetch);
            }

            return ret;
        }
    }
    return pksm::PKX::getPKM<pksm::Generation::SEVEN>(nullptr, pksm::PK7::BOX_LENGTH);
}

std::vector<std::unique_ptr<pksm::PKX>> GroupCloudAccess::group(size_t groupIndex) const
{
    std::vector<std::unique_ptr<pksm::PKX>> ret;
    if (groupIndex < (*current->data)["bundles"].size())
    {
        auto& group = (*current->data)["bundles"][groupIndex];
        for (int i = 0; i < group["count"].get<int>(); i++)
        {
            ret.emplace_back(pkm(groupIndex, i));
        }
    }
    return ret;
}

std::vector<std::unique_ptr<pksm::PKX>> GroupCloudAccess::fetchGroup(size_t groupIndex) const
{
    std::vector<std::unique_ptr<pksm::PKX>> ret;
    if (groupIndex < (*current->data)["bundles"].size())
    {
        auto& group = (*current->data)["bundles"][groupIndex];
        for (int i = 0; i < group["count"].get<int>(); i++)
        {
            // When the full group is downloaded, all the individual download counters will be
            // incremented
            ret.emplace_back(pkm(groupIndex, i));
        }
        if (auto fetch = Fetch::init(WEBSITE_URL "api/v2/gpss/download/bundles/" +
                                         group["download_code"].get<std::string>(),
                true, nullptr, nullptr, ""))
        {
            Fetch::performAsync(fetch);
        }
    }

    return ret;
}

long GroupCloudAccess::group(std::vector<std::unique_ptr<pksm::PKX>> sendMe)
{
    long ret = 0;

    const std::string pksm_version =
        "source: PKSM " +
        std::format("v{:d}.{:d}.{:d}-{:s}", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO, GIT_REV);

    std::string amount     = "count: " + std::to_string(sendMe.size());
    std::string code       = Configuration::getInstance().patronCode();
    std::string generation = "generations: ";
    for (const auto& mon : sendMe)
    {
        generation += (std::string)mon->generation() + ',';
    }
    // Remove trailing comma
    generation.pop_back();
    if (!code.empty())
    {
        code = "patreon: " + code;
    }

    struct curl_slist* headers = NULL;
    headers                    = curl_slist_append(headers, amount.c_str());
    headers                    = curl_slist_append(headers, pksm_version.c_str());
    headers                    = curl_slist_append(headers, generation.c_str());
    if (!code.empty())
    {
        headers = curl_slist_append(headers, code.c_str());
    }

    std::string writeData;
    if (auto fetch =
            Fetch::init(WEBSITE_URL "api/v2/gpss/upload/bundle", true, &writeData, headers, ""))
    {
        auto mimeThing = fetch->mimeInit();
        for (size_t i = 0; i < sendMe.size(); i++)
        {
            curl_mimepart* field  = curl_mime_addpart(mimeThing.get());
            std::string fieldName = std::format("pkmn{:d}", i + 1);
            curl_mime_name(field, fieldName.c_str());
            curl_mime_data(field, (char*)sendMe[i]->rawData().data(), sendMe[i]->getLength());
            curl_mime_filename(field, fieldName.c_str());
        }
        fetch->setopt(CURLOPT_MIMEPOST, mimeThing.get());

        auto res = Fetch::perform(fetch);
        if (res.index() == 1 && std::get<1>(res) == CURLE_OK)
        {
            fetch->getinfo(CURLINFO_RESPONSE_CODE, &ret);
            nlohmann::json retJson = nlohmann::json::parse(writeData, nullptr, false);
            Gui::warn(
                i18n::localize("SHARE_DOWNLOAD_CODE") + '\n' + retJson["code"].get<std::string>());
            refreshPages();
        }
        else
        {
            nlohmann::json retJson = nlohmann::json::parse(writeData, nullptr, false);
            Gui::warn(
                retJson["type"].get<std::string>() + '\n' + retJson["error"].get<std::string>());
        }
    }
    curl_slist_free_all(headers);
    return ret;
}

bool GroupCloudAccess::pageIsGood(const nlohmann::json& page)
{
    // clang-format off
    if (!page.is_object() ||
        !page.contains("total") || !page["total"].is_number_integer() ||
        !page.contains("pages") || !page["pages"].is_number_integer() ||
        !page.contains("bundles") || !page["bundles"].is_array())
    // clang-format on
    {
        return false;
    }
    else
    {
        for (const auto& group : page["bundles"])
        {
            // clang-format off
            if (!group.is_object() ||
                !group.contains("download_code") || !group["download_code"].is_string() ||
                !group.contains("count") || !group["count"].is_number_integer() ||
                !group.contains("download_codes") || !group["download_codes"].is_array() ||
                !group.contains("pokemons") || !group["pokemons"].is_array())
            // clang-format on
            {
                return false;
            }
        }
    }
    return true;
}
