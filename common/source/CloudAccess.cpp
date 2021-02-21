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

#include "CloudAccess.hpp"
#include "Configuration.hpp"
#include "app.hpp"
#include "base64.hpp"
#include "fetch.hpp"
#include "nlohmann/json.hpp"
#include "pkx/PK7.hpp"
#include "pkx/PKX.hpp"
#include "thread.hpp"
#include "website.h"
#include <unistd.h>

namespace
{
    std::string sortTypeToString(CloudAccess::SortType type)
    {
        switch (type)
        {
            case CloudAccess::SortType::LATEST:
                return "latest";
            case CloudAccess::SortType::POPULAR:
                return "popular";
        }
        return "";
    }
}

CloudAccess::Page::~Page() {}

void CloudAccess::downloadCloudPage(std::shared_ptr<Page> page, int number, SortType type,
    bool ascend, bool legal, pksm::Generation low, pksm::Generation high, bool LGPE)
{
    std::string* retData = new std::string;
    auto fetch = Fetch::init(CloudAccess::makeURL(number, type, ascend, legal, low, high, LGPE),
        true, retData, nullptr, "");
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
                    if (!page->data || !pageIsGood(*page->data))
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

CloudAccess::CloudAccess() : pageNumber(1)
{
    refreshPages();
}

void CloudAccess::refreshPages()
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
            downloadCloudPage(next, page, sort, ascend, legal, lowGen, highGen, showLGPE);
            if (pages() > 2)
            {
                page = pageNumber - 1 == 0 ? pages() : pageNumber - 1;
                prev = std::make_shared<Page>();
                downloadCloudPage(prev, page, sort, ascend, legal, lowGen, highGen, showLGPE);
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

nlohmann::json CloudAccess::grabPage(int num)
{
    std::string retData;
    auto fetch = Fetch::init(
        makeURL(num, sort, ascend, legal, lowGen, highGen, showLGPE), true, &retData, nullptr, "");
    auto res = Fetch::perform(fetch);
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

std::string CloudAccess::makeURL(int num, SortType type, bool ascend, bool legal,
    pksm::Generation low, pksm::Generation high, bool LGPE)
{
    return WEBSITE_URL "api/v1/gpss/all?pksm=yes&count=30&sort=" + sortTypeToString(type) +
           "&dir=" + (ascend ? std::string("ascend") : std::string("descend")) +
           "&legal_only=" + (legal ? std::string("True") : std::string("False")) +
           "&page=" + std::to_string(num) + "&min_gen=" + (std::string)low +
           "&max_gen=" + (std::string)high +
           "&lgpe=" + (LGPE ? std::string("yes") : std::string("no"));
}

std::unique_ptr<pksm::PKX> CloudAccess::pkm(size_t slot) const
{
    if (slot < (*current->data)["results"].size())
    {
        std::string b64Data  = (*current->data)["results"][slot]["base_64"].get<std::string>();
        pksm::Generation gen = pksm::Generation::fromString(
            (*current->data)["results"][slot]["generation"].get<std::string>());
        // Legal info: needs thought
        auto retData = base64_decode(b64Data.data(), b64Data.size());

        auto ret = pksm::PKX::getPKM(gen, retData.data(), retData.size());
        if (ret)
        {
            return ret;
        }
    }
    return pksm::PKX::getPKM<pksm::Generation::SEVEN>(nullptr);
}

bool CloudAccess::isLegal(size_t slot) const
{
    if (slot < (*current->data)["results"].size())
    {
        return (*current->data)["results"][slot]["legal"].get<bool>();
    }
    return false;
}

std::unique_ptr<pksm::PKX> CloudAccess::fetchPkm(size_t slot) const
{
    if (slot < (*current->data)["results"].size())
    {
        auto ret = pkm(slot);

        if (auto fetch =
                Fetch::init(WEBSITE_URL "gpss/download/" +
                                (*current->data)["results"][slot]["code"].get<std::string>(),
                    true, nullptr, nullptr, ""))
        {
            Fetch::performAsync(fetch);
        }

        return ret;
    }
    return pksm::PKX::getPKM<pksm::Generation::SEVEN>(nullptr);
}

std::optional<int> CloudAccess::nextPage()
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
    downloadCloudPage(next, nextPage, sort, ascend, legal, lowGen, highGen, showLGPE);

    // If there's a mon number desync, also download the previous page again
    if ((*current->data)["total_pkm"] != (*prev->data)["total_pkm"])
    {
        int prevPage = pageNumber - 1 == 0 ? pages() : pageNumber - 1;
        downloadCloudPage(prev, prevPage, sort, ascend, legal, lowGen, highGen, showLGPE);
    }

    return std::nullopt;
}

std::optional<int> CloudAccess::prevPage()
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
    downloadCloudPage(prev, prevPage, sort, ascend, legal, lowGen, highGen, showLGPE);

    // If there's a mon number desync, also download the next page again
    if ((*current->data)["total_pkm"] != (*next->data)["total_pkm"])
    {
        int nextPage = (pageNumber % pages()) + 1;
        downloadCloudPage(next, nextPage, sort, ascend, legal, lowGen, highGen, showLGPE);
    }

    return std::nullopt;
}

long CloudAccess::pkm(std::unique_ptr<pksm::PKX> mon)
{
    long ret            = 0;
    std::string version = "Generation: " + (std::string)mon->generation();
    std::string code    = Configuration::getInstance().patronCode();
    if (!code.empty())
    {
        code = "PC: " + code;
    }
    struct curl_slist* headers = NULL;
    headers                    = curl_slist_append(headers, "Content-Type: multipart/form-data");
    headers                    = curl_slist_append(headers, version.c_str());
    if (!code.empty())
    {
        headers = curl_slist_append(headers, code.c_str());
    }

    std::string writeData = "";
    if (auto fetch = Fetch::init(WEBSITE_URL "gpss/share", true, &writeData, headers, ""))
    {
        auto mimeThing       = fetch->mimeInit();
        curl_mimepart* field = curl_mime_addpart(mimeThing.get());
        curl_mime_name(field, "pkmn");
        curl_mime_data(field, (char*)mon->rawData(), mon->getLength());
        curl_mime_filename(field, "pkmn");
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

int CloudAccess::pages() const
{
    return (*current->data)["pages"].get<int>();
}

void CloudAccess::filterToGen(pksm::Generation g)
{
    if (g != pksm::Generation::LGPE)
    {
        lowGen = highGen = g;
        showLGPE         = false;
    }
    else
    {
        lowGen   = pksm::Generation::EIGHT;
        highGen  = pksm::Generation::FOUR;
        showLGPE = true;
    }
}

void CloudAccess::removeGenFilter()
{
    lowGen   = pksm::Generation::THREE;
    highGen  = pksm::Generation::EIGHT;
    showLGPE = true;
}

bool CloudAccess::pageIsGood(const nlohmann::json& page)
{
    // clang-format off
    if (!page.is_object() ||
        !page.contains("total_pkm") || !page["total_pkm"].is_number_integer() ||
        !page.contains("results") || !page["results"].is_array() ||
        !page.contains("pages") || !page["pages"].is_number_integer())
    // clang-format on
    {
        return false;
    }
    else
    {
        for (auto& json : page["results"])
        {
            // clang-format off
            if (!json.is_object() ||
                !json.contains("base_64") || !json["base_64"].is_string() ||
                !json.contains("generation") || !json["generation"].is_string() ||
                !json.contains("legal") || !json["legal"].is_boolean() ||
                !json.contains("code") || !json["code"].is_string())
            // clang-format on
            {
                return false;
            }
        }
    }
    return true;
}
