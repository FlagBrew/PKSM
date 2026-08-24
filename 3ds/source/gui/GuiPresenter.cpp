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

#include "GuiPresenter.hpp"
#include "Configuration.hpp"
#include "gui.hpp"
#include "utils/format.hpp"
#include <string>

namespace
{
    // A notice's wording, and whether it is shown with the code the domain reported.
    struct Rendered
    {
        std::string message;
        bool withCode = false;
    };

    Rendered render(const pksm::Report& report)
    {
        using pksm::Notice;
        switch (report.notice)
        {
            case Notice::BankCorrupt:
                return {i18n::localize("BANK_CORRUPT")};
            case Notice::BankFromNewerVersion:
            case Notice::ConfigFromNewerVersion:
                return {i18n::localize("THE_FUCK") + '\n' + i18n::localize("DO_NOT_DOWNGRADE")};
            case Notice::BankNameWriteFailed:
                return {i18n::localize("BANK_NAME_ERROR"), true};
            case Notice::BankWriteFailed:
                return {i18n::localize("BANK_SAVE_ERROR"), true};
            case Notice::BankConvertFailed:
                return {i18n::localize("BANK_BAD_CONVERT"), true};
            case Notice::BankCritical:
                return {i18n::localize("CRITICAL_BANK_ERROR_1") + '\n' +
                        i18n::localize("CRITICAL_BANK_ERROR_2")};

            case Notice::ConfigCorrupt:
                return {i18n::localize("CONFIGURATION_FILE_CORRUPTED_1") + '\n' +
                        i18n::localize("CONFIGURATION_USING_DEFAULT")};
            case Notice::ConfigBadFormat:
                return {i18n::localize("CONFIGURATION_INCORRECT_FORMAT") + '\n' +
                        i18n::localize("CONFIGURATION_USING_DEFAULT")};

            case Notice::SaveBackupOpenFailed:
                return {i18n::localize("BAD_OPEN_BACKUP")};
            // Diagnostics for a save PKSM cannot make sense of: untranslated on purpose,
            // because they are quoted back to the developers in bug reports.
            case Notice::SaveUninitialized:
                return {"Uninitialized save"};
            case Notice::SaveSingleCmacInvalid:
                return {"Invalid single CMAC"};
            case Notice::SaveBothCmacsInvalid:
                return {"Both CMACs are invalid"};
            case Notice::SaveOpenFailed:
                return {i18n::localize("BAD_OPEN_SAVE"), true};
            case Notice::SaveInvalid:
                return {i18n::localize("SAVE_INVALID"), true};
            case Notice::SaveFileInvalid:
                return {report.detail + '\n' + i18n::localize("SAVE_INVALID")};
            case Notice::SaveNotASaveFile:
                return {report.detail + '\n' + i18n::localize("NOT_A_SAVE_FILE")};
            case Notice::SaveWrongSize:
                return {i18n::localize("WRONG_SIZE"), true};
            case Notice::SaveWrongSizeReport:
                return {i18n::localize("WRONG_SIZE") + '\n' + i18n::localize("Please report")};
            case Notice::SaveCommitFailed:
                return {i18n::localize("FAIL_SAVE_COMMIT"), true};
            case Notice::SecureValueFailed:
                return {i18n::localize("SECURE_VALUE_ERROR"), true};
            case Notice::GbaSaveUninitialized:
                return {i18n::localize("UNINIT_GBA_SAVE")};
            case Notice::TitleScanCritical:
                return {i18n::localize("LOADER_CRITICAL_ERROR")};

            case Notice::GpssDownloadCode:
                return {i18n::localize("SHARE_DOWNLOAD_CODE") + '\n' + report.detail};
            case Notice::GpssCommunicationError:
                return {pksm::format(i18n::localize("GPSS_COMMUNICATION_ERROR"), report.code)};
        }
        return {};
    }

    class Citro2dPresenter : public pksm::Presenter
    {
    public:
        void show(const pksm::Report& report) override
        {
            const auto rendered = render(report);
            if (rendered.withCode)
            {
                Gui::error(rendered.message, report.code);
            }
            else
            {
                Gui::warn(rendered.message);
            }
        }

        bool ask(pksm::Question question) override
        {
            switch (question)
            {
                case pksm::Question::SaveBankWithoutBackup:
                    return Gui::showChoiceMessage(i18n::localize("BACKUP_FAIL_SAVE_1") + '\n' +
                                                  i18n::localize("BACKUP_FAIL_SAVE_2"));
                case pksm::Question::OverwriteCardSave:
                    return Gui::showChoiceMessage(i18n::localize("SAVE_OVERWRITE_1") + '\n' +
                                                  i18n::localize("SAVE_OVERWRITE_CARD"));
                case pksm::Question::OverwriteInstalledSave:
                    return Gui::showChoiceMessage(i18n::localize("SAVE_OVERWRITE_1") + '\n' +
                                                  i18n::localize("SAVE_OVERWRITE_INSTALL"));
            }
            return false;
        }

        void busy(pksm::Task task) override
        {
            switch (task)
            {
                case pksm::Task::BankLoad:
                    Gui::waitFrame(i18n::localize("BANK_LOAD"), ScreenTarget::TOP);
                    return;
                case pksm::Task::BankCreate:
                    Gui::waitFrame(i18n::localize("BANK_CREATE"), ScreenTarget::TOP);
                    return;
                case pksm::Task::BankSave:
                    Gui::waitFrame(i18n::localize("BANK_SAVE"));
                    return;
                case pksm::Task::BankBackup:
                    Gui::waitFrame(i18n::localize("BANK_BACKUP"), ScreenTarget::TOP);
                    return;
                case pksm::Task::BankConvert:
                    Gui::waitFrame(i18n::localize("BANK_CONVERT"), ScreenTarget::TOP);
                    return;
                case pksm::Task::BankResize:
                    Gui::showResizeStorage();
                    return;
                case pksm::Task::ScanSaves:
                    Gui::waitFrame(i18n::localize("SCAN_SAVES"), ScreenTarget::TOP);
                    return;
                case pksm::Task::BackUpSave:
                    Gui::waitFrame(i18n::localize("LOADER_BACKING_UP"), ScreenTarget::TOP);
                    return;
            }
        }

        void progress(pksm::Progress bar, u32 partial, u32 total) override
        {
            switch (bar)
            {
                case pksm::Progress::RestoreSave:
                    Gui::showRestoreProgress(partial, total);
                    return;
            }
        }
    };
}

std::unique_ptr<pksm::Presenter> GuiPresenter::create()
{
    return std::make_unique<Citro2dPresenter>();
}
