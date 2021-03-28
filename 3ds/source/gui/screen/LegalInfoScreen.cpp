/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2021 Bernardo Giordano, Admiral Fish, piepie62
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

#include "LegalInfoScreen.hpp"
#include "ClickButton.hpp"
#include "Configuration.hpp"
#include "base64.hpp"
#include "fetch.hpp"
#include "gui.hpp"
#include "loader.hpp"
#include "website.h"
#include "nlohmann/json.hpp"
#include "pkx/PKX.hpp"
#include "sav/Sav.hpp"

LegalInfoScreen::LegalInfoScreen(const std::string& string, pksm::PKX& pk)
    : ScrollingTextScreen(string, pk)
{
    if (string.substr(0, 6) != "Legal!")
    {
        legalButton = std::make_unique<ClickButton>(
            3, 211, 28, 28,
            [this]() {
                this->attemptLegalization();
                return false;
            },
            ui_sheet_button_legalize_idx, "", 0.0f, COLOR_BLACK);

        instructions.addCircle(false, 17, 225, 8, COLOR_GREY);
        instructions.addLine(false, 17, 175, 17, 225, 4, COLOR_GREY);
        instructions.addBox(
            false, 15, 175, 120, 18, COLOR_GREY, i18n::localize("LEGALIZE"), COLOR_WHITE);
    }
}

void LegalInfoScreen::drawBottom() const
{
    ScrollingTextScreen::drawBottom();
    if (legalButton)
    {
        legalButton->draw();
    }
}

void LegalInfoScreen::update(touchPosition* touch)
{
    if (justSwitched)
    {
        if (hidKeysHeld() & KEY_TOUCH)
        {
            return;
        }
        else
        {
            justSwitched = false;
        }
    }
    if (legalButton)
    {
        legalButton->update(touch);
    }

    ScrollingTextScreen::update(touch);
}

void LegalInfoScreen::attemptLegalization()
{
    long status_code = 0;
    std::string version;
    if (TitleLoader::save)
    {
        version = "version: " + std::to_string((int)TitleLoader::save->version());
    }
    else
    {
        version = "version: " +
                  std::to_string((int)pksm::GameVersion::oldestVersion(pkm->get().generation()));
    }
    std::string generation = "generation: " + (std::string)pkm->get().generation();

    curl_slist* headers = curl_slist_append(NULL, version.c_str());
    headers             = curl_slist_append(headers, generation.c_str());

    std::string url = Configuration::getInstance().useApiUrl()
                          ? Configuration::getInstance().apiUrl()
                          : WEBSITE_URL;

    std::string writeData;
    if (auto fetch = Fetch::init(url + "api/v1/pksm/legalize", true, &writeData, headers, ""))
    {
        auto mimeThing       = fetch->mimeInit();
        curl_mimepart* field = curl_mime_addpart(mimeThing.get());
        curl_mime_name(field, "pkmn");
        curl_mime_data(field, (char*)pkm->get().rawData(), pkm->get().getLength());
        curl_mime_filename(field, "pkmn");
        fetch->setopt(CURLOPT_MIMEPOST, mimeThing.get());

        auto res = Fetch::perform(fetch);
        curl_slist_free_all(headers);
        if (res.index() == 0)
        {
            Gui::error(i18n::localize("CURL_ERROR"), std::get<0>(res));
        }
        else if (std::get<1>(res) != CURLE_OK)
        {
            Gui::error(i18n::localize("CURL_ERROR"), std::get<1>(res) + 100);
        }
        else
        {
            fetch->getinfo(CURLINFO_RESPONSE_CODE, &status_code);
            switch (status_code)
            {
                case 200:
                {
                    nlohmann::json retJson = nlohmann::json::parse(writeData, nullptr, false);
                    // clang-format off
                    if (retJson.is_object() &&
                        retJson.contains("pokemon") && (retJson["pokemon"].is_string() || retJson["pokemon"].is_null()) &&
                        retJson.contains("ran") && retJson["ran"].is_boolean() &&
                        retJson.contains("success") && retJson["success"].is_boolean())
                    // clang-format on
                    {
                        if (!retJson["success"].get<bool>())
                        {
                            Gui::warn(i18n::localize("AUTO_LEGALIZE_ERROR"));
                            Gui::screenBack();
                            return;
                        }
                        else if (!retJson["ran"].get<bool>())
                        {
                            Gui::warn(i18n::localize("ALREADY_LEGAL"));
                            Gui::screenBack();
                            return;
                        }
                        else if (!retJson["pokemon"].is_null())
                        {
                            std::vector<u8> pkmData =
                                base64_decode(retJson["pokemon"].get<std::string>());
                            auto fixed = pksm::PKX::getPKM(
                                pkm->get().generation(), pkmData.data(), pkmData.size(), true);
                            if (fixed)
                            {
                                std::copy(fixed->rawData(),
                                    fixed->rawData() +
                                        std::min(pkm->get().getLength(), fixed->getLength()),
                                    pkm->get().rawData());
                                Gui::warn(i18n::localize("PKM_LEGALIZED"));
                                Gui::screenBack();
                                return;
                            }
                            else
                            {
                                Gui::error(i18n::localize("PROBLEM_LEGALIZED_LENGTH") + '\n' +
                                               i18n::localize("REPORT_THIS_TO_FLAGBREW"),
                                    pkmData.size());
                                return;
                            }
                        }
                    }
                }
                // falls through
                default:
                    Gui::error(i18n::localize("HTTP_UNKNOWN_ERROR"), status_code);
                    return;
                case 503:
                    Gui::warn(
                        i18n::localize("LEGALIZE_IN_QUEUE") + '\n' + i18n::localize("PLEASE_WAIT"));
                    return;
            }
        }
    }
}
