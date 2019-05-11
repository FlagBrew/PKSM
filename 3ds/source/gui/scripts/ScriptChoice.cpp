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

#include "ScriptChoice.hpp"
#include "gui.hpp"

int ScriptChoice::run()
{
    while (aptMainLoop() && !finished())
    {
        hidScanInput();
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

        extern C3D_RenderTarget* g_renderTargetTop;
        C2D_SceneBegin(g_renderTargetTop);
        C2D_TargetClear(g_renderTargetTop, COLOR_BLACK);
        drawTop();

        extern C3D_RenderTarget* g_renderTargetBottom;
        C2D_SceneBegin(g_renderTargetBottom);
        C2D_TargetClear(g_renderTargetBottom, COLOR_BLACK);
        drawBottom();

        touchPosition touch;
        hidTouchRead(&touch);
        update(&touch);

        if (!aptIsHomeAllowed() && aptIsHomePressed())
        {
            Gui::setDoHomeDraw();
        }

        Gui::drawNoHome();

        C3D_FrameEnd(0);
    }
    return finalVal;
}

void ScriptChoice::drawBottom() const
{
    Gui::backgroundBottom(false);
    Gui::text(question, 160, 120, FONT_SIZE_18, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
}
