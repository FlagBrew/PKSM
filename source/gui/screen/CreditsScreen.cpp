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

#include "CreditsScreen.hpp"
#include "gui.hpp"
#include "MainMenu.hpp"
#include "Configuration.hpp"

static const char* credits = 
R"(Naxann and Anty-Lemon for various contributions
Kaphotics and SciresM for PKHeX and memecrypto
J-K-D for direct save import/export
Astronautlevel for QR code support
ArchitDate for serveLegality
Slownic and zaksabeast for servepkx
Slashcash for PCHex++
TuxSH for TWLSaveTool
ProjectPokemon.org for most of the wondercards
Simona for being my best supporter
All the countless translators who worked on the localization
All the contributors on Github
Everyone supporting the development

www.github.com/BernardoGiordano/PKSM)";

void CreditsScreen::update(touchPosition* touch)
{
    Screen::update();
    if (touch) {}
    if (hidKeysDown() & KEY_B)
    {
        Gui::screenBack();
    }
}

void CreditsScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    Gui::backgroundTop(false);
    // Not sure what to put on the top
    Gui::staticText(GFX_TOP, 4, "Credits", FONT_SIZE_14, FONT_SIZE_14, COLOR_BLUE);

    C2D_SceneBegin(g_renderTargetBottom);
    Gui::backgroundBottom(false);

    Gui::staticText(credits, 20, 30, FONT_SIZE_9, FONT_SIZE_9, COLOR_LIGHTBLUE);
    Gui::staticText(GFX_BOTTOM, 225, "Press B to return", FONT_SIZE_9, FONT_SIZE_9, COLOR_LIGHTBLUE);
}