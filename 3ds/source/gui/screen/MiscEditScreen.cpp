/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2020 Bernardo Giordano, Admiral Fish, piepie62
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

#include "MiscEditScreen.hpp"
#include "AccelButton.hpp"
#include "AppLegalityOverlay.hpp"
#include "ClickButton.hpp"
#include "Configuration.hpp"
#include "LegalInfoScreen.hpp"
#include "LocationOverlay.hpp"
#include "VersionOverlay.hpp"
#include "ViewOverlay.hpp"
#include "base64.hpp"
#include "fetch.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "loader.hpp"
#include "pkx/PB7.hpp"
#include "pkx/PK6.hpp"
#include "pkx/PK7.hpp"
#include "pkx/PK8.hpp"
#include "sav/Sav.hpp"

MiscEditScreen::MiscEditScreen(pksm::PKX& pkm) : pkm(pkm)
{
    buttons.push_back(std::make_unique<ClickButton>(
        283, 211, 34, 28,
        [this]() {
            Gui::screenBack();
            return true;
        },
        ui_sheet_button_back_idx, "", 0.0f, COLOR_BLACK));
    buttons.push_back(std::make_unique<ClickButton>(
        204, 171, 108, 30,
        [this]() {
            otAndMet = !otAndMet;
            return true;
        },
        ui_sheet_button_editor_idx, "", 0.0f, COLOR_BLACK));

    instructions.addCircle(false, 272 + 24, 24, 6, COLOR_GREY);
    instructions.addLine(false, 272 + 24, 24, 272 + 24, 64, 4, COLOR_GREY);
    instructions.addBox(
        false, 76 + 24, 64, 200, 18, COLOR_GREY, i18n::localize("APP_LEGALIZE"), COLOR_WHITE);
    buttons.push_back(std::make_unique<ClickButton>(
        278, 0, 42, 42,
        [this]() {
            addOverlay<AppLegalityOverlay>(this->pkm);
            return true;
        },
        ui_sheet_gpssmobile_curve_idx, "", 0.0f, COLOR_BLACK));

    instructions.addCircle(false, 22, 225, 8, COLOR_GREY);
    instructions.addLine(false, 22, 175, 22, 225, 4, COLOR_GREY);
    instructions.addBox(
        false, 20, 175, 170, 18, COLOR_GREY, i18n::localize("Y_LEGALIZE"), COLOR_WHITE);
    buttons.push_back(std::make_unique<ClickButton>(
        3, 211, 34, 28,
        [this]() {
            validate();
            return true;
        },
        ui_sheet_button_wireless_idx, "", 0.0f, COLOR_BLACK));

    buttons.push_back(std::make_unique<AccelButton>(
        94, 34, 13, 13, [this]() { return this->changeMetLevel(false); },
        ui_sheet_button_minus_small_idx, "", 0.0f, COLOR_BLACK));
    buttons.push_back(std::make_unique<Button>(
        109, 34, 31, 13,
        [this]() {
            setMetLevel();
            return false;
        },
        ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK));
    buttons.push_back(std::make_unique<AccelButton>(
        142, 34, 13, 13, [this]() { return this->changeMetLevel(true); },
        ui_sheet_button_plus_small_idx, "", 0.0f, COLOR_BLACK));

    buttons.push_back(std::make_unique<Button>(
        95, 54, 15, 12,
        [this]() {
            day();
            return true;
        },
        ui_sheet_button_info_detail_editor_dark_idx, "", 0.0f, COLOR_BLACK));
    buttons.push_back(std::make_unique<ClickButton>(
        95, 74, 15, 12,
        [this]() {
            month();
            return true;
        },
        ui_sheet_button_info_detail_editor_dark_idx, "", 0.0f, COLOR_BLACK));
    buttons.push_back(std::make_unique<Button>(
        95, 94, 15, 12,
        [this]() {
            year();
            return true;
        },
        ui_sheet_button_info_detail_editor_dark_idx, "", 0.0f, COLOR_BLACK));
    buttons.push_back(std::make_unique<Button>(
        95, 114, 15, 12,
        [this]() {
            addOverlay<LocationOverlay>(this->pkm, this->otAndMet);
            return true;
        },
        ui_sheet_button_info_detail_editor_dark_idx, "", 0.0f, COLOR_BLACK));
    buttons.push_back(std::make_unique<Button>(
        95, 134, 15, 12,
        [this]() {
            addOverlay<VersionOverlay>(this->pkm);
            return true;
        },
        ui_sheet_button_info_detail_editor_dark_idx, "", 0.0f, COLOR_BLACK));

    if (pkm.generation() > pksm::Generation::FIVE)
    {
        buttons.push_back(std::make_unique<AccelButton>(
            94, 154, 13, 13, [this]() { return this->changeEnjoyment(false); },
            ui_sheet_button_minus_small_idx, "", 0.0f, COLOR_BLACK));
        buttons.push_back(std::make_unique<Button>(
            109, 154, 31, 13,
            [this]() {
                setEnjoyment();
                return false;
            },
            ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK));
        buttons.push_back(std::make_unique<AccelButton>(
            142, 154, 13, 13, [this]() { return this->changeEnjoyment(true); },
            ui_sheet_button_plus_small_idx, "", 0.0f, COLOR_BLACK));

        buttons.push_back(std::make_unique<AccelButton>(
            94, 174, 13, 13, [this]() { return this->changeFullness(false); },
            ui_sheet_button_minus_small_idx, "", 0.0f, COLOR_BLACK));
        buttons.push_back(std::make_unique<Button>(
            109, 174, 31, 13,
            [this]() {
                setFullness();
                return false;
            },
            ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK));
        buttons.push_back(std::make_unique<AccelButton>(
            142, 174, 13, 13, [this]() { return this->changeFullness(true); },
            ui_sheet_button_plus_small_idx, "", 0.0f, COLOR_BLACK));

        if (pkm.generation() < pksm::Generation::EIGHT)
        {
            buttons.push_back(std::make_unique<AccelButton>(
                94, 194, 13, 13, [this]() { return this->changeAffection(false); },
                ui_sheet_button_minus_small_idx, "", 0.0f, COLOR_BLACK));
            buttons.push_back(std::make_unique<Button>(
                109, 194, 31, 13,
                [this]() {
                    setAffection();
                    return false;
                },
                ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK));
            buttons.push_back(std::make_unique<AccelButton>(
                142, 194, 13, 13, [this]() { return this->changeAffection(true); },
                ui_sheet_button_plus_small_idx, "", 0.0f, COLOR_BLACK));
        }
    }

    addOverlay<ViewOverlay>(this->pkm, false);
}

