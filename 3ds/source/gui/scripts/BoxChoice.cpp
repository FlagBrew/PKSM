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

#include "BoxChoice.hpp"
#include "AccelButton.hpp"
#include "ClickButton.hpp"
#include "Configuration.hpp"
#include "TitleLoadScreen.hpp"
#include "ViewOverlay.hpp"
#include "banks.hpp"
#include "format.h"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "loader.hpp"
#include "pkx/PKX.hpp"
#include "sav/Sav.hpp"
#include "utils/random.hpp"

BoxChoice::BoxChoice(bool doCrypt) : RunnableScreen(std::make_tuple(0, -1, -1)), doCrypt(doCrypt)
{
    mainButtons[0] = std::make_unique<Button>(
        212, 47, 108, 28, [this]() { return this->showViewer(); }, ui_sheet_button_editor_idx,
        i18n::localize("VIEW"), FONT_SIZE_12, COLOR_BLACK);
    mainButtons[1] = std::make_unique<Button>(
        4, 212, 33, 28, [this]() { return false; }, ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK);
    mainButtons[2] = std::make_unique<Button>(
        283, 211, 34, 28, [this]() { return this->backButton(); }, ui_sheet_button_back_idx, "",
        0.0f, COLOR_BLACK);
    mainButtons[3] = std::make_unique<AccelButton>(
        8, 15, 17, 24, [this]() { return this->prevBox(true); }, ui_sheet_res_null_idx, "", 0.0f,
        COLOR_BLACK, 10, 5);
    mainButtons[4] = std::make_unique<AccelButton>(
        189, 15, 17, 24, [this]() { return this->nextBox(true); }, ui_sheet_res_null_idx, "", 0.0f,
        COLOR_BLACK, 10, 5);

    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO && TitleLoader::save->language() != pksm::Language::JPN) ? 20 : 30;

    // Pokemon buttons
    u16 y = 45;
    for (u8 row = 0; row < 5; row++)
    {
        u16 x = 4;
        if (maxPkmInBox == 20)
        {
            x += 34;
        }
        for (u8 column = 0; column < (maxPkmInBox / 5); column++)
        {
            clickButtons[row * 6 + column] = std::make_unique<ClickButton>(
                x, y, 34, 30,
                [this, row, column]() { return this->clickBottomIndex(row * 6 + column + 1); },
                ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK);
            x += 34;
        }
        y += 30;
    }

    if (doCrypt)
    {
        TitleLoader::save->cryptBoxData(true);
    }
}

BoxChoice::~BoxChoice()
{
    if (doCrypt)
    {
        TitleLoader::save->cryptBoxData(false);
    }
}

void BoxChoice::drawBottom() const
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO && TitleLoader::save->language() != pksm::Language::JPN) ? 20 : 30;

    Gui::sprite(ui_sheet_emulated_bg_bottom_green, 0, 0);
    Gui::sprite(ui_sheet_bg_style_bottom_idx, 0, 0);
    Gui::sprite(ui_sheet_bar_arc_bottom_green_idx, 0, 206);

    Gui::sprite(ui_sheet_bar_boxname_with_arrows_idx, 7, 15);
    Gui::sprite(ui_sheet_storage_box_corner_idx, maxPkmInBox == 20 ? 36 : 2, 44);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_horizontal_idx, maxPkmInBox == 20 ? 168 : 202, 44);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_vertical_idx, maxPkmInBox == 20 ? 36 : 2, 193);
    Gui::sprite(ui_sheet_emulated_storage_box_corner_flipped_both_idx, maxPkmInBox == 20 ? 168 : 202, 193);
    for (const auto& b : mainButtons)
    {
        b->draw();
    }

    u16 y = 45;
    for (u8 row = 0; row < 5; row++)
    {
        u16 x = 4;
        if (maxPkmInBox == 20)
        {
            x += 34;
        }
        for (u8 column = 0; column < (maxPkmInBox / 5); column++)
        {
            if (TitleLoader::save->generation() == pksm::Generation::LGPE &&
                row * 6 + column + boxBox * 30 >= TitleLoader::save->maxSlot())
            {
                Gui::drawSolidRect(x, y, 34, 30, PKSM_Color(128, 128, 128, 128));
            }
            else
            {
                std::shared_ptr<pksm::PKX> pokemon =
                    TitleLoader::save->pkm(boxBox, row * (maxPkmInBox / 5) + column);
                if (pokemon->species() != pksm::Species::None)
                {
                    Gui::pkm(*pokemon, x, y);
                }
            }
            x += 34;
        }
        y += 30;
    }

    std::string show = TitleLoader::save->boxName(boxBox);
    if (show.empty())
    {
        show = i18n::localize("BOX") + std::to_string(boxBox);
    }
    Gui::text(show, 107, 18, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);

    if (!storageChosen)
    {
        if (cursorIndex == 0)
        {
            int dy = Gui::pointerBob();
            Gui::sprite(ui_sheet_pointer_arrow_idx, 106, -4 + dy);
        }
        else
        {
            int tempIndex = cursorIndex - 1;
            int yMod      = (tempIndex / (maxPkmInBox / 5)) * maxPkmInBox + Gui::pointerBob();
            Gui::sprite(ui_sheet_pointer_arrow_idx, (maxPkmInBox == 20 ? 55 : 21) + (tempIndex % (maxPkmInBox / 5)) * 34, 30 + yMod);
        }
    }

    if (overlay)
    {
        Gui::drawSolidRect(0, 0, 320, 240, COLOR_MASKBLACK);
    }
}

