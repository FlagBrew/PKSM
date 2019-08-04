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

#include "EditorScreen.hpp"
#include "AccelButton.hpp"
#include "BallOverlay.hpp"
#include "ClickButton.hpp"
#include "Configuration.hpp"
#include "FormOverlay.hpp"
#include "HexEditScreen.hpp"
#include "MiscEditScreen.hpp"
#include "MoveEditScreen.hpp"
#include "NatureOverlay.hpp"
#include "PB7.hpp"
#include "PkmItemOverlay.hpp"
#include "SpeciesOverlay.hpp"
#include "StatsEditScreen.hpp"
#include "banks.hpp"
#include "gui.hpp"
#include "loader.hpp"
#include "random.hpp"

#define NO_TEXT_BUTTON(x, y, w, h, function, image) std::make_unique<Button>(x, y, w, h, function, image, "", 0.0f, 0)
#define NO_TEXT_ACCEL(x, y, w, h, function, image) std::make_unique<AccelButton>(x, y, w, h, function, image, "", 0.0f, 0)
#define NO_TEXT_CLICK(x, y, w, h, function, image) std::make_unique<ClickButton>(x, y, w, h, function, image, "", 0.0f, 0)

static constexpr int statValues[] = {0, 1, 2, 4, 5, 3};

EditorScreen::EditorScreen(std::shared_ptr<PKX> pokemon, int box, int index, bool emergency)
    : pkm(pokemon), box(box), index(index), emergency(emergency)
{
    if (!pkm || (pkm->encryptionConstant() == 0 && pkm->species() == 0))
    {
        pkm = TitleLoader::save->emptyPkm();
        if (Configuration::getInstance().useSaveInfo())
        {
            pkm->TID(TitleLoader::save->TID());
            pkm->SID(TitleLoader::save->SID());
            pkm->otName(TitleLoader::save->otName());
            pkm->otGender(TitleLoader::save->gender());
        }
        else
        {
            pkm->TID(Configuration::getInstance().defaultTID());
            pkm->SID(Configuration::getInstance().defaultSID());
            switch (pkm->generation())
            {
                case Generation::FOUR:
                case Generation::FIVE:
                default:
                    pkm->otName(Configuration::getInstance().defaultOT().substr(0, 7));
                    break;
                case Generation::SIX:
                case Generation::SEVEN:
                    pkm->otName(Configuration::getInstance().defaultOT());
                    break;
            }
        }
        pkm->ball(4);
        pkm->encryptionConstant((((u32)randomNumbers()) % 0xFFFFFFFF) + 1);
        pkm->version(TitleLoader::save->version());
        switch (pkm->version())
        {
            case 7:
            case 8:
                pkm->metLocation(0x0095); // Route 1, HGSS
                break;
            case 10:
            case 11:
            case 12:
                pkm->metLocation(0x0010); // Route 201, DPPt
                break;
            case 20:
            case 21:
            case 22:
            case 23:
                pkm->metLocation(0x000e); // Route 1, BWB2W2
                break;
            case 24:
            case 25:
                pkm->metLocation(0x0008); // Route 1, XY
                break;
            case 26:
            case 27:
                pkm->metLocation(0x00cc); // Route 101, ORAS
                break;
            case 30:
            case 31:
            case 32:
            case 33:
                pkm->metLocation(0x0006); // Route 1, SMUSUM
                break;
            case 42:
            case 43:
                pkm->metLocation(0x0003); // Route 1, LGPE
                break;
        }
        pkm->fixMoves();
        // pkm->PID((u32)randomNumbers());
        pkm->language(Configuration::getInstance().language());
        const time_t current = time(NULL);
        pkm->metDay(Configuration::getInstance().day() ? Configuration::getInstance().day() : gmtime(&current)->tm_mday);
        pkm->metMonth(Configuration::getInstance().month() ? Configuration::getInstance().month() : gmtime(&current)->tm_mon);
        pkm->metYear(Configuration::getInstance().year() ? Configuration::getInstance().year() - 2000 : gmtime(&current)->tm_year - 2000);
        pkm->metLevel(1);
        if (pkm->generation() == Generation::SIX)
        {
            ((PK6*)pkm.get())->consoleRegion(Configuration::getInstance().nationality());
            ((PK6*)pkm.get())->geoCountry(0, Configuration::getInstance().defaultCountry());
            ((PK6*)pkm.get())->geoRegion(0, Configuration::getInstance().defaultRegion());
            ((PK6*)pkm.get())->country(Configuration::getInstance().defaultCountry());
            ((PK6*)pkm.get())->region(Configuration::getInstance().defaultRegion());
        }
        else if (pkm->generation() == Generation::SEVEN)
        {
            ((PK7*)pkm.get())->consoleRegion(Configuration::getInstance().nationality());
            ((PK7*)pkm.get())->geoCountry(0, Configuration::getInstance().defaultCountry());
            ((PK7*)pkm.get())->geoRegion(0, Configuration::getInstance().defaultRegion());
            ((PK7*)pkm.get())->country(Configuration::getInstance().defaultCountry());
            ((PK7*)pkm.get())->region(Configuration::getInstance().defaultRegion());
        }
        // if (pkm->generation() == Generation::LGPE)
        // {
        //     ((PB7*)pkm.get())->consoleRegion(Configuration::getInstance().nationality());
        //     ((PB7*)pkm.get())->geoCountry(0, Configuration::getInstance().defaultCountry());
        //     ((PB7*)pkm.get())->geoRegion(0, Configuration::getInstance().defaultRegion());
        // }
        currentOverlay = std::make_unique<SpeciesOverlay>(*this, pkm);
    }

    if (this->box == 0xFF)
    {
        switch (pkm->generation())
        {
            case Generation::FOUR:
                if (pkm->getLength() == 136)
                {
                    u8 pkmData[236] = {0};
                    std::copy(pkm->rawData(), pkm->rawData() + pkm->getLength(), pkmData);
                    pkm = std::make_shared<PK4>(pkmData, false, true);
                    partyUpdate();
                    currentOverlay = std::make_unique<SpeciesOverlay>(*this, pkm);
                }
                break;
            case Generation::FIVE:
                if (pkm->getLength() == 136)
                {
                    u8 pkmData[220] = {0};
                    std::copy(pkm->rawData(), pkm->rawData() + pkm->getLength(), pkmData);
                    pkm = std::make_shared<PK5>(pkmData, false, true);
                    partyUpdate();
                    currentOverlay = std::make_unique<SpeciesOverlay>(*this, pkm);
                }
                break;
            case Generation::SIX:
            case Generation::SEVEN:
                if (pkm->getLength() == 232)
                {
                    u8 pkmData[260] = {0};
                    std::copy(pkm->rawData(), pkm->rawData() + pkm->getLength(), pkmData);
                    if (pkm->generation() == Generation::SIX)
                    {
                        pkm = std::make_shared<PK6>(pkmData, false, true);
                    }
                    else
                    {
                        pkm = std::make_shared<PK7>(pkmData, false, true);
                    }
                    partyUpdate();
                    currentOverlay = std::make_unique<SpeciesOverlay>(*this, pkm);
                }
                break;
            case Generation::LGPE:
                break; // Always a party Pokemon
            default:
                Gui::warn(i18n::localize("THE_FUCK"));
        }

        for (int i = 0; i < 6; i++)
        {
            origPartyStats[i] = pkm->partyStat(i);
        }
        origPartyLevel  = pkm->partyLevel();
        origPartyCurrHP = pkm->partyCurrHP();
        if (pkm->generation() == Generation::LGPE)
        {
            origPartyCP = ((PB7*)pkm.get())->partyCP();
        }
    }

    buttons.push_back(NO_TEXT_CLICK(9, 211, 34, 28, [this]() { return this->goBack(); }, ui_sheet_button_back_idx));
    instructions.addCircle(false, 12, 11, 4, COLOR_GREY);
    instructions.addBox(false, 10, 11, 4, 32, COLOR_GREY);
    instructions.addBox(false, 10, 43, 50, 16, COLOR_GREY, i18n::localize("BALL"), COLOR_WHITE);
    buttons.push_back(NO_TEXT_BUTTON(4, 3, 20, 19, [this]() { return this->selectBall(); }, ui_sheet_res_null_idx));
    instructions.addBox(false, 224, 33, 60, 68, COLOR_GREY, i18n::localize("CHANGE_FORM"), COLOR_WHITE);
    buttons.push_back(NO_TEXT_BUTTON(224, 33, 60, 68, [this]() { return this->selectForm(); }, ui_sheet_res_null_idx));
    instructions.addCircle(false, 305, 14, 11, COLOR_GREY);
    instructions.addBox(false, 303, 14, 4, 92, COLOR_GREY);
    instructions.addBox(false, 207, 106, 100, 16, COLOR_GREY, i18n::localize("HEX_EDIT"), COLOR_WHITE);
    buttons.push_back(NO_TEXT_BUTTON(291, 2, 27, 23, [this]() { return this->hexEdit(); }, ui_sheet_icon_hex_idx));
    buttons.push_back(NO_TEXT_ACCEL(94, 34, 13, 13, [this]() { return this->changeLevel(false); }, ui_sheet_button_minus_small_idx));
    buttons.push_back(NO_TEXT_BUTTON(109, 34, 31, 13,
        [this]() {
            Gui::setNextKeyboardFunc([this]() { setLevel(); });
            return false;
        },
        ui_sheet_res_null_idx));
    buttons.push_back(NO_TEXT_ACCEL(142, 34, 13, 13, [this]() { return this->changeLevel(true); }, ui_sheet_button_plus_small_idx));
    buttons.push_back(NO_TEXT_BUTTON(75, 54, 15, 12, [this]() { return this->selectNature(); }, ui_sheet_button_info_detail_editor_dark_idx));
    buttons.push_back(NO_TEXT_CLICK(75, 74, 15, 12, [this]() { return this->selectAbility(); }, ui_sheet_button_info_detail_editor_dark_idx));
    buttons.push_back(NO_TEXT_BUTTON(75, 94, 15, 12, [this]() { return this->selectItem(); }, ui_sheet_button_info_detail_editor_dark_idx));
    buttons.push_back(NO_TEXT_CLICK(75, 114, 15, 12,
        [this]() {
            pkm->shiny(!pkm->shiny());
            return false;
        },
        ui_sheet_button_info_detail_editor_dark_idx));
    buttons.push_back(NO_TEXT_CLICK(75, 134, 15, 12, [this]() { return this->togglePokerus(); }, ui_sheet_button_info_detail_editor_dark_idx));
    buttons.push_back(NO_TEXT_BUTTON(75, 154, 15, 12,
        [this]() {
            Gui::setNextKeyboardFunc([this]() { return this->setOT(); });
            return false;
        },
        ui_sheet_button_info_detail_editor_dark_idx));
    buttons.push_back(NO_TEXT_BUTTON(75, 174, 15, 12,
        [this]() {
            Gui::setNextKeyboardFunc([this]() { return this->setNick(); });
            return false;
        },
        ui_sheet_button_info_detail_editor_dark_idx));
    buttons.push_back(NO_TEXT_ACCEL(94, 194, 13, 13, [this]() { return this->changeFriendship(false); }, ui_sheet_button_minus_small_idx));
    buttons.push_back(NO_TEXT_BUTTON(109, 194, 31, 13,
        [this]() {
            Gui::setNextKeyboardFunc([this]() { return this->setFriendship(); });
            return false;
        },
        ui_sheet_res_null_idx));
    buttons.push_back(NO_TEXT_ACCEL(142, 194, 13, 13, [this]() { return this->changeFriendship(true); }, ui_sheet_button_plus_small_idx));
    buttons.push_back(std::make_unique<Button>(204, 109, 108, 30,
        [this]() {
            Gui::setScreen(std::make_unique<StatsEditScreen>(pkm));
            justSwitched = true;
            return true;
        },
        ui_sheet_button_editor_idx, i18n::localize("EDITOR_STATS"), FONT_SIZE_12, COLOR_BLACK));
    buttons.push_back(std::make_unique<Button>(204, 140, 108, 30,
        [this]() {
            Gui::setScreen(std::make_unique<MoveEditScreen>(pkm));
            justSwitched = true;
            return true;
        },
        ui_sheet_button_editor_idx, i18n::localize("EDITOR_MOVES"), FONT_SIZE_12, COLOR_BLACK));
    buttons.push_back(std::make_unique<Button>(204, 171, 108, 30,
        [this]() {
            Gui::setScreen(std::make_unique<MiscEditScreen>(pkm));
            justSwitched = true;
            return true;
        },
        ui_sheet_button_editor_idx, i18n::localize("EDITOR_MISC"), FONT_SIZE_12, COLOR_BLACK));
    buttons.push_back(std::make_unique<ClickButton>(204, 202, 108, 30,
        [this]() {
            this->save();
            this->goBack();
            return true;
        },
        ui_sheet_button_editor_idx, i18n::localize("EDITOR_SAVE"), FONT_SIZE_12, COLOR_BLACK));
    instructions.addBox(false, 25, 5, 120, 15, COLOR_GREY, i18n::localize("CHANGE_SPECIES"), COLOR_WHITE);
    buttons.push_back(NO_TEXT_BUTTON(25, 5, 120, 13, [this]() { return this->selectSpecies(); }, ui_sheet_res_null_idx));
    instructions.addCircle(false, 192, 13, 6, COLOR_GREY);
    instructions.addBox(false, 190, 11, 4, 32, COLOR_GREY);
    instructions.addBox(false, 124, 43, 70, 16, COLOR_GREY, i18n::localize("GENDER"), COLOR_WHITE);
    buttons.push_back(NO_TEXT_CLICK(186, 7, 12, 12, [this]() { return this->genderSwitch(); }, ui_sheet_res_null_idx));
    instructions.addCircle(false, 260, 14, 11, COLOR_GREY);
    instructions.addBox(false, 214, 12, 46, 4, COLOR_GREY);
    instructions.addBox(false, 214, 16, 4, 48, COLOR_GREY);
    instructions.addBox(false, 98, 64, 120, 16, COLOR_GREY, i18n::localize("SET_SAVE_INFO"), COLOR_WHITE);
    buttons.push_back(NO_TEXT_CLICK(239, 3, 43, 22, [this]() { return this->setSaveInfo(); }, ui_sheet_button_trainer_info_idx));

    sha256(origHash.data(), pkm->rawData(), pkm->getLength());
}

