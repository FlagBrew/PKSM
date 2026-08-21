/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2025 Bernardo Giordano, Admiral Fish, piepie62, Allen Lydiard
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
#include <algorithm>
#include <atomic>
#include <curl/curl.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <utility>

extern "C" {
#include <sys/lock.h>
}

// One request, and everything curl allocated for it. The easy handle, the header list and the
// mime tree have to die in that order and never before the transfer ends, which is the whole
// reason callers used to be handed a curl handle: now they are handed this, and never look
// inside it.
class Fetch::Request
{
public:
    Request() = default;

    ~Request()
    {
        // The mime tree points back at the easy handle, so it goes first.
        if (mime)
        {
            curl_mime_free(mime);
        }
        if (headerList)
        {
            curl_slist_free_all(headerList);
        }
        if (curl)
        {
            curl_easy_cleanup(curl);
        }
    }

    Request(const Request&)            = delete;
    Request& operator=(const Request&) = delete;

    CURL* handle() const { return curl; }

    // Turns curl's verdict into the value callers see. Only called once a transfer is over.
    Response finish(CURLcode code)
    {
        if (code != CURLE_OK)
        {
            return Response::failed(Error::Transport, int(code) + 100);
        }
        long status = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
        return {.status = status, .body = std::move(body)};
    }

    CURL* curl             = nullptr;
    curl_slist* headerList = nullptr;
    curl_mime* mime        = nullptr;
    std::string body;
};

namespace
{
    using Fetch::Callback;
    using Fetch::Error;
    using Fetch::Headers;
    using Fetch::Part;
    using Fetch::Response;
    // Not `using Fetch::Handle`: libctru has a Handle of its own at global scope.
    using RequestHandle = Fetch::Handle;

    // The connection is dead rather than slow past this, so end it instead of holding the
    // timeout open. See STALL_GUARD_LIMIT for when it is armed at all.
    constexpr long STALL_FLOOR   = 300;
    constexpr long STALL_SECONDS = 10;

    // The 3DS has no certificate store to check a chain against, so peer verification is off.
    // It is a deliberate, single decision rather than a default: PKSM talks to a user-supplied
    // apiUrl and to GitHub, and turning this on without shipping a CA bundle would fail every
    // HTTPS request rather than secure it. Ship a bundle and CURLOPT_CAINFO first if this ever
    // changes.
    constexpr long VERIFY_PEER = 0;

    // A connection-level failure is worth trying again; a timeout is not, because a second go
    // just spends the timeout twice. Only ever applied to the idempotent verbs.
    constexpr int MAX_ATTEMPTS = 3;

    constexpr timespec ONE_MS      = {0, 1000000};
    constexpr timespec RETRY_PAUSE = {0, 250000000};

    struct Record
    {
        RequestHandle request;
        Callback onDone;
    };

    std::atomic<bool> threadRunning = false;
    std::vector<Record> inFlight;
    _LOCK_T inFlightMutex;
    CURLM* multiHandle = nullptr;
    _LOCK_T multiMutex;
    bool initialized = false;

    size_t writeToString(char* ptr, size_t size, size_t nmemb, void* userdata)
    {
        ((std::string*)userdata)->append(ptr, size * nmemb);
        return size * nmemb;
    }

    int reportProgress(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t, curl_off_t)
    {
        (*(const Fetch::Progress*)clientp)(u64(dlnow), u64(dltotal));
        return 0;
    }

    bool worthRetrying(const Response& response)
    {
        if (response.error != Error::Transport)
        {
            return false;
        }
        switch (CURLcode(response.code - 100))
        {
            case CURLE_COULDNT_CONNECT:
            case CURLE_COULDNT_RESOLVE_HOST:
            case CURLE_COULDNT_RESOLVE_PROXY:
            case CURLE_SEND_ERROR:
            case CURLE_RECV_ERROR:
            case CURLE_PARTIAL_FILE:
            case CURLE_GOT_NOTHING:
                return true;
            default:
                return false;
        }
    }

    // Builds the mime tree onto a request. curl copies every part, so the caller's bytes are
    // free the moment this returns.
    bool attachParts(const RequestHandle& request, std::span<const Part> parts)
    {
        request->mime = curl_mime_init(request->curl);
        if (!request->mime)
        {
            return false;
        }
        for (const auto& part : parts)
        {
            curl_mimepart* field = curl_mime_addpart(request->mime);
            curl_mime_name(field, part.name.c_str());
            curl_mime_data(field, (const char*)part.data.data(), part.data.size());
            curl_mime_filename(field, part.name.c_str());
        }
        curl_easy_setopt(request->curl, CURLOPT_MIMEPOST, request->mime);
        return true;
    }

