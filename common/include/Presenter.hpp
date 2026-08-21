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

#ifndef PRESENTER_HPP
#define PRESENTER_HPP

#include "utils/coretypes.h"
#include <memory>
#include <string>

// The seam between the domain and the screens. A domain module names what happened and
// hands it over; one adapter decides how it reads, what language it reads in, and which
// screen it lands on. Nothing here pulls in citro2d, so a module on this side of the seam
// can be built and run off-device.
namespace pksm
{
    // Something the user has to be told. The wording lives in the presenter, not here: a
    // notice that needs a number or a name carries it as `code` or `detail` instead.
    enum class Notice : u8
    {
        // Bank
        BankCorrupt,
        BankFromNewerVersion,
        BankNameWriteFailed, // code
        BankWriteFailed,     // code
        BankConvertFailed,   // code
        BankCritical,

        // Configuration
        ConfigCorrupt,
        ConfigBadFormat,
        ConfigFromNewerVersion,

        // Titles and saves
        SaveBackupOpenFailed,
        SaveUninitialized,
        SaveSingleCmacInvalid,
        SaveBothCmacsInvalid,
        SaveOpenFailed, // code
        SaveInvalid,
        SaveFileInvalid, // detail: the file name
        SaveWrongSize,   // code
        SaveWrongSizeReport,
        SaveCommitFailed,  // code
        SecureValueFailed, // code
        GbaSaveUninitialized,
        TitleScanCritical,

        // GPSS
        GpssDownloadCode,       // detail: the share code
        GpssCommunicationError, // code: the HTTP status
    };

    // A step long enough that the user needs to see something happening. What that looks
    // like, and on which screen, is the presenter's call.
    enum class Task : u8
    {
        BankLoad,
        BankCreate,
        BankSave,
        BankBackup,
        BankConvert,
        BankResize,
        ScanSaves,
        BackUpSave,
    };

    // A yes/no the domain cannot answer by itself.
    enum class Question : u8
    {
        SaveBankWithoutBackup,
        OverwriteCardSave,
        OverwriteInstalledSave,
    };

    // A step that can say how far along it is.
    enum class Progress : u8
    {
        RestoreSave,
    };

    // Everything a notice carries beyond its identity.
    struct Report
    {
        Notice notice;
        s32 code = 0;       // an OS Result or HTTP status, where the notice has one
        std::string detail; // a name the message quotes, where the notice has one
    };

    class Presenter
    {
    public:
        virtual ~Presenter()                                        = default;
        virtual void show(const Report& report)                     = 0;
        virtual bool ask(Question question)                         = 0;
        virtual void busy(Task task)                                = 0;
        virtual void progress(Progress bar, u32 partial, u32 total) = 0;
    };

    // What the domain calls. Before a presenter is installed reports are queued and
    // questions take their safe answer, so a module that runs before the GUI exists
    // still works: install() drains the queue once there is something to draw with.
    namespace present
    {
        void show(Notice notice, s32 code = 0, std::string detail = {});
        bool ask(Question question);
        void busy(Task task);
        void progress(Progress bar, u32 partial, u32 total);

        void install(std::unique_ptr<Presenter> presenter);
        void uninstall();
    }
}

#endif
