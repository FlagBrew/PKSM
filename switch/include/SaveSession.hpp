#pragma once

#include <functional>
#include <future>
#include <optional>
#include <string>
#include <switch.h>
#include <vector>

#include "data/providers/interfaces/ISaveDataAccessor.hpp"
#include "data/providers/interfaces/ISaveDataProvider.hpp"

namespace pksm::data {
class AccountManager;
}

namespace pksm {

// The loaded save's transitions: a load parsed on a worker thread, the leave prompt, and the
// save-and-exit write. A worker is polled each frame, with input blocked until it finishes.
class SaveSession {
public:
    // The application's overlays and dialogs, and where a finished transition goes
    struct Hooks {
        // Input stays blocked until endOverlay; the block is consumed each frame, so a worker
        // still running re-arms it with keepInputBlocked
        std::function<void(const std::string& message)> showBlockingToast;
        std::function<void()> keepInputBlocked;
        std::function<void()> endOverlay;
        std::function<void(const std::string& message)> showErrorToast;
        // Blocking choice among options, the last one cancelling; the index taken, negative for none
        std::function<int(const std::string& title, const std::string& message, const std::vector<std::string>& options)>
            requestChoice;
        std::function<void()> onSaveLoaded;
        std::function<void()> onSaveLeft;
    };

    SaveSession(
        ISaveDataProvider::Ref saveProvider,
        ISaveDataAccessor::Ref saveDataAccessor,
        data::AccountManager& accountManager,
        Hooks hooks
    );

    // A title screen pick: resolved on the UI thread, read and parsed on a worker
    void Load(titles::Title::Ref title, saves::Save::Ref save);
    // The main menu's back: prompts for unsaved changes; a Save writes on a worker before leaving
    void Leave();
    // Render callback: commits whichever worker has finished
    void Poll();

private:
    void FinishPendingWrite();
    // Lets the Sav go and hands the screens the leave
    void ReleaseSave();
    void FinishPendingLoad();

    ISaveDataProvider::Ref saveProvider;
    ISaveDataAccessor::Ref saveDataAccessor;
    data::AccountManager& accountManager;
    Hooks hooks;

    // Save-and-exit write on a worker thread; polled each frame with input blocked
    std::future<bool> saveWriteResult;

    // Worker reads+parses; mounts and the commit stay on the UI thread
    std::future<std::optional<saves::LoadedSave>> saveLoadResult;
    std::optional<saves::PendingLoad> pendingSaveLoad;
    titles::Title::Ref pendingLoadTitle;
    std::string pendingLoadSaveName;
    AccountUid pendingLoadUserId{};
    u64 pendingLoadStartTick = 0;
};

}  // namespace pksm
