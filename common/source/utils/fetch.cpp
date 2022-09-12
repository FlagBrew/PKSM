/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2021 Bernardo Giordano, Admiral Fish, piepie62, Allen Lydiard
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
#include "thread.hpp"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

namespace
{
    struct MultiFetchRecord
    {
        MultiFetchRecord(std::shared_ptr<Fetch> fetch                      = nullptr,
            std::function<void(CURLcode, std::shared_ptr<Fetch>)> onFinish = nullptr,
            std::function<void(std::shared_ptr<Fetch>)> onCancel           = nullptr)
            : fetch(fetch), onFinish(onFinish), onCancel(onCancel)
        {
        }
        std::shared_ptr<Fetch> fetch;
        std::function<void(CURLcode, std::shared_ptr<Fetch>)> onFinish;
        std::function<void(std::shared_ptr<Fetch>)> onCancel;
    };

    constexpr int MAX_FILE_BUFFER_SIZE = 0x10000;

    std::atomic<bool> multiThreadInfo = false;
    std::vector<MultiFetchRecord> fetches;
    _LOCK_T fetchesMutex;
    CURLM* multiHandle = nullptr;
    _LOCK_T multiHandleMutex;
    bool multiInitialized = false;

    size_t string_write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
    {
        std::string* str = (std::string*)userdata;
        str->append(ptr, size * nmemb);
        return size * nmemb;
    }
}

std::shared_ptr<Fetch> Fetch::init(const std::string& url, bool ssl, std::string* writeData,
    struct curl_slist* headers, const std::string& postdata)
{
    auto fetch = std::shared_ptr<Fetch>(new Fetch);
    fetch->curl =
        std::unique_ptr<CURL, decltype(curl_easy_cleanup)*>(curl_easy_init(), &curl_easy_cleanup);
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
        fetch->setopt(CURLOPT_USERAGENT, "PKSM-curl/7.69.1");
        fetch->setopt(CURLOPT_FOLLOWLOCATION, 1L);
        fetch->setopt(CURLOPT_LOW_SPEED_LIMIT, 300L);
        fetch->setopt(CURLOPT_LOW_SPEED_TIME, 10L);
    }
    else
    {
        fetch = nullptr;
    }
    return fetch;
}

Result Fetch::download(const std::string& url, const std::string& path, const std::string& postData,
    curl_xferinfo_callback progress, void* progressInfo)
{
    FILE* file = fopen(path.c_str(), "wb");
    if (!file)
    {
        return -errno;
    }

    if (auto fetch = Fetch::init(url, url.substr(0, 5) == "https", nullptr, nullptr, postData))
    {
        setvbuf(file, nullptr, _IOFBF, MAX_FILE_BUFFER_SIZE);
        fetch->setopt(CURLOPT_WRITEFUNCTION, fwrite);
        fetch->setopt(CURLOPT_WRITEDATA, file);
        if (progress)
        {
            fetch->setopt(CURLOPT_NOPROGRESS, 0L);
            fetch->setopt(CURLOPT_XFERINFOFUNCTION, progress);
            fetch->setopt(CURLOPT_XFERINFODATA, progressInfo);
        }

        auto res = Fetch::perform(fetch);

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
        fclose(file);
        return -1;
    }

    return 0;
}

std::unique_ptr<curl_mime, decltype(curl_mime_free)*> Fetch::mimeInit()
{
    if (curl)
    {
        return std::unique_ptr<curl_mime, decltype(curl_mime_free)*>(
            curl_mime_init(curl.get()), &curl_mime_free);
    }
    return std::unique_ptr<curl_mime, decltype(curl_mime_free)*>(nullptr, &curl_mime_free);
}

