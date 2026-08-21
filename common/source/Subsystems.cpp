/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2025 Bernardo Giordano, Admiral Fish, piepie62
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

#include "Subsystems.hpp"
#include <algorithm>

namespace pksm
{
    bool Subsystems::record(std::string_view name, Status status, Teardown down)
    {
        if (failed(status))
        {
            // The teardown is dropped on the floor: `up` did not come up, so there is
            // nothing for it to take down. What is already held stays held, and unwinds
            // when the caller releases.
            mFailure = Failure{std::string(name), status};
            return false;
        }

        mHeld.push_back(Held{std::string(name), std::move(down)});
        if (mObserver)
        {
            mObserver(mHeld.back().name, Event::Acquired);
        }
        return true;
    }

    void Subsystems::releaseAll() noexcept
    {
        while (!mHeld.empty())
        {
            // Pop before releasing, so a teardown that re-enters this object cannot see a
            // subsystem that is already on its way down, and cannot run one twice.
            Held held = std::move(mHeld.back());
            mHeld.pop_back();
            if (mObserver)
            {
                mObserver(held.name, Event::Released);
            }
            if (held.down)
            {
                held.down();
            }
        }
    }

    bool Subsystems::holds(std::string_view name) const
    {
        return std::any_of(
            mHeld.begin(), mHeld.end(), [name](const Held& held) { return held.name == name; });
    }
}