    // Everything every request gets. Returns nullptr when curl has nothing to give.
    RequestHandle build(const std::string& url, const Headers& headers, long timeout)
    {
        auto request  = std::make_shared<Fetch::Request>();
        request->curl = curl_easy_init();
        if (!request->curl)
        {
            return nullptr;
        }

        for (const auto& header : headers)
        {
            request->headerList = curl_slist_append(request->headerList, header.c_str());
        }

        curl_easy_setopt(request->curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(request->curl, CURLOPT_HTTPHEADER, request->headerList);
        curl_easy_setopt(request->curl, CURLOPT_SSL_VERIFYPEER, VERIFY_PEER);
        curl_easy_setopt(request->curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(request->curl, CURLOPT_USERAGENT, "PKSM-curl/8.4.0");
        curl_easy_setopt(request->curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(request->curl, CURLOPT_TIMEOUT, timeout);
        if (timeout <= Fetch::STALL_GUARD_LIMIT)
        {
            curl_easy_setopt(request->curl, CURLOPT_LOW_SPEED_LIMIT, STALL_FLOOR);
            curl_easy_setopt(request->curl, CURLOPT_LOW_SPEED_TIME, STALL_SECONDS);
        }

        curl_easy_setopt(request->curl, CURLOPT_WRITEDATA, &request->body);
        curl_easy_setopt(request->curl, CURLOPT_WRITEFUNCTION, writeToString);

        return request;
    }

    // Hands the request to the fetch thread. onDone runs there when it comes back - or here and
    // now if it never went out, so that it always runs exactly once.
    RequestHandle submit(RequestHandle request, Callback onDone)
    {
        if (!initialized || !request)
        {
            if (onDone)
            {
                onDone(request ? Response::failed(Error::Submit, int(CURLM_LAST))
                               : Response::failed(Error::Init));
            }
            return nullptr;
        }

        __lock_acquire(multiMutex);
        CURLMcode res = curl_multi_add_handle(multiHandle, request->handle());
        if (res == CURLM_OK)
        {
            __lock_acquire(inFlightMutex);
            inFlight.push_back(Record{request, std::move(onDone)});
            __lock_release(inFlightMutex);
        }
        __lock_release(multiMutex);

        if (res != CURLM_OK)
        {
            if (onDone)
            {
                onDone(Response::failed(Error::Submit, int(res)));
            }
            return nullptr;
        }
        return request;
    }

    // Waits on the fetch thread rather than transferring here: SOC is not thread-safe, so there
    // is only ever the one thread inside curl.
    Response await(RequestHandle request)
    {
        Response out;
        std::atomic<bool> finished{false};

        submit(std::move(request),
            [&out, &finished](Response response)
            {
                out = std::move(response);
                finished.store(true, std::memory_order_release);
            });

        while (!finished.load(std::memory_order_acquire))
        {
            nanosleep(&ONE_MS, nullptr);
        }
        return out;
    }

    void mainThread()
    {
        int trash;
        while (threadRunning)
        {
            std::vector<std::pair<Record, CURLcode>> done;

            __lock_acquire(multiMutex);
            CURLMcode mc = curl_multi_perform(multiHandle, &trash);
            __lock_release(multiMutex);
            if (mc == CURLM_OK)
            {
                int numFDs = 0;
                __lock_acquire(multiMutex);
                curl_multi_wait(multiHandle, nullptr, 0, 1, &numFDs);
                __lock_release(multiMutex);
                if (numFDs == 0)
                {
                    nanosleep(&ONE_MS, nullptr);
                }
            }

            // Drained in full before anything else touches the multi handle: a CURLMsg is only
            // valid until the next curl_multi_* call.
            std::vector<std::pair<CURL*, CURLcode>> finished;
            __lock_acquire(multiMutex);
            while (CURLMsg* msg = curl_multi_info_read(multiHandle, &trash))
            {
                if (msg->msg == CURLMSG_DONE)
                {
                    finished.emplace_back(msg->easy_handle, msg->data.result);
                }
            }

            __lock_acquire(inFlightMutex);
            for (const auto& [easy, code] : finished)
            {
                curl_multi_remove_handle(multiHandle, easy);
                auto it = std::find_if(inFlight.begin(), inFlight.end(),
                    [easy](const Record& record) { return record.request->handle() == easy; });
                if (it != inFlight.end())
                {
                    done.emplace_back(std::move(*it), code);
                    inFlight.erase(it);
                }
            }
            __lock_release(inFlightMutex);
            __lock_release(multiMutex);

            // A completion decodes a page, or starts a fetch of its own. It runs with nothing
            // held, so neither can deadlock against the transfer loop.
            for (auto& [record, code] : done)
            {
                if (record.onDone)
                {
                    record.onDone(record.request->finish(code));
                }
            }
        }
    }
}

Result Fetch::init()
{
    __lock_init(inFlightMutex);
    __lock_init(multiMutex);
    if (curl_global_init(CURL_GLOBAL_NOTHING) != CURLE_OK)
    {
        return -1;
    }
    multiHandle   = curl_multi_init();
    threadRunning = true;
    if (!multiHandle || !Threads::create(8 * 1024, mainThread))
    {
        threadRunning = false;
        curl_global_cleanup();
        return -1;
    }
    initialized = true;
    return 0;
}

void Fetch::exit()
{
    threadRunning = false; // Stop the fetch thread
    if (initialized)
    {
        initialized = false;
        __lock_acquire(multiMutex);
        __lock_acquire(inFlightMutex);
        for (const auto& record : inFlight)
        {
            curl_multi_remove_handle(multiHandle, record.request->handle());
        }
        inFlight.clear();
        __lock_release(inFlightMutex);
        __lock_close(inFlightMutex);
        curl_multi_cleanup(multiHandle);
        multiHandle = nullptr;
        __lock_release(multiMutex);
        __lock_close(multiMutex);
    }
    curl_global_cleanup();
}

Response Fetch::get(const std::string& url, const Headers& headers, long timeout)
{
    Response response = Response::failed(Error::Init);
    for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++)
    {
        if (attempt)
        {
            nanosleep(&RETRY_PAUSE, nullptr);
        }
        auto request = build(url, headers, timeout);
        if (!request)
        {
            return Response::failed(Error::Init);
        }
        response = await(std::move(request));
        if (!worthRetrying(response))
        {
            break;
        }
    }
    return response;
}

Response Fetch::postJson(
    const std::string& url, const std::string& body, const Headers& headers, long timeout)
{
    auto request = build(url, headers, timeout);
    if (!request)
    {
        return Response::failed(Error::Init);
    }
    curl_easy_setopt(request->curl, CURLOPT_POSTFIELDSIZE, body.length());
    curl_easy_setopt(request->curl, CURLOPT_COPYPOSTFIELDS, body.data());
    return await(std::move(request));
}

Response Fetch::postMultipart(
    const std::string& url, std::span<const Part> parts, const Headers& headers, long timeout)
{
    auto request = build(url, headers, timeout);
    if (!request || !attachParts(request, parts))
    {
        return Response::failed(Error::Init);
    }
    return await(std::move(request));
}

Fetch::Handle Fetch::getAsync(
    const std::string& url, Callback onDone, const Headers& headers, long timeout)
{
    return submit(build(url, headers, timeout), std::move(onDone));
}

Fetch::Handle Fetch::postJsonAsync(const std::string& url, const std::string& body, Callback onDone,
    const Headers& headers, long timeout)
{
    auto request = build(url, headers, timeout);
    if (request)
    {
        curl_easy_setopt(request->curl, CURLOPT_POSTFIELDSIZE, body.length());
        curl_easy_setopt(request->curl, CURLOPT_COPYPOSTFIELDS, body.data());
    }
    return submit(std::move(request), std::move(onDone));
}

void Fetch::cancel(const Fetch::Handle& request)
{
    if (!initialized || !request)
    {
        return;
    }

    Callback onDone;
    bool found = false;

    __lock_acquire(multiMutex);
    __lock_acquire(inFlightMutex);
    auto it = std::find_if(inFlight.begin(), inFlight.end(),
        [&request](const Record& record) { return record.request == request; });
    if (it != inFlight.end())
    {
        curl_multi_remove_handle(multiHandle, request->handle());
        onDone = std::move(it->onDone);
        inFlight.erase(it);
        found = true;
    }
    __lock_release(inFlightMutex);
    __lock_release(multiMutex);

    if (found && onDone)
    {
        onDone(Response::failed(Error::Cancelled));
    }
}

Result Fetch::download(
    const std::string& url, const std::string& path, const Progress& onProgress, long timeout)
{
    // Big enough that a 3DSX lands in a handful of writes rather than thousands.
    constexpr int FILE_BUFFER_SIZE = 0x10000;

    Response response = Response::failed(Error::Init);
    for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++)
    {
        if (attempt)
        {
            nanosleep(&RETRY_PAUSE, nullptr);
        }

        auto request = build(url, {}, timeout);
        if (!request)
        {
            return -1;
        }

        // Truncates whatever a failed attempt left behind.
        FILE* file = fopen(path.c_str(), "wb");
        if (!file)
        {
            return -errno;
        }
        setvbuf(file, nullptr, _IOFBF, FILE_BUFFER_SIZE);
        curl_easy_setopt(request->curl, CURLOPT_WRITEFUNCTION, fwrite);
        curl_easy_setopt(request->curl, CURLOPT_WRITEDATA, file);
        if (onProgress)
        {
            curl_easy_setopt(request->curl, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(request->curl, CURLOPT_XFERINFOFUNCTION, reportProgress);
            curl_easy_setopt(request->curl, CURLOPT_XFERINFODATA, &onProgress);
        }

        // Closed here rather than by the request, which the fetch thread may still be holding:
        // the bytes have to be on the SD card before the code below decides to remove them.
        response = await(std::move(request));
        fclose(file);

        if (!worthRetrying(response))
        {
            break;
        }
    }

    if (!response.ok())
    {
        remove(path.c_str());
        return response.code ? -response.code : -1;
    }
    // The transfer worked, but what arrived was the server's error page, not the file.
    if (response.status < 200 || response.status > 299)
    {
        remove(path.c_str());
        return -Result(response.status);
    }
    return 0;
}
