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

#include "GroupCloudScreen.hpp"
#include "AccelButton.hpp"
#include "ClickButton.hpp"
#include "CloudScreen.hpp"
#include "CloudViewOverlay.hpp"
#include "Configuration.hpp"
#include "FilterScreen.hpp"
#include "QRScanner.hpp"
#include "banks.hpp"
#include "base64.hpp"
#include "fetch.hpp"
#include "format.h"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "io.hpp"
#include "loader.hpp"
#include "nlohmann/json.hpp"
#include "pkx/PK7.hpp"
#include "pkx/PKFilter.hpp"
#include "sav/Sav.hpp"
#include "website.h"
#include <algorithm>
#include <sys/stat.h>

GroupCloudScreen::GroupCloudScreen(int storageBox, std::shared_ptr<pksm::PKFilter> filter)
    : Screen(i18n::localize("A_PICKUP") + '\n' + i18n::localize("X_SHARE") + '\n' +
             i18n::localize("Y_GROUP_SINGLE") + '\n' + i18n::localize("START_FILTER_LEGAL") + '\n' +
             i18n::localize("L_BOX_PREV") + '\n' + i18n::localize("R_BOX_NEXT") + '\n' +
             i18n::localize("B_BACK")),
      filter(filter ? filter : std::make_shared<pksm::PKFilter>()),
      storageBox(storageBox)
{
    mainButtons[0] = std::make_unique<ClickButton>(
        212, 78, 108, 28,
        [this]()
        {
            Gui::setScreen(std::make_unique<FilterScreen>(this->filter));
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

void GroupCloudScreen::drawBottom() const
{
    if (!access.good())
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
            std::shared_ptr<pksm::PKX> pokemon = Banks::bank->pkm(storageBox, row * 6 + column);
            if (pokemon->species() != pksm::Species::None)
            {
                float blend = *pokemon == *filter ? 0.0f : 0.5f;
                Gui::pkm(*pokemon, x, y, 1.0f, COLOR_BLACK, blend);
            }
            for (size_t i = 0; i < toSend.size(); i++)
            {
                if (toSend[i].first == storageBox && toSend[i].second == row * 6 + column)
                {
                    Gui::sprite(ui_sheet_emulated_party_indicator_1_idx + i, x + 26, y + 24);
                }
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
            if (!groupPkm.empty())
            {
                int x = 97 - 34;
                for (size_t i = groupPkm.size(); i > 0; i--)
                {
                    float blend = *groupPkm[i - 1] == *filter ? 0.0f : 0.5f;
                    Gui::pkm(*groupPkm[i - 1], x += 34, 10 + dy, 1.0f, COLOR_GREY_BLEND, 1.0f);
                    Gui::pkm(*groupPkm[i - 1], x - 3, 5 + dy, 1.0f, COLOR_BLACK, blend);
                }
            }
            Gui::sprite(ui_sheet_pointer_arrow_idx, 106, -4 + dy);
        }
        else
        {
            int tempIndex = cursorIndex - 1;
            int yMod      = (tempIndex / 6) * 30 + Gui::pointerBob();
            if (!groupPkm.empty())
            {
                int x = 12 + (tempIndex % 6) * 34 - 34;
                for (size_t i = groupPkm.size(); i > 0; i--)
                {
                    float blend = *groupPkm[i - 1] == *filter ? 0.0f : 0.5f;
                    Gui::pkm(*groupPkm[i - 1], x += 34, 44 + yMod, 1.0f, COLOR_GREY_BLEND, 1.0f);
                    Gui::pkm(*groupPkm[i - 1], x - 3, 39 + yMod, 1.0f, COLOR_BLACK, blend);
                }
            }
            Gui::sprite(ui_sheet_pointer_arrow_idx, 21 + (tempIndex % 6) * 34, 30 + yMod);
        }
    }
}

void GroupCloudScreen::drawTop() const
{
    if (!access.good())
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
        if (access.isLegal((cursorIndex - 1) / 6, (cursorIndex - 1) % 6))
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
    Gui::text(fmt::format(i18n::localize("CLOUD_BOX"), access.page()), 69 + 156 / 2, 24,
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
            auto pkm = access.pkm(row, column);
            if (pkm->species() != pksm::Species::None)
            {
                float blend = *pkm == *filter ? 0.0f : 0.5f;
                Gui::pkm(*pkm, x, y, 1.0f, COLOR_BLACK, blend);
            }
            x += 34;
        }
        y += 30;
    }

    Gui::sprite(ui_sheet_stripe_separator_idx, 274, 97);
    Gui::sprite(ui_sheet_stripe_separator_idx, 274, 137);
    Gui::sprite(ui_sheet_stripe_separator_idx, 274, 177);
    Gui::sprite(ui_sheet_point_big_idx, 265, 66);
    Gui::sprite(ui_sheet_point_big_idx, 265, 103);
    Gui::sprite(ui_sheet_point_big_idx, 265, 146);
    Gui::sprite(ui_sheet_point_big_idx, 265, 186);

    if (cloudChosen)
    {
        if (cursorIndex == 0)
        {
            int dy = Gui::pointerBob();
            if (!groupPkm.empty())
            {
                int x = 138 - 34;
                for (size_t i = groupPkm.size(); i > 0; i--)
                {
                    float blend = *groupPkm[i - 1] == *filter ? 0.0f : 0.5f;
                    Gui::pkm(*groupPkm[i - 1], x += 34, 16 + dy, 1.0f, COLOR_GREY_BLEND, 1.0f);
                    Gui::pkm(*groupPkm[i - 1], x - 3, 11 + dy, 1.0f, COLOR_BLACK, blend);
                }
            }
            Gui::sprite(ui_sheet_pointer_arrow_idx, 147, 2 + dy);
        }
        else
        {
            int tempIndex = cursorIndex - 1;
            int yMod      = (tempIndex / 6) * 30 + Gui::pointerBob();
            if (!groupPkm.empty())
            {
                int x = 53 + (tempIndex % 6) * 34 - 34;
                for (size_t i = groupPkm.size(); i > 0; i--)
                {
                    float blend = *groupPkm[i - 1] == *filter ? 0.0f : 0.5f;
                    Gui::pkm(*groupPkm[i - 1], x += 34, 65 + yMod, 1.0f, COLOR_GREY_BLEND, 1.0f);
                    Gui::pkm(*groupPkm[i - 1], x - 3, 60 + yMod, 1.0f, COLOR_BLACK, blend);
                }
            }
            Gui::sprite(ui_sheet_pointer_arrow_idx, 62 + (tempIndex % 6) * 34, 51 + yMod);
        }
    }

    if (infoMon)
    {
        Gui::text(
            infoMon->nickname(), 276, 61, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        std::string info = "#" + std::to_string(size_t(infoMon->species()));
        Gui::text(info, 273, 77, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        info      = i18n::localize("LV") + std::to_string(infoMon->level());
        auto text = Gui::parseText(info, FONT_SIZE_12, 0.0f);
        int width = text->maxWidth(FONT_SIZE_12);
        Gui::text(text, 375 - width, 77, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
            TextPosY::TOP);
        switch (infoMon->gender())
        {
            case pksm::Gender::Male:
                Gui::sprite(ui_sheet_icon_male_idx, 362 - width, 80);
                break;
            case pksm::Gender::Female:
                Gui::sprite(ui_sheet_icon_female_idx, 364 - width, 80);
                break;
            case pksm::Gender::Genderless:
                Gui::sprite(ui_sheet_icon_genderless_idx, 364 - width, 80);
                break;
            case pksm::Gender::INVALID:
                break;
        }
        if (infoMon->shiny())
        {
            Gui::sprite(ui_sheet_icon_shiny_idx, 352 - width, 81);
        }

        Gui::text(infoMon->species().localize(Configuration::getInstance().language()), 276, 98,
            FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        pksm::Type firstType  = infoMon->type1();
        pksm::Type secondType = infoMon->type2();
        if (firstType != secondType)
        {
            Gui::type(Configuration::getInstance().language(), firstType, 276, 115);
            Gui::type(Configuration::getInstance().language(), secondType, 325, 115);
        }
        else
        {
            Gui::type(Configuration::getInstance().language(), firstType, 300, 115);
        }

        info = infoMon->otName() + '\n' + i18n::localize("LOADER_ID") +
               std::to_string(infoMon->versionTID());
        Gui::text(info, 276, 141, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);

        Gui::text(infoMon->generation() >= pksm::Generation::THREE
                      ? infoMon->nature().localize(Configuration::getInstance().language())
                      : "—",
            276, 181, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        info  = i18n::localize("IV") + ": ";
        text  = Gui::parseText(info, FONT_SIZE_12, 0.0f);
        width = text->maxWidth(FONT_SIZE_12);
        Gui::text(
            text, 276, 197, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        info = fmt::format(FMT_STRING("{:2d}/{:2d}/{:2d}"), infoMon->iv(pksm::Stat::HP),
            infoMon->iv(pksm::Stat::ATK), infoMon->iv(pksm::Stat::DEF));
        Gui::text(info, 276 + width + 70 / 2, 197, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER,
            TextPosY::TOP);
        info = fmt::format(FMT_STRING("{:2d}/{:2d}/{:2d}"), infoMon->iv(pksm::Stat::SPATK),
            infoMon->iv(pksm::Stat::SPDEF), infoMon->iv(pksm::Stat::SPD));
        Gui::text(info, 276 + width + 70 / 2, 209, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER,
            TextPosY::TOP);
        Gui::format(*infoMon, 276, 213);
    }
}

void GroupCloudScreen::update(touchPosition* touch)
{
    if (!access.good())
    {
        if (access.currentPageError() != 0)
        {
            Gui::warn(
                fmt::format(i18n::localize("GPSS_COMMUNICATION_ERROR"), access.currentPageError()));
        }
        else
        {
            Gui::warn(i18n::localize("OFFLINE_ERROR"));
        }
        Gui::screenBack();
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
    u32 kRepeat = hidKeysDownRepeat();

    if (kDown & KEY_B)
    {
        backButton();
        return;
    }

    for (auto& button : mainButtons)
    {
        if (button->update(touch))
            return;
    }
    for (auto& button : clickButtons)
    {
        if (button->update(touch))
            return;
    }

    if (kDown & KEY_A)
    {
        if (cursorIndex != 0)
        {
            pickup();
        }
    }
    else if (kDown & KEY_Y)
    {
        std::unique_ptr<Screen> screen = std::make_unique<CloudScreen>(storageBox, filter);
        Gui::screenBack();
        Gui::setScreen(std::move(screen));
        return;
    }
    else if (kDown & KEY_X)
    {
        if (toSend.size() > 1)
        {
            if (!Gui::showChoiceMessage(i18n::localize("UPLOAD_GROUP")))
            {
                return;
            }
            shareSend();
        }
        else if (groupPkm.empty())
        {
            if (!Gui::showChoiceMessage(i18n::localize("SHARE_CODE_ENTER_PROMPT")))
            {
                return;
            }
            shareReceive();
        }
    }
    else if (kDown & KEY_START)
    {
        access.filterLegal(!access.filterLegal());
    }

    if (kRepeat & KEY_LEFT)
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
        if (nextBoxTop())
        {
            return;
        }
    }
    else if (kRepeat & KEY_ZL)
    {
        if (prevBoxTop())
        {
            return;
        }
    }

    if (cursorIndex != 0)
    {
        if (cloudChosen)
        {
            infoMon = access.pkm((cursorIndex - 1) / 6, (cursorIndex - 1) % 6);
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

void GroupCloudScreen::pickup()
{
    if (groupPkm.empty())
    {
        if (cloudChosen)
        {
            auto group = access.group((cursorIndex - 1) / 6);
            if (!group.empty() && (size_t)(cursorIndex - 1) % 6 < group.size())
            {
                if (Gui::showChoiceMessage(i18n::localize("GPSS_DOWNLOAD_GROUP")))
                {
                    auto temp = access.fetchGroup((cursorIndex - 1) / 6);
                    for (auto it = temp.rbegin(); it != temp.rend(); ++it)
                    {
                        groupPkm.emplace_back(std::move(*it));
                    }
                }
            }
        }
        else
        {
            std::pair<int, int> thisPair = {storageBox, cursorIndex - 1};
            auto it                      = std::find(toSend.begin(), toSend.end(), thisPair);
            if (it == toSend.end())
            {
                if (Banks::bank->pkm(storageBox, cursorIndex - 1)->species() !=
                        pksm::Species::None &&
                    toSend.size() < 6)
                {
                    toSend.emplace_back(thisPair);
                    if (toSend.size() == 6 &&
                        Gui::showChoiceMessage(i18n::localize("UPLOAD_GROUP")))
                    {
                        shareSend();
                    }
                }
            }
            else
            {
                toSend.erase(it);
            }
        }
    }
    else if (!cloudChosen &&
             Banks::bank->pkm(storageBox, cursorIndex - 1)->species() == pksm::Species::None)
    {
        Banks::bank->pkm(*groupPkm.back(), storageBox, cursorIndex - 1);
        groupPkm.pop_back();
    }
}

bool GroupCloudScreen::prevBox(bool forceBottom)
{
    if (cloudChosen && !forceBottom)
    {
        return prevBoxTop();
    }
    else
    {
        storageBox--;
        if (storageBox == -1)
        {
            storageBox = Banks::bank->boxes() - 1;
        }
    }
    return false;
}

bool GroupCloudScreen::prevBoxTop()
{
    if (auto err = access.prevPage())
    {
        if (*err != 0)
        {
            Gui::warn(fmt::format(i18n::localize("GPSS_COMMUNICATION_ERROR"), *err));
        }
        else
        {
            Gui::warn(i18n::localize("OFFLINE_ERROR"));
        }
        Gui::screenBack();
        return true;
    }
    return false;
}

bool GroupCloudScreen::nextBox(bool forceBottom)
{
    if (cloudChosen && !forceBottom)
    {
        return nextBoxTop();
    }
    else
    {
        storageBox++;
        if (storageBox == Banks::bank->boxes())
        {
            storageBox = 0;
        }
    }
    return false;
}

bool GroupCloudScreen::nextBoxTop()
{
    if (auto err = access.nextPage())
    {
        if (*err != 0)
        {
            Gui::warn(fmt::format(i18n::localize("GPSS_COMMUNICATION_ERROR"), *err));
        }
        else
        {
            Gui::warn(i18n::localize("OFFLINE_ERROR"));
        }
        Gui::screenBack();
        return true;
    }
    return false;
}

bool GroupCloudScreen::backButton()
{
    if (!toSend.empty())
    {
        toSend.clear();
        return false;
    }
    if (!groupPkm.empty())
    {
        groupPkm.pop_back();
        return false;
    }
    Gui::screenBack();
    return true;
}

bool GroupCloudScreen::showViewer()
{
    if (cursorIndex == 0)
    {
        return false;
    }

    if (infoMon && infoMon->species() != pksm::Species::None)
    {
        justSwitched = true;
        addOverlay<CloudViewOverlay>(infoMon);
    }
    return true;
}

bool GroupCloudScreen::releasePkm()
{
    if (!cloudChosen && cursorIndex != 0)
    {
        auto pkm = Banks::bank->pkm(storageBox, cursorIndex - 1);
        if (pkm && pkm->species() != pksm::Species::None &&
            Gui::showChoiceMessage(i18n::localize("BANK_CONFIRM_RELEASE")))
        {
            auto it = std::find(
                toSend.begin(), toSend.end(), std::pair<int, int>{storageBox, cursorIndex - 1});
            if (it != toSend.end())
            {
                toSend.erase(it);
            }
            Banks::bank->pkm(
                *pksm::PKX::getPKM<pksm::Generation::SEVEN>(nullptr, pksm::PK7::BOX_LENGTH),
                storageBox, cursorIndex - 1);
            return false;
        }
    }
    return false;
}

bool GroupCloudScreen::dumpPkm()
{
    if (!cloudChosen && cursorIndex != 0)
    {
        auto dumpMon = Banks::bank->pkm(storageBox, cursorIndex - 1);
        if (dumpMon && dumpMon->species() != pksm::Species::None &&
            Gui::showChoiceMessage(i18n::localize("BANK_CONFIRM_DUMP")))
        {
            DateTime now     = DateTime::now();
            std::string path = fmt::format(FMT_STRING("/3ds/PKSM/dumps/{0:d}-{1:d}-{2:d}"),
                now.year(), now.month(), now.day());
            mkdir(path.c_str(), 777);
            path += fmt::format(
                FMT_STRING("/{0:d}-{1:d}-{2:d}"), now.hour(), now.minute(), now.second());
            if (cursorIndex == 0)
            {
                return false;
            }
            else
            {
                path += " - " + std::to_string(int(dumpMon->species())) + " - " +
                        dumpMon->nickname() + " - " +
                        fmt::format(FMT_STRING("{:08X}"), dumpMon->PID()) +
                        dumpMon->extension().data();
                FILE* out = fopen(path.c_str(), "wb");
                if (out)
                {
                    fwrite(dumpMon->rawData().data(), 1, dumpMon->getLength(), out);
                    fclose(out);
                }
                else
                {
                    Gui::error(i18n::localize("FAILED_OPEN_DUMP"), errno);
                }
            }
            return true;
        }
    }
    return false;
}

bool GroupCloudScreen::clickBottomIndex(int index)
{
    if (cursorIndex == index && !cloudChosen && cursorIndex != 0)
    {
        pickup();
    }
    else
    {
        cursorIndex = index;
        cloudChosen = false;
    }
    return false;
}

void GroupCloudScreen::shareSend()
{
    std::vector<std::unique_ptr<pksm::PKX>> sendMe;
    for (const auto& index : toSend)
    {
        sendMe.emplace_back(Banks::bank->pkm(index.first, index.second));
    }
    long status_code = access.group(std::move(sendMe));
    switch (status_code)
    {
        case 200:
        case 201:
            break;
        case 400:
            Gui::error(i18n::localize("SHARE_FAILED_CHECK"), status_code);
            break;
        case 401:
            Gui::warn(i18n::localize("GPSS_BANNED"));
            break;
        case 502:
            Gui::error(i18n::localize("HTTP_OFFLINE"), status_code);
            break;
        case 429:
        case 503:
            Gui::warn(i18n::localize("GPSS_TEMP_DISABLED") + '\n' + i18n::localize("PLEASE_WAIT"));
            break;
        default:
            Gui::error(i18n::localize("HTTP_UNKNOWN_ERROR"), status_code);
            break;
    }
}

void GroupCloudScreen::shareReceive()
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
        const std::string url = WEBSITE_URL "api/v2/gpss/download/bundle/" + std::string(input);
        std::string jsonData  = "";
        if (auto fetch = Fetch::init(url, true, &jsonData, nullptr, ""))
        {
            long status_code = 0;

            auto res = Fetch::perform(fetch);
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
                nlohmann::json groupJson = nlohmann::json::parse(jsonData, nullptr, false);

                if (groupJson.is_object() && groupJson.contains("pokemons") &&
                    groupJson["pokemons"].is_array())
                {
                    groupPkm.clear();
                    std::string badVersions;
                    std::vector<std::unique_ptr<pksm::PKX>> temPkm;
                    for (const auto& pkm : groupJson["pokemons"])
                    {
                        // clang-format off
                        if (pkm.is_object() && pkm.contains("generation") && pkm["generation"].is_string() &&
                            pkm.contains("pokemon") && pkm["pokemon"].is_string())
                        {
                            // clang-format on
                            pksm::Generation gen =
                                pksm::Generation::fromString(pkm["generation"].get<std::string>());
                            std::vector<u8> data = base64_decode(pkm["pokemon"].get<std::string>());

                            if (gen != pksm::Generation::UNUSED)
                            {
                                temPkm.emplace_back(
                                    pksm::PKX::getPKM(gen, data.data(), data.size()));
                            }
                            else
                            {
                                badVersions += pkm["generation"].get<std::string>() + ", ";
                            }
                        }
                    }

                    for (auto it = temPkm.rbegin(); it != temPkm.rend(); ++it)
                    {
                        groupPkm.emplace_back(std::move(*it));
                    }

                    if (!badVersions.empty())
                    {
                        badVersions.pop_back();
                        badVersions.pop_back();
                        Gui::warn(i18n::localize("UNSUPPORTED_FORMATS") + badVersions);
                    }
                }
            }
        }
    }
}
