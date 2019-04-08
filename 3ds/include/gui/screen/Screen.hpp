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

#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <3ds.h>
#include <citro3d.h>
#include <memory>
#include "Overlay.hpp"
#include "Instructions.hpp"

extern C3D_RenderTarget* g_renderTargetTop;
extern C3D_RenderTarget* g_renderTargetBottom;

enum ScreenType
{
    TITLELOAD,
    MAINMENU,
    STORAGE,
    EDITOR,
    EDITSELECT,
    EVENTS,
    HEXEDIT,
    INJECTOR,
    SCRIPTS,
    SCRIPTSELECT,
    SELECTOR,
    SPECIES_SELECT,
    SETTINGS,
    VIEWER,
    BAG,
    ITEMEDIT
};

class Screen
{
friend class Overlay;
public:
    virtual ~Screen() {}
    virtual void update(void) {
        // increase timer
        mTimer += 0.025f;
    }
    // Call currentOverlay->update if it exists, and update if it doesn't
    virtual void doUpdate(touchPosition* touch) final;
    virtual void update(touchPosition* touch) = 0;
    virtual ScreenType type() const = 0;
    // Call draw, then currentOverlay->draw if it exists
    virtual void doDraw() const final;
    virtual void draw() const = 0;
    virtual float timer() const final { return mTimer; }
    void removeOverlay() { currentOverlay = nullptr; }
    void setOverlay(std::shared_ptr<Overlay>& overlay) { currentOverlay = overlay; }
    const Instructions& getInstructions() const { return currentOverlay ? currentOverlay->getInstructions() : instructions; }

protected:
    // No point in restricting this to only being editable during update, especially since it's drawn afterwards. Allows setting it before the first draw loop is done
    mutable std::shared_ptr<Overlay> currentOverlay = nullptr;
    // Should only be changed in constructor
    Instructions instructions = Instructions("Hello");

private:
    float mTimer = 0;
};

#endif
