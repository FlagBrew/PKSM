#pragma once

#include <memory>
#include <optional>
#include <string>
#include <switch.h>
#include <vector>

#include "data/saves/LoadedSave.hpp"
#include "data/saves/Save.hpp"
#include "data/titles/Title.hpp"

class ISaveDataProvider {
public:
    PU_SMART_CTOR(ISaveDataProvider)
    virtual ~ISaveDataProvider() = default;

    // Get list of available saves for a given title and optional user.
    // Names only label the UI; listings keep them unique per title so every
    // entry stays distinguishable.
    virtual std::vector<pksm::saves::Save::Ref> GetSavesForTitle(
        const pksm::titles::Title::Ref& title,
        const std::optional<AccountUid>& currentUser = std::nullopt
    ) const = 0;

    // LoadSave split for a worker thread: ResolveLoad (UI thread; mounts),
    // ExecuteLoad (thread-safe), FinishLoad (UI thread, every outcome, exactly once).
    // The save is one of GetSavesForTitle's entries; its path picks the source.
    virtual std::optional<pksm::saves::PendingLoad> ResolveLoad(
        const pksm::titles::Title::Ref& title,
        const pksm::saves::Save::Ref& save,
        const AccountUid* userId = nullptr
    ) = 0;
    virtual std::optional<pksm::saves::LoadedSave> ExecuteLoad(const pksm::saves::PendingLoad& pending) = 0;
    virtual void FinishLoad(const pksm::saves::PendingLoad& pending) = 0;

    // Containers exist as soon as a profile launches a game; an empty one
    // must not count as a loadable save
    virtual bool HasConsoleSaveData(u64 titleId, const AccountUid& userId) const {
        return true;
    }

    // Gates which catalog games earn an Emulator-tab tile
    virtual bool HasDiscoveredEmulatorSaves(u64 titleId) const { return false; }

    // Gates which catalog games earn a Custom-tab tile
    virtual bool HasConfiguredEmulatorSaves(u64 titleId) const { return false; }
};