void MiscEditScreen::drawBottom() const
{
    pksm::Language lang = Configuration::getInstance().language();
    Gui::sprite(ui_sheet_emulated_bg_bottom_blue, 0, 0);
    Gui::sprite(ui_sheet_bg_style_bottom_idx, 0, 0);
    Gui::sprite(ui_sheet_bar_arc_bottom_blue_idx, 0, 206);
    Gui::backgroundAnimatedBottom();

    Gui::sprite(ui_sheet_textbox_name_bottom_idx, 0, 1);

    for (int i = 0; i < 5; i++)
    {
        Gui::sprite(ui_sheet_stripe_info_row_idx, 0, 30 + i * 40);
    }

    for (const auto& button : buttons)
    {
        button->draw();
    }

    // Gui::text(i18n::localize("APP_LEGALIZE"), 258, 155, FONT_SIZE_12, COLOR_BLACK,
    // TextPosX::CENTER, TextPosY::CENTER, TextWidthAction::WRAP, 100.0f);

    Gui::text(i18n::localize(otAndMet ? "HT_EGG" : "OT_MET"), 258, 186, FONT_SIZE_12, COLOR_BLACK,
        TextPosX::CENTER, TextPosY::CENTER, TextWidthAction::SQUISH_OR_SCROLL, 100.0f);
    Gui::text(i18n::localize("MET_LEVEL"), 5, 32, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
        TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 87);
    Gui::text(i18n::localize(otAndMet ? "MET_DAY" : "EGG_DAY"), 5, 52, FONT_SIZE_12, COLOR_BLACK,
        TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 87);
    Gui::text(i18n::localize(otAndMet ? "MET_MONTH" : "EGG_MONTH"), 5, 72, FONT_SIZE_12,
        COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 87);
    Gui::text(i18n::localize(otAndMet ? "MET_YEAR" : "EGG_YEAR"), 5, 92, FONT_SIZE_12, COLOR_BLACK,
        TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 87);
    Gui::text(i18n::localize(otAndMet ? "MET_LOCATION" : "EGG_LOCATION"), 5, 112, FONT_SIZE_12,
        COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 87);
    Gui::text(i18n::localize("ORIGIN_GAME"), 5, 132, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
        TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 87);
    if (pkm.generation() > pksm::Generation::FIVE)
    {
        Gui::text(i18n::localize("ENJOYMENT"), 5, 152, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
            TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 87);
        Gui::text(i18n::localize("FULLNESS"), 5, 172, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
            TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 87);
        if (pkm.generation() < pksm::Generation::EIGHT)
        {
            Gui::text(i18n::localize(otAndMet ? "OT_AFFECTION" : "HT_AFFECTION"), 5, 192,
                FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP,
                TextWidthAction::SQUISH_OR_SCROLL, 87);
        }
    }

    Gui::text(std::to_string((int)pkm.metLevel()), 107 + 35 / 2, 32, FONT_SIZE_12, COLOR_BLACK,
        TextPosX::CENTER, TextPosY::TOP);
    Date date = otAndMet ? pkm.metDate() : pkm.eggDate();
    Gui::text(std::to_string((int)date.day()), 115, 52, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
        TextPosY::TOP);
    Gui::text(std::to_string((int)date.month()), 115, 72, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
        TextPosY::TOP);
    Gui::text(std::to_string(date.year()), 115, 92, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
        TextPosY::TOP);
    Gui::text(i18n::location(lang, (pksm::Generation)pkm.version(),
                  otAndMet ? pkm.metLocation() : pkm.eggLocation()),
        115, 112, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(i18n::game(lang, pkm.version()), 115, 132, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
        TextPosY::TOP);
    if (pkm.generation() > pksm::Generation::FIVE)
    {
        int print = 0;
        if (pkm.generation() == pksm::Generation::SIX)
        {
            print = reinterpret_cast<pksm::PK6&>(pkm).enjoyment();
        }
        else if (pkm.generation() == pksm::Generation::SEVEN)
        {
            print = reinterpret_cast<pksm::PK7&>(pkm).enjoyment();
        }
        else if (pkm.generation() == pksm::Generation::LGPE)
        {
            print = reinterpret_cast<pksm::PB7&>(pkm).enjoyment();
        }
        else if (pkm.generation() == pksm::Generation::EIGHT)
        {
            print = reinterpret_cast<pksm::PK8&>(pkm).enjoyment();
        }
        Gui::text(std::to_string(print), 107 + 35 / 2, 152, FONT_SIZE_12, COLOR_BLACK,
            TextPosX::CENTER, TextPosY::TOP);
        if (pkm.generation() == pksm::Generation::SIX)
        {
            print = reinterpret_cast<pksm::PK6&>(pkm).fullness();
        }
        else if (pkm.generation() == pksm::Generation::SEVEN)
        {
            print = reinterpret_cast<pksm::PK7&>(pkm).fullness();
        }
        else if (pkm.generation() == pksm::Generation::LGPE)
        {
            print = reinterpret_cast<pksm::PB7&>(pkm).fullness();
        }
        else if (pkm.generation() == pksm::Generation::EIGHT)
        {
            print = reinterpret_cast<pksm::PK8&>(pkm).fullness();
        }
        Gui::text(std::to_string(print), 107 + 35 / 2, 172, FONT_SIZE_12, COLOR_BLACK,
            TextPosX::CENTER, TextPosY::TOP);
        if (pkm.generation() < pksm::Generation::EIGHT)
        {
            if (otAndMet)
            {
                if (pkm.generation() == pksm::Generation::SIX)
                {
                    print = reinterpret_cast<pksm::PK6&>(pkm).otAffection();
                }
                else if (pkm.generation() == pksm::Generation::SEVEN)
                {
                    print = reinterpret_cast<pksm::PK7&>(pkm).otAffection();
                }
                else if (pkm.generation() == pksm::Generation::LGPE)
                {
                    print = reinterpret_cast<pksm::PB7&>(pkm).otAffection();
                }
            }
            else
            {
                if (pkm.generation() == pksm::Generation::SIX)
                {
                    print = reinterpret_cast<pksm::PK6&>(pkm).htAffection();
                }
                else if (pkm.generation() == pksm::Generation::SEVEN)
                {
                    print = reinterpret_cast<pksm::PK7&>(pkm).htAffection();
                }
                else if (pkm.generation() == pksm::Generation::LGPE)
                {
                    print = reinterpret_cast<pksm::PB7&>(pkm).htAffection();
                }
            }
            Gui::text(std::to_string(print), 107 + 35 / 2, 192, FONT_SIZE_12, COLOR_BLACK,
                TextPosX::CENTER, TextPosY::TOP);
        }
    }
}

void MiscEditScreen::update(touchPosition* touch)
{
    u32 kDown = hidKeysDown();
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

    if (kDown & KEY_B)
    {
        Gui::screenBack();
        return;
    }
    else if (kDown & KEY_Y)
    {
        validate();
        return;
    }

    for (auto& button : buttons)
    {
        if (button->update(touch))
        {
            return;
        }
    }
}

bool MiscEditScreen::changeAffection(bool up)
{
    if (otAndMet)
    {
        if (up)
        {
            if (pkm.generation() == pksm::Generation::SIX)
            {
                reinterpret_cast<pksm::PK6&>(pkm).otAffection(
                    reinterpret_cast<pksm::PK6&>(pkm).otAffection() + 1);
            }
            else if (pkm.generation() == pksm::Generation::SEVEN)
            {
                reinterpret_cast<pksm::PK7&>(pkm).otAffection(
                    reinterpret_cast<pksm::PK7&>(pkm).otAffection() + 1);
            }
            else if (pkm.generation() == pksm::Generation::LGPE)
            {
                reinterpret_cast<pksm::PB7&>(pkm).otAffection(
                    reinterpret_cast<pksm::PB7&>(pkm).otAffection() + 1);
            }
        }
        else
        {
            if (pkm.generation() == pksm::Generation::SIX)
            {
                reinterpret_cast<pksm::PK6&>(pkm).otAffection(
                    reinterpret_cast<pksm::PK6&>(pkm).otAffection() - 1);
            }
            else if (pkm.generation() == pksm::Generation::SEVEN)
            {
                reinterpret_cast<pksm::PK7&>(pkm).otAffection(
                    reinterpret_cast<pksm::PK7&>(pkm).otAffection() - 1);
            }
            else if (pkm.generation() == pksm::Generation::LGPE)
            {
                reinterpret_cast<pksm::PB7&>(pkm).otAffection(
                    reinterpret_cast<pksm::PB7&>(pkm).otAffection() - 1);
            }
        }
    }
    else
    {
        if (up)
        {
            if (pkm.generation() == pksm::Generation::SIX)
            {
                reinterpret_cast<pksm::PK6&>(pkm).htAffection(
                    reinterpret_cast<pksm::PK6&>(pkm).htAffection() + 1);
            }
            else if (pkm.generation() == pksm::Generation::SEVEN)
            {
                reinterpret_cast<pksm::PK7&>(pkm).htAffection(
                    reinterpret_cast<pksm::PK7&>(pkm).htAffection() + 1);
            }
            else if (pkm.generation() == pksm::Generation::LGPE)
            {
                reinterpret_cast<pksm::PB7&>(pkm).htAffection(
                    reinterpret_cast<pksm::PB7&>(pkm).htAffection() + 1);
            }
        }
        else
        {
            if (pkm.generation() == pksm::Generation::SIX)
            {
                reinterpret_cast<pksm::PK6&>(pkm).htAffection(
                    reinterpret_cast<pksm::PK6&>(pkm).htAffection() - 1);
            }
            else if (pkm.generation() == pksm::Generation::SEVEN)
            {
                reinterpret_cast<pksm::PK7&>(pkm).htAffection(
                    reinterpret_cast<pksm::PK7&>(pkm).htAffection() - 1);
            }
            else if (pkm.generation() == pksm::Generation::LGPE)
            {
                reinterpret_cast<pksm::PB7&>(pkm).htAffection(
                    reinterpret_cast<pksm::PB7&>(pkm).htAffection() - 1);
            }
        }
    }
    return false;
}

void MiscEditScreen::setAffection()
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NUMPAD, 2, 3);
    swkbdSetFeatures(&state, SWKBD_FIXED_WIDTH);
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    char input[4]   = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[3]        = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        u8 affection = (u8)std::min(std::stoi(input), 255);
        if (otAndMet)
        {
            if (pkm.generation() == pksm::Generation::SIX)
            {
                reinterpret_cast<pksm::PK6&>(pkm).otAffection(affection);
            }
            else if (pkm.generation() == pksm::Generation::SEVEN)
            {
                reinterpret_cast<pksm::PK7&>(pkm).otAffection(affection);
            }
            else if (pkm.generation() == pksm::Generation::LGPE)
            {
                reinterpret_cast<pksm::PB7&>(pkm).otAffection(affection);
            }
        }
        else
        {
            if (pkm.generation() == pksm::Generation::SIX)
            {
                reinterpret_cast<pksm::PK6&>(pkm).htAffection(affection);
            }
            else if (pkm.generation() == pksm::Generation::SEVEN)
            {
                reinterpret_cast<pksm::PK7&>(pkm).htAffection(affection);
            }
            else if (pkm.generation() == pksm::Generation::LGPE)
            {
                reinterpret_cast<pksm::PB7&>(pkm).htAffection(affection);
            }
        }
    }
}

