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
        C2D_TargetClear(g_renderTargetTop, COLOR_BLACK);
        C2D_TargetClear(g_renderTargetBottom, COLOR_BLACK);

        draw();
        touchPosition touch;
        hidTouchRead(&touch);
        update(&touch);

        C3D_FrameEnd(0);
        Gui::clearTextBufs();
    }
    return finalVal;
}

void ScriptChoice::drawBottom() const
{
    C2D_SceneBegin(g_renderTargetBottom);
    Gui::backgroundBottom(false);
    std::vector<std::string> lines;
    std::string tmp = question;
    while (tmp.find('\n') != std::string::npos)
    {
        lines.push_back(tmp.substr(0, tmp.find('\n')));
        tmp = tmp.substr(tmp.find('\n') + 1);
    }
    // Add the final line
    lines.push_back(tmp);
    int y = 115 - 10 * lines.size();
    for (size_t i = 0; i < lines.size(); i++)
    {
        y += 20 * i;
        Gui::dynamicText(GFX_BOTTOM, y, lines[i], FONT_SIZE_18, FONT_SIZE_18, COLOR_WHITE);
    }
}