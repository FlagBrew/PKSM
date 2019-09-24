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

#include "CloudScreen.hpp"
#include "AccelButton.hpp"
#include "ClickButton.hpp"
#include "CloudOverlay.hpp"
#include "CloudViewOverlay.hpp"
#include "Configuration.hpp"
#include "FSStream.hpp"
#include "FilterScreen.hpp"
#include "QRScanner.hpp"
#include "banks.hpp"
#include "base64.hpp"
#include "fetch.hpp"
#include "io.hpp"
#include <sys/stat.h>

CloudScreen::CloudScreen(int storageBox, std::shared_ptr<PKFilter> filter)
    : Screen(i18n::localize("A_PICKUP") + '\n' + i18n::localize("X_SHARE") + '\n' + i18n::localize("START_SORT_FILTER") + '\n' +
             i18n::localize("L_BOX_PREV") + '\n' + i18n::localize("R_BOX_NEXT") + '\n' + i18n::localize("B_BACK")),
      storageBox(storageBox),
      filter(filter == nullptr ? std::make_shared<PKFilter>() : filter)
{
    mainButtons[0] = std::make_unique<ClickButton>(212, 78, 108, 28,
        [this]() {
            Gui::setScreen(std::make_unique<FilterScreen>(this->filter));
            return true;
        },
        ui_sheet_button_editor_idx, i18n::localize("FILTER"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[1] = std::make_unique<Button>(
        212, 109, 108, 28, [this]() { return this->showViewer(); }, ui_sheet_button_editor_idx, i18n::localize("VIEW"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[2] = std::make_unique<Button>(
        212, 140, 108, 28, [this]() { return this->releasePkm(); }, ui_sheet_button_editor_idx, i18n::localize("RELEASE"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[3] = std::make_unique<Button>(
        212, 171, 108, 28, [this]() { return this->dumpPkm(); }, ui_sheet_button_editor_idx, i18n::localize("DUMP"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[4] =
        std::make_unique<Button>(283, 211, 34, 28, [this]() { return this->backButton(); }, ui_sheet_button_back_idx, "", 0.0f, COLOR_BLACK);
    mainButtons[5] =
        std::make_unique<AccelButton>(8, 15, 17, 24, [this]() { return this->prevBox(true); }, ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK, 10, 5);
    mainButtons[6] =
        std::make_unique<AccelButton>(189, 15, 17, 24, [this]() { return this->nextBox(true); }, ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK, 10, 5);

    // Pokemon buttons
    u16 y = 45;
    for (u8 row = 0; row < 5; row++)
    {
        u16 x = 4;
        for (u8 column = 0; column < 6; column++)
        {
            clickButtons[row * 6 + column] = std::make_unique<ClickButton>(x, y, 34, 30,
                [this, row, column]() { return this->clickBottomIndex(row * 6 + column + 1); }, ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK);
            x += 34;
        }
        y += 30;
    }
    clickButtons[30] =
        std::make_unique<ClickButton>(25, 15, 164, 24, [this]() { return this->clickBottomIndex(0); }, ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK);
}

void CloudScreen::drawBottom() const
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
    for (auto& b : mainButtons)
    {
        b->draw();
    }

    u16 y = 45;
    for (u8 row = 0; row < 5; row++)
    {
        u16 x = 4;
        for (u8 column = 0; column < 6; column++)
        {
            std::shared_ptr<PKX> pokemon = Banks::bank->pkm(storageBox, row * 6 + column);
            if (pokemon->species() > 0)
            {
                float blend = *pokemon == *filter ? 0.0f : 0.5f;
                Gui::pkm(*pokemon, x, y, 1.0f, COLOR_BLACK, blend);
            }
            x += 34;
        }
        y += 30;
    }

    Gui::text(Banks::bank->boxName(storageBox), 25 + 164 / 2, 18, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);

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
                Gui::pkm(*moveMon, 12 + (tempIndex % 6) * 34, 44 + yMod, 1.0f, COLOR_GREY_BLEND, 1.0f);
                Gui::pkm(*moveMon, 9 + (tempIndex % 6) * 34, 39 + yMod, 1.0f, COLOR_BLACK, blend);
            }
            Gui::sprite(ui_sheet_pointer_arrow_idx, 21 + (tempIndex % 6) * 34, 30 + yMod);
        }
    }
}

void CloudScreen::drawTop() const
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
        if (access.isLegal(cursorIndex - 1))
        {
            Gui::sprite(ui_sheet_textbox_legal_idx, 261, 34);
            Gui::text(i18n::localize("LEGALITY_LEGAL"), 394, 38, FONT_SIZE_14, COLOR_WHITE, TextPosX::RIGHT, TextPosY::TOP);
        }
        else
        {
            Gui::sprite(ui_sheet_emulated_textbox_illegal_idx, 261, 34);
            Gui::text(i18n::localize("LEGALITY_ILLEGAL"), 394, 38, FONT_SIZE_14, COLOR_WHITE, TextPosX::RIGHT, TextPosY::TOP);
        }
    }

    Gui::sprite(ui_sheet_bar_boxname_empty_idx, 44, 21);
    Gui::text("\uE004", 45 + 24 / 2, 24, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    Gui::text("\uE005", 225 + 24 / 2, 24, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    Gui::text(StringUtils::format(i18n::localize("CLOUD_BOX"), access.page()), 69 + 156 / 2, 24, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER,
        TextPosY::TOP);

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
            auto pkm = access.pkm(row * 6 + column);
            if (pkm->species() > 0)
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
                Gui::pkm(*moveMon, 53 + (tempIndex % 6) * 34, 65 + yMod, 1.0f, COLOR_GREY_BLEND, 1.0f);
                Gui::pkm(*moveMon, 50 + (tempIndex % 6) * 34, 60 + yMod, 1.0f, COLOR_BLACK, blend);
            }
            Gui::sprite(ui_sheet_pointer_arrow_idx, 62 + (tempIndex % 6) * 34, 51 + yMod);
        }
    }

    if (infoMon)
    {
        Gui::text(infoMon->nickname(), 276, 61, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        std::string info = "#" + std::to_string(infoMon->species());
        Gui::text(info, 273, 77, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        info      = i18n::localize("LV") + std::to_string(infoMon->level());
        auto text = Gui::parseText(info, FONT_SIZE_12, 0.0f);
        int width = text->maxWidth(FONT_SIZE_12);
        Gui::text(text, 375 - width, 77, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        if (infoMon->gender() == 0)
        {
            Gui::sprite(ui_sheet_icon_male_idx, 362 - width, 80);
        }
        else if (infoMon->gender() == 1)
        {
            Gui::sprite(ui_sheet_icon_female_idx, 364 - width, 80);
        }
        else if (infoMon->gender() == 2)
        {
            Gui::sprite(ui_sheet_icon_genderless_idx, 364 - width, 80);
        }
        if (infoMon->shiny())
        {
            Gui::sprite(ui_sheet_icon_shiny_idx, 352 - width, 81);
        }

        Gui::text(i18n::species(Configuration::getInstance().language(), infoMon->species()), 276, 98, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
            TextPosY::TOP);
        u8 firstType  = infoMon->type1();
        u8 secondType = infoMon->type2();
        if (infoMon->generation() == Generation::FOUR)
        {
            if (firstType > 8)
                firstType--;
            if (secondType > 8)
                secondType--;
        }
        if (firstType != secondType)
        {
            Gui::type(Configuration::getInstance().language(), firstType, 276, 115);
            Gui::type(Configuration::getInstance().language(), secondType, 325, 115);
        }
        else
        {
            Gui::type(Configuration::getInstance().language(), firstType, 300, 115);
        }

        info = infoMon->otName() + '\n' + i18n::localize("LOADER_ID") + std::to_string(infoMon->versionTID());
        Gui::text(info, 276, 141, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);

        Gui::text(i18n::nature(Configuration::getInstance().language(), infoMon->nature()), 276, 181, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
            TextPosY::TOP);
        info  = i18n::localize("IV") + ": ";
        text  = Gui::parseText(info, FONT_SIZE_12, 0.0f);
        width = text->maxWidth(FONT_SIZE_12);
        Gui::text(text, 276, 197, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        info = StringUtils::format("%2i/%2i/%2i", infoMon->iv(0), infoMon->iv(1), infoMon->iv(2));
        Gui::text(info, 276 + width + 70 / 2, 197, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
        info = StringUtils::format("%2i/%2i/%2i", infoMon->iv(4), infoMon->iv(5), infoMon->iv(3));
        Gui::text(info, 276 + width + 70 / 2, 209, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
        Gui::format(*infoMon, 276, 213);
    }
}

void CloudScreen::update(touchPosition* touch)
{
    if (!access.good())
    {
        Gui::warn(i18n::localize("OFFLINE_ERROR"));
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
    static bool sleep = true;
    u32 kDown         = hidKeysDown();
    u32 kHeld         = hidKeysHeld();

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
            if (!Gui::showChoiceMessage(i18n::localize("SHARE_CODE_ENTER_PROMPT")))
            {
                return;
            }
            shareReceive();
        }
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

    static int buttonCooldown = 10;

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
    else if (buttonCooldown <= 0)
    {
        sleep = false;
        if (kHeld & KEY_LEFT)
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
            sleep = true;
        }
        else if (kHeld & KEY_RIGHT)
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
            sleep = true;
        }
        else if (kHeld & KEY_UP)
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
            sleep = true;
        }
        else if (kHeld & KEY_DOWN)
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
            sleep = true;
        }
        else if (kHeld & KEY_R)
        {
            if (nextBox())
            {
                return;
            }
            sleep = true;
        }
        else if (kHeld & KEY_L)
        {
            if (prevBox())
            {
                return;
            }
            sleep = true;
        }
        else if (kHeld & KEY_ZR)
        {
            if (nextBoxTop())
            {
                return;
            }
            sleep = true;
        }
        else if (kHeld & KEY_ZL)
        {
            if (prevBoxTop())
            {
                return;
            }
            sleep = true;
        }

        if (sleep)
            buttonCooldown = 10;
    }
    if (sleep)
        buttonCooldown--;

    if (cursorIndex != 0)
    {
        infoMon = cloudChosen ? access.pkm(cursorIndex - 1) : Banks::bank->pkm(storageBox, cursorIndex - 1);
    }
    else
    {
        infoMon = nullptr;
    }
    if (infoMon && infoMon->species() == 0)
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
            if (cloudMon && cloudMon->species() != 0 && Gui::showChoiceMessage(i18n::localize("GPSS_DOWNLOAD")))
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

        if (moveMon && moveMon->species() == 0)
        {
            moveMon = nullptr;
        }
    }
    else
    {
        if (cloudChosen && Gui::showChoiceMessage(i18n::localize("SHARE_SEND_CONFIRM")))
        {
            switch (long status_code = access.pkm(moveMon))
            {
                case 200:
                case 201:
                    break;
                case 401:
                    Gui::warn(i18n::localize("GPSS_BANNED"));
                    break;
                case 503:
                    Gui::warn(i18n::localize("GPSS_TEMP_DISABLED") + '\n' + i18n::localize("PLEASE_WAIT"));
                    break;
                default:
                    Gui::error(i18n::localize("HTTP_UNKNOWN_ERROR"), status_code);
                    break;
            }
            moveMon = nullptr;
        }
        else if (!cloudChosen)
        {
            auto oldMon = Banks::bank->pkm(storageBox, cursorIndex - 1);
            Banks::bank->pkm(moveMon, storageBox, cursorIndex - 1);
            if (oldMon && oldMon->species() == 0)
            {
                moveMon = nullptr;
            }
            else
            {
                moveMon = oldMon;
            }
        }
    }
}

bool CloudScreen::prevBox(bool forceBottom)
{
    if (cloudChosen && !forceBottom)
    {
        if (!access.prevPage())
        {
            Gui::warn(i18n::localize("OFFLINE_ERROR"));
            Gui::screenBack();
            return true;
        }
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

bool CloudScreen::prevBoxTop()
{
    if (!access.prevPage())
    {
        Gui::warn(i18n::localize("OFFLINE_ERROR"));
        Gui::screenBack();
        return true;
    }
    return false;
}

bool CloudScreen::nextBox(bool forceBottom)
{
    if (cloudChosen && !forceBottom)
    {
        if (!access.nextPage())
        {
            Gui::warn(i18n::localize("OFFLINE_ERROR"));
            Gui::screenBack();
            return true;
        }
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

bool CloudScreen::nextBoxTop()
{
    if (!access.nextPage())
    {
        Gui::warn(i18n::localize("OFFLINE_ERROR"));
        Gui::screenBack();
        return true;
    }
    return false;
}

bool CloudScreen::backButton()
{
    if (moveMon)
    {
        moveMon = nullptr;
        return false;
    }
    Gui::screenBack();
    return true;
}

bool CloudScreen::showViewer()
{
    if (cursorIndex == 0)
    {
        return false;
    }

    if (infoMon && infoMon->species() != 0)
    {
        justSwitched = true;
        addOverlay<CloudViewOverlay>(infoMon);
    }
    return true;
}

bool CloudScreen::releasePkm()
{
    if (!cloudChosen && cursorIndex != 0)
    {
        auto pkm = Banks::bank->pkm(storageBox, cursorIndex - 1);
        if (pkm && pkm->species() != 0 && Gui::showChoiceMessage(i18n::localize("BANK_CONFIRM_RELEASE")))
        {
            Banks::bank->pkm(std::make_shared<PK7>(), storageBox, cursorIndex - 1);
            return false;
        }
    }
    return false;
}

bool CloudScreen::dumpPkm()
{
    if (!cloudChosen && cursorIndex != 0)
    {
        auto pkm = Banks::bank->pkm(storageBox, cursorIndex - 1);
        if (pkm && pkm->species() != 0 && Gui::showChoiceMessage(i18n::localize("BANK_CONFIRM_DUMP")))
        {
            char stringDate[12]   = {0};
            char stringTime[11]   = {0};
            time_t unixTime       = time(NULL);
            struct tm* timeStruct = gmtime((const time_t*)&unixTime);
            std::strftime(stringDate, 11, "%Y-%m-%d", timeStruct);
            std::strftime(stringTime, 10, "/%H-%M-%S", timeStruct);
            std::string path = std::string("/3ds/PKSM/dumps/") + stringDate;
            mkdir(path.c_str(), 777);
            path += stringTime;
            if (cursorIndex == 0)
            {
                return false;
            }
            else
            {
                auto dumpMon = Banks::bank->pkm(storageBox, cursorIndex - 1);
                path += " - " + std::to_string(dumpMon->species()) + " - " + dumpMon->nickname() + " - " + StringUtils::format("%08X") +
                        (dumpMon->generation() != Generation::LGPE ? ".pk" + genToString(dumpMon->generation()) : ".pb7");
                FSStream out(Archive::sd(), StringUtils::UTF8toUTF16(path), FS_OPEN_CREATE | FS_OPEN_WRITE, dumpMon->getLength());
                if (out.good())
                {
                    out.write(dumpMon->rawData(), dumpMon->getLength());
                }
                else
                {
                    Gui::error(i18n::localize("FAILED_OPEN_DUMP"), out.result());
                }
                out.close();
            }
            return true;
        }
    }
    return false;
}

bool CloudScreen::clickBottomIndex(int index)
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

static size_t generation_from_header_callback(char* buffer, size_t size, size_t nitems, void* userdata)
{
    std::string tmp(buffer, size * nitems);
    if (tmp.find("Generation:") == 0)
    {
        tmp = tmp.substr(12);
        if (tmp.find("4") == 0)
        {
            *(Generation*)(userdata) = Generation::FOUR;
        }
        else if (tmp.find("5") == 0)
        {
            *(Generation*)(userdata) = Generation::FIVE;
        }
        else if (tmp.find("6") == 0)
        {
            *(Generation*)(userdata) = Generation::SIX;
        }
        else if (tmp.find("7") == 0)
        {
            *(Generation*)(userdata) = Generation::SEVEN;
        }
        else if (tmp.find("LGPE") == 0)
        {
            *(Generation*)(userdata) = Generation::LGPE;
        }
    }
    return nitems * size;
}

void CloudScreen::shareSend()
{
    long status_code    = 0;
    std::string version = "Generation: " + genToString(infoMon->generation());
    std::string code    = Configuration::getInstance().patronCode();
    if (!code.empty())
    {
        code = "PC: " + code;
    }
    struct curl_slist* headers = NULL;
    headers                    = curl_slist_append(headers, "Content-Type: multipart/form-data");
    headers                    = curl_slist_append(headers, version.c_str());
    if (!code.empty())
    {
        headers = curl_slist_append(headers, code.c_str());
    }

    std::string writeData = "";
    if (auto fetch = Fetch::init("https://flagbrew.org/gpss/share", false, true, &writeData, headers, ""))
    {
        auto mimeThing       = fetch->mimeInit();
        curl_mimepart* field = curl_mime_addpart(mimeThing.get());
        curl_mime_name(field, "pkmn");
        curl_mime_data(field, (char*)infoMon->rawData(), infoMon->getLength());
        curl_mime_filename(field, "pkmn");
        fetch->setopt(CURLOPT_MIMEPOST, mimeThing.get());

        CURLcode res = fetch->perform();
        if (res != CURLE_OK)
        {
            Gui::error(i18n::localize("CURL_ERROR"), abs(res));
        }
        else
        {
            fetch->getinfo(CURLINFO_RESPONSE_CODE, &status_code);
            switch (status_code)
            {
                case 200:
                case 201:
                    Gui::warn(i18n::localize("SHARE_DOWNLOAD_CODE") + '\n' + writeData);
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
                case 503:
                    Gui::warn(i18n::localize("GPSS_TEMP_DISABLED") + '\n' + i18n::localize("PLEASE_WAIT"));
                    break;
                default:
                    Gui::error(i18n::localize("HTTP_UNKNOWN_ERROR"), status_code);
                    break;
            }
        }
    }
    curl_slist_free_all(headers);
}

void CloudScreen::shareReceive()
{
    static SwkbdState state;
    static bool first = true;
    if (first)
    {
        swkbdInit(&state, SWKBD_TYPE_NUMPAD, 3, 10);
        first = false;
    }
    swkbdSetFeatures(&state, SWKBD_FIXED_WIDTH);
    swkbdSetValidation(&state, SWKBD_FIXEDLEN, 0, 0);
    swkbdSetButton(&state, SwkbdButton::SWKBD_BUTTON_MIDDLE, i18n::localize("QR_SCANNER").c_str(), false);
    char input[11]  = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[10]       = '\0';
    CURLcode res;
    if (ret == SWKBD_BUTTON_MIDDLE)
    {
        std::vector<u8> data = QRScanner::scan(NUMBER);
        if (!data.empty() && data.size() < 12)
        {
            std::copy(data.begin(), data.end(), input);
            input[10] = '\0';
            ret       = SWKBD_BUTTON_CONFIRM;
        }
    }
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        const std::string url  = "https://flagbrew.org/gpss/download/" + std::string(input);
        std::string retB64Data = "";
        if (auto fetch = Fetch::init(url, false, true, &retB64Data, nullptr, ""))
        {
            long status_code = 0;
            Generation gen   = Generation::UNUSED;
            fetch->setopt(CURLOPT_HEADERDATA, &gen);
            fetch->setopt(CURLOPT_HEADERFUNCTION, generation_from_header_callback);
            res = fetch->perform();
            if (res != CURLE_OK)
            {
                Gui::error(i18n::localize("CURL_ERROR"), abs(res));
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
                auto retData = base64_decode(retB64Data.data(), retB64Data.size());

                size_t targetLength = 0;
                switch (gen)
                {
                    case Generation::FOUR:
                    case Generation::FIVE:
                        targetLength = 138;
                        break;
                    case Generation::SIX:
                    case Generation::SEVEN:
                        targetLength = 234;
                        break;
                    case Generation::LGPE:
                        targetLength = 261;
                        break;
                    default:
                        break;
                }
                if (retData.size() != targetLength)
                {
                    Gui::error(i18n::localize("SHARE_ERROR_INCORRECT_VERSION"), retData.size());
                    return;
                }

                auto pkm = PKX::getPKM(gen, retData.data());

                if (!cloudChosen && cursorIndex != 0)
                {
                    Banks::bank->pkm(pkm, storageBox, cursorIndex - 1);
                }
                else
                {
                    moveMon = pkm;
                }
            }
        }
    }
}
