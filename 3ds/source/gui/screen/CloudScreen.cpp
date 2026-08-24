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

#include "CloudScreen.hpp"
#include "AccelButton.hpp"
#include "banks.hpp"
#include "base64.hpp"
#include "ClickButton.hpp"
#include "CloudOverlay.hpp"
#include "Configuration.hpp"
#include "fetch.hpp"
#include "FilterScreen.hpp"
#include "GroupCloudScreen.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "io.hpp"
#include "loader.hpp"
#include "nlohmann/json.hpp"
#include "PkmSummary.hpp"
#include "pkx/PK7.hpp"
#include "pkx/PKFilter.hpp"
#include "QRScanner.hpp"
#include "revision.h"
#include "sav/Sav.hpp"
#include "ScreenStack.hpp"
#include "utils/format.hpp"
#include <format>

CloudScreen::CloudScreen(int storageBox, std::shared_ptr<pksm::PKFilter> filter)
    : GpssScreen(i18n::localize("A_PICKUP") + '\n' + i18n::localize("X_SHARE") + '\n' +
                     i18n::localize("Y_GROUP_SINGLE") + '\n' + i18n::localize("START_SORT_FILTER") +
                     '\n' + "SELECT: " + i18n::localize("BOX_JUMP") + '\n' +
                     i18n::localize("L_BOX_PREV") + '\n' + i18n::localize("R_BOX_NEXT") + '\n' +
                     i18n::localize("B_BACK"),
          filter, storageBox)
{
    mainButtons[0] = std::make_unique<ClickButton>(
        212, 78, 108, 28,
        [this]()
        {
            ScreenStack::push(std::make_unique<FilterScreen>(this->filter));
            return true;
        },
        ui_sheet_button_editor_idx, i18n::localize("FILTER"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[1] = std::make_unique<Button>(
        212, 109, 108, 28, [this]() { return this->showViewer(); }, ui_sheet_button_editor_idx,
        i18n::localize("VIEW"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[2] = std::make_unique<Button>(
        212, 140, 108, 28, [this]() { return this->releasePkm(); }, ui_sheet_button_editor_idx,
        i18n::localize("RELEASE"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[3] = std::make_unique<Button>(
        212, 171, 108, 28, [this]() { return this->dumpPkm(); }, ui_sheet_button_editor_idx,
        i18n::localize("DUMP"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[4] = std::make_unique<Button>(
        283, 211, 34, 28, [this]() { return this->backButton(); }, ui_sheet_button_back_idx, "",
        0.0f, COLOR_BLACK);
    mainButtons[5] = std::make_unique<AccelButton>(
        8, 15, 17, 24, [this]() { return this->prevBox(true); }, ui_sheet_res_null_idx, "", 0.0f,
        COLOR_BLACK, 10, 5);
    mainButtons[6] = std::make_unique<AccelButton>(
        189, 15, 17, 24, [this]() { return this->nextBox(true); }, ui_sheet_res_null_idx, "", 0.0f,
        COLOR_BLACK, 10, 5);

    // Pokemon buttons
    u16 y = 45;
    for (u8 row = 0; row < 5; row++)
    {
        u16 x = 4;
        for (u8 column = 0; column < 6; column++)
        {
            clickButtons[row * 6 + column] = std::make_unique<ClickButton>(
                x, y, 34, 30,
                [this, row, column]() { return this->clickBottomIndex(row * 6 + column + 1); },
                ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK);
            x += 34;
        }
        y += 30;
    }
    clickButtons[30] = std::make_unique<ClickButton>(
        25, 15, 164, 24, [this]() { return this->clickBottomIndex(0); }, ui_sheet_res_null_idx, "",
        0.0f, COLOR_BLACK);
}

void CloudScreen::drawBottom() const
{
    if (!paging().good())
    {
        return;
    }
    Gui::sprite(ui_sheet_emulated_bg_bottom_green, 0, 0);
    Gui::sprite(ui_sheet_bg_style_bottom_idx, 0, 0);
    Gui::sprite(ui_sheet_bar_arc_bottom_green_idx, 0, 206);

    Gui::sprite(ui_sheet_bar_boxname_with_arrows_idx, 7, 15);
    Gui::sprite(ui_sheet_storage_box_corner_idx, 2, 44);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_horizontal_idx, 202, 44);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_vertical_idx, 2, 193);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_both_idx, 202, 193);
    for (const auto& b : mainButtons)
    {
        b->draw();
    }

    u16 y = 45;
    for (u8 row = 0; row < 5; row++)
    {
        u16 x = 4;
        for (u8 column = 0; column < 6; column++)
        {
            auto pokemon = Banks::bank->pkm(storageBox, row * 6 + column);
            if (pokemon->species() != pksm::Species::None)
            {
                float blend = *pokemon == *filter ? 0.0f : 0.5f;
                Gui::pkm(*pokemon, x, y, 1.0f, COLOR_BLACK, blend);
            }
            x += 34;
        }
        y += 30;
    }

    Gui::text(Banks::bank->boxName(storageBox), 25 + 164 / 2, 18, FONT_SIZE_14, COLOR_BLACK,
        TextPosX::CENTER, TextPosY::TOP);

    if (!cloudChosen)
    {
        if (cursorIndex == 0)
        {
            int dy = Gui::pointerBob();
            if (moveMon)
            {
                float blend = *moveMon == *filter ? 0.0f : 0.5f;
                Gui::pkm(*moveMon, 97, 10 + dy, 1.0f, COLOR_GREY_BLEND, 1.0f);
                Gui::pkm(*moveMon, 94, 5 + dy, 1.0f, COLOR_BLACK, blend);
            }
            Gui::sprite(ui_sheet_pointer_arrow_idx, 106, -4 + dy);
        }
        else
        {
            int tempIndex = cursorIndex - 1;
            int yMod      = (tempIndex / 6) * 30 + Gui::pointerBob();
            if (moveMon)
            {
                float blend = *moveMon == *filter ? 0.0f : 0.5f;
                Gui::pkm(
                    *moveMon, 12 + (tempIndex % 6) * 34, 44 + yMod, 1.0f, COLOR_GREY_BLEND, 1.0f);
                Gui::pkm(*moveMon, 9 + (tempIndex % 6) * 34, 39 + yMod, 1.0f, COLOR_BLACK, blend);
            }
            Gui::sprite(ui_sheet_pointer_arrow_idx, 21 + (tempIndex % 6) * 34, 30 + yMod);
        }
    }
}

void CloudScreen::drawTop() const
{
    if (!paging().good())
    {
        return;
    }
    Gui::sprite(ui_sheet_emulated_bg_top_green, 0, 0);
    Gui::sprite(ui_sheet_bg_style_top_idx, 0, 0);
    Gui::backgroundAnimatedTop();
    Gui::sprite(ui_sheet_bar_arc_top_green_idx, 0, 0);

    Gui::sprite(ui_sheet_textbox_pksm_idx, 261, 3);
    Gui::text("GPSS", 394, 7, FONT_SIZE_14, COLOR_WHITE, TextPosX::RIGHT, TextPosY::TOP);

    if (cloudChosen && infoMon)
    {
        if (access.isLegal(cursorIndex - 1))
        {
            Gui::sprite(ui_sheet_textbox_legal_idx, 261, 34);
            Gui::text(i18n::localize("LEGALITY_LEGAL"), 394, 38, FONT_SIZE_14, COLOR_WHITE,
                TextPosX::RIGHT, TextPosY::TOP);
        }
        else
        {
            Gui::sprite(ui_sheet_emulated_textbox_illegal_idx, 261, 34);
            Gui::text(i18n::localize("LEGALITY_ILLEGAL"), 394, 38, FONT_SIZE_14, COLOR_WHITE,
                TextPosX::RIGHT, TextPosY::TOP);
        }
    }

    Gui::sprite(ui_sheet_bar_boxname_empty_idx, 44, 21);
    Gui::text(
        "\uE004", 45 + 24 / 2, 24, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    Gui::text(
        "\uE005", 225 + 24 / 2, 24, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    Gui::text(pksm::format(i18n::localize("CLOUD_BOX"), paging().page()), 69 + 156 / 2, 24,
        FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);

    Gui::sprite(ui_sheet_storagemenu_cross_idx, 36, 50);
    Gui::sprite(ui_sheet_storagemenu_cross_idx, 246, 50);
    Gui::sprite(ui_sheet_storagemenu_cross_idx, 36, 220);
    Gui::sprite(ui_sheet_storagemenu_cross_idx, 246, 220);

    int y = 66;
    for (u8 row = 0; row < 5; row++)
    {
        u16 x = 45;
        for (u8 column = 0; column < 6; column++)
        {
            const auto* pkm = access.peek(row * 6 + column);
            if (pkm && pkm->species() != pksm::Species::None)
            {
                float blend = *pkm == *filter ? 0.0f : 0.5f;
                Gui::pkm(*pkm, x, y, 1.0f, COLOR_BLACK, blend);
            }
            x += 34;
        }
        y += 30;
    }

    if (cloudChosen)
    {
        if (cursorIndex == 0)
        {
            int dy = Gui::pointerBob();
            if (moveMon)
            {
                float blend = *moveMon == *filter ? 0.0f : 0.5f;
                Gui::pkm(*moveMon, 138, 16 + dy, 1.0f, COLOR_GREY_BLEND, 1.0f);
                Gui::pkm(*moveMon, 135, 11 + dy, 1.0f, COLOR_BLACK, blend);
            }
            Gui::sprite(ui_sheet_pointer_arrow_idx, 147, 2 + dy);
        }
        else
        {
            int tempIndex = cursorIndex - 1;
            int yMod      = (tempIndex / 6) * 30 + Gui::pointerBob();
            if (moveMon)
            {
                float blend = *moveMon == *filter ? 0.0f : 0.5f;
                Gui::pkm(
                    *moveMon, 53 + (tempIndex % 6) * 34, 65 + yMod, 1.0f, COLOR_GREY_BLEND, 1.0f);
                Gui::pkm(*moveMon, 50 + (tempIndex % 6) * 34, 60 + yMod, 1.0f, COLOR_BLACK, blend);
            }
            Gui::sprite(ui_sheet_pointer_arrow_idx, 62 + (tempIndex % 6) * 34, 51 + yMod);
        }
    }

    PkmSummary::draw(infoMon.get());
}

void CloudScreen::update(touchPosition* touch)
{
    if (!paging().good())
    {
        leftOnPageError(paging().errorCode());
        return;
    }
    if (justSwitched)
    {
        if ((keysHeld() | keysDown()) & KEY_TOUCH)
        {
            return;
        }
        else
        {
            justSwitched = false;
        }
    }
    u32 kDown   = hidKeysDown();
    u32 kUp     = hidKeysUp();
    u32 kRepeat = hidKeysDownRepeat();

    if (pendingPageJump)
    {
        if (hidKeysHeld() & KEY_SELECT)
        {
            pendingPageJumpFrames++;
            if (pendingPageJumpFrames >= 15)
            {
                pendingPageJump       = false;
                pendingPageJumpFrames = 0;
            }
            return;
        }

        pendingPageJump       = false;
        pendingPageJumpFrames = 0;
        if ((kUp & KEY_SELECT) && jumpBoxTop())
        {
            return;
        }
    }

    if (kDown & KEY_B)
    {
        backButton();
        return;
    }

    if (kDown & KEY_X)
    {
        if (infoMon && !cloudChosen)
        {
            if (!Gui::showChoiceMessage(i18n::localize("SHARE_SEND_CONFIRM")))
            {
                return;
            }
            shareSend();
        }
        else
        {
            shareReceive();
        }
    }

    if (kDown & KEY_Y)
    {
        std::unique_ptr<Screen> screen = std::make_unique<GroupCloudScreen>(storageBox, filter);
        ScreenStack::requestPop();
        ScreenStack::push(std::move(screen));
        return;
    }
    else if (kDown & KEY_SELECT)
    {
        pendingPageJump       = true;
        pendingPageJumpFrames = 1;
        return;
    }

    for (auto& button : mainButtons)
    {
        if (button->update(touch))
        {
            return;
        }
    }
    for (auto& button : clickButtons)
    {
        if (button->update(touch))
        {
            return;
        }
    }

    if (kDown & KEY_A)
    {
        if (cursorIndex != 0)
        {
            pickup();
        }
    }
    else if (kDown & KEY_START)
    {
        addOverlay<CloudOverlay>(access);
        justSwitched = true;
    }
    else if (kRepeat & KEY_LEFT)
    {
        if (cursorIndex == 0)
        {
            if (prevBox())
            {
                return;
            }
        }
        else if (cursorIndex > 1)
        {
            cursorIndex--;
        }
        else if (cursorIndex == 1)
        {
            if (prevBox())
            {
                return;
            }
            cursorIndex = 30;
        }
    }
    else if (kRepeat & KEY_RIGHT)
    {
        if (cursorIndex == 0)
        {
            if (nextBox())
            {
                return;
            }
        }
        else if (cursorIndex < 30)
        {
            cursorIndex++;
        }
        else if (cursorIndex == 30)
        {
            if (nextBox())
            {
                return;
            }
            cursorIndex = 1;
        }
    }
    else if (kRepeat & KEY_UP)
    {
        if (cursorIndex == 0 && !cloudChosen)
        {
            cloudChosen = true;
            cursorIndex = 27;
        }
        else if (cursorIndex > 0 && cursorIndex <= 6)
        {
            cursorIndex = 0;
        }
        else if (cursorIndex > 6)
        {
            cursorIndex -= 6;
        }
    }
    else if (kRepeat & KEY_DOWN)
    {
        if (cursorIndex >= 25 && cloudChosen)
        {
            cloudChosen = false;
            cursorIndex = 0;
        }
        else if (cursorIndex == 0)
        {
            cursorIndex = 3;
        }
        else if (cursorIndex < 25)
        {
            cursorIndex += 6;
        }
    }
    else if (kRepeat & KEY_R)
    {
        if (nextBox())
        {
            return;
        }
    }
    else if (kRepeat & KEY_L)
    {
        if (prevBox())
        {
            return;
        }
    }
    else if (kRepeat & KEY_ZR)
    {
        const int pageJump = Configuration::getInstance().cloudPageJump();
        if ((pageJump <= 1 ? nextBoxTop() : jumpBoxTopBy(pageJump)))
        {
            return;
        }
    }
    else if (kRepeat & KEY_ZL)
    {
        const int pageJump = Configuration::getInstance().cloudPageJump();
        if ((pageJump <= 1 ? prevBoxTop() : jumpBoxTopBy(-pageJump)))
        {
            return;
        }
    }

    if (cursorIndex != 0)
    {
        if (cloudChosen)
        {
            infoMon = access.pkm(cursorIndex - 1);
        }
        else
        {
            infoMon = Banks::bank->pkm(storageBox, cursorIndex - 1);
        }
    }
    else
    {
        infoMon = nullptr;
    }
    if (infoMon && infoMon->species() == pksm::Species::None)
    {
        infoMon = nullptr;
    }
}

void CloudScreen::pickup()
{
    if (!moveMon)
    {
        if (cloudChosen)
        {
            auto cloudMon = access.pkm(cursorIndex - 1);
            if (cloudMon && cloudMon->species() != pksm::Species::None &&
                Gui::showChoiceMessage(i18n::localize("GPSS_DOWNLOAD")))
            {
                moveMon = access.fetchPkm(cursorIndex - 1);
            }
            else
            {
                moveMon = nullptr;
            }
        }
        else
        {
            moveMon = Banks::bank->pkm(storageBox, cursorIndex - 1);
        }

        if (moveMon && moveMon->species() == pksm::Species::None)
        {
            moveMon = nullptr;
        }
    }
    else
    {
        if (cloudChosen && Gui::showChoiceMessage(i18n::localize("SHARE_SEND_CONFIRM")))
        {
            reportUploadStatus(access.pkm(std::move(moveMon)));
            moveMon = nullptr;
        }
        else if (!cloudChosen)
        {
            auto oldMon = Banks::bank->pkm(storageBox, cursorIndex - 1);
            Banks::bank->pkm(*moveMon, storageBox, cursorIndex - 1);

            if (oldMon->species() == pksm::Species::None)
            {
                moveMon = nullptr;
            }
            else
            {
                moveMon = std::move(oldMon);
            }
        }
    }
}

bool CloudScreen::backButton()
{
    if (moveMon)
    {
        moveMon = nullptr;
        return false;
    }
    ScreenStack::requestPop();
    return true;
}

bool CloudScreen::releasePkm()
{
    if (!cloudChosen && cursorIndex != 0)
    {
        auto pkm = Banks::bank->pkm(storageBox, cursorIndex - 1);
        if (pkm->species() != pksm::Species::None &&
            Gui::showChoiceMessage(i18n::localize("BANK_CONFIRM_RELEASE")))
        {
            Banks::bank->pkm(
                *pksm::PKX::getPKM<pksm::Generation::SEVEN>(nullptr, pksm::PK7::BOX_LENGTH),
                storageBox, cursorIndex - 1);
            return false;
        }
    }
    return false;
}

namespace
{
    // One Pokemon up or down, with the server only checking it: nothing here needs longer.
    constexpr long SHARE_TIMEOUT = 10;
}

void CloudScreen::shareSend()
{
    std::string version = "generation: " + (std::string)infoMon->generation();
    const std::string pksm_version =
        "source: PKSM " +
        std::format("v{:d}.{:d}.{:d}-{:s}", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO, GIT_REV);

    const Fetch::Part parts[] = {
        {"pkmn", infoMon->rawData()}
    };
    auto response = Fetch::postMultipart(
        websiteURL + "api/v2/gpss/upload/pokemon", parts, {pksm_version, version}, SHARE_TIMEOUT);
    if (!response.ok())
    {
        Gui::error(i18n::localize("CURL_ERROR"), response.code);
        return;
    }

    const long status_code = response.status;
    if (status_code == 200 || status_code == 201)
    {
        // Only a success carries a download code. An error body may not even be JSON,
        // so it is reported from the status alone.
        nlohmann::json retJson = nlohmann::json::parse(response.body, nullptr, false);
        if (retJson.is_object() && retJson.contains("code") && retJson["code"].is_string())
        {
            Gui::warn(
                i18n::localize("SHARE_DOWNLOAD_CODE") + '\n' + retJson["code"].get<std::string>());
        }
        else
        {
            Gui::warn(pksm::format(i18n::localize("GPSS_COMMUNICATION_ERROR"), status_code));
        }
    }
    else
    {
        reportUploadStatus(status_code);
    }
}

void CloudScreen::shareReceive()
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NUMPAD, 3, 10);
    swkbdSetFeatures(&state, SWKBD_FIXED_WIDTH);
    swkbdSetValidation(&state, SWKBD_FIXEDLEN, 0, 0);
    swkbdSetButton(
        &state, SwkbdButton::SWKBD_BUTTON_MIDDLE, i18n::localize("QR_SCANNER").c_str(), false);
    char input[11]  = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[10]       = '\0';
    if (ret == SWKBD_BUTTON_MIDDLE)
    {
        std::string data = QRScanner<std::string>::scan();
        if (data.length() == 10)
        {
            std::copy(data.begin(), data.end(), input);
            input[10] = '\0';
            ret       = SWKBD_BUTTON_CONFIRM;
        }
        else if (!data.empty())
        {
            Gui::warn(i18n::localize("QR_WRONG_FORMAT"));
        }
    }
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        const std::string url = websiteURL + "api/v2/gpss/download/pokemon/" + std::string(input);
        auto response         = Fetch::get(url);
        if (!response.ok())
        {
            Gui::error(i18n::localize("CURL_ERROR"), response.code);
        }
        else
        {
            const long status_code = response.status;
            switch (status_code)
            {
                case 200:
                    break;
                case 400:
                case 404:
                    Gui::error(i18n::localize("SHARE_INVALID_CODE"), status_code);
                    return;
                case 502:
                    Gui::error(i18n::localize("HTTP_OFFLINE"), status_code);
                    return;
                default:
                    Gui::error(i18n::localize("HTTP_UNKNOWN_ERROR"), status_code);
                    return;
            }

            nlohmann::json retJson = nlohmann::json::parse(response.body, nullptr, false);

            std::string generation = "";
            if (retJson.contains("generation"))
            {
                if (retJson["generation"].is_string())
                {
                    generation = retJson["generation"].get<std::string>();
                }
                else if (retJson["generation"].is_number())
                {
                    // convert the number to a string
                    generation = std::to_string(retJson["generation"].get<int>());
                }
            }

            if (retJson.is_object() && generation != "" && retJson.contains("pokemon") &&
                retJson["pokemon"].is_string())
            {
                pksm::Generation gen = pksm::Generation::fromString(generation);

                auto retData = base64_decode(retJson["pokemon"].get_ref<std::string&>());

                std::unique_ptr<pksm::PKX> pkm =
                    pksm::PKX::getPKM(gen, retData.data(), retData.size());

                if (!pkm)
                {
                    Gui::error(i18n::localize("SHARE_ERROR_INCORRECT_VERSION"), retData.size());
                    return;
                }

                if (!cloudChosen && cursorIndex != 0)
                {
                    Banks::bank->pkm(*pkm, storageBox, cursorIndex - 1);
                }
                else
                {
                    moveMon = std::move(pkm);
                }
            }
        }
    }
}
