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
#include "random.hpp"
#include "thread.hpp"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

static size_t string_write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    std::string* str = (std::string*)userdata;
    str->append(ptr, size * nmemb);
    return size * nmemb;
}

std::shared_ptr<Fetch> Fetch::init(const std::string& url, bool ssl, std::string* writeData, struct curl_slist* headers, const std::string& postdata)
{
    auto fetch  = std::shared_ptr<Fetch>(new Fetch);
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
        if (!postdata.empty())
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

Result Fetch::download(
    const std::string& url, const std::string& path, const std::string& postData, curl_xferinfo_callback progress, void* progressInfo)
{
    FILE* file = fopen(path.c_str(), "wb");
    if (!file)
    {
        return -errno;
    }

    bool doPost = !postData.empty();
    if (auto fetch = Fetch::init(url, doPost, nullptr, nullptr, postData))
    {
        fetch->setopt(CURLOPT_WRITEFUNCTION, fwrite);
        fetch->setopt(CURLOPT_WRITEDATA, file);
        if (progress)
        {
            fetch->setopt(CURLOPT_NOPROGRESS, 0L);
            fetch->setopt(CURLOPT_XFERINFOFUNCTION, progress);
            fetch->setopt(CURLOPT_XFERINFODATA, progressInfo);
        }
        fetch->setopt(CURLOPT_LOW_SPEED_LIMIT, 300L);
        fetch->setopt(CURLOPT_LOW_SPEED_TIME, 30);

        auto res = MultiFetch::getInstance().execute(fetch);

        fclose(file);

        if (res.index() == 0)
        {
            remove(path.c_str());
            return -std::get<0>(res);
        }
        else if (std::get<1>(res) != CURLE_OK)
        {
            remove(path.c_str());
            return -(std::get<1>(res) + 100);
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

void MultiFetch::multiMainThread()
{
    int lastActive = INT_MIN;
    while (multiThreadInfo)
    {
        CURLMcode mc;
        int active;

        __lock_acquire(multiHandleMutex);
        mc = curl_multi_perform(multiHandle, &active);
        __lock_release(multiHandleMutex);
        if (mc == CURLM_OK)
        {
            int numFDs;
            mc = curl_multi_wait(multiHandle, nullptr, 0, 1, &numFDs);
            if (numFDs == 0)
            {
                usleep(1000);
            }
        }

        if (active < lastActive)
        {
            int msgs;
            __lock_acquire(multiHandleMutex);
            auto msg = curl_multi_info_read(multiHandle, &msgs);
            __lock_release(multiHandleMutex);
            while (msg != nullptr)
            {
                // Find the done handle
                __lock_acquire(fetchesMutex);
                auto it = std::find_if(
                    fetches.begin(), fetches.end(), [&msg](const MultiFetchRecord& record) { return record.fetch->curl.get() == msg->easy_handle; });
                // And delete it
                if (it != fetches.end())
                {
                    if (it->function)
                    {
                        it->function(msg->data.result, it->fetch);
                    }
                    __lock_acquire(multiHandleMutex);
                    curl_multi_remove_handle(multiHandle, it->fetch->curl.get());
                    __lock_release(multiHandleMutex);
                    fetches.erase(it);
                }
                __lock_release(fetchesMutex);

                __lock_acquire(multiHandleMutex);
                msg = curl_multi_info_read(multiHandle, &msgs);
                __lock_release(multiHandleMutex);
            }
        }

        lastActive = active;

        // Terrible things have happened, but I don't know what to do
    }

    multiThreadInfo = true;
}

MultiFetch::MultiFetch()
{
    __lock_init(fetchesMutex);
    __lock_init(multiHandleMutex);
    multiHandle     = curl_multi_init();
    multiThreadInfo = true;
    Threads::create([](void* arg) { ((MultiFetch*)arg)->multiMainThread(); }, this);
}

void MultiFetch::exit()
{
    multiThreadInfo = false; // Stop multi thread
    while (!multiThreadInfo) // Wait for it to be done
    {
        usleep(100);
    }
    // And finally clean up
    __lock_acquire(fetchesMutex);
    __lock_acquire(multiHandleMutex);
    for (auto& i : fetches)
    {
        curl_multi_remove_handle(multiHandle, i.fetch->curl.get());
    }
    fetches.clear();
    __lock_release(fetchesMutex);
    __lock_close(fetchesMutex);
    curl_multi_cleanup(multiHandle);
    __lock_release(multiHandleMutex);
    __lock_close(multiHandleMutex);
}

CURLMcode MultiFetch::add(std::shared_ptr<Fetch> fetch, std::function<void(CURLcode, std::shared_ptr<Fetch>)> onComplete)
{
    __lock_acquire(multiHandleMutex);
    CURLMcode res = curl_multi_add_handle(multiHandle, fetch->curl.get());
    __lock_release(multiHandleMutex);
    if (res == CURLM_OK)
    {
        __lock_acquire(fetchesMutex);
        fetches.emplace_back(fetch, onComplete);
        __lock_release(fetchesMutex);
    }
    return res;
}

std::variant<CURLMcode, CURLcode> MultiFetch::execute(std::shared_ptr<Fetch> fetch)
{
    CURLcode cres;
    std::atomic_flag wait;
    wait.test_and_set();
    CURLMcode mRes = MultiFetch::getInstance().add(fetch, [&cres, &wait](CURLcode code, std::shared_ptr<Fetch>) {
        cres = code;
        wait.clear();
    });
    if (mRes != CURLM_OK)
    {
        return mRes;
    }

    while (wait.test_and_set())
    {
        usleep(100);
    }

    return cres;
}
