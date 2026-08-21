/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2026 Bernardo Giordano, Admiral Fish, piepie62
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

#include "ScreenStack.hpp"
#include "Screen.hpp"
#include <cstddef>
#include <utility>
#include <vector>

namespace
{
    struct PendingChange
    {
        // A Screen means push; nullptr means pop.
        std::unique_ptr<Screen> screen;
    };

    std::vector<std::unique_ptr<Screen>> screens;
    std::vector<PendingChange> pendingChanges;

    bool replacementSurvives(const std::vector<PendingChange>& changes, std::size_t currentChange)
    {
        std::size_t depth = 0;
        for (std::size_t i = currentChange + 1; i < changes.size(); i++)
        {
            if (changes[i].screen)
            {
                depth++;
            }
            else if (depth > 0)
            {
                depth--;
            }
        }
        return depth > 0;
    }
}

void ScreenStack::push(std::unique_ptr<Screen> screen)
{
    if (screen)
    {
        pendingChanges.push_back({std::move(screen)});
    }
}

void ScreenStack::requestPop(void)
{
    pendingChanges.push_back({nullptr});
}

bool ScreenStack::applyPending(void)
{
    bool changed = false;
    // A Screen destructor may itself enqueue work. Keep that work in the new queue for the next
    // frame instead of invalidating the changes currently being applied.
    auto changes = std::move(pendingChanges);
    pendingChanges.clear();

    for (std::size_t i = 0; i < changes.size(); i++)
    {
        if (changes[i].screen)
        {
            screens.push_back(std::move(changes[i].screen));
            changed = true;
        }
        else if (!screens.empty() && (screens.size() > 1 || replacementSurvives(changes, i)))
        {
            screens.pop_back();
            changed = true;
        }
    }
    return changed;
}

Screen* ScreenStack::top(void)
{
    return screens.empty() ? nullptr : screens.back().get();
}