bool MiscEditScreen::changeFullness(bool up)
{
    if (up)
    {
        if (pkm.generation() == pksm::Generation::SIX)
        {
            reinterpret_cast<pksm::PK6&>(pkm).fullness(
                reinterpret_cast<pksm::PK6&>(pkm).fullness() + 1);
        }
        else if (pkm.generation() == pksm::Generation::SEVEN)
        {
            reinterpret_cast<pksm::PK7&>(pkm).fullness(
                reinterpret_cast<pksm::PK7&>(pkm).fullness() + 1);
        }
        else if (pkm.generation() == pksm::Generation::LGPE)
        {
            reinterpret_cast<pksm::PB7&>(pkm).fullness(
                reinterpret_cast<pksm::PB7&>(pkm).fullness() + 1);
        }
        else if (pkm.generation() == pksm::Generation::EIGHT)
        {
            reinterpret_cast<pksm::PK8&>(pkm).fullness(
                reinterpret_cast<pksm::PK8&>(pkm).fullness() + 1);
        }
    }
    else
    {
        if (pkm.generation() == pksm::Generation::SIX)
        {
            reinterpret_cast<pksm::PK6&>(pkm).fullness(
                reinterpret_cast<pksm::PK6&>(pkm).fullness() - 1);
        }
        else if (pkm.generation() == pksm::Generation::SEVEN)
        {
            reinterpret_cast<pksm::PK7&>(pkm).fullness(
                reinterpret_cast<pksm::PK7&>(pkm).fullness() - 1);
        }
        else if (pkm.generation() == pksm::Generation::LGPE)
        {
            reinterpret_cast<pksm::PB7&>(pkm).fullness(
                reinterpret_cast<pksm::PB7&>(pkm).fullness() - 1);
        }
        else if (pkm.generation() == pksm::Generation::EIGHT)
        {
            reinterpret_cast<pksm::PK8&>(pkm).fullness(
                reinterpret_cast<pksm::PK8&>(pkm).fullness() - 1);
        }
    }
    return false;
}

