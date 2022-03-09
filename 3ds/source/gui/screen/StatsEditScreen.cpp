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

#include "StatsEditScreen.hpp"
#include "AccelButton.hpp"
#include "ClickButton.hpp"
#include "HiddenPowerOverlay.hpp"
#include "ViewOverlay.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "pkx/PB7.hpp"

namespace
{
    constexpr pksm::Stat statValues[] = {pksm::Stat::HP, pksm::Stat::ATK, pksm::Stat::DEF,
        pksm::Stat::SPATK, pksm::Stat::SPDEF, pksm::Stat::SPD};
}

StatsEditScreen::StatsEditScreen(pksm::PKX& pkm) : pkm(pkm)
{
    buttons.push_back(std::make_unique<ClickButton>(
        283, 211, 34, 28,
        [this]()
        {
            Gui::screenBack();
            return true;
        },
        ui_sheet_button_back_idx, "", 0.0f, COLOR_BLACK));
    for (int i = 0; i < 6; i++)
    {
        int y = 54 + i * 20;
        buttons.push_back(std::make_unique<AccelButton>(
            106, y, 13, 13, [this, i]() { return this->changeIV(statValues[i], false); },
            ui_sheet_button_minus_small_idx, "", 0.0f, COLOR_BLACK));
        buttons.push_back(std::make_unique<Button>(
            121, y, 23, 13,
            [this, i]()
            {
                setIV(statValues[i]);
                return false;
            },
            ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK));
        instructions.addCircle(false, 132, y + 6, 9, COLOR_GREY);

        buttons.push_back(std::make_unique<AccelButton>(
            146, y, 13, 13, [this, i]() { return this->changeIV(statValues[i], true); },
            ui_sheet_button_plus_small_idx, "", 0.0f, COLOR_BLACK));

        buttons.push_back(std::make_unique<AccelButton>(
            182, y, 13, 13, [this, i]() { return this->changeSecondaryStat(statValues[i], false); },
            ui_sheet_button_minus_small_idx, "", 0.0f, COLOR_BLACK));
        buttons.push_back(std::make_unique<Button>(
            197, y, 32, 13,
            [this, i]()
            {
                setSecondaryStat(statValues[i]);
                return false;
            },
            ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK));
        instructions.addCircle(false, 213, y + 6, 9, COLOR_GREY);

        buttons.push_back(std::make_unique<AccelButton>(
            231, y, 13, 13, [this, i]() { return this->changeSecondaryStat(statValues[i], true); },
            ui_sheet_button_plus_small_idx, "", 0.0f, COLOR_BLACK));
    }
    instructions.addLine(false, 132, 34, 132, 168, 4, COLOR_GREY);
    instructions.addLine(false, 213, 34, 213, 168, 4, COLOR_GREY);
    instructions.addBox(false, 130, 18, 85, 16, COLOR_GREY, i18n::localize("EDIT"), COLOR_WHITE);

    buttons.push_back(std::make_unique<Button>(
        300, 184, 15, 12, [this]() { return this->setHP(); },
        ui_sheet_button_info_detail_editor_light_idx, "", 0.0f, COLOR_BLACK));

    addOverlay<ViewOverlay>(this->pkm, false);
}

void StatsEditScreen::setIV(pksm::Stat which)
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
        u8 iv = (u8)std::stoi(input);
        pkm.iv(which, std::min(pkm.generation() >= pksm::Generation::THREE ? (u8)31 : (u8)15, iv));
    }
}

bool StatsEditScreen::changeIV(pksm::Stat which, bool up)
{
    if (up)
    {
        if (pkm.generation() >= pksm::Generation::THREE)
        {
            if (pkm.iv(which) < 31)
            {
                pkm.iv(which, pkm.iv(which) + 1);
            }
            else
            {
                pkm.iv(which, 0);
            }
        }
        else
        {
            if (pkm.iv(which) < 15)
            {
                pkm.iv(which, pkm.iv(which) + 1);
            }
            else
            {
                pkm.iv(which, 0);
            }
        }
    }
    else
    {
        if (pkm.iv(which) > 0)
        {
            pkm.iv(which, pkm.iv(which) - 1);
        }
        else
        {
            pkm.iv(which, pkm.generation() >= pksm::Generation::THREE ? 31 : 15);
        }
    }
    return false;
}

void StatsEditScreen::setSecondaryStat(pksm::Stat which)
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NUMPAD, 2, pkm.generation() >= pksm::Generation::THREE ? 3 : 5);
    swkbdSetFeatures(&state, SWKBD_FIXED_WIDTH);
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    char input[6]   = {0};
    SwkbdButton ret = swkbdInputText(&state, input, pkm.generation() >= pksm::Generation::THREE ? 4 : 6);
    input[pkm.generation() >= pksm::Generation::THREE ? 3 : 5] = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        u16 val = (u16)std::min(std::stoi(input), pkm.generation() >= pksm::Generation::THREE ? 252 : 65535);
        if (pkm.generation() != pksm::Generation::LGPE)
        {
            pkm.ev(which, val);
            if (pkm.generation() >= pksm::Generation::THREE)
            {
                u16 total = 0;
                for (int i = 0; i < 6; i++)
                {
                    total += statValues[i] != which ? pkm.ev(statValues[i]) : 0;
                }
                if (total + val > 510)
                {
                    pkm.ev(which, 510 - total);
                }
            }
        }
        else
        {
            static_cast<pksm::PB7&>(pkm).awakened(which, std::min((int)val, 200));
        }
    }
}

