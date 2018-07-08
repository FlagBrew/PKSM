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

void CreditsScreen::update(touchPosition* touch)
{
    if (touch) {}
    if (hidKeysDown() & KEY_B)
    {
        Gui::setScreen(new MainMenu);
    }
}

void CreditsScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    Gui::backgroundTop();
    // Not sure what to put on the top
    Gui::staticText(GFX_TOP, 4, "Credits", FONT_SIZE_14, FONT_SIZE_14, COLOR_BLUE);

    C2D_SceneBegin(g_renderTargetBottom);
    Gui::backgroundBottom();
    Gui::dynamicText("Naxann and Anty-Lemon for various contributions\nKaphotics and SciresM for PKHeX and memecrypto\nJ-K-D for direct save import/export\nAstronautlevel for QR code support\nArchitDate for serveLegality\nSlownic and zaksabeast for servepkx\nSlashcash for PCHex++\nTuxSH for TWLSaveTool\nProjectPokemon.org for most of the wondercards\nSimona for being my best supporter\nAll the countless translators who worked on the localization\nAll the contributors on Github\nEveryone supporting the development\n\nwww.github.com/BernardoGiordano/PKSM", 20, 30, FONT_SIZE_9, FONT_SIZE_9, COLOR_LIGHTBLUE);
    Gui::staticText(GFX_BOTTOM, 225, "Press B to return", FONT_SIZE_9, FONT_SIZE_9, COLOR_LIGHTBLUE);
}