void MiscEditScreen::setFullness()
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NUMPAD, 2, 3);
    swkbdSetFeatures(&state, SWKBD_FIXED_WIDTH);
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    char input[4]   = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[3]        = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        u8 fullness = (u8)std::min(std::stoi(input), 255);
        if (pkm.generation() == pksm::Generation::SIX)
        {
            reinterpret_cast<pksm::PK6&>(pkm).fullness(fullness);
        }
        else if (pkm.generation() == pksm::Generation::SEVEN)
        {
            reinterpret_cast<pksm::PK7&>(pkm).fullness(fullness);
        }
        else if (pkm.generation() == pksm::Generation::LGPE)
        {
            reinterpret_cast<pksm::PB7&>(pkm).fullness(fullness);
        }
        else if (pkm.generation() == pksm::Generation::EIGHT)
        {
            reinterpret_cast<pksm::PK8&>(pkm).fullness(fullness);
        }
    }
}

bool MiscEditScreen::changeEnjoyment(bool up)
{
    if (up)
    {
        if (pkm.generation() == pksm::Generation::SIX)
        {
            reinterpret_cast<pksm::PK6&>(pkm).enjoyment(
                reinterpret_cast<pksm::PK6&>(pkm).enjoyment() + 1);
        }
        else if (pkm.generation() == pksm::Generation::SEVEN)
        {
            reinterpret_cast<pksm::PK7&>(pkm).enjoyment(
                reinterpret_cast<pksm::PK7&>(pkm).enjoyment() + 1);
        }
        else if (pkm.generation() == pksm::Generation::LGPE)
        {
            reinterpret_cast<pksm::PB7&>(pkm).enjoyment(
                reinterpret_cast<pksm::PB7&>(pkm).enjoyment() + 1);
        }
        else if (pkm.generation() == pksm::Generation::EIGHT)
        {
            reinterpret_cast<pksm::PK8&>(pkm).enjoyment(
                reinterpret_cast<pksm::PK8&>(pkm).enjoyment() + 1);
        }
    }
    else
    {
        if (pkm.generation() == pksm::Generation::SIX)
        {
            reinterpret_cast<pksm::PK6&>(pkm).enjoyment(
                reinterpret_cast<pksm::PK6&>(pkm).enjoyment() - 1);
        }
        else if (pkm.generation() == pksm::Generation::SEVEN)
        {
            reinterpret_cast<pksm::PK7&>(pkm).enjoyment(
                reinterpret_cast<pksm::PK7&>(pkm).enjoyment() - 1);
        }
        else if (pkm.generation() == pksm::Generation::LGPE)
        {
            reinterpret_cast<pksm::PB7&>(pkm).enjoyment(
                reinterpret_cast<pksm::PB7&>(pkm).enjoyment() - 1);
        }
        else if (pkm.generation() == pksm::Generation::EIGHT)
        {
            reinterpret_cast<pksm::PK8&>(pkm).enjoyment(
                reinterpret_cast<pksm::PK8&>(pkm).enjoyment() - 1);
        }
    }
    return false;
}