bool StatsEditScreen::changeSecondaryStat(pksm::Stat which, bool up)
{
    if (up)
    {
        if (pkm.generation() != pksm::Generation::LGPE)
        {
            if (pkm.generation() >= pksm::Generation::THREE)
            {
                u16 total = 0;
                for (int i = 0; i < 6; i++)
                {
                    total += pkm.ev(statValues[i]);
                }
                // TODO: remove hardcoded value and set it in classes
                if (total < 510 || pkm.ev(which) == 0xFC)
                {
                    if (pkm.ev(which) < 0xFC)
                    {
                        pkm.ev(which, pkm.ev(which) + 1);
                    }
                    else
                    {
                        pkm.ev(which, 0);
                    }
                }
            }
            else
            {
                if (pkm.ev(which) != 65535)
                {
                    pkm.ev(which, pkm.ev(which) + 1);
                }
            }
        }
        else
        {
            pksm::PB7& pb7 = static_cast<pksm::PB7&>(pkm);
            if (pb7.awakened(which) < 200)
            {
                pb7.awakened(which, pb7.awakened(which) + 1);
            }
            else
            {
                pb7.awakened(which, 0);
            }
        }
    }
    else
    {
        if (pkm.generation() != pksm::Generation::LGPE)
        {
            if (pkm.ev(which) > 0)
            {
                pkm.ev(which, pkm.ev(which) - 1);
            }
            else if (pkm.generation() >= pksm::Generation::THREE)
            {
                u16 total = 0xFC;
                for (int i = 0; i < 6; i++)
                {
                    total += pkm.ev(statValues[i]);
                }
                // TODO: remove hardcoded value and set it in classes
                if (total <= 510)
                {
                    pkm.ev(which, 0xFC);
                }
            }
            else
            {
                pkm.ev(which, 65535);
            }
        }
        else
        {
            pksm::PB7& pb7 = static_cast<pksm::PB7&>(pkm);
            if (pb7.awakened(which) > 0)
            {
                pb7.awakened(which, pb7.awakened(which) - 1);
            }
            else
            {
                pb7.awakened(which, 200);
            }
        }
    }
    return false;
}

bool StatsEditScreen::setHP()
{
    addOverlay<HiddenPowerOverlay>(pkm);
    return false;
}

void StatsEditScreen::drawBottom() const
{
    pksm::Language lang = Configuration::getInstance().language();
    Gui::sprite(ui_sheet_emulated_bg_bottom_blue, 0, 0);
    Gui::sprite(ui_sheet_bg_style_bottom_idx, 0, 0);
    Gui::sprite(ui_sheet_bar_arc_bottom_blue_idx, 0, 206);
    Gui::backgroundAnimatedBottom();

    Gui::sprite(ui_sheet_textbox_name_bottom_idx, 0, 1);
    Gui::sprite(ui_sheet_textbox_hidden_power_idx, 57, 177);

    for (int i = 0; i < 4; i++)
    {
        Gui::sprite(ui_sheet_stripe_stats_editor_idx, 0, 30 + i * 40);
    }
    for (int i = 0; i < 6; i++)
    {
        Gui::sprite(ui_sheet_point_small_idx, 92, 58 + i * 20);
        Gui::sprite(ui_sheet_point_small_idx, 168, 58 + i * 20);
        Gui::sprite(ui_sheet_point_big_idx, 252, 57 + i * 20);
    }

    for (const auto& button : buttons)
    {
        button->draw();
    }

    if (pkm.generation() == pksm::Generation::LGPE)
    {
        Gui::text(
            i18n::localize("EDITOR_CP") + std::to_string((int)static_cast<pksm::PB7&>(pkm).CP()), 4,
            5, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    }
    Gui::text(i18n::localize("EDITOR_STATS"), 4, 32, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
        TextPosY::TOP);
    Gui::text(
        i18n::localize("IV"), 132, 32, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    Gui::text(pkm.generation() == pksm::Generation::LGPE ? i18n::localize("AWAKENED")
                                                         : i18n::localize("EV"),
        213, 32, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    Gui::text(i18n::localize("TOTAL"), 274, 32, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER,
        TextPosY::TOP);
    Gui::text(
        i18n::localize("HP"), 4, 52, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(
        i18n::localize("ATTACK"), 4, 72, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(
        i18n::localize("DEFENSE"), 4, 92, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(
        i18n::localize("SPATK"), 4, 112, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(
        i18n::localize("SPDEF"), 4, 132, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(
        i18n::localize("SPEED"), 4, 152, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);

    for (int i = 0; i < 6; i++)
    {
        Gui::text(std::to_string((int)pkm.iv(statValues[i])), 132, 52 + i * 20, FONT_SIZE_12,
            pkm.hyperTrain(statValues[i]) ? COLOR_UNSELECTRED : COLOR_BLACK, TextPosX::CENTER,
            TextPosY::TOP);
        if (pkm.generation() != pksm::Generation::LGPE)
        {
            Gui::text(std::to_string((int)pkm.ev(statValues[i])), 213, 52 + i * 20, FONT_SIZE_12,
                COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
        }
        else
        {
            Gui::text(std::to_string((int)static_cast<pksm::PB7&>(pkm).awakened(statValues[i])),
                213, 52 + i * 20, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
        }
        Gui::text(std::to_string((int)pkm.stat(statValues[i])), 274, 52 + i * 20, FONT_SIZE_12,
            COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    }
    Gui::text(i18n::localize("EDITOR_HIDDEN_POWER") + i18n::type(lang, pkm.hpType()), 295, 181,
        FONT_SIZE_12, COLOR_WHITE, TextPosX::RIGHT, TextPosY::TOP);
}

void StatsEditScreen::update(touchPosition* touch)
{
    u32 downKeys = keysDown();

    for (size_t i = 0; i < buttons.size(); i++)
    {
        if (buttons[i]->update(touch))
        {
            return;
        }
    }

    if (downKeys & KEY_B)
    {
        Gui::screenBack();
        return;
    }
}