void BoxChoice::drawTop() const
{
    if (!overlay)
    {
        Gui::sprite(ui_sheet_emulated_bg_top_green, 0, 0);
        Gui::sprite(ui_sheet_bg_style_top_idx, 0, 0);
        Gui::backgroundAnimatedTop();
        Gui::sprite(ui_sheet_bar_arc_top_green_idx, 0, 0);

        Gui::sprite(ui_sheet_textbox_pksm_idx, 261, 3);
        Gui::text("PKSM", 394, 7, FONT_SIZE_14, COLOR_WHITE, TextPosX::RIGHT, TextPosY::TOP);

        Gui::sprite(ui_sheet_bar_boxname_empty_idx, 44, 21);
        Gui::text(
            "\uE004", 45 + 24 / 2, 24, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
        Gui::text(
            "\uE005", 225 + 24 / 2, 24, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
        Gui::text(Banks::bank->boxName(storageBox), 69 + 156 / 2, 24, FONT_SIZE_14, COLOR_BLACK,
            TextPosX::CENTER, TextPosY::TOP);

        Gui::sprite(ui_sheet_storagemenu_cross_idx, 36, 50);
        Gui::sprite(ui_sheet_storagemenu_cross_idx, 246, 50);
        Gui::sprite(ui_sheet_storagemenu_cross_idx, 36, 220);
        Gui::sprite(ui_sheet_storagemenu_cross_idx, 246, 220);

        u16 y = 66;
        for (u8 row = 0; row < 5; row++)
        {
            u16 x = 45;
            for (u8 column = 0; column < 6; column++)
            {
                auto pkm = Banks::bank->pkm(storageBox, row * 6 + column);
                if (pkm->species() != pksm::Species::None)
                {
                    Gui::pkm(*pkm, x, y);
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

        if (storageChosen)
        {
            if (cursorIndex == 0)
            {
                int dy = Gui::pointerBob();
                Gui::sprite(ui_sheet_pointer_arrow_idx, 147, 2 + dy);
            }
            else
            {
                int tempIndex = cursorIndex - 1;
                int yMod      = (tempIndex / 6) * 30 + Gui::pointerBob();
                Gui::sprite(ui_sheet_pointer_arrow_idx, 62 + (tempIndex % 6) * 34, 51 + yMod);
            }
        }

        if (infoMon)
        {
            Gui::text(infoMon->nickname(), 276, 61, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
                TextPosY::TOP);
            std::string info = "#" + std::to_string(int(infoMon->species()));
            Gui::text(info, 273, 77, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            info      = i18n::localize("LV") + std::to_string(infoMon->level());
            auto text = Gui::parseText(info, FONT_SIZE_12, 0.0f);
            int width = text->maxWidth(FONT_SIZE_12);
            Gui::text(text, 375 - width, 77, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK,
                TextPosX::LEFT, TextPosY::TOP);
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
                   std::to_string(infoMon->TID());
            Gui::text(info, 276, 141, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);

            Gui::text(infoMon->nature().localize(Configuration::getInstance().language()), 276, 181,
                FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            info  = i18n::localize("IV") + ": ";
            text  = Gui::parseText(info, FONT_SIZE_12, 0.0f);
            width = text->maxWidth(FONT_SIZE_12);
            Gui::text(text, 276, 197, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
                TextPosY::TOP);
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
}

void BoxChoice::update(touchPosition* touch)
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO && TitleLoader::save->language() != pksm::Language::JPN) ? 20 : 30;

    if (cursorIndex != 0)
    {
        infoMon = storageChosen ? Banks::bank->pkm(storageBox, cursorIndex - 1)
                                : TitleLoader::save->pkm(boxBox, cursorIndex - 1);
    }
    else
    {
        infoMon = nullptr;
    }
    if (infoMon && infoMon->species() == pksm::Species::None)
    {
        infoMon = nullptr;
    }
    if (justSwitched)
    {
        if (keysHeld() & KEY_TOUCH)
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
    if (!overlay)
    {
        for (size_t i = 0; i < mainButtons.size(); i++)
        {
            if (mainButtons[i]->update(touch))
                return;
        }
        backHeld = false;
        for (auto& button : clickButtons)
        {
            if (button)
            {
                if (button->update(touch))
                    return;
            }
        }

        if (kDown & KEY_A)
        {
            if (cursorIndex != 0)
            {
                finalValue = std::make_tuple(
                    storageChosen ? 1 : 0, storageChosen ? storageBox : boxBox, cursorIndex);
                done = true;
            }
        }
        else if (kDown & KEY_B)
        {
            backButton();
            return;
        }
        else if (kDown & KEY_X)
        {
            showViewer();
            return;
        }

        if (kRepeat & KEY_LEFT)
        {
            if (cursorIndex == 0)
            {
                prevBox();
            }
            else if (cursorIndex > 1)
            {
                cursorIndex--;
            }
            else if (cursorIndex == 1)
            {
                prevBox();
                cursorIndex = storageChosen ? 30 : maxPkmInBox;
            }
        }
        else if (kRepeat & KEY_RIGHT)
        {
            if (cursorIndex == 0)
            {
                nextBox();
            }
            else if (cursorIndex < (storageChosen ? 30 : maxPkmInBox))
            {
                cursorIndex++;
            }
            else if (cursorIndex == (storageChosen ? 30 : maxPkmInBox))
            {
                nextBox();
                cursorIndex = 1;
            }
        }
        else if (kRepeat & KEY_UP)
        {
            if (cursorIndex == 0 && !storageChosen)
            {
                storageChosen = true;
                cursorIndex   = 27;
            }
            else if (cursorIndex > 0 && cursorIndex <= (storageChosen ? 6 : (maxPkmInBox / 5)))
            {
                cursorIndex = 0;
            }
            else if (cursorIndex > (storageChosen ? 6 : (maxPkmInBox / 5)))
            {
                cursorIndex -= (storageChosen ? 6 : (maxPkmInBox / 5));
            }
        }
        else if (kRepeat & KEY_DOWN)
        {
            if (cursorIndex >= 25 && storageChosen)
            {
                storageChosen = false;
                cursorIndex   = 0;
            }
            else if (cursorIndex == 0)
            {
                cursorIndex = ((storageChosen || (maxPkmInBox == 30)) ? 3 : 2);
            }
            else if (cursorIndex < (storageChosen ? 25 : (((maxPkmInBox / 5) * 4) + 1)))
            {
                cursorIndex += (storageChosen ? 6 : (maxPkmInBox / 5));
            }
        }
        else if (kRepeat & KEY_R)
        {
            nextBox();
        }
        else if (kRepeat & KEY_L)
        {
            prevBox();
        }
    }
    else
    {
        if (kDown & KEY_B)
        {
            backButton();
        }
    }
}

bool BoxChoice::prevBox(bool forceBottom)
{
    if (storageChosen && !forceBottom)
    {
        storageBox--;
        if (storageBox == -1)
        {
            storageBox = Banks::bank->boxes() - 1;
        }
    }
    else
    {
        boxBox--;
        if (boxBox == -1)
        {
            boxBox = TitleLoader::save->maxBoxes() - 1;
        }
    }
    return false;
}

bool BoxChoice::nextBox(bool forceBottom)
{
    if (storageChosen && !forceBottom)
    {
        storageBox++;
        if (storageBox == Banks::bank->boxes())
        {
            storageBox = 0;
        }
    }
    else
    {
        boxBox++;
        if (boxBox == TitleLoader::save->maxBoxes())
        {
            boxBox = 0;
        }
    }
    return false;
}

bool BoxChoice::backButton()
{
    if (!backHeld)
    {
        backHeld = true;
        if (overlay)
        {
            removeOverlays();
        }
        else
        {
            finalValue = std::make_tuple(0, -1, -1);
            done       = true;
        }
    }
    return true;
}

bool BoxChoice::showViewer()
{
    u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO && TitleLoader::save->language() != pksm::Language::JPN) ? 20 : 30;

    if (cursorIndex == 0 ||
        (!storageChosen && boxBox * maxPkmInBox + cursorIndex - 1 >= TitleLoader::save->maxSlot()))
    {
        return false;
    }

    if (infoMon && infoMon->species() != pksm::Species::None)
    {
        addOverlay<ViewOverlay>(infoMon, true);
    }
    return true;
}

bool BoxChoice::clickBottomIndex(int index)
{
    if (cursorIndex == index && !storageChosen)
    {
        if (cursorIndex != 0)
        {
            finalValue = std::make_tuple(
                storageChosen ? 1 : 0, storageChosen ? storageBox : boxBox, cursorIndex);
            done = true;
        }
    }
    else
    {
        cursorIndex   = index;
        storageChosen = false;
    }
    return false;
}