void Fetch::multiMainThread(void*)
{
    int trash;
    while (multiThreadInfo)
    {
        CURLMcode mc;

        __lock_acquire(multiHandleMutex);
        mc = curl_multi_perform(multiHandle, &trash);
        __lock_release(multiHandleMutex);
        if (mc == CURLM_OK)
        {
            int numFDs;
            __lock_acquire(multiHandleMutex);
            mc = curl_multi_wait(multiHandle, nullptr, 0, 1, &numFDs);
            __lock_release(multiHandleMutex);
            if (numFDs == 0)
            {
                constexpr timespec sleepTime = {0, 1000000};
                nanosleep(&sleepTime, nullptr);
            }
        }

        __lock_acquire(multiHandleMutex);
        auto msg = curl_multi_info_read(multiHandle, &trash);
        while (msg != nullptr)
        {
            // Find the done handle
            __lock_acquire(fetchesMutex);
            auto it = std::find_if(fetches.begin(), fetches.end(),
                [&msg](const MultiFetchRecord& record)
                { return record.fetch->curl.get() == msg->easy_handle; });
            // And delete it
            if (it != fetches.end())
            {
                if (it->onFinish)
                {
                    it->onFinish(msg->data.result, it->fetch);
                }
                curl_multi_remove_handle(multiHandle, it->fetch->curl.get());
                fetches.erase(it);
            }
            __lock_release(fetchesMutex);

            msg = curl_multi_info_read(multiHandle, &trash);
        }
        __lock_release(multiHandleMutex);

        // Terrible things have happened, but I don't know what to do
    }

    multiThreadInfo = true;
}

Result Fetch::initMulti()
{
    __lock_init(fetchesMutex);
    __lock_init(multiHandleMutex);
    multiHandle     = curl_multi_init();
    multiThreadInfo = true;
    if (!Threads::create(Fetch::multiMainThread, nullptr, 8 * 1024))
    {
        multiInitialized = false;
        return -1;
    }
    multiInitialized = true;
    return 0;
}

void Fetch::exitMulti()
{
    multiThreadInfo = false; // Stop multi thread
    if (multiInitialized)
    {
        while (!multiThreadInfo) // Wait for it to be done
        {
            constexpr timespec sleepTime = {0, 100000};
            nanosleep(&sleepTime, nullptr);
        }
        // And finally clean up
        __lock_acquire(fetchesMutex);
        __lock_acquire(multiHandleMutex);
        for (const auto& i : fetches)
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
}

CURLMcode Fetch::performAsync(std::shared_ptr<Fetch> fetch,
    std::function<void(CURLcode, std::shared_ptr<Fetch>)> onComplete,
    std::function<void(std::shared_ptr<Fetch>)> onCancel)
{
    if (multiInitialized)
    {
        __lock_acquire(multiHandleMutex);
        CURLMcode res = curl_multi_add_handle(multiHandle, fetch->curl.get());
        __lock_release(multiHandleMutex);
        if (res == CURLM_OK)
        {
            __lock_acquire(fetchesMutex);
            fetches.emplace_back(fetch, std::move(onComplete), std::move(onCancel));
            __lock_release(fetchesMutex);
        }
        return res;
    }
    else
    {
        return CURLM_LAST;
    }
}

void Fetch::cancelAsync(std::shared_ptr<Fetch> fetch)
{
    if (multiInitialized)
    {
        __lock_acquire(multiHandleMutex);
        CURLMcode res = curl_multi_remove_handle(multiHandle, fetch->curl.get());
        __lock_release(multiHandleMutex);
        if (res == CURLM_OK)
        {
            __lock_acquire(fetchesMutex);
            for (auto i = fetches.begin(); i != fetches.end(); ++i)
            {
                if (i->fetch == fetch)
                {
                    if (i->onCancel)
                    {
                        i->onCancel(i->fetch);
                    }
                    fetches.erase(i);
                    break;
                }
            }
            __lock_release(fetchesMutex);
        }
        // better not fail
    }
}

std::variant<CURLMcode, CURLcode> Fetch::perform(std::shared_ptr<Fetch> fetch)
{
    if (multiInitialized)
    {
        CURLcode cres;
        std::atomic_flag wait;
        wait.test_and_set();
        CURLMcode mRes = performAsync(fetch,
            [&cres, &wait](CURLcode code, std::shared_ptr<Fetch>)
            {
                cres = code;
                wait.clear();
            });
        if (mRes != CURLM_OK)
        {
            return mRes;
        }

        while (wait.test_and_set())
        {
            constexpr timespec sleepTime = {0, 100000};
            nanosleep(&sleepTime, nullptr);
        }

        return cres;
    }
    else
    {
        return CURLM_LAST;
    }
}
