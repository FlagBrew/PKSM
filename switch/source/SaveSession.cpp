#include "SaveSession.hpp"

#include "data/AccountManager.hpp"
#include "utils/Logger.hpp"

namespace pksm {

SaveSession::SaveSession(
    ISaveDataProvider::Ref saveProvider,
    ISaveDataAccessor::Ref saveDataAccessor,
    data::AccountManager& accountManager,
    Hooks hooks
)
  : saveProvider(std::move(saveProvider)),
    saveDataAccessor(std::move(saveDataAccessor)),
    accountManager(accountManager),
    hooks(std::move(hooks)) {}

void SaveSession::Load(titles::Title::Ref title, saves::Save::Ref save) {
    LOG_DEBUG("Save selected: " + save->getName() + " for title: " + title->getName());

    // Resolution and any console mount stay on the UI thread; the worker only reads+parses
    auto userId = accountManager.GetCurrentAccount();
    pendingLoadStartTick = armGetSystemTick();
    pendingSaveLoad = saveProvider->ResolveLoad(title, save, &userId);
    if (!pendingSaveLoad) {
        saveDataAccessor->applySaveLoadResult(title, save->getName(), &userId, std::nullopt);
        hooks.showErrorToast("This save could not be loaded");
        return;
    }
    pendingLoadTitle = title;
    pendingLoadSaveName = save->getName();
    pendingLoadUserId = userId;
    hooks.showBlockingToast("Loading save...");
    saveLoadResult = std::async(std::launch::async, [this]() { return saveProvider->ExecuteLoad(*pendingSaveLoad); });
}

void SaveSession::Leave() {
    if (saveDataAccessor->hasUnsavedChanges()) {
        const int choice = hooks.requestChoice(
            "Unsaved Changes",
            "Save the changes to this game's save file?",
            {"Save", "Discard", "Cancel"}
        );
        if (choice == 0) {
            hooks.showBlockingToast("Saving... Do not close the app or power off.");
            LOG_DEBUG("Starting save write...");
            LOG_MEMORY();
            saveWriteResult = std::async(std::launch::async, [this]() { return saveDataAccessor->saveChanges(); });
            return;
        }
        if (choice != 1) {
            return;
        }
    }
    ReleaseSave();
}

void SaveSession::Poll() {
    FinishPendingWrite();
    FinishPendingLoad();
}

void SaveSession::FinishPendingWrite() {
    if (!saveWriteResult.valid()) {
        return;
    }
    if (saveWriteResult.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
        // Still writing: keep input blocked
        hooks.keepInputBlocked();
        return;
    }

    // A worker death (allocation failure) is a failed save with changes still loaded, not a crash
    bool saved = false;
    try {
        saved = saveWriteResult.get();
    } catch (const std::exception& e) {
        LOG_ERROR("Save write threw: " + std::string(e.what()));
    }
    LOG_DEBUG(saved ? "Save write completed" : "Save write failed");
    LOG_MEMORY();
    // Flush now - a follow-up crash would lose the 3s flush window
    utils::Logger::Flush();
    hooks.endOverlay();
    if (!saved) {
        hooks.requestChoice("Save Failed", "The save file could not be written. Your changes are still loaded.", {"OK"});
        return;
    }
    ReleaseSave();
}

void SaveSession::FinishPendingLoad() {
    if (!saveLoadResult.valid()) {
        return;
    }
    if (saveLoadResult.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
        // Still parsing: keep input blocked
        hooks.keepInputBlocked();
        return;
    }

    // A worker death (allocation failure) is just a failed load
    std::optional<saves::LoadedSave> loaded;
    try {
        loaded = saveLoadResult.get();
    } catch (const std::exception& e) {
        LOG_ERROR("Save load threw: " + std::string(e.what()));
    }
    // Release the mount on every outcome
    saveProvider->FinishLoad(*pendingSaveLoad);
    pendingSaveLoad.reset();

    const bool ok = saveDataAccessor->applySaveLoadResult(
        pendingLoadTitle,
        pendingLoadSaveName,
        &pendingLoadUserId,
        std::move(loaded)
    );
    pendingLoadTitle = nullptr;
    hooks.endOverlay();
    if (ok) {
        LOG_MEMORY();
        hooks.onSaveLoaded();
        LOG_DEBUG(
            "Save selection to main menu: " +
            std::to_string(armTicksToNs(armGetSystemTick() - pendingLoadStartTick) / 1000000) + " ms total"
        );
    } else {
        LOG_ERROR("Failed to load save data");
        hooks.showErrorToast("This save could not be loaded");
    }
}

void SaveSession::ReleaseSave() {
    // The Sav goes before the screens built on it; a title return logs each step's cost
    const u64 t0 = armGetSystemTick();
    saveDataAccessor->unloadSave();
    LOG_DEBUG("Save released: " + std::to_string(armTicksToNs(armGetSystemTick() - t0) / 1000000) + " ms");
    hooks.onSaveLeft();
}
}  // namespace pksm