void MiscEditScreen::setEnjoyment()
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NUMPAD, 2, 3);
    swkbdSetFeatures(&state, SWKBD_FIXED_WIDTH);
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    char input[4]   = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[3]        = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        u8 enjoyment = (u8)std::min(std::stoi(input), 255);
        if (pkm.generation() == pksm::Generation::SIX)
        {
            reinterpret_cast<pksm::PK6&>(pkm).enjoyment(enjoyment);
        }
        else if (pkm.generation() == pksm::Generation::SEVEN)
        {
            reinterpret_cast<pksm::PK7&>(pkm).enjoyment(enjoyment);
        }
        else if (pkm.generation() == pksm::Generation::LGPE)
        {
            reinterpret_cast<pksm::PB7&>(pkm).enjoyment(enjoyment);
        }
        else if (pkm.generation() == pksm::Generation::EIGHT)
        {
            reinterpret_cast<pksm::PK8&>(pkm).enjoyment(enjoyment);
        }
    }
}

bool MiscEditScreen::changeMetLevel(bool up)
{
    if (up)
    {
        pkm.metLevel(pkm.metLevel() + 1);
    }
    else
    {
        pkm.metLevel(pkm.metLevel() - 1);
    }
    return false;
}

void MiscEditScreen::setMetLevel()
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NUMPAD, 2, 3);
    swkbdSetFeatures(&state, SWKBD_FIXED_WIDTH);
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    char input[4]   = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[3]        = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        u8 level = (u8)std::min(std::stoi(input), 100);
        pkm.metLevel(level);
    }
}

