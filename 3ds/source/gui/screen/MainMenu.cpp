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

#include "MainMenu.hpp"
#include "BagScreen.hpp"
#include "ClickButton.hpp"
#include "ConfigScreen.hpp"
#include "Configuration.hpp"
#include "EditSelectorScreen.hpp"
#include "InjectSelectorScreen.hpp"
#include "MainMenuButton.hpp"
#include "ScriptScreen.hpp"
#include "StorageScreen.hpp"
#include "format.h"
#include "gui.hpp"
#include "loader.hpp"
#include "revision.h"
#include "sav/Sav5.hpp"
#include "utils/crypto.hpp"

namespace
{
    bool goToScreen(int buttonNum)
    {
        switch (buttonNum)
        {
            case 0:
                if (TitleLoader::save->partyCount() < 1)
                {
                    Gui::warn(
                        i18n::localize("NEED_ONE_POKEMON") + '\n' + i18n::localize("GET_STARTER"));
                    return false;
                }
                Gui::setScreen(std::make_unique<StorageScreen>());
                return true;
            case 1:
                if (TitleLoader::save->partyCount() < 1)
                {
                    Gui::warn(
                        i18n::localize("NEED_ONE_POKEMON") + '\n' + i18n::localize("GET_STARTER"));
                    return false;
                }
                Gui::setScreen(std::make_unique<EditSelectorScreen>());
                return true;
            case 2:
                if (TitleLoader::save->generation() >= pksm::Generation::LGPE ||
                    TitleLoader::save->generation() <= pksm::Generation::THREE)
                {
                    Gui::warn(
                        i18n::localize("NO_WONDERCARDS") + '\n' + i18n::localize("USE_INJECTOR_C"));
                    return false;
                }
                Gui::setScreen(std::make_unique<InjectSelectorScreen>());
                return true;
            case 3:
                Gui::setScreen(std::make_unique<ScriptScreen>());
                return true;
            case 4:
                Gui::setScreen(std::make_unique<BagScreen>());
                return true;
            case 5:
                Gui::setScreen(std::make_unique<ConfigScreen>());
                return true;
        }
        return true;
    }
}

MainMenu::MainMenu() : Screen(i18n::localize("X_SAVE"))
{
    oldLang = Configuration::getInstance().language();
    if (TitleLoader::save->generation() == pksm::Generation::FIVE)
    {
        ((pksm::Sav5*)TitleLoader::save.get())->cryptMysteryGiftData();
    }
    oldHash =
        pksm::crypto::sha256(TitleLoader::save->rawData().get(), TitleLoader::save->getLength());
    makeButtons();
    makeInstructions();
}

MainMenu::~MainMenu() {}

void MainMenu::makeButtons()
{
    buttons[0] = std::make_unique<MainMenuButton>(15, 20, 140, 53, []() { return goToScreen(0); },
        ui_sheet_icon_storage_idx, i18n::localize("STORAGE"), FONT_SIZE_15, COLOR_WHITE, 27);
    buttons[1] = std::make_unique<MainMenuButton>(165, 20, 140, 53, []() { return goToScreen(1); },
        ui_sheet_icon_editor_idx, i18n::localize("EDITOR"), FONT_SIZE_15, COLOR_WHITE, 28);
    buttons[2] = std::make_unique<MainMenuButton>(15, 83, 140, 53, []() { return goToScreen(2); },
        ui_sheet_icon_events_idx, i18n::localize("EVENTS"), FONT_SIZE_15, COLOR_WHITE, 93);
    buttons[3] = std::make_unique<MainMenuButton>(165, 83, 140, 53, []() { return goToScreen(3); },
        ui_sheet_icon_scripts_idx, i18n::localize("SCRIPTS"), FONT_SIZE_15, COLOR_WHITE, 91);
    buttons[4] = std::make_unique<MainMenuButton>(15, 146, 140, 53, []() { return goToScreen(4); },
        ui_sheet_icon_bag_idx, i18n::localize("BAG"), FONT_SIZE_15, COLOR_WHITE, 157);
    buttons[5] = std::make_unique<MainMenuButton>(165, 146, 140, 53, []() { return goToScreen(5); },
        ui_sheet_icon_settings_idx, i18n::localize("SETTINGS"), FONT_SIZE_15, COLOR_WHITE, 160);
    buttons[6] = std::make_unique<ClickButton>(289, 211, 28, 28,
        [this]() {
            if (needsSave())
            {
                save();
            }
            return false;
        },
        ui_sheet_button_save_idx, "", 0, COLOR_BLACK);
}

void MainMenu::makeInstructions()
{
    instructions = Instructions(i18n::localize("B_BACK"));
    instructions.addBox(
        false, 200, 218, 60, 14, COLOR_GREY, i18n::localize("EDITOR_SAVE"), COLOR_WHITE);
    instructions.addLine(false, 260, 225, 303, 225, 4, COLOR_GREY);
    instructions.addCircle(false, 303, 225, 4, COLOR_GREY);
}