void EditorScreen::drawBottom() const
{
    Language lang = Configuration::getInstance().language();
    Gui::sprite(ui_sheet_emulated_bg_bottom_blue, 0, 0);
    Gui::sprite(ui_sheet_bg_style_bottom_idx, 0, 0);
    Gui::sprite(ui_sheet_bar_arc_bottom_blue_idx, 0, 206);
    Gui::backgroundAnimatedBottom();

    Gui::sprite(ui_sheet_textbox_name_bottom_idx, 0, 1);

    for (int i = 0; i < 5; i++)
    {
        Gui::sprite(ui_sheet_stripe_info_row_idx, 0, 30 + i * 40);
    }

    for (auto& button : buttons)
    {
        button->draw();
    }

    Gui::text(i18n::localize("LEVEL"), 5, 32, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(i18n::localize("NATURE"), 5, 52, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(i18n::localize("ABILITY"), 5, 72, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(i18n::localize("ITEM"), 5, 92, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(i18n::localize("SHINY"), 5, 112, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(i18n::localize("POKERUS"), 5, 132, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(i18n::localize("OT"), 5, 152, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(i18n::localize("NICKNAME"), 5, 172, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(i18n::localize("FRIENDSHIP"), 5, 192, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);

    Gui::ball(pkm->ball(), 4, 3);
    Gui::text(i18n::species(lang, pkm->species()), 25, 5, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    switch (pkm->gender())
    {
        case 0:
            Gui::sprite(ui_sheet_icon_male_idx, 186, 7);
            break;
        case 1:
            Gui::sprite(ui_sheet_icon_female_idx, 187, 7);
            break;
        case 2:
            Gui::sprite(ui_sheet_icon_genderless_idx, 187, 7);
        default:
            break;
    }
    Gui::text(std::to_string((int)pkm->level()), 107 + 35 / 2, 32, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    Gui::text(i18n::nature(lang, pkm->nature()), 95, 52, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(i18n::ability(lang, pkm->ability()), 95, 72, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(i18n::item(lang, pkm->heldItem()), 95, 92, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(
        pkm->shiny() ? i18n::localize("YES") : i18n::localize("NO"), 95, 112, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(pkm->pkrsDays() > 0 ? i18n::localize("YES") : i18n::localize("NO"), 95, 132, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
        TextPosY::TOP);
    Gui::text(pkm->otName(), 95, 152, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(pkm->nickname(), 95, 172, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(
        std::to_string((int)pkm->currentFriendship()), 107 + 35 / 2, 192, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    Gui::pkm(*pkm, 228, 38, 2.0f, COLOR_GREY_BLEND, 1.0f);
    Gui::pkm(*pkm, 224, 33, 2.0f);
}

void EditorScreen::update(touchPosition* touch)
{
    if (!currentOverlay)
    {
        currentOverlay = std::make_shared<ViewOverlay>(*this, pkm, false);
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
        if (goBack())
        {
            return;
        }
    }

    if (downKeys & KEY_Y)
    {
        Gui::setScreen(std::make_unique<MiscEditScreen>(pkm));
        justSwitched = true;
        return;
    }

    if (downKeys & KEY_START)
    {
        if (!Gui::showChoiceMessage("Update Pokémon data with save info?", "This may cause some legality issues."))
        {
            return;
        }
        setSaveInfo();
        return;
    }

    if (!emergency)
    {
        if (downKeys & KEY_L)
        {
            advanceMon(false);
        }
        else if (downKeys & KEY_R)
        {
            advanceMon(true);
        }
    }
}

bool EditorScreen::goBack()
{
    if (saved() || Gui::showChoiceMessage(i18n::localize("EDITOR_CHECK_EXIT")))
    {
        Gui::screenBack();
        TitleLoader::save->fixParty();
        return true;
    }
    return false;
}

bool EditorScreen::advanceMon(bool forward)
{
    if (saved() || Gui::showChoiceMessage(i18n::localize("EDITOR_CHECK_EXIT")))
    {
        TitleLoader::save->fixParty();
        do
        {
            if (box == 0xFF)
            {
                if (forward)
                {
                    index = (index + 1) % 6;
                }
                else
                {
                    index--;
                    if (index < 0)
                    {
                        index = 5;
                    }
                }
                pkm = TitleLoader::save->pkm(index);
            }
            else
            {
                if (forward)
                {
                    index++;
                    if (index >= 30)
                    {
                        box++;
                        index = 0;
                    }
                    if (box * 30 + index >= TitleLoader::save->maxSlot())
                    {
                        index = 0;
                        box   = 0;
                    }
                }
                else
                {
                    index--;
                    if (index < 0)
                    {
                        box--;
                        index = 29;
                    }
                    if (box < 0)
                    {
                        box = TitleLoader::save->maxBoxes() - 1;
                    }
                    if (box * 30 + index >= TitleLoader::save->maxSlot())
                    {
                        box   = TitleLoader::save->maxBoxes() - 1;
                        index = TitleLoader::save->maxSlot() - box * 30 - 1;
                    }
                }
                pkm = TitleLoader::save->pkm(box, index);
            }
        } while (pkm->encryptionConstant() == 0 && pkm->species() == 0);
        sha256(origHash.data(), pkm->rawData(), pkm->getLength());
    }
    return false;
}

bool EditorScreen::hexEdit()
{
    Gui::setScreen(std::make_unique<HexEditScreen>(pkm));
    return false;
}

bool EditorScreen::changeLevel(bool up)
{
    if (up)
    {
        if (pkm->level() < 100)
        {
            pkm->level(pkm->level() + 1);
        }
    }
    else
    {
        if (pkm->level() > 1)
        {
            pkm->level(pkm->level() - 1);
        }
    }
    return false;
}

void EditorScreen::setLevel()
{
    static SwkbdState state;
    static bool first = true;
    if (first)
    {
        swkbdInit(&state, SWKBD_TYPE_NUMPAD, 2, 3);
        first = false;
    }
    swkbdSetFeatures(&state, SWKBD_FIXED_WIDTH);
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    char input[4]   = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[3]        = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        u8 level = (u8)std::min(std::stoi(input), 100);
        pkm->level(level);
    }
}

bool EditorScreen::togglePokerus()
{
    if (pkm->pkrs() > 0)
    {
        pkm->pkrs(0);
    }
    else
    {
        pkm->pkrs(0xF4);
    }
    return false;
}

void EditorScreen::setOT()
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, pkm->generation() == Generation::SIX || pkm->generation() == Generation::SEVEN ? 12 : (8 - 1));
    swkbdSetHintText(&state, i18n::localize("OT_NAME").c_str());
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, SWKBD_FILTER_PROFANITY, 0);
    char input[25]  = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[24]       = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        pkm->otName(input);
    }
}

void EditorScreen::setNick()
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, pkm->generation() == Generation::SIX || pkm->generation() == Generation::SEVEN ? 12 : (11 - 1));
    swkbdSetHintText(&state, i18n::localize("NICKNAME").c_str());
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, SWKBD_FILTER_PROFANITY, 0);
    char input[25]  = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[24]       = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        pkm->nickname(input);
        std::string speciesName = i18n::species(pkm->language(), pkm->species());
        if (pkm->generation() == Generation::FOUR || pkm->version() <= 15 ||
            (pkm->version() >= 35 && pkm->version() <= 41)) // Gen 4, less than or equal to Colosseum/XD, or in VC territory
        {
            StringUtils::toUpper(speciesName);
        }
        if (pkm->nickname() == speciesName)
        {
            pkm->nicknamed(false);
        }
        else
        {
            pkm->nicknamed(true);
        }
    }
}

bool EditorScreen::changeFriendship(bool up)
{
    if (up)
    {
        if (pkm->currentFriendship() < 0xFF)
        {
            pkm->currentFriendship(pkm->currentFriendship() + 1);
        }
    }
    else
    {
        if (pkm->currentFriendship() > 0)
        {
            pkm->currentFriendship(pkm->currentFriendship() - 1);
        }
    }
    return false;
}

void EditorScreen::setFriendship()
{
    static SwkbdState state;
    static bool first = true;
    if (first)
    {
        swkbdInit(&state, SWKBD_TYPE_NUMPAD, 2, 3);
        first = false;
    }
    swkbdSetFeatures(&state, SWKBD_FIXED_WIDTH);
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    char input[4]   = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[3]        = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        u8 friendship = (u8)std::min(std::stoi(input), 255);
        pkm->currentFriendship(friendship);
    }
}

void EditorScreen::partyUpdate()
{
    // Update party values IF the user hasn't edited them themselves
    for (int i = 0; i < 6; i++)
    {
        if (pkm->partyStat(i) == origPartyStats[i])
        {
            pkm->partyStat(i, pkm->stat(i));
            origPartyStats[i] = pkm->stat(i);
        }
    }
    if (pkm->partyLevel() == origPartyLevel)
    {
        pkm->partyLevel(pkm->level());
        origPartyLevel = pkm->level();
    }
    if (pkm->partyCurrHP() == origPartyCurrHP)
    {
        pkm->partyCurrHP(pkm->stat(0));
        origPartyCurrHP = pkm->stat(0);
    }
    if (pkm->generation() == Generation::LGPE)
    {
        PB7* pb7 = (PB7*)pkm.get();
        if (pb7->partyCP() == origPartyCP)
        {
            pb7->partyCP(pb7->CP());
            origPartyCP = pb7->CP();
        }
    }
}

bool EditorScreen::save()
{
    pkm->refreshChecksum();
    if (emergency)
    {
        Banks::bank->pkm(pkm, box, index);
    }
    else
    {
        if (box != 0xFF)
        {
            if (TitleLoader::save->generation() == Generation::LGPE)
            {
                partyUpdate();
            }
            TitleLoader::save->pkm(pkm, box, index, false);
        }
        else
        {
            partyUpdate();
            TitleLoader::save->pkm(pkm, index);
        }
        TitleLoader::save->dex(pkm);
    }
    sha256(origHash.data(), pkm->rawData(), pkm->getLength());
    return false;
}

bool EditorScreen::selectNature()
{
    currentOverlay = std::make_unique<NatureOverlay>(*this, pkm);
    return false;
}

bool EditorScreen::selectAbility()
{
    if (pkm->generation() == Generation::FOUR)
    {
        u8 setAbility = pkm->ability();
        if (pkm->abilities(0) != setAbility && pkm->abilities(0) != 0)
        {
            pkm->setAbility(0);
        }
        else if (pkm->abilities(1) != 0)
        {
            pkm->setAbility(1);
        }
        else // Just in case
        {
            pkm->setAbility(0);
        }
    }
    else if (pkm->generation() == Generation::FIVE)
    {
        PK5* pk5 = (PK5*)pkm.get();
        switch (pkm->abilityNumber() >> 1)
        {
            case 0:
                if (pkm->abilities(1) != pkm->ability() && pkm->abilities(1) != 0)
                {
                    pkm->ability(pkm->abilities(1));
                    if (pkm->abilities(1) == pkm->abilities(2))
                    {
                        pk5->hiddenAbility(true);
                    }
                }
                else if (pkm->abilities(2) != 0)
                {
                    pkm->ability(pkm->abilities(2));
                    pk5->hiddenAbility(true);
                }
                break;
            case 1:
                if (pkm->abilities(2) != pkm->ability() && pkm->abilities(2) != 0)
                {
                    pkm->ability(pkm->abilities(2));
                    pk5->hiddenAbility(true);
                }
                else if (pkm->abilities(0) != 0)
                {
                    pkm->ability(pkm->abilities(0));
                    pk5->hiddenAbility(false);
                }
                break;
            case 2:
                if (pkm->abilities(0) != pkm->ability() && pkm->abilities(0) != 0)
                {
                    pkm->ability(pkm->abilities(0));
                    pk5->hiddenAbility(false);
                }
                else if (pkm->abilities(1) != 0)
                {
                    pkm->ability(pkm->abilities(1));
                    if (pkm->abilities(1) == pkm->abilities(2))
                    {
                        pk5->hiddenAbility(true);
                    }
                    else
                    {
                        pk5->hiddenAbility(false);
                    }
                }
                break;
        }
    }
    else if (pkm->generation() == Generation::SIX || pkm->generation() == Generation::SEVEN)
    {
        switch (pkm->abilityNumber() >> 1)
        {
            case 0:
                if (pkm->abilities(1) != pkm->ability() && pkm->abilities(1) != 0)
                {
                    pkm->setAbility(1);
                }
                else if (pkm->abilities(2) != 0)
                {
                    pkm->setAbility(2);
                }
                break;
            case 1:
                if (pkm->abilities(2) != pkm->ability() && pkm->abilities(2) != 0)
                {
                    pkm->setAbility(2);
                }
                else if (pkm->abilities(0) != 0)
                {
                    pkm->setAbility(0);
                }
                break;
            case 2:
                if (pkm->abilities(0) != pkm->ability() && pkm->abilities(0) != 0)
                {
                    pkm->setAbility(0);
                }
                else if (pkm->abilities(1) != 0)
                {
                    pkm->setAbility(1);
                }
                break;
        }
    }
    return false;
}

bool EditorScreen::selectItem()
{
    currentOverlay = std::make_unique<PkmItemOverlay>(*this, pkm);
    return false;
}

bool EditorScreen::selectForm()
{
    static const std::vector<u16> noChange = {
        716, 717}; // Xerneas & Yveltal because their forms are dumb and do nothing and we don't have sprites for them
    for (auto bad : noChange)
    {
        if (bad == pkm->species())
            return false;
    }
    u8 count = TitleLoader::save->formCount(pkm->species());
    if (pkm->species() == 664 || pkm->species() == 665)
    {
        count = TitleLoader::save->formCount(666);
    }
    if (count > 1)
    {
        currentOverlay = std::make_unique<FormOverlay>(*this, pkm, count);
    }
    return false;
}

bool EditorScreen::selectBall()
{
    currentOverlay = std::make_unique<BallOverlay>(*this, pkm);
    return false;
}

bool EditorScreen::selectSpecies()
{
    currentOverlay = std::make_unique<SpeciesOverlay>(*this, pkm);
    return false;
}

bool EditorScreen::genderSwitch()
{
    switch (pkm->gender())
    {
        case 0:
            pkm->gender(1);
            break;
        case 1:
            pkm->gender(2);
            break;
        case 2:
            pkm->gender(0);
            break;
    }
    return false;
}

bool EditorScreen::setSaveInfo()
{
    if (pkm->otName() != TitleLoader::save->otName())
    {
        pkm->otName(TitleLoader::save->otName());
    }
    if (pkm->TID() != TitleLoader::save->TID())
    {
        pkm->TID(TitleLoader::save->TID());
    }
    if (pkm->SID() != TitleLoader::save->SID())
    {
        pkm->SID(TitleLoader::save->SID());
    }
    if (pkm->otGender() != TitleLoader::save->gender())
    {
        pkm->otGender(TitleLoader::save->gender());
    }
    if (pkm->version() != TitleLoader::save->version())
    {
        pkm->version(TitleLoader::save->version());
    }
    pkm->currentHandler(0);
    return false;
}

bool EditorScreen::saved()
{
    static std::array<u8, SHA256_BLOCK_SIZE> newHash;
    sha256(newHash.data(), pkm->rawData(), pkm->getLength());
    return !memcmp(newHash.data(), origHash.data(), SHA256_BLOCK_SIZE);
}