void MiscEditScreen::day()
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NUMPAD, 2, 2);
    swkbdSetFeatures(&state, SWKBD_FIXED_WIDTH);
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    char input[3]   = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[2]        = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        Date date = otAndMet ? pkm.metDate() : pkm.eggDate();
        date.day((u8)std::max(1, std::min(std::stoi(input), 31)));
        if (otAndMet)
        {
            pkm.metDate(date);
        }
        else
        {
            pkm.eggDate(date);
        }
    }
}

void MiscEditScreen::month()
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NUMPAD, 2, 2);
    swkbdSetFeatures(&state, SWKBD_FIXED_WIDTH);
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    char input[3]   = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[2]        = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        Date date = otAndMet ? pkm.metDate() : pkm.eggDate();
        date.month((u8)std::max(1, std::min(std::stoi(input), 12)));
        if (otAndMet)
        {
            pkm.metDate(date);
        }
        else
        {
            pkm.eggDate(date);
        }
    }
}

void MiscEditScreen::year()
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NUMPAD, 2, 4);
    swkbdSetFeatures(&state, SWKBD_FIXED_WIDTH);
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    char input[5]   = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[4]        = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        Date date = otAndMet ? pkm.metDate() : pkm.eggDate();
        date.year(std::max(2000, std::stoi(input)));
        if (otAndMet)
        {
            pkm.metDate(date);
        }
        else
        {
            pkm.eggDate(date);
        }
    }
}

