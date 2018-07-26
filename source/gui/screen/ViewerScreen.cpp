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

#include "ViewerScreen.hpp"
#include "gui.hpp"

void ViewerScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    std::array<std::string, 16> labels = {"Localize me"}; // TODO: entries & values; i18n::localize
    int y_desc = 29;
    
    Gui::backgroundAnimated(GFX_TOP);
    Gui::sprite(ui_spritesheet_res_box_top_idx, 0, 2);

    Gui::sprite(pkm->move(0) ? ui_spritesheet_res_bar_move_idx : ui_spritesheet_res_bar_no_move_idx, 252, 155);
    Gui::sprite(pkm->move(1) ? ui_spritesheet_res_bar_move_idx : ui_spritesheet_res_bar_no_move_idx, 252, 176);
    Gui::sprite(pkm->move(2) ? ui_spritesheet_res_bar_move_idx : ui_spritesheet_res_bar_no_move_idx, 252, 197);
    Gui::sprite(pkm->move(3) ? ui_spritesheet_res_bar_move_idx : ui_spritesheet_res_bar_no_move_idx, 252, 218);
    
    Gui::dynamicText("Moves", 251, 138, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLUE); // TODO: i18n::localize
    for (u8 i = 0; i < 10; i++)
    {
        if (i == 8 && pkm->egg())
        {
            Gui::dynamicText("Egg Cycle", 2, y_desc, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLUE); // TODO: i18n::localize
        }
        else
        {
            Gui::dynamicText(labels[i], 2, y_desc, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLUE);
        }
        y_desc += 20;
        if (i == 2) y_desc += 5;
        if (i == 5) y_desc += 6;
    }

    y_desc = 8;
    for (u8 i = 10; i < 16; i++)
    {
        Gui::dynamicText(labels[i], 225, y_desc, FONT_SIZE_12, FONT_SIZE_12, COLOR_LIGHTBLUE);
        y_desc += 20;
    }

    if (pkm != NULL && pkm->species() > 0)
    {
        Gui::ball(pkm->ball(), -2, -5);
        Gui::dynamicText(i18n::species(pkm->language(), pkm->species()), 30, 6, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        
        Gui::generation(pkm.get(), 134, 8);

        if (pkm->gender() == 0)
        {
            Gui::sprite(ui_spritesheet_res_male_idx, 146, 7);
        }
        else if (pkm->gender() == 1)
        {
            Gui::sprite(ui_spritesheet_res_female_idx, 148, 7);
        }

        std::string toDraw;

        toDraw = "Level " + std::to_string(pkm->level()); // TODO: i18n::localize
        Gui::dynamicText(toDraw, 160, 6, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);

        toDraw = pkm->nickname();
        Gui::dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 29, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        toDraw = pkm->otName();
        Gui::dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 49, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);

        toDraw = pkm->pkrsStrain() ? "Yes" : "No"; // TODO: i18n::localize
        Gui::dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 69, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        toDraw = i18n::nature(pkm->language(), pkm->nature());
        Gui::dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 94, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        toDraw = i18n::ability(pkm->language(), pkm->ability());
        Gui::dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 114, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        toDraw = i18n::item(pkm->language(), pkm->heldItem());
        Gui::dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 134, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);

        if (pkm->shiny())
        {
            Gui::sprite(ui_spritesheet_res_shiny_idx, 205, 9);
        }

        if (pkm->egg())
        {
            toDraw = std::to_string(pkm->otFriendship());
        }
        else
        {
            toDraw = std::to_string(pkm->currentFriendship()) + " / " + std::to_string(pkm->otFriendship());
        }
        Gui::dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 200, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);

        toDraw = std::to_string(pkm->PSV()) + " / " + std::to_string(pkm->TSV());
        Gui::dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 160, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        toDraw = std::to_string(pkm->TID()) + " / " + std::to_string(pkm->SID());
        Gui::dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 180, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);

        toDraw = i18n::hp(pkm->language(), pkm->hpType());
        Gui::dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 220, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        
        int max = textWidth("252", FONT_SIZE_12);
        u16 y_moves = 159;
        for (u8 i = 0; i < 4; i++)
        {
            if (pkm->move(i))
            {
                toDraw = i18n::move(pkm->language(), pkm->move(i));
                Gui::dynamicText(toDraw, 396 - textWidth(toDraw, FONT_SIZE_12), y_moves, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
            }
            y_moves += 21;
        }

        for (u8 i = 0; i < 6; i++)
        {
            toDraw = std::to_string(pkm->iv(i));
            Gui::dynamicText(toDraw, 289 + (max - textWidth(toDraw, FONT_SIZE_12)) / 2, 8 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
            toDraw = std::to_string(pkm->ev(i));
            Gui::dynamicText(toDraw, 328 + (max - textWidth(toDraw, FONT_SIZE_12)) / 2, 8 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
            toDraw = std::to_string(pkm->stat(i));
            Gui::dynamicText(toDraw, 369 + (max - textWidth(toDraw, FONT_SIZE_12)) / 2, 8 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        }
    }
}

/*
void Gui::pkmInfoViewer(PKX* pkm)
{
    C2D_SceneBegin(g_renderTargetTop);
    std::vector<std::string> labels = {"Localize me"}; // TODO: entries & values; i18n::localize
    int y_desc = 29;
    
    backgroundAnimated(GFX_TOP);
    sprite(ui_spritesheet_res_info_top_idx, 0, 2);

    sprite(pkm->move(0) ? ui_spritesheet_res_bar_move_idx : ui_spritesheet_res_bar_no_move_idx, 252, 155);
    sprite(pkm->move(1) ? ui_spritesheet_res_bar_move_idx : ui_spritesheet_res_bar_no_move_idx, 252, 155);
    sprite(pkm->move(2) ? ui_spritesheet_res_bar_move_idx : ui_spritesheet_res_bar_no_move_idx, 252, 155);
    sprite(pkm->move(3) ? ui_spritesheet_res_bar_move_idx : ui_spritesheet_res_bar_no_move_idx, 252, 155);
    
    dynamicText("Moves", 251, 138, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLUE); // TODO: i18n::localize
    for (int i = 0; i < 10; i++)
    {
        if (i == 8 && pkm->egg())
        {
            dynamicText("Egg Cycle", 2, y_desc, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLUE); // TODO: i18n::localize
        }
        else
        {
            dynamicText(labels[i], 2, y_desc, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLUE);
        }
        y_desc += 20;
        if (i == 2) y_desc += 5;
        if (i == 5) y_desc += 6;
    }

    y_desc = 8;
    for (int i = 10; i < 16; i++)
    {
        dynamicText(labels[i], 225, y_desc, FONT_SIZE_12, FONT_SIZE_12, COLOR_LIGHTBLUE);
        y_desc += 20;
    }

    if (pkm != NULL && pkm->species() > 0)
    {
        ball(pkm->ball(), -2, -5);
        dynamicText(i18n::species(pkm->language(), pkm->species()), 30, 6, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        
        printGen(pkm, 134, 8);

        if (pkm->gender() == 0)
        {
            sprite(ui_spritesheet_res_male_idx, 146, 7);
        }
        else if (pkm->gender() == 1)
        {
            sprite(ui_spritesheet_res_female_idx, 148, 7);
        }

        std::string toDraw;

        toDraw = "Level " + std::to_string(pkm->level()); // TODO: i18n::localize
        dynamicText(toDraw, 160, 6, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);

        toDraw = pkm->nickname();
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 29, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        toDraw = pkm->otName();
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 49, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);

        toDraw = pkm->pkrsStrain() ? "Yes" : "No"; // TODO: i18n::localize
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 69, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        toDraw = i18n::nature(pkm->language(), pkm->nature());
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 94, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        toDraw = i18n::ability(pkm->language(), pkm->ability());
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 114, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        toDraw = i18n::item(pkm->language(), pkm->heldItem());
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 134, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);

        if (pkm->shiny())
        {
            sprite(ui_spritesheet_res_shiny_idx, 205, 9);
        }

        if (pkm->egg())
        {
            toDraw = std::to_string(pkm->otFriendship());
        }
        else
        {
            toDraw = std::to_string(pkm->currentFriendship()) + " / " + std::to_string(pkm->otFriendship());
        }
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 200, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);

        toDraw = std::to_string(pkm->PSV()) + " / " + std::to_string(pkm->TSV());
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 160, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        toDraw = std::to_string(pkm->TID()) + " / " + std::to_string(pkm->SID());
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 180, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);

        toDraw = i18n::hp(pkm->language(), pkm->hpType());
        dynamicText(toDraw, 215 - textWidth(toDraw, FONT_SIZE_12), 220, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        
        int max = textWidth("252", FONT_SIZE_12);
        int y_moves = 159;
        for (int i = 0; i < 4; i++)
        {
            if (pkm->move(i))
            {
                toDraw = i18n::move(pkm->language(), pkm->move(i));
                dynamicText(toDraw, 396 - textWidth(toDraw, FONT_SIZE_12), y_moves, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
            }
            y_moves += 21;
        }

        for (int i = 0; i < 6; i++)
        {
            toDraw = std::to_string(pkm->iv(i));
            dynamicText(toDraw, 289 + (max - textWidth(toDraw, FONT_SIZE_12)) / 2, 8 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
            toDraw = std::to_string(pkm->ev(i));
            dynamicText(toDraw, 328 + (max - textWidth(toDraw, FONT_SIZE_12)) / 2, 8 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
            toDraw = std::to_string(pkm->stat(i));
            dynamicText(toDraw, 369 + (max - textWidth(toDraw, FONT_SIZE_12)) / 2, 8 + i * 20, FONT_SIZE_12, FONT_SIZE_12, COLOR_WHITE);
        }
    }
}
*/