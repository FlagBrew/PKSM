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

// The seam between the domain and the screens, exercised with no screens at all. If this
// links, the domain side of the seam really is free of citro2d.

#include "Presenter.hpp"
#include <cstdio>
#include <utility>
#include <vector>

namespace
{
    int failures = 0;

    void check(bool condition, const char* what)
    {
        if (!condition)
        {
            failures++;
            std::printf("FAIL: %s\n", what);
        }
    }

    // The headless adapter: it writes down what it was asked to show instead of drawing it.
    class Recorder : public pksm::Presenter
    {
    public:
        explicit Recorder(bool answer = false) : answer(answer) {}

        void show(const pksm::Report& report) override { shown.push_back(report); }

        bool ask(pksm::Question question) override
        {
            asked.push_back(question);
            return answer;
        }

        void busy(pksm::Task task) override { busied.push_back(task); }

        void progress(pksm::Progress bar, u32 partial, u32 total) override
        {
            progressed.push_back({bar, partial, total});
        }

        struct Step
        {
            pksm::Progress bar;
            u32 partial;
            u32 total;
        };

        std::vector<pksm::Report> shown;
        std::vector<pksm::Question> asked;
        std::vector<pksm::Task> busied;
        std::vector<Step> progressed;

    private:
        bool answer;
    };

    // Handed to install(), which takes ownership; the raw pointer stays valid until the
    // next install() or uninstall().
    Recorder* install(bool answer = false)
    {
        auto owned = std::make_unique<Recorder>(answer);
        auto* raw  = owned.get();
        pksm::present::install(std::move(owned));
        return raw;
    }

    void reportsBeforeInstallSurviveUntilThereIsSomethingToDrawWith()
    {
        pksm::present::uninstall();
        pksm::present::show(pksm::Notice::ConfigCorrupt);
        pksm::present::show(pksm::Notice::ConfigBadFormat);

        auto* recorder = install();
        check(recorder->shown.size() == 2, "both queued reports are drained on install");
        check(recorder->shown[0].notice == pksm::Notice::ConfigCorrupt &&
                  recorder->shown[1].notice == pksm::Notice::ConfigBadFormat,
            "the backlog is drained in the order it was reported");
        pksm::present::uninstall();
    }

    void theBacklogIsDrainedOnlyOnce()
    {
        pksm::present::uninstall();
        pksm::present::show(pksm::Notice::BankCorrupt);

        install();
        auto* second = install();
        check(second->shown.empty(), "a second install has nothing left to drain");
        pksm::present::uninstall();
    }

    void detailAndCodeCrossTheSeamIntact()
    {
        pksm::present::uninstall();
        auto* recorder = install();
        pksm::present::show(pksm::Notice::SaveOpenFailed, -42);
        pksm::present::show(pksm::Notice::GpssDownloadCode, 0, "ABCDE");

        check(recorder->shown.size() == 2, "reports reach an installed presenter directly");
        check(recorder->shown[0].code == -42, "an error code survives the seam");
        check(recorder->shown[1].detail == "ABCDE", "a quoted string survives the seam");
        pksm::present::uninstall();
    }

    void questionsTakeTheSafeAnswerWithNobodyToAsk()
    {
        pksm::present::uninstall();
        check(!pksm::present::ask(pksm::Question::SaveBankWithoutBackup),
            "no presenter means no permission to skip the backup");
        check(!pksm::present::ask(pksm::Question::OverwriteCardSave),
            "no presenter means no permission to overwrite");

        auto* recorder = install(true);
        check(pksm::present::ask(pksm::Question::OverwriteCardSave),
            "an installed presenter answers for itself");
        check(recorder->asked.size() == 1, "the question reached the presenter exactly once");
        pksm::present::uninstall();
    }

    void busyAndProgressAreDroppedRatherThanQueued()
    {
        pksm::present::uninstall();
        // A busy indicator for work that has already finished is worse than none: these
        // are dropped, not held, so nothing stale is drawn once the GUI comes up.
        pksm::present::busy(pksm::Task::BankLoad);
        pksm::present::progress(pksm::Progress::RestoreSave, 1, 2);

        auto* recorder = install();
        check(recorder->busied.empty(), "a busy indicator raised before install is not replayed");
        check(recorder->progressed.empty(), "progress raised before install is not replayed");

        pksm::present::busy(pksm::Task::BankSave);
        pksm::present::progress(pksm::Progress::RestoreSave, 3, 4);
        check(recorder->busied.size() == 1 && recorder->busied[0] == pksm::Task::BankSave,
            "an installed presenter gets the busy indicator");
        check(recorder->progressed.size() == 1 && recorder->progressed[0].partial == 3 &&
                  recorder->progressed[0].total == 4,
            "an installed presenter gets the progress figures");
        pksm::present::uninstall();
    }

    void theBacklogIsBounded()
    {
        pksm::present::uninstall();
        for (int i = 0; i < 100; i++)
        {
            pksm::present::show(pksm::Notice::BankCorrupt);
        }

        auto* recorder = install();
        check(!recorder->shown.empty(), "a bounded backlog still keeps the first reports");
        check(recorder->shown.size() < 100, "a module reporting in a loop cannot grow the backlog");
        pksm::present::uninstall();
    }

    void uninstallGoesBackToQueueing()
    {
        pksm::present::uninstall();
        install();
        pksm::present::uninstall();

        pksm::present::show(pksm::Notice::BankCritical);
        auto* recorder = install();
        check(recorder->shown.size() == 1, "reports queue again after uninstall");
        pksm::present::uninstall();
    }
}

int main()
{
    reportsBeforeInstallSurviveUntilThereIsSomethingToDrawWith();
    theBacklogIsDrainedOnlyOnce();
    detailAndCodeCrossTheSeamIntact();
    questionsTakeTheSafeAnswerWithNobodyToAsk();
    busyAndProgressAreDroppedRatherThanQueued();
    theBacklogIsBounded();
    uninstallGoesBackToQueueing();

    if (failures == 0)
    {
        std::printf("Presenter: all checks passed\n");
        return 0;
    }
    std::printf("Presenter: %d check(s) failed\n", failures);
    return 1;
}
