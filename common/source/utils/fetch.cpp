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
#include <time.h>

#define SPEED_TOO_SLOW 300
#define CALLS_TOO_SLOW 100

static size_t string_write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    std::string* str = (std::string*)userdata;
    str->append(ptr, size * nmemb);
    return size * nmemb;
}

std::unique_ptr<Fetch> Fetch::init(
    const std::string& url, bool post, bool ssl, std::string* writeData, struct curl_slist* headers, const std::string& postdata)
{
    auto fetch  = std::unique_ptr<Fetch>(new Fetch);
    fetch->curl = std::unique_ptr<CURL, decltype(curl_easy_cleanup)*>(curl_easy_init(), &curl_easy_cleanup);
    if (fetch->curl)
    {
        fetch->setopt(CURLOPT_URL, url.c_str());
        fetch->setopt(CURLOPT_HTTPHEADER, headers);
        if (ssl)
        {
            fetch->setopt(CURLOPT_SSL_VERIFYPEER, 0L);
        }
        if (writeData)
        {
            fetch->setopt(CURLOPT_WRITEDATA, writeData);
            fetch->setopt(CURLOPT_WRITEFUNCTION, string_write_callback);
        }
        if (post)
        {
            fetch->setopt(CURLOPT_POSTFIELDSIZE, postdata.length());
            fetch->setopt(CURLOPT_COPYPOSTFIELDS, postdata.data());
        }
        fetch->setopt(CURLOPT_NOPROGRESS, 1L);
        fetch->setopt(CURLOPT_USERAGENT, "PKSM-curl/7.59.0");
        fetch->setopt(CURLOPT_FOLLOWLOCATION, 1L);
        fetch->setopt(CURLOPT_LOW_SPEED_LIMIT, 300L);
        fetch->setopt(CURLOPT_LOW_SPEED_TIME, 30);
    }
    else
    {
        fetch = nullptr;
    }
    return fetch;
}

CURLcode Fetch::perform()
{
    return curl_easy_perform(curl.get());
}

struct callbackWrapper
{
    curl_xferinfo_callback progress;
    void* progressInfo;
};

static int down_callback_wrap(void* wrapper, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    callbackWrapper* data            = (callbackWrapper*)wrapper;
    thread_local curl_off_t oldDlNow = dlnow;
    thread_local int timesTooSlow    = 0;
    if (oldDlNow + SPEED_TOO_SLOW >= dlnow)
    {
        timesTooSlow++;
        oldDlNow = dlnow;
    }
    if (timesTooSlow >= CALLS_TOO_SLOW)
    {
        return 1;
    }
    else
    {
        if (timesTooSlow > 0)
        {
            timesTooSlow--;
        }
        oldDlNow = dlnow;
        return data->progress(data->progressInfo, dltotal, dlnow, ultotal, ulnow);
    }
}

Result Fetch::download(
    const std::string& url, const std::string& path, const std::string& postData, curl_xferinfo_callback progress, void* progressInfo)
{
    FILE* file = fopen(path.c_str(), "wb");
    if (!file)
    {
        return -errno;
    }

    bool doPost             = !postData.empty();
    callbackWrapper wrapper = {progress, progressInfo};
    if (auto fetch = Fetch::init(url, doPost, true, nullptr, nullptr, postData))
    {
        fetch->setopt(CURLOPT_WRITEFUNCTION, fwrite);
        fetch->setopt(CURLOPT_WRITEDATA, file);
        if (progress)
        {
            fetch->setopt(CURLOPT_NOPROGRESS, 0L);
            fetch->setopt(CURLOPT_XFERINFOFUNCTION, down_callback_wrap);
            fetch->setopt(CURLOPT_XFERINFODATA, &wrapper);
            fetch->setopt(CURLOPT_LOW_SPEED_LIMIT, 0L);
            fetch->setopt(CURLOPT_LOW_SPEED_TIME, 0L);
        }
        CURLcode cres = fetch->perform();

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

std::unique_ptr<curl_mime, decltype(curl_mime_free)*> Fetch::mimeInit()
{
    if (curl)
    {
        return std::unique_ptr<curl_mime, decltype(curl_mime_free)*>(curl_mime_init(curl.get()), &curl_mime_free);
    }
    return std::unique_ptr<curl_mime, decltype(curl_mime_free)*>(nullptr, &curl_mime_free);
}
