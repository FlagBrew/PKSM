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
#include "PkmItemOverlay.hpp"
#include "PkmUtils.hpp"
#include "SpeciesOverlay.hpp"
#include "StatsEditScreen.hpp"
#include "ViewOverlay.hpp"
#include "banks.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "loader.hpp"
#include "pkx/PB7.hpp"
#include "pkx/PK1.hpp"
#include "pkx/PK2.hpp"
#include "pkx/PK3.hpp"
#include "pkx/PK4.hpp"
#include "pkx/PK5.hpp"
#include "pkx/PK6.hpp"
#include "pkx/PK7.hpp"
#include "pkx/PK8.hpp"
#include "sav/Sav.hpp"
#include "utils/random.hpp"
#include "utils/utils.hpp"

#define NO_TEXT_BUTTON(x, y, w, h, function, image)                                                \
    std::make_unique<Button>(x, y, w, h, function, image, "", 0.0f, COLOR_BLACK)
#define NO_TEXT_ACCEL(x, y, w, h, function, image)                                                 \
    std::make_unique<AccelButton>(x, y, w, h, function, image, "", 0.0f, COLOR_BLACK)
#define NO_TEXT_CLICK(x, y, w, h, function, image)                                                 \
    std::make_unique<ClickButton>(x, y, w, h, function, image, "", 0.0f, COLOR_BLACK)

