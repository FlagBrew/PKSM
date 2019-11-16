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

#include "CloudAccess.hpp"
#include "Configuration.hpp"
#include "PK7.hpp"
#include "PKX.hpp"
#include "app.hpp"
#include "base64.hpp"
#include "fetch.hpp"
#include "thread.hpp"
#include <unistd.h>

static Generation numToGen(int num)
{
    switch (num)
    {
        case 4:
            return Generation::FOUR;
        case 5:
            return Generation::FIVE;
        case 6:
            return Generation::SIX;
        case 7:
            return Generation::SEVEN;
        case 254:
            return Generation::LGPE;
    }
    return Generation::UNUSED;
}

void CloudAccess::downloadCloudPage(std::shared_ptr<Page> page, int number, SortType type, bool ascend, bool legal)
{
    std::string* retData = new std::string;
    auto fetch           = Fetch::init(CloudAccess::makeURL(number, type, ascend, legal), true, retData, nullptr, "");
    MultiFetch::getInstance().add(fetch, [page, retData](CURLcode code, std::shared_ptr<Fetch> fetch) {
        if (code == CURLE_OK)
        {
            long status_code;
            fetch->getinfo(CURLINFO_RESPONSE_CODE, &status_code);
            switch (status_code)
            {
                case 200:
                    page->data = nlohmann::json::parse(*retData, nullptr, false);
                    break;
                default:
                    page->data = {};
                    break;
            }
        }
        else
        {
            page->data = {};
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
    current->data      = grabPage(pageNumber);
    current->available = true;
    isGood             = !current->data.is_discarded() && current->data.size() > 0;
    if (pageNumber >= pages())
    {
        current->data      = grabPage(pages());
        pageNumber         = pages();
        current->available = true;
        isGood             = !current->data.is_discarded() && current->data.size() > 0;
    }
    if (isGood)
    {
        if (pages() > 1)
        {
            int page = (pageNumber % pages()) + 1;
            next     = std::make_shared<Page>();
            downloadCloudPage(next, page, sort, ascend, legal);
            if (pages() > 2)
            {
                page = pageNumber - 1 == 0 ? pages() : pageNumber - 1;
                prev = std::make_shared<Page>();
                downloadCloudPage(prev, page, sort, ascend, legal);
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
    auto fetch = Fetch::init(makeURL(num, sort, ascend, legal), true, &retData, nullptr, "");
    auto res   = MultiFetch::getInstance().execute(fetch);
    if (res.index() == 0)
    {
        return {};
    }
    else if (std::get<1>(res) == CURLE_OK)
    {
        long status_code;
        fetch->getinfo(CURLINFO_RESPONSE_CODE, &status_code);
        switch (status_code)
        {
            case 200:
                break;
            default:
                return {};
        }
        return nlohmann::json::parse(retData, nullptr, false);
    }
    else
    {
        return {};
    }
}

static std::string sortTypeToString(CloudAccess::SortType type)
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

std::string CloudAccess::makeURL(int num, SortType type, bool ascend, bool legal)
{
    return "https://flagbrew.org/api/v1/gpss/all?pksm=yes&count=30&sort=" + sortTypeToString(type) +
           "&dir=" + (ascend ? std::string("ascend") : std::string("descend")) + "&legal_only=" + (legal ? std::string("yes") : std::string("no")) +
           "&page=" + std::to_string(num);
}

std::shared_ptr<PKX> CloudAccess::pkm(size_t slot) const
{
    if (slot < current->data["results"].size())
    {
        std::string b64Data = current->data["results"][slot]["base_64"].get<std::string>();
        Generation gen      = numToGen(current->data["results"][slot]["generation"].get<int>());
        // Legal info: needs thought
        auto retData = base64_decode(b64Data.data(), b64Data.size());

        size_t targetLength = 0;
        switch (gen)
        {
            case Generation::FOUR:
            case Generation::FIVE:
                targetLength = 138;
                break;
            case Generation::SIX:
            case Generation::SEVEN:
                targetLength = 234;
                break;
            case Generation::LGPE:
                targetLength = 261;
                break;
            case Generation::UNUSED:
                break;
        }
        if (targetLength != retData.size())
        {
            return std::make_shared<PK7>();
        }

        return PKX::getPKM(gen, retData.data());
    }
    return std::make_shared<PK7>();
}

bool CloudAccess::isLegal(size_t slot) const
{
    if (slot < current->data["results"].size())
    {
        return current->data["results"][slot]["pokemon"]["legal"].get<bool>();
    }
    return false;
}

std::shared_ptr<PKX> CloudAccess::fetchPkm(size_t slot) const
{
    if (slot < current->data["results"].size())
    {
        std::string b64Data = current->data["results"][slot]["base_64"].get<std::string>();
        Generation gen      = numToGen(current->data["results"][slot]["generation"].get<int>());
        // Legal info: needs thought
        auto retData = base64_decode(b64Data.data(), b64Data.size());

        size_t targetLength = 0;
        switch (gen)
        {
            case Generation::FOUR:
            case Generation::FIVE:
                targetLength = 138;
                break;
            case Generation::SIX:
            case Generation::SEVEN:
                targetLength = 234;
                break;
            case Generation::LGPE:
                targetLength = 261;
                break;
            case Generation::UNUSED:
                break;
        }
        if (targetLength != retData.size())
        {
            return std::make_shared<PK7>();
        }

        if (auto fetch = Fetch::init(
                "https://flagbrew.org/gpss/download/" + current->data["results"][slot]["code"].get<std::string>(), true, nullptr, nullptr, ""))
        {
            MultiFetch::getInstance().add(fetch);
        }

        return PKX::getPKM(gen, retData.data());
    }
    return std::make_shared<PK7>();
}

bool CloudAccess::nextPage()
{
    if (pages() > 2)
    {
        while (!next->available)
        {
            usleep(100);
        }
        if (next->data.empty() || next->data.is_discarded())
        {
            return isGood = false;
        }
        // Update data
        pageNumber = (pageNumber % pages()) + 1;
        prev       = current;
        current    = next;
        next       = std::make_shared<Page>();

        // Download next page in the background
        int nextPage = (pageNumber % pages()) + 1;
        downloadCloudPage(next, nextPage, sort, ascend, legal);

        // If there's a mon number desync, also download the previous page again
        if (current->data["total_pkm"] != prev->data["total_pkm"])
        {
            int prevPage = pageNumber - 1 == 0 ? pages() : pageNumber - 1;
            downloadCloudPage(prev, prevPage, sort, ascend, legal);
        }
    }
    else if (pages() == 2)
    {
        while (!next->available)
        {
            usleep(100);
        }
        if (next->data.empty() || next->data.is_discarded())
        {
            return isGood = false;
        }

        prev    = current;
        current = next;
        next    = prev;
    }
    // Otherwise there's only one page, so no necessary action
    return isGood;
}

bool CloudAccess::prevPage()
{
    if (pages() > 2)
    {
        while (!prev->available)
        {
            usleep(100);
        }
        if (prev->data.empty() || prev->data.is_discarded())
        {
            return isGood = false;
        }
        // Update data
        pageNumber = pageNumber - 1 == 0 ? pages() : pageNumber - 1;
        next       = current;
        current    = prev;
        prev       = std::make_shared<Page>();

        // Download the next page in the background
        int prevPage = pageNumber - 1 == 0 ? pages() : pageNumber - 1;
        downloadCloudPage(prev, prevPage, sort, ascend, legal);

        // If there's a mon number desync, also download the next page again
        if (current->data["total_pkm"] != next->data["total_pkm"])
        {
            int nextPage = (pageNumber % pages()) + 1;
            downloadCloudPage(next, nextPage, sort, ascend, legal);
        }
    }
    else if (pages() == 2)
    {
        while (!prev->available)
        {
            usleep(100);
        }
        if (prev->data.empty() || prev->data.is_discarded())
        {
            return isGood = false;
        }

        // Swap pages around
        next    = current;
        current = prev;
        prev    = next;
    }
    // Otherwise, there's only one page, so no action is necessary
    return isGood;
}

long CloudAccess::pkm(std::shared_ptr<PKX> mon)
{
    long ret            = 0;
    std::string version = "Generation: " + genToString(mon->generation());
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
    if (auto fetch = Fetch::init("https://flagbrew.org/gpss/share", true, &writeData, headers, ""))
    {
        auto mimeThing       = fetch->mimeInit();
        curl_mimepart* field = curl_mime_addpart(mimeThing.get());
        curl_mime_name(field, "pkmn");
        curl_mime_data(field, (char*)mon->rawData(), mon->getLength());
        curl_mime_filename(field, "pkmn");
        fetch->setopt(CURLOPT_MIMEPOST, mimeThing.get());

        auto res = MultiFetch::getInstance().execute(fetch);
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
