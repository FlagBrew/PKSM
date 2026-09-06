#pragma once

#include <map>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <vector>

#include "data/emulator/EmulatorGameCatalog.hpp"
#include "data/providers/interfaces/ISaveDataProvider.hpp"
#include "data/providers/sources/BackupSaveSource.hpp"
#include "data/providers/sources/NsoContainerSource.hpp"
#include "data/saves/Save.hpp"
#include "data/saves/SaveValidationCache.hpp"
#include "utils/AccountUtil.hpp"

// Threading rule: everything that mounts the shared "save:" device runs on
// the UI thread; of the load pipeline only ExecuteLoad is worker-safe.
class SwitchSaveDataProvider : public ISaveDataProvider {
private:
    // Cache for save data
    struct SaveCache {
        std::vector<pksm::saves::Save::Ref> consoleSaves;
        bool isLoaded = false;
    };

    mutable std::unordered_map<u64, std::unordered_map<AccountUid, SaveCache, AccountUidHash>> consoleSaveCache;
    mutable BackupSaveSource backupSaves;

    // Catalog of non-installed games (loaded once; romfs is immutable)
    std::vector<pksm::data::emulator::EmulatorGameEntry> emulatorGames;
    std::unordered_map<u64, pksm::data::emulator::EmulatorGameEntry> emulatorCatalog;

    // Emulator save scan results, built on first use and kept for the session
    mutable std::optional<std::unordered_map<u64, std::vector<std::string>>> discoveredSaves;
    const std::unordered_map<u64, std::vector<std::string>>& DiscoveredSaves() const;

    mutable std::mutex discoveryMutex;

    mutable NsoContainerSource nsoSaves;

    // Helper methods
    void RefreshConsoleSaves(const pksm::titles::Title::Ref& title, const AccountUid& userId) const;
    std::vector<pksm::saves::Save::Ref> ListDiscoveredSaves(u64 titleId) const;
    std::vector<pksm::saves::Save::Ref> ListConfiguredSaves(u64 titleId) const;

    // Filesystem mounting helpers
    Result MountSaveData(FsFileSystem* fs, u64 titleId, AccountUid userId) const;
    void UnmountSaveData() const;

    // Declared last: its destructor joins the prewarm worker, whose path
    // collection reads the members above
    mutable pksm::saves::SaveValidationCache validationCache;

public:
    SwitchSaveDataProvider();
    PU_SMART_CTOR(SwitchSaveDataProvider)

    // Warm the validation cache on a background thread; main-thread-only, call once
    void PrewarmValidationCache();

    // Implementation of ISaveDataProvider
    std::vector<pksm::saves::Save::Ref> GetSavesForTitle(
        const pksm::titles::Title::Ref& title,
        const std::optional<AccountUid>& currentUser = std::nullopt
    ) const override;

    std::optional<pksm::saves::PendingLoad> ResolveLoad(
        const pksm::titles::Title::Ref& title,
        const pksm::saves::Save::Ref& save,
        const AccountUid* userId = nullptr
    ) override;
    std::optional<pksm::saves::LoadedSave> ExecuteLoad(const pksm::saves::PendingLoad& pending) override;
    void FinishLoad(const pksm::saves::PendingLoad& pending) override;

    bool HasConsoleSaveData(u64 titleId, const AccountUid& userId) const override;

    bool HasDiscoveredEmulatorSaves(u64 titleId) const override;

    bool HasConfiguredEmulatorSaves(u64 titleId) const override;

    // Load save from different sources
    std::optional<pksm::saves::PendingLoad> ResolveConsoleSave(
        const pksm::titles::Title::Ref& title,
        const AccountUid& userId
    );
};