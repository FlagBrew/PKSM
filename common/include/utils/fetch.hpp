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

#include "types.h"
#include <curl/curl.h>
#include <functional>
#include <memory>
#include <string>

namespace Fetch
{
    extern std::unique_ptr<CURL, decltype(curl_easy_cleanup)*> curl;
    bool init(const std::string& url, bool post, bool ssl, std::string* writeData, struct curl_slist* headers, const std::string& postdata);
    CURLcode perform();
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
    Result download(const std::string& url, const std::string& path);
    void exit();
} // namespace Fetch
