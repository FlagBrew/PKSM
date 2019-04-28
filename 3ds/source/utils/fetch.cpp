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
#include <stdarg.h>

CURL* curl = nullptr;

static size_t string_write_callback(char *ptr, size_t size, size_t nmemb, void* userdata)
{
    std::string* str = (std::string*) userdata;
    str->append(ptr, size*nmemb);
    return size * nmemb;
}

CURL* Fetch::init(const std::string& url, bool post, bool ssl, std::string* writeData, struct curl_slist* headers, const std::string& postdata)
{
	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		if (ssl)
		{
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		}
		if (writeData)
		{
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, writeData);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, string_write_callback);
		}
		if (post)
		{
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata.data());
        	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postdata.length());
		}
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "PKSM-curl/7.59.0");
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
	}
	return curl;
}

CURLcode Fetch::perform()
{
	return curl_easy_perform(curl);
}

void Fetch::exit()
{
	curl_easy_cleanup(curl);
}
