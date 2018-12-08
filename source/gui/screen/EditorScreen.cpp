/*
*   This file is part of PKSM
*   Copyright (C) 2016-2018 Bernardo Giordano, Admiral Fish, piepie62
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
#include "gui.hpp"
#include "Configuration.hpp"
#include "loader.hpp"
#include "HexEditScreen.hpp"
#include "MoveSelectionScreen.hpp"
#include "HiddenPowerSelectionScreen.hpp"
#include "NatureSelectionScreen.hpp"
#include "ItemSelectionScreen.hpp"
#include "SpeciesSelectionScreen.hpp"
#include "FormSelectionScreen.hpp"
#include "BallSelectionScreen.hpp"
#include "AccelButton.hpp"
#include "ClickButton.hpp"
#include "PB7.hpp"

#define NO_TEXT_BUTTON(x, y, w, h, function, image) new Button(x, y, w, h, function, image, "", 0.0f, 0)
#define NO_TEXT_ACCEL(x, y, w, h, function, image) new AccelButton(x, y, w, h, function, image, "", 0.0f, 0)
#define NO_TEXT_CLICK(x, y, w, h, function, image) new ClickButton(x, y, w, h, function, image, "", 0.0f, 0)

static constexpr int statValues[] = { 0, 1, 2, 4, 5, 3 };

EditorScreen::EditorScreen(std::shared_ptr<ViewerScreen> viewer, std::shared_ptr<PKX> pokemon, int box, int index)
                : view(viewer), pkm(pokemon), box(box), index(index)
{
    if (!view)
    {
        view = std::make_shared<ViewerScreen>(pkm, false);
    }
    if (!pkm || pkm->species() == 0)
    {
        pkm = TitleLoader::save->emptyPkm()->clone();
        pkm->TID(Configuration::getInstance().defaultTID());
        pkm->SID(Configuration::getInstance().defaultSID());
        pkm->otName(Configuration::getInstance().defaultOT().c_str());
        pkm->ball(4);
        pkm->encryptionConstant((u32)rand());
        pkm->version(TitleLoader::save->version());
        pkm->fixMoves();
        pkm->PID((u32)rand());
        selector = std::make_unique<SpeciesSelectionScreen>(pkm);
        // No clue why this is necessary
        view->setPkm(nullptr);
        view->setPkm(pkm);
    }

    for (int i = 0; i < 6; i++)
    {
        origPartyStats[i] = pkm->partyStat(i);
    }
    origPartyLevel = pkm->partyLevel();
    origPartyCurrHP = pkm->partyCurrHP();
    if (pkm->generation() == Generation::LGPE)
    {
        origPartyCP = ((PB7*)pkm.get())->partyCP();
    }

    u8 tab = 0;
    // Back button first, always. Needs to have the same index for each one
    buttons[tab].push_back(NO_TEXT_CLICK(283, 211, 34, 28, [this](){ return this->goBack(); }, ui_sheet_button_back_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(4, 3, 20, 19, [this](){ return this->selectBall(); }, ui_sheet_res_null_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(224, 33, 60, 68, [this](){ return this->selectForm(); }, ui_sheet_res_null_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(291, 2, 27, 23, [this](){ return this->hexEdit(); }, ui_sheet_icon_hex_idx));
    buttons[tab].push_back(NO_TEXT_ACCEL(94, 34, 13, 13, [this](){ return this->changeLevel(false); }, ui_sheet_button_minus_small_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(109, 34, 31, 13, [this](){ Gui::setNextKeyboardFunc([this](){ setLevel(); }); return false; }, ui_sheet_res_null_idx));
    buttons[tab].push_back(NO_TEXT_ACCEL(142, 34, 13, 13, [this](){ return this->changeLevel(true); }, ui_sheet_button_plus_small_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(75, 54, 15, 12, [this](){ return this->selectNature(); }, ui_sheet_button_info_detail_editor_dark_idx));
    buttons[tab].push_back(NO_TEXT_CLICK(75, 74, 15, 12, [this](){ return this->selectAbility(); }, ui_sheet_button_info_detail_editor_dark_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(75, 94, 15, 12, [this](){ return this->selectItem(); }, ui_sheet_button_info_detail_editor_dark_idx));
    buttons[tab].push_back(NO_TEXT_CLICK(75, 114, 15, 12, [this](){ pkm->shiny(!pkm->shiny()); return false; }, ui_sheet_button_info_detail_editor_dark_idx));
    buttons[tab].push_back(NO_TEXT_CLICK(75, 134, 15, 12, [this](){ return this->togglePokerus(); }, ui_sheet_button_info_detail_editor_dark_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(75, 154, 15, 12, [this](){ Gui::setNextKeyboardFunc([this](){ return this->setOT(); }); return false; }, ui_sheet_button_info_detail_editor_dark_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(75, 174, 15, 12, [this](){ Gui::setNextKeyboardFunc([this](){ return this->setNick(); }); return false; }, ui_sheet_button_info_detail_editor_dark_idx));
    buttons[tab].push_back(NO_TEXT_ACCEL(94, 194, 13, 13, [this](){ return this->changeFriendship(false); }, ui_sheet_button_minus_small_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(109, 194, 31, 13, [this](){ Gui::setNextKeyboardFunc([this](){ return this->setFriendship(); }); return false; }, ui_sheet_res_null_idx));
    buttons[tab].push_back(NO_TEXT_ACCEL(142, 194, 13, 13, [this](){ return this->changeFriendship(true); }, ui_sheet_button_plus_small_idx));
    buttons[tab].push_back(new Button(204, 109, 108, 30, [this](){ currentTab = 1; return true; }, ui_sheet_button_editor_idx, i18n::localize("EDITOR_STATS"), FONT_SIZE_12, COLOR_BLACK));
    buttons[tab].push_back(new Button(204, 140, 108, 30, [this](){ currentTab = 2; return true; }, ui_sheet_button_editor_idx, i18n::localize("EDITOR_MOVES"), FONT_SIZE_12, COLOR_BLACK));
    buttons[tab].push_back(new ClickButton(204, 171, 108, 30, [this](){ return this->save(); }, ui_sheet_button_editor_idx, i18n::localize("EDITOR_SAVE"), FONT_SIZE_12, COLOR_BLACK));
    buttons[tab].push_back(NO_TEXT_BUTTON(25, 5, 120, 13, [this](){ return this->selectSpecies(); }, ui_sheet_res_null_idx));

    tab = 1;
    buttons[tab].push_back(buttons[0][0]);
    for (int i = 0; i < 6; i++)
    {
        int y = 54 + i * 20;
        buttons[tab].push_back(NO_TEXT_ACCEL(106, y, 13, 13, [=](){ return this->changeIV(statValues[i], false); }, ui_sheet_button_minus_small_idx));
        buttons[tab].push_back(NO_TEXT_BUTTON(121, y, 23, 13, [=](){ Gui::setNextKeyboardFunc([=](){ return this->setIV(statValues[i]); }); return false; }, ui_sheet_res_null_idx));
        buttons[tab].push_back(NO_TEXT_ACCEL(146, y, 13, 13, [=](){ return this->changeIV(statValues[i], true); }, ui_sheet_button_plus_small_idx));

        buttons[tab].push_back(NO_TEXT_ACCEL(182, y, 13, 13, [=](){ return this->changeSecondaryStat(statValues[i], false); }, ui_sheet_button_minus_small_idx));
        buttons[tab].push_back(NO_TEXT_BUTTON(197, y, 32, 13, [=](){ Gui::setNextKeyboardFunc([=](){ return this->setSecondaryStat(statValues[i]); }); return false; }, ui_sheet_res_null_idx));
        buttons[tab].push_back(NO_TEXT_ACCEL(231, y, 13, 13, [=](){ return this->changeSecondaryStat(statValues[i], true); }, ui_sheet_button_plus_small_idx));
    }
    buttons[tab].push_back(NO_TEXT_BUTTON(300, 184, 15, 12, [this](){ return this->setHP(); }, ui_sheet_button_info_detail_editor_light_idx));

    tab = 2;
    buttons[tab].push_back(buttons[0][0]);
}

void EditorScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetBottom);
    Language lang = Configuration::getInstance().language();
    Gui::sprite(ui_sheet_emulated_bg_bottom_blue, 0, 0);
    Gui::sprite(ui_sheet_bg_style_bottom_idx, 0, 0);
    Gui::sprite(ui_sheet_bar_arc_bottom_blue_idx, 0, 206);
    Gui::backgroundAnimatedBottom();

    Gui::sprite(ui_sheet_textbox_name_bottom_idx, 0, 1);
    std::string text;
    switch (currentTab)
    {
        // Main part
        case 0:
            for (int i = 0; i < 5; i++)
            {
                Gui::sprite(ui_sheet_stripe_info_row_idx, 0, 30 + i * 40);
            }

            for (auto button : buttons[currentTab])
            {
                button->draw();
            }

            Gui::staticText(i18n::localize("LEVEL"), 5, 32, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText(i18n::localize("NATURE"), 5, 52, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText(i18n::localize("ABILITY"), 5, 72, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText(i18n::localize("ITEM"), 5, 92, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText(i18n::localize("SHINY"), 5, 112, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText(i18n::localize("POKERUS"), 5, 132, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText(i18n::localize("OT"), 5, 152, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText(i18n::localize("NICKNAME"), 5, 172, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText(i18n::localize("FRIENDSHIP"), 5, 192, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);

            Gui::ball(pkm->ball(), 4, 3);
            Gui::dynamicText(i18n::species(lang, pkm->species()), 25, 5, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE, false);
            Gui::dynamicText(107, 32, 35, std::to_string((int)pkm->level()), FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
            Gui::dynamicText(i18n::nature(lang, pkm->nature()), 95, 52, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::dynamicText(i18n::ability(lang, pkm->ability()), 95, 72, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::dynamicText(i18n::item(lang, pkm->heldItem()), 95, 92, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::dynamicText(pkm->shiny() ? i18n::localize("YES") : i18n::localize("NO"), 95, 112, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::dynamicText(pkm->pkrsDays() > 0 ? i18n::localize("YES") : i18n::localize("NO"), 95, 132, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::dynamicText(pkm->otName(), 95, 152, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::dynamicText(pkm->nickname(), 95, 172, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::dynamicText(107, 192, 35, std::to_string((int) pkm->currentFriendship()), FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
            Gui::pkm(pkm.get(), 224, 33, 2.0f);
            break;
        // Stats screen
        case 1:
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

            for (auto button : buttons[currentTab])
            {
                button->draw();
            }
            
            if (pkm->generation() == Generation::LGPE)
            {
                Gui::dynamicText(i18n::localize("EDITOR_CP") + std::to_string((int)((PB7*)pkm.get())->CP()), 4, 5, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE, false);
            }
            Gui::staticText(i18n::localize("STATS"), 4, 32, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText(119, 32, 27, i18n::localize("IV"), FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
            text = pkm->generation() == Generation::LGPE ? i18n::localize("AWAKENED") : i18n::localize("EV");
            Gui::staticText(195, 32, 36, text, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
            Gui::staticText(249, 32, 51, i18n::localize("TOTAL"), FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
            Gui::staticText(i18n::localize("HP"), 4, 52, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText(i18n::localize("ATTACK"), 4, 72, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText(i18n::localize("DEFENSE"), 4, 92, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText(i18n::localize("SPATK"), 4, 112, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText(i18n::localize("SPDEF"), 4, 132, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText(i18n::localize("SPEED"), 4, 152, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);

            for (int i = 0; i < 6; i++)
            {
                Gui::dynamicText(119, 52 + i * 20, 27, std::to_string((int) pkm->iv(statValues[i])), FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
                if (pkm->generation() != Generation::LGPE)
                {
                    Gui::dynamicText(195, 52 + i * 20, 36, std::to_string((int) pkm->ev(statValues[i])), FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
                }
                else
                {
                    Gui::dynamicText(195, 52 + i * 20, 36, std::to_string((int) ((PB7*)pkm.get())->awakened(statValues[i])), FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
                }
                Gui::dynamicText(249, 52 + i * 20, 51, std::to_string((int) pkm->stat(statValues[i])), FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
            }
            Gui::dynamicText(i18n::localize("EDITOR_HIDDEN_POWER") + i18n::hp(lang, pkm->hpType()), 295, 181, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE, true);
            break;
        // Moves screen
        case 2:
            Gui::sprite(ui_sheet_textbox_relearn_moves_idx, 0, 109);
            for (int i = 0; i < 2; i++)
            {
                Gui::sprite(ui_sheet_stripe_move_editor_row_idx, 0, 30 + i * 40);
                Gui::sprite(ui_sheet_stripe_move_editor_row_idx, 0, 139 + i * 40);
            }
            for (int i = 0; i < 4; i++)
            {
                Gui::sprite(ui_sheet_point_small_idx, 15, 37 + i * 20);
                Gui::sprite(ui_sheet_point_small_idx, 15, 146 + i * 20);
            }
            for (auto button : buttons[currentTab])
            {
                button->draw();
            }

            Gui::staticText(i18n::localize("MOVES"), 12, 5, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE, false);
            Gui::staticText(i18n::localize("RELEARN_MOVES"), 12, 113, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE, false);

            for (int i = 0; i < 4; i++)
            {
                Gui::dynamicText(i18n::move(lang, pkm->move(i)), 24, 32 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
                if (pkm->generation() == Generation::SIX)
                {
                    Gui::dynamicText(i18n::move(lang, ((PK6*)pkm.get())->relearnMove(i)), 24, 141 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
                }
                else if (pkm->generation() == Generation::SEVEN)
                {
                    Gui::dynamicText(i18n::move(lang, ((PK7*)pkm.get())->relearnMove(i)), 24, 141 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
                }
                else
                {
                    Gui::staticText(i18n::localize("EDITOR_NOT_APPLICABLE_GEN"), 24, 141 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
                }
            }

            if (moveSelected < 4)
            {
                Gui::sprite(ui_sheet_emulated_pointer_horizontal_flipped_idx, 169, 31 + moveSelected * 20);
                Gui::staticText("\uE000", 190, 29 + moveSelected * 20, FONT_SIZE_18, FONT_SIZE_18, COLOR_BLACK, false);
            }
            else
            {
                Gui::sprite(ui_sheet_emulated_pointer_horizontal_flipped_idx, 169, 140 + (moveSelected - 4) * 20);
                Gui::staticText("\uE000", 190, 138 + (moveSelected - 4) * 20, FONT_SIZE_18, FONT_SIZE_18, COLOR_BLACK, false);
            }
            break;
    }
    if (!selector)
    {
        view->draw();
    }
    else
    {
        C2D_DrawRectSolid(0, 0, 0.5f, 320, 240, C2D_Color32(0, 0, 0, 120));
        Gui::staticText(GFX_BOTTOM, 115, i18n::localize("EDITOR_INST"), FONT_SIZE_18, FONT_SIZE_18, COLOR_WHITE);
        selector->draw();
    }
}

void EditorScreen::update(touchPosition* touch)
{
    if (!selector)
    {
        u32 downKeys = keysDown();

        for (size_t i = 0; i < buttons[currentTab].size(); i++)
        {
            if (buttons[currentTab][i]->update(touch))
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

        if (currentTab == 2)
        {
            if (downKeys & KEY_A)
            {
                changeMove();
            }
            else if (downKeys & KEY_DOWN)
            {
                if (moveSelected < 7)
                {
                    moveSelected++;
                }
            }
            else if (downKeys & KEY_UP)
            {
                if (moveSelected > 0)
                {
                    moveSelected--;
                }
            }
        }
    }
    else
    {
        selector->update(touch);
        if (selector->finished())
        {
            selector = nullptr;
        }
    }
}

bool EditorScreen::goBack()
{
    if (currentTab != 0)
    {
        currentTab = 0;
        return false;
    }
    else
    {
        Gui::screenBack();
        return true;
    }
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
    char input[4] = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[3] = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        u8 level = (u8) std::min(std::stoi(input), 100);
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
    bool first = true;
    if (first)
    {
        swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, pkm->generation() == Generation::SIX || pkm->generation() == Generation::SEVEN ? 12 : (8 - 1));
        first = false;
    }
    swkbdSetHintText(&state, i18n::localize("OT_NAME").c_str());
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    char input[25] = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[24] = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        pkm->otName(input);
    }
}

void EditorScreen::setNick()
{
    SwkbdState state;
    bool first = true;
    if (first)
    {
        swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, pkm->generation() == Generation::SIX || pkm->generation() == Generation::SEVEN ? 12 : (11 - 1));
        first = false;
    }
    swkbdSetHintText(&state, i18n::localize("NICKNAME").c_str());
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    char input[25] = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[24] = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        pkm->nickname(input);
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
    char input[4] = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[3] = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        u8 friendship = (u8) std::min(std::stoi(input), 255);
        pkm->currentFriendship(friendship);
    }
}

bool EditorScreen::save()
{
    pkm->refreshChecksum();
    if (box != 0xFF)
    {
        TitleLoader::save->pkm(*pkm, box, index);
    }
    else
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
        TitleLoader::save->pkm(*pkm, index);
    }
    return false;
}

void EditorScreen::setIV(int which)
{
    static SwkbdState state;
    static bool first = true;
    if (first)
    {
        swkbdInit(&state, SWKBD_TYPE_NUMPAD, 2, 2);
        first = false;
    }
    swkbdSetFeatures(&state, SWKBD_FIXED_WIDTH);
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    char input[3] = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[2] = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        u8 iv = (u8) std::stoi(input);
        pkm->iv(which, std::min((u8)31, iv));
    }
}

bool EditorScreen::changeIV(int which, bool up)
{
    if (up)
    {
        if (pkm->iv(which) < 31)
        {
            pkm->iv(which, pkm->iv(which) + 1);
        }
    }
    else
    {
        if (pkm->iv(which) > 0)
        {
            pkm->iv(which, pkm->iv(which) - 1);
        }
    }
    return false;
}

void EditorScreen::setSecondaryStat(int which)
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
    char input[4] = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[3] = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        u8 val = (u8) std::min(std::stoi(input), 0xFF);
        if (pkm->generation() != Generation::LGPE)
        {
            pkm->ev(which, val);
        }
        else
        {
            ((PB7*)pkm.get())->awakened(which, std::min((int)val, 200));
        }
    }
}

bool EditorScreen::changeSecondaryStat(int which, bool up)
{
    if (up)
    {
        if (pkm->generation() != Generation::LGPE)
        {
            if (pkm->ev(which) < 0xFF)
            {
                pkm->ev(which, pkm->ev(which) + 1);
            }
        }
        else
        {
            PB7* pb7 = (PB7*)pkm.get();
            if (pb7->awakened(which) < 200)
            {
                pb7->awakened(which, pb7->awakened(which) + 1);
            }
        }
    }
    else
    {
        if (pkm->generation() != Generation::LGPE)
        {
            if (pkm->ev(which) > 0)
            {
                pkm->ev(which, pkm->ev(which) - 1);
            }
        }
        else
        {
            PB7* pb7 = (PB7*)pkm.get();
            if (pb7->awakened(which) > 0)
            {
                pb7->awakened(which, pb7->awakened(which) - 1);
            }
        }
    }
    return false;
}

bool EditorScreen::setHP()
{
    selector = std::make_unique<HiddenPowerSelectionScreen>(pkm);
    return false;
}

void EditorScreen::changeMove()
{
    selector = std::make_unique<MoveSelectionScreen>(pkm, moveSelected);
}

bool EditorScreen::selectNature()
{
    selector = std::make_unique<NatureSelectionScreen>(pkm);
    return false;
}

bool EditorScreen::selectAbility()
{
    if (pkm->generation() == Generation::FOUR)
    {
        u8 setAbility = pkm->ability();
        if (PersonalDPPtHGSS::ability(pkm->species(), 0) != setAbility && PersonalDPPtHGSS::ability(pkm->species(), 0) != 0)
        {
            pkm->ability(PersonalDPPtHGSS::ability(pkm->species(), 0));
        }
        else if (PersonalDPPtHGSS::ability(pkm->species(), 1) != 0)
        {
            pkm->ability(PersonalDPPtHGSS::ability(pkm->species(), 1));
        }
    }
    else if (pkm->generation() == Generation::FIVE)
    {
        PK5* pk5 = (PK5*) pkm.get();
        auto abilityResolver = PersonalBWB2W2::ability;
        switch (pkm->abilityNumber() >> 1)
        {
            case 0:
                if (abilityResolver(pkm->species(), 1) != pkm->ability() && abilityResolver(pkm->species(), 1) != 0)
                {
                    pkm->ability(abilityResolver(pkm->species(), 1));
                    if (abilityResolver(pkm->species(), 1) == abilityResolver(pkm->species(), 2))
                    {
                        pk5->hiddenAbility(true);
                    }
                }
                else if (abilityResolver(pkm->species(), 2) != 0)
                {
                    pkm->ability(abilityResolver(pkm->species(), 2));
                    pk5->hiddenAbility(true);
                }
                break;
            case 1:
                if (abilityResolver(pkm->species(), 2) != pkm->ability() && abilityResolver(pkm->species(), 2) != 0)
                {
                    pkm->ability(abilityResolver(pkm->species(), 2));
                    pk5->hiddenAbility(true);
                }
                else if (abilityResolver(pkm->species(), 0) != 0)
                {
                    pkm->ability(abilityResolver(pkm->species(), 0));
                    pk5->hiddenAbility(false);
                }
                break;
            case 2:
                if (abilityResolver(pkm->species(), 0) != pkm->ability() && abilityResolver(pkm->species(), 0) != 0)
                {
                    pkm->ability(abilityResolver(pkm->species(), 0));
                    pk5->hiddenAbility(false);
                }
                else if (abilityResolver(pkm->species(), 1) != 0)
                {
                    pkm->ability(abilityResolver(pkm->species(), 1));
                    if (abilityResolver(pkm->species(), 1) == abilityResolver(pkm->species(), 2))
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
        auto abilityResolver = pkm->generation() == Generation::SIX ? PersonalXYORAS::ability : PersonalSMUSUM::ability;
        switch (pkm->abilityNumber() >> 1)
        {
            case 0:
                if (abilityResolver(pkm->species(), 1) != pkm->ability() && abilityResolver(pkm->species(), 1) != 0)
                {
                    pkm->ability(1);
                }
                else if (abilityResolver(pkm->species(), 2) != 0)
                {
                    pkm->ability(2);
                }
                break;
            case 1:
                if (abilityResolver(pkm->species(), 2) != pkm->ability() && abilityResolver(pkm->species(), 2) != 0)
                {
                    pkm->ability(2);
                }
                else if (abilityResolver(pkm->species(), 0) != 0)
                {
                    pkm->ability(0);
                }
                break;
            case 2:
                if (abilityResolver(pkm->species(), 0) != pkm->ability() && abilityResolver(pkm->species(), 0) != 0)
                {
                    pkm->ability(0);
                }
                else if (abilityResolver(pkm->species(), 1) != 0)
                {
                    pkm->ability(1);
                }
                break;
        }
    }
    return false;
}

bool EditorScreen::selectItem()
{
    selector = std::make_unique<ItemSelectionScreen>(pkm);
    return false;
}

bool EditorScreen::selectForm()
{
    static const std::vector<u16> noChange = { 493, 649, 773 };
    for (auto bad : noChange)
    {
        if (bad == pkm->species())
            return false;
    }
    u8 (*formCounter)(u16);
    switch (TitleLoader::save->generation())
    {
        case Generation::FOUR:
            formCounter = PersonalDPPtHGSS::formCount;
            break;
        case Generation::FIVE:
            formCounter = PersonalBWB2W2::formCount;
            break;
        case Generation::SIX:
            formCounter = PersonalXYORAS::formCount;
            break;
        case Generation::SEVEN:
        default:
            formCounter = PersonalSMUSUM::formCount;
            break;
    }
    u8 count = formCounter(pkm->species());
    if (count > 1)
    {
        selector = std::make_unique<FormSelectionScreen>(pkm, count);
    }
    return false;
}

bool EditorScreen::selectBall()
{
    selector = std::make_unique<BallSelectionScreen>(pkm);
    return false;
}

bool EditorScreen::selectSpecies()
{
    selector = std::make_unique<SpeciesSelectionScreen>(pkm);
    return false;
}