void MainMenu::drawTop() const
{
    Gui::sprite(ui_sheet_emulated_bg_top_blue, 0, 0);
    Gui::sprite(ui_sheet_bg_style_top_idx, 0, 0);
    Gui::sprite(ui_sheet_bar_arc_top_blue_idx, 0, 0);
    Gui::backgroundAnimatedTop();
    Gui::sprite(ui_sheet_textbox_hidden_power_idx, 137, 3);

    for (int y = 34; y < 156; y += 40)
    {
        Gui::sprite(ui_sheet_stripe_info_row_idx, 0, y);
    }

    for (int y = 40; y < 180; y += 20)
    {
        Gui::sprite(ui_sheet_point_big_idx, 1, y);
    }

    for (int i = 0; i < 7; i++)
    {
        int y = 36 + i * 20;
        switch (i)
        {
            case 0:
                Gui::text(fmt::format(i18n::localize("GENERATION"),
                              (std::string)TitleLoader::save->generation()),
                    10, y, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
                break;
            case 1:
                Gui::text(fmt::format(i18n::localize("TRAINER_NAME"), TitleLoader::save->otName()),
                    10, y, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
                break;
            case 2:
                Gui::text(i18n::localize("TID_SID") + ": " +
                              std::to_string(TitleLoader::save->displayTID()) + "/" +
                              std::to_string(TitleLoader::save->displaySID()),
                    10, y, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
                break;
            case 3:
                Gui::text(fmt::format(TitleLoader::save->generation() == pksm::Generation::SEVEN
                                          ? i18n::localize("STAMPS")
                                          : i18n::localize("BADGES"),
                              TitleLoader::save->badges()),
                    10, y, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
                break;
            case 4:
                Gui::text(
                    fmt::format(i18n::localize("WC_NUM"), TitleLoader::save->currentGiftAmount()),
                    10, y, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
                break;
            case 5:
                Gui::text(fmt::format(i18n::localize("DEX_SEEN"), TitleLoader::save->dexSeen()), 10,
                    y, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
                break;
            case 6:
                Gui::text(fmt::format(i18n::localize("DEX_CAUGHT"), TitleLoader::save->dexCaught()),
                    10, y, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
                break;
            default:
                break;
        }
    }

    const std::string version = fmt::format(
        FMT_STRING("v{:d}.{:d}.{:d}-{:s}"), VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO, GIT_REV);
    Gui::text("PKSM", 282, 16, FONT_SIZE_14, COLOR_WHITE, TextPosX::RIGHT, TextPosY::CENTER);
    Gui::text(version, 398, 17, FONT_SIZE_11, COLOR_LIGHTBLUE, TextPosX::RIGHT, TextPosY::CENTER);
}

void MainMenu::drawBottom() const
{
    Gui::sprite(ui_sheet_emulated_bg_bottom_blue, 0, 0);
    Gui::sprite(ui_sheet_bg_style_bottom_idx, 0, 0);
    Gui::sprite(ui_sheet_bar_arc_bottom_blue_idx, 0, 206);
    Gui::backgroundAnimatedBottom();
    for (auto& button : buttons)
    {
        button->draw();
    }
}

void MainMenu::update(touchPosition* touch)
{
    if (Configuration::getInstance().language() != oldLang)
    {
        oldLang = Configuration::getInstance().language();
        makeButtons();
        makeInstructions();
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
    for (auto& button : buttons)
    {
        if (button->update(touch))
            return;
    }
    if (keysDown() & KEY_B)
    {
        if (!needsSave() || Gui::showChoiceMessage(i18n::localize("EDITOR_CHECK_EXIT")))
        {
            setLoadedSaveFromBridge(false);
            Gui::screenBack();
            return;
        }
    }
    else if (keysDown() & KEY_X)
    {
        if (needsSave())
        {
            save();
        }
    }
}

bool MainMenu::needsSave()
{
    auto newHash =
        pksm::crypto::sha256(TitleLoader::save->rawData().get(), TitleLoader::save->getLength());
    if (newHash != oldHash)
    {
        return true;
    }
    return false;
}

void MainMenu::save()
{
    if (isLoadedSaveFromBridge())
    {
        if (Gui::showChoiceMessage(i18n::localize("BRIDGE_SHOULD_SEND_1") + '\n' +
                                   i18n::localize("BRIDGE_SHOULD_SEND_2")))
        {
            if (TitleLoader::save->generation() == pksm::Generation::FIVE)
            {
                ((pksm::Sav5*)TitleLoader::save.get())->cryptMysteryGiftData();
            }
            TitleLoader::save->finishEditing();
            bool sent = sendSaveToBridge();
            if (!sent)
            {
                // save a copy of the modified save to the SD card
                backupBridgeChanges();
            }
            TitleLoader::save->beginEditing();
            if (TitleLoader::save->generation() == pksm::Generation::FIVE)
            {
                ((pksm::Sav5*)TitleLoader::save.get())->cryptMysteryGiftData();
            }
        }
    }
    else
    {
        Gui::waitFrame(i18n::localize("SAVING"));
        if (TitleLoader::save->generation() == pksm::Generation::FIVE)
        {
            ((pksm::Sav5*)TitleLoader::save.get())->cryptMysteryGiftData();
        }
        TitleLoader::saveChanges();
        if (TitleLoader::save->generation() == pksm::Generation::FIVE)
        {
            ((pksm::Sav5*)TitleLoader::save.get())->cryptMysteryGiftData();
        }
    }
    oldHash =
        pksm::crypto::sha256(TitleLoader::save->rawData().get(), TitleLoader::save->getLength());
}