EditorScreen::EditorScreen(std::unique_ptr<pksm::PKX> pokemon, int box, int index, bool emergency)
    : pkm(std::move(pokemon)), box(box), index(index), emergency(emergency)
{
    addOverlay<ViewOverlay>(pkm, false);

    if (this->box == PARTY_MAGIC_NUM)
    {
        if (!pkm->isParty())
        {
            pkm = pkm->partyClone();
        }

        constexpr pksm::Stat stats[] = {pksm::Stat::HP, pksm::Stat::ATK, pksm::Stat::DEF,
            pksm::Stat::SPD, pksm::Stat::SPATK, pksm::Stat::SPDEF};
        for (int i = 0; i < 6; i++)
        {
            origPartyStats[i] = pkm->partyStat(stats[i]);
        }
        origPartyLevel  = pkm->partyLevel();
        origPartyCurrHP = pkm->partyCurrHP();
        if (pkm->generation() == pksm::Generation::LGPE)
        {
            origPartyCP = ((pksm::PB7*)pkm.get())->partyCP();
        }
    }

    if (!pkm || pkm->species() == pksm::Species::None)
    {
        pkm      = PkmUtils::getDefault(TitleLoader::save->generation());
        u8 level = pkm->level();
        pkm->species(pksm::Species::None); // Intentionally set species to none
        if (Configuration::getInstance().useSaveInfo())
        {
            pkm->TID(TitleLoader::save->TID());
            pkm->SID(TitleLoader::save->SID());
            pkm->otName(TitleLoader::save->otName());
            pkm->otGender(TitleLoader::save->gender());
            pkm->version(TitleLoader::save->version());
            switch (pkm->version())
            {
                case pksm::GameVersion::C:
                    pkm->metLocation(0x02); // Route 29, probably C
                    break;
                case pksm::GameVersion::R:
                case pksm::GameVersion::S:
                case pksm::GameVersion::E:
                    pkm->metLocation(0x0010); // Route 101, probably RSE
                    break;
                case pksm::GameVersion::FR:
                case pksm::GameVersion::LG:
                    pkm->metLocation(0x0065); // Route 1, probably FRLG
                    break;
                case pksm::GameVersion::HG:
                case pksm::GameVersion::SS:
                    pkm->metLocation(0x0095); // Route 1, HGSS
                    break;
                case pksm::GameVersion::D:
                case pksm::GameVersion::P:
                case pksm::GameVersion::Pt:
                    pkm->metLocation(0x0010); // Route 201, DPPt
                    break;
                case pksm::GameVersion::B:
                case pksm::GameVersion::W:
                case pksm::GameVersion::B2:
                case pksm::GameVersion::W2:
                    pkm->metLocation(0x000e); // Route 1, BWB2W2
                    break;
                case pksm::GameVersion::X:
                case pksm::GameVersion::Y:
                    pkm->metLocation(0x0008); // Route 1, XY
                    break;
                case pksm::GameVersion::OR:
                case pksm::GameVersion::AS:
                    pkm->metLocation(0x00cc); // Route 101, ORAS
                    break;
                case pksm::GameVersion::SN:
                case pksm::GameVersion::MN:
                case pksm::GameVersion::US:
                case pksm::GameVersion::UM:
                    pkm->metLocation(0x0006); // Route 1, SMUSUM
                    break;
                case pksm::GameVersion::GP:
                case pksm::GameVersion::GE:
                    pkm->metLocation(0x0003); // Route 1, LGPE
                    break;
                case pksm::GameVersion::SW:
                case pksm::GameVersion::SH:
                    pkm->metLocation(0x000C); // Route 1, SWSH
                    break;
                default:
                    break;
            }
        }
        addOverlay<SpeciesOverlay>(*pkm, level);
    }

    buttons.push_back(NO_TEXT_CLICK(
        9, 211, 34, 28, [this]() { return this->goBack(); }, ui_sheet_button_back_idx));
    instructions.addCircle(false, 12, 11, 4, COLOR_GREY);
    instructions.addLine(false, 12, 11, 12, 43, 4, COLOR_GREY);
    instructions.addBox(false, 10, 43, 50, 16, COLOR_GREY, i18n::localize("BALL"), COLOR_WHITE);
    buttons.push_back(NO_TEXT_BUTTON(
        4, 3, 20, 19, [this]() { return this->selectBall(); }, ui_sheet_res_null_idx));
    instructions.addBox(
        false, 224, 33, 60, 68, COLOR_GREY, i18n::localize("CHANGE_FORM"), COLOR_WHITE);
    buttons.push_back(NO_TEXT_BUTTON(
        224, 33, 60, 68, [this]() { return this->selectForm(); }, ui_sheet_res_null_idx));
    instructions.addCircle(false, 305, 14, 11, COLOR_GREY);
    instructions.addLine(false, 305, 14, 305, 106, 4, COLOR_GREY);
    instructions.addBox(
        false, 207, 106, 100, 16, COLOR_GREY, i18n::localize("HEX_EDIT"), COLOR_WHITE);
    buttons.push_back(NO_TEXT_BUTTON(
        291, 2, 27, 23, [this]() { return this->hexEdit(); }, ui_sheet_icon_hex_idx));
    buttons.push_back(NO_TEXT_ACCEL(
        94, 34, 13, 13, [this]() { return this->changeLevel(false); },
        ui_sheet_button_minus_small_idx));
    buttons.push_back(NO_TEXT_BUTTON(
        109, 34, 31, 13,
        [this]()
        {
            setLevel();
            return false;
        },
        ui_sheet_res_null_idx));
    buttons.push_back(NO_TEXT_ACCEL(
        142, 34, 13, 13, [this]() { return this->changeLevel(true); },
        ui_sheet_button_plus_small_idx));
    buttons.push_back(NO_TEXT_BUTTON(
        75, 54, 15, 12, [this]() { return this->selectNature(); },
        ui_sheet_button_info_detail_editor_dark_idx));
    buttons.push_back(NO_TEXT_CLICK(
        75, 74, 15, 12, [this]() { return this->selectAbility(); },
        ui_sheet_button_info_detail_editor_dark_idx));
    buttons.push_back(NO_TEXT_BUTTON(
        75, 94, 15, 12, [this]() { return this->selectItem(); },
        ui_sheet_button_info_detail_editor_dark_idx));
    buttons.push_back(NO_TEXT_CLICK(
        75, 114, 15, 12,
        [this]()
        {
            pkm->shiny(!pkm->shiny());
            return false;
        },
        ui_sheet_button_info_detail_editor_dark_idx));
    buttons.push_back(NO_TEXT_CLICK(
        75, 134, 15, 12, [this]() { return this->togglePokerus(); },
        ui_sheet_button_info_detail_editor_dark_idx));
    buttons.push_back(NO_TEXT_BUTTON(
        75, 154, 15, 12,
        [this]()
        {
            setOT();
            return false;
        },
        ui_sheet_button_info_detail_editor_dark_idx));
    buttons.push_back(NO_TEXT_BUTTON(
        75, 174, 15, 12,
        [this]()
        {
            setNick();
            return false;
        },
        ui_sheet_button_info_detail_editor_dark_idx));
    buttons.push_back(NO_TEXT_ACCEL(
        94, 194, 13, 13, [this]() { return this->changeFriendship(false); },
        ui_sheet_button_minus_small_idx));
    buttons.push_back(NO_TEXT_BUTTON(
        109, 194, 31, 13,
        [this]()
        {
            setFriendship();
            return false;
        },
        ui_sheet_res_null_idx));
    buttons.push_back(NO_TEXT_ACCEL(
        142, 194, 13, 13, [this]() { return this->changeFriendship(true); },
        ui_sheet_button_plus_small_idx));
    buttons.push_back(std::make_unique<Button>(
        204, 109, 108, 30,
        [this]()
        {
            Gui::setScreen(std::make_unique<StatsEditScreen>(*pkm));
            justSwitched = true;
            return true;
        },
        ui_sheet_button_editor_idx, i18n::localize("EDITOR_STATS"), FONT_SIZE_12, COLOR_BLACK));
    buttons.push_back(std::make_unique<Button>(
        204, 140, 108, 30,
        [this]()
        {
            Gui::setScreen(std::make_unique<MoveEditScreen>(*pkm));
            justSwitched = true;
            return true;
        },
        ui_sheet_button_editor_idx, i18n::localize("EDITOR_MOVES"), FONT_SIZE_12, COLOR_BLACK));
    buttons.push_back(std::make_unique<Button>(
        204, 171, 108, 30,
        [this]()
        {
            Gui::setScreen(std::make_unique<MiscEditScreen>(*pkm));
            justSwitched = true;
            return true;
        },
        ui_sheet_button_editor_idx, i18n::localize("EDITOR_MISC"), FONT_SIZE_12, COLOR_BLACK));
    buttons.push_back(std::make_unique<ClickButton>(
        204, 202, 108, 30,
        [this]()
        {
            this->save();
            this->goBack();
            return true;
        },
        ui_sheet_button_editor_idx, i18n::localize("EDITOR_SAVE"), FONT_SIZE_12, COLOR_BLACK));
    instructions.addBox(
        false, 25, 5, 120, 15, COLOR_GREY, i18n::localize("CHANGE_SPECIES"), COLOR_WHITE);
    buttons.push_back(NO_TEXT_BUTTON(
        25, 5, 120, 13, [this]() { return this->selectSpecies(); }, ui_sheet_res_null_idx));
    instructions.addCircle(false, 192, 13, 6, COLOR_GREY);
    instructions.addLine(false, 192, 13, 192, 45, 4, COLOR_GREY);
    instructions.addBox(false, 124, 43, 70, 16, COLOR_GREY, i18n::localize("GENDER"), COLOR_WHITE);
    buttons.push_back(NO_TEXT_CLICK(
        186, 7, 12, 12, [this]() { return this->genderSwitch(); }, ui_sheet_res_null_idx));
    instructions.addCircle(false, 260, 14, 11, COLOR_GREY);
    instructions.addLine(false, 214, 14, 260, 14, 4, COLOR_GREY);
    instructions.addLine(false, 216, 16, 216, 64, 4, COLOR_GREY);
    instructions.addBox(
        false, 98, 64, 120, 16, COLOR_GREY, i18n::localize("SET_SAVE_INFO"), COLOR_WHITE);
    buttons.push_back(NO_TEXT_CLICK(
        239, 3, 43, 22, [this]() { return this->setSaveInfo(); },
        ui_sheet_button_trainer_info_idx));

    origHash = pksm::crypto::sha256(pkm->rawData(), pkm->getLength());
}

