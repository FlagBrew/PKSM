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

#include "Presenter.hpp"
#include <mutex>
#include <utility>
#include <vector>

namespace
{
    // Guards the installed presenter and the pre-install queue. Held only long enough to
    // read the pointer or move the queue out: showing a report can block for frames, and
    // the domain thread that reported it must not be able to hold up another one.
    std::mutex mutex;
    std::shared_ptr<pksm::Presenter> presenter;

    // Only startup reports land here, and only until App::init installs the GUI adapter.
    // The cap is a backstop against a module that reports in a loop with nothing to drain
    // it; losing the tail of such a run is better than growing without bound.
    constexpr size_t MAX_QUEUED = 16;
    std::vector<pksm::Report> queued;

    // The answer that changes nothing. Every question the domain asks is a request to do
    // something more destructive than the default, so "no" is always the safe reply, and
    // that is what a caller gets when there is nobody to ask.
    constexpr bool SAFE_ANSWER = false;

    std::shared_ptr<pksm::Presenter> current()
    {
        std::scoped_lock lock(mutex);
        return presenter;
    }
}

namespace pksm::present
{
    void show(Notice notice, s32 code, std::string detail)
    {
        Report report{notice, code, std::move(detail)};
        if (auto p = current())
        {
            p->show(report);
            return;
        }

        std::scoped_lock lock(mutex);
        if (queued.size() < MAX_QUEUED)
        {
            queued.emplace_back(std::move(report));
        }
    }

    bool ask(Question question)
    {
        if (auto p = current())
        {
            return p->ask(question);
        }
        return SAFE_ANSWER;
    }

    void busy(Task task)
    {
        if (auto p = current())
        {
            p->busy(task);
        }
    }

    void progress(Progress bar, u32 partial, u32 total)
    {
        if (auto p = current())
        {
            p->progress(bar, partial, total);
        }
    }

    void install(std::unique_ptr<Presenter> next)
    {
        std::vector<Report> backlog;
        std::shared_ptr<Presenter> installed;
        {
            std::scoped_lock lock(mutex);
            presenter = std::move(next);
            installed = presenter;
            backlog   = std::exchange(queued, {});
        }

        if (!installed)
        {
            return;
        }
        // Drained outside the lock: each of these can be a modal that runs its own frames.
        for (const auto& report : backlog)
        {
            installed->show(report);
        }
    }

    void uninstall()
    {
        std::shared_ptr<Presenter> dying;
        {
            std::scoped_lock lock(mutex);
            // Kept alive past the lock so the destructor does not run under it.
            dying = std::exchange(presenter, nullptr);
            queued.clear();
        }
    }
}
