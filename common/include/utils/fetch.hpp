/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2022 Bernardo Giordano, Admiral Fish, piepie62, Allen Lydiard
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

#ifndef FETCH_HPP
#define FETCH_HPP

extern "C" {
#include <sys/lock.h>
}
#include "types.h"
#include <atomic>
#include <curl/curl.h>
#include <functional>
#include <memory>
#include <string>
#include <variant>
#include <vector>

// This should theoretically be thread-safe, but on the 3DS it is not, because SOC stuff is not
// thread safe. Whee
class Fetch
{
public:
    [[nodiscard]] static std::shared_ptr<Fetch> init(const std::string& url, bool ssl,
        std::string* writeData, struct curl_slist* headers, const std::string& postdata);
    static Result download(const std::string& url, const std::string& path,
        const std::string& postData = "", curl_xferinfo_callback progress = nullptr,
        void* progressInfo = nullptr);

    static CURLMcode performAsync(std::shared_ptr<Fetch> fetch,
        std::function<void(CURLcode, std::shared_ptr<Fetch>)> onComplete = nullptr,
        std::function<void(std::shared_ptr<Fetch>)> onCancel             = nullptr);
    static std::variant<CURLMcode, CURLcode> perform(std::shared_ptr<Fetch> fetch);
    static void cancelAsync(std::shared_ptr<Fetch> fetch);

    static Result initMulti();
    static void exitMulti();

    template <typename T>
    CURLcode setopt(CURLoption opt, T data)
    {
        return curl_easy_setopt(curl.get(), opt, data);
    }

    template <typename T>
    CURLcode getinfo(CURLINFO info, T outvar)
    {
        return curl_easy_getinfo(curl.get(), info, outvar);
    }

    std::unique_ptr<curl_mime, decltype(curl_mime_free)*> mimeInit();

private:
    Fetch() : curl(nullptr, &curl_easy_cleanup) {}

    Fetch(const Fetch&)            = delete;
    Fetch(Fetch&&)                 = default;
    Fetch& operator=(const Fetch&) = delete;
    Fetch& operator=(Fetch&&)      = default;
    std::unique_ptr<CURL, decltype(curl_easy_cleanup)*> curl;

    static void multiMainThread();
};

#endif
