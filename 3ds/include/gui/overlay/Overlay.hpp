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

#ifndef OVERLAY_HPP
#define OVERLAY_HPP

#include "Instructions.hpp"
#include <3ds.h>
#include <memory>

class Screen;

class Overlay
{
public:
    Overlay(Screen& screen, const std::string& instructions = "");
    Overlay(Overlay& overlay, const std::string& instructions = "");
    virtual ~Overlay() {}
    template<typename Class, typename... Params>
    void addOverlay(Params&&... args)
    {
        if (overlay)
        {
            overlay->addOverlay<Class>(std::forward<Params>(args)...);
        }
        else
        {
            overlay = std::make_shared<Class>(*this, std::forward<Params>(args)...);
        }
    }
    bool willHandleUpdate() const;
    bool willReplaceBottom() const;
    bool willReplaceTop() const;
    void doUpdate(touchPosition* touch);
    void doTopDraw() const;
    void doBottomDraw() const;
    virtual void update(touchPosition* touch) = 0;
    virtual void drawTop() const              = 0;
    virtual void drawBottom() const           = 0;
    virtual bool replacesTop() const { return false; }
    virtual bool replacesBottom() const { return false; }
    virtual bool handlesUpdate() const { return true; }
    void dim(void) const;
    const Instructions& getInstructions() const { return instructions; }

protected:
    std::shared_ptr<Overlay>& me;
    std::shared_ptr<Overlay> overlay = nullptr;
    Instructions instructions;
};

#endif