void EditorScreen::drawBottom() const
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

    Gui::text(i18n::localize("LEVEL"), 5, 32, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
        TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 86);
    Gui::text(i18n::localize("NATURE"), 5, 52, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
        TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 65);
    Gui::text(i18n::localize("ABILITY"), 5, 72, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
        TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 65);
    Gui::text(i18n::localize("ITEM"), 5, 92, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
        TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 65);
    Gui::text(i18n::localize("SHINY"), 5, 112, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
        TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 65);
    Gui::text(i18n::localize("POKERUS"), 5, 132, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
        TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 65);
    Gui::text(i18n::localize("OT"), 5, 152, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
        TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 65);
    Gui::text(i18n::localize("NICKNAME"), 5, 172, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
        TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 65);
    Gui::text(i18n::localize("FRIENDSHIP"), 5, 192, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT,
        TextPosY::TOP, TextWidthAction::SQUISH_OR_SCROLL, 86);

    Gui::ball(pkm->ball(), 4, 3);
    Gui::text(pkm->species().localize(lang), 25, 5, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT,
        TextPosY::TOP);
    switch (pkm->gender())
    {
        case pksm::Gender::Male:
            Gui::sprite(ui_sheet_icon_male_idx, 186, 7);
            break;
        case pksm::Gender::Female:
            Gui::sprite(ui_sheet_icon_female_idx, 187, 7);
            break;
        case pksm::Gender::Genderless:
            Gui::sprite(ui_sheet_icon_genderless_idx, 187, 7);
        default:
            break;
    }
    Gui::text(std::to_string((int)pkm->level()), 107 + 35 / 2, 32, FONT_SIZE_12, COLOR_BLACK,
        TextPosX::CENTER, TextPosY::TOP);
    Gui::text(pkm->generation() >= pksm::Generation::THREE ? pkm->nature().localize(lang) : "—", 95,
        52, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(pkm->ability().localize(lang), 95, 72, FONT_SIZE_12,
        pkm->abilityNumber() == 4 ? COLOR_UNSELECTRED : COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);

    if (pkm->generation() == pksm::Generation::ONE)
    {
        Gui::text(i18n::item2(lang, static_cast<pksm::PK1*>(pkm.get())->heldItem2()), 95, 92,
            FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    }
    else if (pkm->generation() == pksm::Generation::TWO)
    {
        Gui::text(i18n::item2(lang, static_cast<pksm::PK2*>(pkm.get())->heldItem2()), 95, 92,
            FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    }
    else if (pkm->generation() == pksm::Generation::THREE)
    {
        Gui::text(i18n::item3(lang, static_cast<pksm::PK3*>(pkm.get())->heldItem3()), 95, 92,
            FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    }

    Gui::text(pkm->shiny() ? i18n::localize("YES") : i18n::localize("NO"), 95, 112, FONT_SIZE_12,
        COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(pkm->pkrsDays() > 0 ? i18n::localize("YES") : i18n::localize("NO"), 95, 132,
        FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(pkm->otName(), 95, 152, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(pkm->nickname(), 95, 172, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(std::to_string((int)pkm->currentFriendship()), 107 + 35 / 2, 192, FONT_SIZE_12,
        COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    Gui::pkm(*pkm, 228, 38, 2.0f, COLOR_GREY_BLEND, 1.0f);
    Gui::pkm(*pkm, 224, 33, 2.0f);
}

void EditorScreen::update(touchPosition* touch)
{
    if (justSwitched)
    {
        if (keysHeld() & KEY_TOUCH)
        {
            return;
        }
        else if (pkm->species() == pksm::Species::None)
        {
            Gui::screenBack();
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
        Gui::setScreen(std::make_unique<MiscEditScreen>(*pkm));
        justSwitched = true;
        return;
    }

    if (downKeys & KEY_START)
    {
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
        if (!emergency && TitleLoader::save)
        {
            TitleLoader::save->fixParty();
        }
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
            if (box == PARTY_MAGIC_NUM)
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
                u8 maxPkmInBox = (TitleLoader::save->generation() <= pksm::Generation::TWO &&
                                     TitleLoader::save->language() != pksm::Language::JPN)
                                   ? 20
                                   : 30;
                if (forward)
                {
                    index++;
                    if (index >= maxPkmInBox)
                    {
                        box++;
                        index = 0;
                    }
                    if (box * maxPkmInBox + index >= TitleLoader::save->maxSlot())
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
                    if (box * maxPkmInBox + index >= TitleLoader::save->maxSlot())
                    {
                        box   = TitleLoader::save->maxBoxes() - 1;
                        index = TitleLoader::save->maxSlot() - box * maxPkmInBox - 1;
                    }
                }
                pkm = TitleLoader::save->pkm(box, index);
            }
        }
        while (pkm->species() == pksm::Species::None);
        origHash = pksm::crypto::sha256(pkm->rawData(), pkm->getLength());
    }
    return false;
}

bool EditorScreen::hexEdit()
{
    Gui::setScreen(std::make_unique<HexEditScreen>(*pkm));
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
    swkbdInit(
        &state, SWKBD_TYPE_NORMAL, 2, pkm->generation() >= pksm::Generation::SIX ? 12 : (8 - 1));
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
    swkbdInit(
        &state, SWKBD_TYPE_NORMAL, 2, pkm->generation() >= pksm::Generation::SIX ? 12 : (11 - 1));
    swkbdSetHintText(&state, i18n::localize("NICKNAME").c_str());
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, SWKBD_FILTER_PROFANITY, 0);
    char input[25]  = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[24]       = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        pkm->nickname(input);
        std::string speciesName = pkm->species().localize(pkm->language());
        if (pkm->generation() == pksm::Generation::FOUR ||
            pkm->version() <= pksm::GameVersion::CXD ||
            (pkm->version() >= pksm::GameVersion::RD &&
                pkm->version() <= pksm::GameVersion::C)) // Gen 4, less than or equal to
                                                         // Colosseum/XD, or in VC territory
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
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NUMPAD, 2, 3);
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
    constexpr pksm::Stat stats[] = {pksm::Stat::HP, pksm::Stat::ATK, pksm::Stat::DEF,
        pksm::Stat::SPD, pksm::Stat::SPATK, pksm::Stat::SPDEF};
    for (int i = 0; i < 6; i++)
    {
        if (pkm->partyStat(stats[i]) == origPartyStats[i])
        {
            pkm->partyStat(stats[i], pkm->stat(stats[i]));
            origPartyStats[i] = pkm->stat(stats[i]);
        }
    }
    if (pkm->partyLevel() == origPartyLevel)
    {
        pkm->partyLevel(pkm->level());
        origPartyLevel = pkm->level();
    }
    if (pkm->partyCurrHP() == origPartyCurrHP)
    {
        pkm->partyCurrHP(pkm->stat(pksm::Stat::HP));
        origPartyCurrHP = pkm->stat(pksm::Stat::HP);
    }
    if (pkm->generation() == pksm::Generation::LGPE)
    {
        pksm::PB7* pb7 = (pksm::PB7*)pkm.get();
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
        if (box != PARTY_MAGIC_NUM)
        {
            Banks::bank->pkm(*pkm, box, index);
        }
        else
        {
            PkmUtils::setDefault(pkm->clone());
        }
    }
    else
    {
        if (box != PARTY_MAGIC_NUM)
        {
            if (TitleLoader::save->generation() == pksm::Generation::LGPE ||
                TitleLoader::save->generation() == pksm::Generation::EIGHT)
            {
                partyUpdate();
            }
            TitleLoader::save->pkm(*pkm, box, index, false);
        }
        else
        {
            partyUpdate();
            TitleLoader::save->pkm(*pkm, index);
        }
        TitleLoader::save->dex(*pkm);
    }
    origHash = pksm::crypto::sha256(pkm->rawData(), pkm->getLength());
    return false;
}

bool EditorScreen::selectNature()
{
    if (pkm->generation() >= pksm::Generation::THREE)
    {
        addOverlay<NatureOverlay>(*pkm);
    }
    return false;
}

bool EditorScreen::selectAbility()
{
    if (pkm->generation() == pksm::Generation::FOUR || pkm->generation() == pksm::Generation::THREE)
    {
        pksm::Ability setAbility = pkm->ability();
        if (pkm->abilities(0) != setAbility && pkm->abilities(0) != pksm::Ability::None)
        {
            pkm->setAbility(0);
        }
        else if (pkm->abilities(1) != pksm::Ability::None && pkm->abilities(0) != pkm->abilities(1))
        {
            pkm->setAbility(1);
        }
        else // Just in case
        {
            pkm->setAbility(0);
        }
    }
    else if (pkm->generation() == pksm::Generation::FIVE)
    {
        pksm::PK5* pk5 = (pksm::PK5*)pkm.get();
        switch (pkm->abilityNumber() >> 1)
        {
            case 0:
                if (pkm->abilities(1) != pkm->ability() && pkm->abilities(1) != pksm::Ability::None)
                {
                    pkm->setAbility(1);
                    if (pk5->abilities(1) == pk5->abilities(2))
                    {
                        pk5->hiddenAbility(true);
                    }
                }
                else if (pkm->abilities(2) != pkm->ability() &&
                         pkm->abilities(2) != pksm::Ability::None)
                {
                    pkm->setAbility(2);
                }
                break;
            case 1:
                if (pkm->abilities(2) != pkm->ability() && pkm->abilities(2) != pksm::Ability::None)
                {
                    pkm->setAbility(2);
                }
                else if (pkm->abilities(0) != pkm->ability() &&
                         pkm->abilities(0) != pksm::Ability::None)
                {
                    pkm->setAbility(0);
                }
                break;
            case 2:
                if (pkm->abilities(0) != pkm->ability() && pkm->abilities(0) != pksm::Ability::None)
                {
                    pkm->setAbility(0);
                }
                else if (pkm->abilities(1) != pkm->ability() &&
                         pkm->abilities(1) != pksm::Ability::None)
                {
                    pkm->setAbility(1);
                    if (pkm->abilities(1) == pkm->abilities(2))
                    {
                        pk5->hiddenAbility(true);
                    }
                }
                break;
            default: // should never happen, but just in case
                pkm->setAbility(0);
                break;
        }
    }
    else
    {
        switch (pkm->abilityNumber() >> 1)
        {
            case 0:
                if (pkm->abilities(1) != pkm->ability() && pkm->abilities(1) != pksm::Ability::None)
                {
                    pkm->setAbility(1);
                }
                else if (pkm->abilities(2) != pkm->ability() &&
                         pkm->abilities(2) != pksm::Ability::None)
                {
                    pkm->setAbility(2);
                }
                break;
            case 1:
                if (pkm->abilities(2) != pkm->ability() && pkm->abilities(2) != pksm::Ability::None)
                {
                    pkm->setAbility(2);
                }
                else if (pkm->abilities(0) != pkm->ability() &&
                         pkm->abilities(0) != pksm::Ability::None)
                {
                    pkm->setAbility(0);
                }
                break;
            case 2:
                if (pkm->abilities(0) != pkm->ability() && pkm->abilities(0) != pksm::Ability::None)
                {
                    pkm->setAbility(0);
                }
                else if (pkm->abilities(1) != pkm->ability() &&
                         pkm->abilities(1) != pksm::Ability::None)
                {
                    pkm->setAbility(1);
                }
                break;
            default: // should never happen, but just in case
                pkm->setAbility(0);
                break;
        }
    }
    return false;
}

bool EditorScreen::selectItem()
{
    addOverlay<PkmItemOverlay>(*pkm);
    return false;
}

bool EditorScreen::selectForm()
{
    static constexpr std::array<pksm::Species, 3> noChange = {pksm::Species::Xerneas,
        pksm::Species::Yveltal,
        pksm::Species::Mothim}; // Xerneas & Yveltal because their forms are dumb and do nothing
                                // and we don't have sprites for them
                                // Mothim because there's no difference
    if (std::any_of(noChange.begin(), noChange.end(),
            [this](const pksm::Species& badSpecies) { return badSpecies == pkm->species(); }))
    {
        return false;
    }
    u8 count;
    pksm::Species formCountSpec = pkm->species();
    if (formCountSpec == pksm::Species::Scatterbug || formCountSpec == pksm::Species::Spewpa)
    {
        formCountSpec = pksm::Species::Vivillon;
    }
    if (TitleLoader::save)
    {
        count = TitleLoader::save->formCount(formCountSpec);
    }
    else
    {
        count = pksm::VersionTables::formCount(
            pksm::GameVersion::oldestVersion(pkm->generation()), formCountSpec);
    }
    if (count > 1)
    {
        addOverlay<FormOverlay>(*pkm, count);
    }
    return false;
}

bool EditorScreen::selectBall()
{
    addOverlay<BallOverlay>(*pkm);
    return false;
}

bool EditorScreen::selectSpecies()
{
    addOverlay<SpeciesOverlay>(*pkm);
    return false;
}

bool EditorScreen::genderSwitch()
{
    switch (pkm->gender())
    {
        case pksm::Gender::Male:
            pkm->gender(pksm::Gender::Female);
            break;
        case pksm::Gender::Female:
            pkm->gender(pksm::Gender::Genderless);
            break;
        case pksm::Gender::Genderless:
            pkm->gender(pksm::Gender::Male);
            break;
        case pksm::Gender::INVALID:
            break;
    }
    return false;
}

bool EditorScreen::setSaveInfo()
{
    if (TitleLoader::save)
    {
        pkm->otName(TitleLoader::save->otName());
        pkm->TID(TitleLoader::save->TID());
        pkm->SID(TitleLoader::save->SID());
        pkm->otGender(TitleLoader::save->gender());
        pkm->version(TitleLoader::save->version());
        pkm->currentHandler(0);
    }
    return false;
}

bool EditorScreen::saved()
{
    auto newHash = pksm::crypto::sha256(pkm->rawData(), pkm->getLength());
    return newHash == origHash;
}
