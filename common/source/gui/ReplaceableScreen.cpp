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

#include "ReplaceableScreen.hpp"
#include "gui.hpp"

void ReplaceableScreen::dim() const
{
#if defined(_3DS)
    Gui::drawSolidRect(0, 0, 400, 240, COLOR_MASKBLACK);
#elif defined(__SWITCH__)
    Gui::drawSolidRect(0, 0, 1080, 720, COLOR_MASKBLACK);
#endif
}

#if defined(_3DS)
void ReplaceableScreen::doTopDraw() const
{
    if (overlay)
    {
        if (!overlay->willReplaceTop())
        {
            drawTop();
        }
        overlay->doTopDraw();
    }
    else
    {
        drawTop();
    }
}

void ReplaceableScreen::doBottomDraw() const
{
    if (overlay)
    {
        if (!overlay->willReplaceBottom())
        {
            drawBottom();
        }
        overlay->doBottomDraw();
    }
    else
    {
        drawBottom();
    }
}

bool ReplaceableScreen::willReplaceBottom() const
{
    if (overlay && overlay->willReplaceBottom())
    {
        return true;
    }
    else
    {
        return replacesBottom();
    }
}

bool ReplaceableScreen::willReplaceTop() const
{
    if (overlay && overlay->willReplaceTop())
    {
        return true;
    }
    else
    {
        return replacesTop();
    }
}
#elif defined(__SWITCH__)
void ReplaceableScreen::doDraw() const
{
    if (overlay)
    {
        if (!overlay->willReplaceScreen())
        {
            draw();
        }
        overlay->doDraw();
    }
    else
    {
        draw();
    }
}

bool ReplaceableScreen::willReplaceScreen() const
{
    if (overlay)
    {
        return overlay->willReplaceScreen();
    }
    else
    {
        return replacesScreen();
    }
}
#endif

void ReplaceableScreen::doUpdate(touchPosition* touch)
{
    if (overlay && overlay->willHandleUpdate())
    {
        overlay->doUpdate(touch);
    }
    else
    {
        update(touch);
    }
}

bool ReplaceableScreen::willHandleUpdate() const
{
    if (overlay && overlay->willHandleUpdate())
    {
        return true;
    }
    else
    {
        return handlesUpdate();
    }
}
