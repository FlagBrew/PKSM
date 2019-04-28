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

#include "ViewPokemonOverlay.hpp"
#include "gui.hpp"
#include <curl/curl.h>
#include <loader.hpp>

extern "C" {
#include "base64.h"
}

void ViewPokemonOverlay::draw() const
{
    ViewOverlay::draw();

    C2D_SceneBegin(g_renderTargetBottom);
    dim();
    if (clone.empty())
    {
        Gui::staticText(i18n::localize("PRESS_TO_CLONE"), 160, 110, FONT_SIZE_18, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
        Gui::staticText(i18n::localize("SHARE_PRESS_TO_SHARE"), 160, 130, FONT_SIZE_18, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
    }
}

void ViewPokemonOverlay::update(touchPosition* touch)
{
    u32 kDown = hidKeysDown();
    if (kDown & KEY_B)
    {
        screen.removeOverlay();
    }
    else if (clone.empty() && kDown & KEY_X)
    {
        clone.emplace_back(pkm->clone());
        partyNum.push_back(-1);
        cloneDims = {1,1};
        currentlySelecting = false;
        screen.removeOverlay();
    }
    else if (kDown & KEY_Y)
    {
        share();
        screen.removeOverlay();
    }
}

static size_t write_callback(char *ptr, size_t size, size_t nmemb, void* userdata) {
    std::string* str = (std::string*) userdata;
    str->append(ptr, size*nmemb);
    return size * nmemb;
}
 
void ViewPokemonOverlay::share()
{
    const u8* rawData = pkm->rawData();
    CURLcode res;
    std::string postdata = "";
    size_t outSize;
    long status_code = 0;
    char *b64Data = base64_encode((char *)rawData, pkm->getLength(), &outSize);
    postdata += b64Data;

    std::string version = "Generation: " + genToString(pkm->generation());
    std::string size = "Size: " + std::to_string(pkm->getLength());
    free(b64Data);
    CURL* curl = curl_easy_init();
    if (curl)
    {
        std::string s = "";
        struct curl_slist *h = NULL;
        h = curl_slist_append(h, "Content-Type: application/base64");
        h = curl_slist_append(h, version.c_str());
        h = curl_slist_append(h, size.c_str());
        curl_easy_setopt(curl, CURLOPT_URL, "https://flagbrew.org/pksm/share");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, h);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata.data());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postdata.length());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "PKSM-curl/7.59.0");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            Gui::error(i18n::localize("CURL_ERROR"), abs(res));
        }
        else
        {
            curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &status_code);
            switch (status_code)
            {
                case 200:
                    Gui::warn(i18n::localize("SHARE_DOWNLOAD_CODE"), s);
                    break;
                case 502:
                    Gui::error(i18n::localize("HTTP_OFFLINE"), status_code);
                    break;
                default:
                    Gui::error(i18n::localize("HTTP_UNKNOWN_ERROR"), status_code);
                    break;
            }
        }
        curl_easy_cleanup(curl);
    }
    return;
}
