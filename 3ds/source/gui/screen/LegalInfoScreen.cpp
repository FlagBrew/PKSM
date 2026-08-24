/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2025 Bernardo Giordano, Admiral Fish, piepie62
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
#include "base64.hpp"
#include "ClickButton.hpp"
#include "Configuration.hpp"
#include "fetch.hpp"
#include "gui.hpp"
#include "loader.hpp"
#include "nlohmann/json.hpp"
#include "pkx/PKX.hpp"
#include "sav/Sav.hpp"
#include "ScreenStack.hpp"

LegalInfoScreen::LegalInfoScreen(const std::string& string, pksm::PKX& pk)
    : ScrollingTextScreen(string, pk)
{
    if (string.substr(0, 6) != "Legal!")
    {
        legalButton = std::make_unique<ClickButton>(
            3, 211, 28, 28,
            [this]()
            {
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

namespace
{
    // Legalizing can mean brute-forcing, and the server says nothing while it does. Deliberately
    // over Fetch::STALL_GUARD_LIMIT: that is what keeps the stall guard off a request that is
    // quiet by design. Before Fetch owned that rule, this call site set the long timeout and left
    // the guard armed, so a legalization that took its time was killed at ten seconds.
    constexpr long LEGALIZE_TIMEOUT = 120;
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

    std::string url = Configuration::getInstance().apiUrl();

    if (url == "")
    {
        Gui::warn(i18n::localize("API_URL_REQUIRED"));
        return;
    }

    const Fetch::Part parts[] = {
        {"pkmn", pkm->get().rawData()}
    };
    auto response = Fetch::postMultipart(
        url + "api/v2/pksm/legalize", parts, {version, generation}, LEGALIZE_TIMEOUT);
    if (!response.ok())
    {
        Gui::error(i18n::localize("CURL_ERROR"), response.code);
    }
    else
    {
        status_code = response.status;
        switch (status_code)
        {
            case 200:
            {
                nlohmann::json retJson = nlohmann::json::parse(response.body, nullptr, false);
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
                        ScreenStack::requestPop();
                        return;
                    }
                    else if (!retJson["ran"].get<bool>())
                    {
                        Gui::warn(i18n::localize("ALREADY_LEGAL"));
                        ScreenStack::requestPop();
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
                            std::ranges::copy(
                                fixed->rawData().subspan(
                                    0, std::min(pkm->get().getLength(), fixed->getLength())),
                                pkm->get().rawData().begin());
                            Gui::warn(i18n::localize("PKM_LEGALIZED"));
                            ScreenStack::requestPop();
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
