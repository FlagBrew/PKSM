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

#ifndef REPLACEABLESCREEN_HPP
#define REPLACEABLESCREEN_HPP

#include "Instructions.hpp"
#include <memory>

class ReplaceableScreen
{
public:
    ReplaceableScreen(ReplaceableScreen* parent, const std::string& instructions = "")
        : parent(parent), instructions(instructions)
    {
    }
    virtual ~ReplaceableScreen() = default;
    template <typename Class, typename... Params>
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
    void doUpdate(touchPosition* touch);
#if defined(__3DS__)
    bool willReplaceBottom() const;
    bool willReplaceTop() const;
    void doTopDraw() const;
    void doBottomDraw() const;
    virtual void drawTop() const    = 0;
    virtual void drawBottom() const = 0;
    virtual bool replacesTop() const { return false; }
    virtual bool replacesBottom() const { return false; }
#elif defined(__SWITCH__)
    bool willReplaceScreen() const;
    void doDraw() const;
    virtual void draw() const = 0;
    virtual bool replacesScreen() const { return false; }
#endif
    virtual void update(touchPosition* touch) = 0;
    virtual bool handlesUpdate() const { return true; }
    void removeOverlay()
    {
        if (overlay->overlay)
        {
            overlay->removeOverlay();
        }
        else
        {
            overlay = nullptr;
        }
    }
    void dim(void) const;
    const Instructions& getInstructions() const
    {
        return (overlay && !overlay->getInstructions().empty()) ? overlay->getInstructions()
                                                                : instructions;
    }

protected:
    ReplaceableScreen* parent;
    std::shared_ptr<ReplaceableScreen> overlay = nullptr;
    Instructions instructions;
};

#endif
