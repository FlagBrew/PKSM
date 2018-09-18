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
#include <bitset>

#define NO_TEXT_BUTTON(x, y, w, h, function, image) new Button(x, y, w, h, function, image, "", 0.0f, 0)

static constexpr int statValues[] = { 0, 1, 2, 4, 5, 3 };

EditorScreen::EditorScreen(std::shared_ptr<ViewerScreen> viewer, std::shared_ptr<PKX> pokemon, int box, int index)
                : view(viewer), pkm(pokemon), box(box), index(index)
{
    if (!pkm)
    {
        pkm = TitleLoader::save->emptyPkm();
    }
    if (!view)
    {
        view = std::shared_ptr<ViewerScreen>(new ViewerScreen(pkm, false));
    }
    u8 tab = 0;
    // Back button first, always. Needs to have the same index for each one
    buttons[tab].push_back(NO_TEXT_BUTTON(283, 211, 34, 28, std::bind(&EditorScreen::goBack, this), ui_sheet_button_back_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(291, 2, 27, 23, std::bind(&EditorScreen::hexEdit, this), ui_sheet_icon_hex_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(94, 34, 13, 13, std::bind(&EditorScreen::changeLevel, this, false), ui_sheet_button_minus_small_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(109, 34, 31, 13, std::bind(&EditorScreen::setLevel, this), ui_sheet_res_null_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(142, 34, 13, 13, std::bind(&EditorScreen::changeLevel, this, true), ui_sheet_button_plus_small_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(75, 54, 15, 12, std::bind(&EditorScreen::selectNature, this), ui_sheet_button_info_detail_editor_dark_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(75, 74, 15, 12, std::bind(&EditorScreen::selectAbility, this), ui_sheet_button_info_detail_editor_dark_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(75, 94, 15, 12, std::bind(&EditorScreen::selectItem, this), ui_sheet_button_info_detail_editor_dark_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(75, 114, 15, 12, [this](){ pkm->shiny(!pkm->shiny()); return false; }, ui_sheet_button_info_detail_editor_dark_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(75, 134, 15, 12, std::bind(&EditorScreen::togglePokerus, this), ui_sheet_button_info_detail_editor_dark_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(75, 154, 15, 12, std::bind(&EditorScreen::setOT, this), ui_sheet_button_info_detail_editor_dark_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(75, 174, 15, 12, std::bind(&EditorScreen::setNick, this), ui_sheet_button_info_detail_editor_dark_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(94, 194, 13, 13, std::bind(&EditorScreen::changeFriendship, this, false), ui_sheet_button_minus_small_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(109, 194, 31, 13, std::bind(&EditorScreen::setFriendship, this), ui_sheet_res_null_idx));
    buttons[tab].push_back(NO_TEXT_BUTTON(142, 194, 13, 13, std::bind(&EditorScreen::changeFriendship, this, true), ui_sheet_button_plus_small_idx));
    buttons[tab].push_back(new Button(204, 109, 108, 30, [this](){ currentTab = 1; return true; }, ui_sheet_button_editor_idx, "STATS", FONT_SIZE_12, COLOR_BLACK));
    buttons[tab].push_back(new Button(204, 140, 108, 30, [this](){ currentTab = 2; return true; }, ui_sheet_button_editor_idx, "MOVES", FONT_SIZE_12, COLOR_BLACK));
    buttons[tab].push_back(new Button(204, 171, 108, 30, std::bind(&EditorScreen::save, this), ui_sheet_button_editor_idx, "SAVE", FONT_SIZE_12, COLOR_BLACK));

    tab = 1;
    buttons[tab].push_back(NO_TEXT_BUTTON(283, 211, 34, 28, std::bind(&EditorScreen::goBack, this), ui_sheet_button_back_idx));
    for (int i = 0; i < 6; i++)
    {
        int y = 54 + i * 20;
        buttons[tab].push_back(NO_TEXT_BUTTON(106, y, 13, 13, std::bind(&EditorScreen::changeIV, this, statValues[i], false), ui_sheet_button_minus_small_idx));
        buttons[tab].push_back(NO_TEXT_BUTTON(121, y, 23, 13, std::bind(&EditorScreen::setIV, this, statValues[i]), ui_sheet_res_null_idx));
        buttons[tab].push_back(NO_TEXT_BUTTON(146, y, 13, 13, std::bind(&EditorScreen::changeIV, this, statValues[i], true), ui_sheet_button_plus_small_idx));

        buttons[tab].push_back(NO_TEXT_BUTTON(182, y, 13, 13, std::bind(&EditorScreen::changeEV, this, statValues[i], false), ui_sheet_button_minus_small_idx));
        buttons[tab].push_back(NO_TEXT_BUTTON(197, y, 32, 13, std::bind(&EditorScreen::setEV, this, statValues[i]), ui_sheet_res_null_idx));
        buttons[tab].push_back(NO_TEXT_BUTTON(231, y, 13, 13, std::bind(&EditorScreen::changeEV, this, statValues[i], true), ui_sheet_button_plus_small_idx));
    }
    buttons[tab].push_back(NO_TEXT_BUTTON(300, 184, 15, 12, std::bind(&EditorScreen::setHP, this), ui_sheet_button_info_detail_editor_light_idx));

    tab = 2;
    buttons[tab].push_back(NO_TEXT_BUTTON(283, 211, 34, 28, std::bind(&EditorScreen::goBack, this), ui_sheet_button_back_idx));
}

void EditorScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetBottom);
    Language lang = Configuration::getInstance().language();
    Gui::sprite(ui_sheet_emulated_bg_bottom_blue, 0, 0);
    Gui::sprite(ui_sheet_bg_style_bottom_idx, 0, 0);
    Gui::sprite(ui_sheet_bar_arc_bottom_blue_idx, 0, 206);
    Gui::backgroundAnimated(GFX_BOTTOM);

    Gui::sprite(ui_sheet_textbox_name_bottom_idx, 0, 1);
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

            Gui::staticText("Level", 5, 32, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText("Nature", 5, 52, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText("Ability", 5, 72, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText("Item", 5, 92, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText("Shiny", 5, 112, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText("Pok\u00E9rus", 5, 132, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText("OT", 5, 152, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText("Nickname", 5, 172, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText("Friendship", 5, 192, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);

            Gui::ball(pkm->ball(), 4, 3);
            Gui::dynamicText(i18n::species(lang, pkm->species()), 25, 4, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE, false);
            Gui::dynamicText(107, 32, 35, std::to_string((int) pkm->level()), FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
            Gui::dynamicText(i18n::nature(lang, pkm->nature()), 95, 52, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::dynamicText(i18n::ability(lang, pkm->ability()), 95, 72, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::dynamicText(i18n::item(lang, pkm->heldItem()), 95, 92, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::dynamicText(pkm->shiny() ? "Yes" : "No", 95, 112, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::dynamicText(pkm->pkrsDays() > 0 ? "Yes" : "No", 95, 132, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
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

            Gui::staticText("STATS", 4, 32, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText(119, 32, 27, "IV", FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
            Gui::staticText(195, 32, 36, "EV", FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
            Gui::staticText(249, 32, 51, "TOTAL", FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
            Gui::staticText("HP", 4, 52, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText("Attack", 4, 72, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText("Defense", 4, 92, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText("Sp. Attack", 4, 112, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText("Sp. Defense", 4, 132, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
            Gui::staticText("Speed", 4, 152, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);

            for (int i = 0; i < 6; i++)
            {
                Gui::dynamicText(119, 52 + i * 20, 27, std::to_string((int) pkm->iv(statValues[i])), FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
                Gui::dynamicText(195, 52 + i * 20, 36, std::to_string((int) pkm->ev(statValues[i])), FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
                Gui::dynamicText(249, 52 + i * 20, 51, std::to_string((int) pkm->stat(statValues[i])), FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK);
            }
            Gui::dynamicText("Hidden Power " + i18n::hp(lang, pkm->hpType()), 295, 181, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE, true);
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

            Gui::staticText("Moves", 12, 4, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE, false);
            Gui::staticText("Relearn Moves", 12, 112, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE, false);

            for (int i = 0; i < 4; i++)
            {
                Gui::dynamicText(i18n::move(lang, pkm->move(i)), 24, 32 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
                if (pkm->gen6())
                {
                    Gui::dynamicText(i18n::move(lang, ((PK6*)pkm.get())->relearnMove(i)), 24, 141 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
                }
                else if (pkm->gen7())
                {
                    Gui::dynamicText(i18n::move(lang, ((PK7*)pkm.get())->relearnMove(i)), 24, 141 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
                }
                else
                {
                    Gui::staticText("Not applicable for this generation", 24, 141 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, false);
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
    view->draw();
}

void EditorScreen::update(touchPosition* touch)
{
    static bool dirtyBack = false;
    static int timers[38] = {-1};
    u32 downKeys = keysDown();
    u32 heldKeys = keysHeld();

    for (int i = 0; i < buttons[currentTab].size(); i++)
    {
        if (i == 0)
        {
            if (!dirtyBack)
            {
                if (buttons[currentTab][i]->clicked(touch))
                {
                    dirtyBack = true;
                }
                if (buttons[currentTab][i]->update(touch))
                {
                    return;
                }
            }
            else
            {
                if (!buttons[currentTab][i]->clicked(touch))
                {
                    dirtyBack = false;
                }
            }
        }
        else
        {
            if (timers[i] < 0)
            {
                if (buttons[currentTab][i]->update(touch))
                {
                    return;
                }
                if (buttons[currentTab][i]->clicked(touch))
                {
                    timers[i] = 10;
                }
            }
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
            pkm->fixMoves();
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

    for (int i = 0; i < 38; i++)
    {
        if (timers[i] > -1)
        {
            timers[i]--;
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
    Gui::setScreen(std::unique_ptr<Screen>(new HexEditScreen(pkm)));
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

bool EditorScreen::setLevel()
{
    // swkbd stuff
    return false;
}

bool EditorScreen::selectNature()
{
    // How are we going to do this?
    return false;
}
bool EditorScreen::selectAbility()
{
    // How are we going to do this?
    return false;
}
bool EditorScreen::selectItem()
{
    // How are we going to do this?
    return false;
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
bool EditorScreen::setOT()
{
    // swkbd stuff
    return false;
}
bool EditorScreen::setNick()
{
    // swkbd stuff
    return false;
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
bool EditorScreen::setFriendship()
{
    // swkbd stuff
    return false;
}
bool EditorScreen::save()
{
    TitleLoader::save->pkm(*pkm, box, index);
    return false;
}
bool EditorScreen::setIV(int which)
{
    // swkbd stuff
    return false;
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
bool EditorScreen::setEV(int which)
{
    // swkbd stuff
    return false;
}
bool EditorScreen::changeEV(int which, bool up)
{
    if (up)
    {
        if (pkm->ev(which) < 0xFF)
        {
            pkm->ev(which, pkm->ev(which) + 1);
        }
    }
    else
    {
        if (pkm->ev(which) > 0)
        {
            pkm->ev(which, pkm->ev(which) - 1);
        }
    }
    return false;
}
bool EditorScreen::setHP()
{
    // How are we going to do this?
    return false;
}
void EditorScreen::changeMove()
{
    // How are we going to do this?
}