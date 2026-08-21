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

#ifndef SUBSYSTEMS_HPP
#define SUBSYSTEMS_HPP

#include "utils/coretypes.h"
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

// Subsystem lifetime, and nothing else.
//
// Bringing PKSM up is an ordered sequence in which every step depends on the ones before it,
// and taking it down is that same sequence reversed. Written out as two separate lists, the
// second is only a memory of the first: it drifts as steps are added, it runs against
// subsystems that never came up, and a failure partway up skips it entirely.
//
// Here, acquiring a subsystem records how to release it. Whatever came up goes back down, in
// reverse, exactly once - whether the sequence finished or stopped on its third step. No
// libctru and no GUI, so the ordering rule can be exercised off-device.
namespace pksm
{
    class Subsystems
    {
    public:
        // What a subsystem reported, following libctru's Result convention: negative is
        // failure. An `up` returning bool or nothing at all is mapped onto this, so an init
        // of any of the three shapes can be handed over unchanged.
        using Status = s32;

        static constexpr Status OK      = 0;
        static constexpr Status GENERIC = -1;

        static constexpr bool failed(Status status) { return status < 0; }

        // The first step that did not come up. Every later step is skipped, so this is the
        // only failure there is to report.
        struct Failure
        {
            std::string name;
            Status status = GENERIC;
        };

        enum class Event
        {
            Acquired,
            Released,
        };

        // Called after a subsystem comes up and before it goes back down. The 3DS build
        // wires this to the startup log; nothing in this module needs it.
        using Observer = std::function<void(std::string_view name, Event event)>;

        using Teardown = std::function<void()>;

        Subsystems() = default;

        ~Subsystems() { releaseAll(); }

        Subsystems(const Subsystems&)            = delete;
        Subsystems& operator=(const Subsystems&) = delete;

        void observe(Observer observer) { mObserver = std::move(observer); }

        // Bring `name` up with `up`; if that works, record `down` as the way to take it back
        // down. Returns whether it came up, so a sequence chains with && and stops at the
        // first failure. Once a step has failed, later calls do nothing at all: `up` is not
        // run and nothing is recorded.
        template <typename Up, typename Down>
        bool acquire(std::string_view name, Up&& up, Down&& down)
        {
            if (mFailure)
            {
                return false;
            }
            const Status status = invokeUp(std::forward<Up>(up));
            return record(name, status, Teardown{std::forward<Down>(down)});
        }

        // A step with nothing of its own to release: a check, a download, an initialization
        // some other subsystem's teardown already covers. It still takes its place in the
        // sequence, so the log and holds() show it.
        template <typename Up>
        bool acquire(std::string_view name, Up&& up)
        {
            if (mFailure)
            {
                return false;
            }
            const Status status = invokeUp(std::forward<Up>(up));
            return record(name, status, Teardown{});
        }

        // Release everything held, in reverse order of acquisition, then forget it.
        // Idempotent: calling it again releases nothing, and so does the destructor.
        void releaseAll() noexcept;

        const std::optional<Failure>& failure() const { return mFailure; }

        bool holds(std::string_view name) const;

        std::size_t size() const { return mHeld.size(); }

    private:
        struct Held
        {
            std::string name;
            Teardown down;
        };

        // Normalize the three shapes an init comes in. A bool `up` says whether it worked; a
        // void `up` cannot fail; anything else is already a Result.
        template <typename Up>
        static Status invokeUp(Up&& up)
        {
            using Reported = std::invoke_result_t<Up>;
            if constexpr (std::is_void_v<Reported>)
            {
                std::invoke(std::forward<Up>(up));
                return OK;
            }
            else if constexpr (std::is_same_v<std::remove_cv_t<Reported>, bool>)
            {
                return std::invoke(std::forward<Up>(up)) ? OK : GENERIC;
            }
            else
            {
                return static_cast<Status>(std::invoke(std::forward<Up>(up)));
            }
        }

        bool record(std::string_view name, Status status, Teardown down);

        std::vector<Held> mHeld;
        std::optional<Failure> mFailure;
        Observer mObserver;
    };
}

#endif