void MiscEditScreen::validate()
{
    std::string generation     = "Generation: " + (std::string)pkm.generation();
    struct curl_slist* headers = NULL;
    headers                    = curl_slist_append(headers, "Content-Type: multipart/form-data");
    headers                    = curl_slist_append(headers, generation.c_str());

    std::string url = Configuration::getInstance().useApiUrl()
                          ? Configuration::getInstance().apiUrl()
                          : "https://flagbrew.org/";

    std::string writeData = "";
    if (auto fetch = Fetch::init(url + "pksm/legality/check", true, &writeData, headers, ""))
    {
        auto mimeThing       = fetch->mimeInit();
        curl_mimepart* field = curl_mime_addpart(mimeThing.get());
        curl_mime_name(field, "pkmn");
        pkm.refreshChecksum();
        curl_mime_data(field, (char*)pkm.rawData(), pkm.getLength());
        curl_mime_filename(field, "pkmn");
        fetch->setopt(CURLOPT_MIMEPOST, mimeThing.get());

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
            long status_code;
            fetch->getinfo(CURLINFO_RESPONSE_CODE, &status_code);
            switch (status_code)
            {
                case 200:
                    // std::copy(dataToWrite.begin(), dataToWrite.end(), pkm.rawData());
                    if (writeData.size() > 0)
                    {
                        Gui::setScreen(std::make_unique<LegalInfoScreen>(writeData, pkm));
                    }
                    break;
                case 502:
                    Gui::error(i18n::localize("HTTP_OFFLINE"), status_code);
                    break;
                default:
                    Gui::error(i18n::localize("HTTP_UNKNOWN_ERROR"), status_code);
                    break;
            }
        }
    }
    curl_slist_free_all(headers);
    // dataToWrite.clear();
    return;
}
