/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2019 Bernardo Giordano, Admiral Fish, piepie62, Allen Lydiard
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

#include "fetch.hpp"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>

std::unique_ptr<CURL, decltype(curl_easy_cleanup)*> Fetch::curl(nullptr, &curl_easy_cleanup);

static size_t string_write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    std::string* str = (std::string*)userdata;
    str->append(ptr, size * nmemb);
    return size * nmemb;
}

bool Fetch::init(const std::string& url, bool post, bool ssl, std::string* writeData, struct curl_slist* headers, const std::string& postdata)
{
    curl = std::unique_ptr<CURL, decltype(curl_easy_cleanup)*>(curl_easy_init(), &curl_easy_cleanup);
    if (curl)
    {
        Fetch::setopt(CURLOPT_URL, url.c_str());
        Fetch::setopt(CURLOPT_HTTPHEADER, headers);
        if (ssl)
        {
            Fetch::setopt(CURLOPT_SSL_VERIFYPEER, 0L);
        }
        if (writeData)
        {
            Fetch::setopt(CURLOPT_WRITEDATA, writeData);
            Fetch::setopt(CURLOPT_WRITEFUNCTION, string_write_callback);
        }
        if (post)
        {
            Fetch::setopt(CURLOPT_POSTFIELDS, postdata.data());
            Fetch::setopt(CURLOPT_POSTFIELDSIZE, postdata.length());
        }
        Fetch::setopt(CURLOPT_NOPROGRESS, 1L);
        Fetch::setopt(CURLOPT_USERAGENT, "PKSM-curl/7.59.0");
        Fetch::setopt(CURLOPT_FOLLOWLOCATION, 1L);
        Fetch::setopt(CURLOPT_LOW_SPEED_LIMIT, 300L);
        Fetch::setopt(CURLOPT_LOW_SPEED_TIME, 60);
    }
    return (bool)curl;
}

CURLcode Fetch::perform()
{
    return curl_easy_perform(curl.get());
}

void Fetch::exit()
{
    curl = nullptr;
}

Result Fetch::download(const std::string& url, const std::string& path)
{
    FILE* file = fopen(path.c_str(), "wb");
    if (!file)
    {
        return -errno;
    }

    if (Fetch::init(url, false, true, nullptr, nullptr, ""))
    {
        Fetch::setopt(CURLOPT_WRITEFUNCTION, fwrite);
        Fetch::setopt(CURLOPT_WRITEDATA, file);
        CURLcode cres = Fetch::perform();

        // cleanup
        Fetch::exit();

        fclose(file);

        if (cres != CURLE_OK)
        {
            remove(path.c_str());
            return -cres;
        }
    }
    else
    {
        return -1;
    }

    return 0;
}
