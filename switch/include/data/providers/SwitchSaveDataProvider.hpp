#pragma once

#include <atomic>
#include <cstdint>
#include <filesystem>
#include <map>
#include <mutex>
#include <optional>
#include <thread>
#include <unordered_map>
#include <vector>

#include "data/emulator/EmulatorGameCatalog.hpp"
#include "data/providers/interfaces/ISaveDataProvider.hpp"
#include "data/saves/Save.hpp"
#include "utils/AccountUtil.hpp"

class SwitchSaveDataProvider : public ISaveDataProvider {
private:
    // Path where Checkpoint stores saves
    static constexpr const char* CHECKPOINT_BASE_PATH = "sdmc:/switch/Checkpoint/saves/";

    // Path where JKSV stores backups
    static constexpr const char* JKSV_BASE_PATH = "sdmc:/JKSV/";

    // Cache for save data
    struct SaveCache {
        std::vector<pksm::saves::Save::Ref> consoleSaves;
        bool isLoaded = false;
    };

    mutable std::unordered_map<u64, std::unordered_map<AccountUid, SaveCache, AccountUidHash>> consoleSaveCache;
    mutable std::unordered_map<u64, std::vector<pksm::saves::Save::Ref>> checkpointSaveCache;
    mutable std::unordered_map<u64, std::vector<pksm::saves::Save::Ref>> jksvSaveCache;

    // Catalog of non-installed games (loaded once; romfs is immutable)
    std::vector<pksm::data::emulator::EmulatorGameEntry> emulatorGames;
    std::unordered_map<u64, pksm::data::emulator::EmulatorGameEntry> emulatorCatalog;

    // Saves found by scanning the card's emulator locations, built on
    // first use and kept for the session (a fresh scan needs a relaunch)
    mutable std::optional<std::unordered_map<u64, std::vector<std::string>>> discoveredSaves;
    const std::unordered_map<u64, std::vector<std::string>>& DiscoveredSaves() const;

    // Core-validation results per file, invalidated when the file changes.
    // Guarded by validationMutex: the prewarm worker fills it while the UI
    // thread reads through it
    struct ValidatedFile {
        std::filesystem::file_time_type mtime;
        std::uintmax_t size = 0;
        bool valid = false;
    };
    mutable std::unordered_map<std::string, ValidatedFile> validationCache;
    mutable std::mutex validationMutex;
    mutable std::mutex discoveryMutex;
    std::thread prewarmThread;
    std::atomic<bool> prewarmStop{false};

    // Live saves inside the NSO emulator apps' own console save containers,
    // keyed by catalog game. Scanned once per account, UI thread only (the
    // scan mounts the shared "save" device)
    mutable std::optional<AccountUid> nsoScanUser;
    mutable std::unordered_map<u64, std::vector<pksm::saves::Save::Ref>> nsoConsoleSaves;
    void ScanNSOContainers(const AccountUid& userId) const;
    std::vector<pksm::saves::Save::Ref>
    ListNSOConsoleSaves(u64 titleId, const std::optional<AccountUid>& user) const;
    std::optional<pksm::saves::PendingLoad> ResolveNSOConsoleSave(const std::string& nsoPath, const AccountUid& userId);

    // Helper methods
    void RefreshConsoleSaves(const pksm::titles::Title::Ref& title, const AccountUid& userId) const;
    void RefreshCheckpointSaves(const pksm::titles::Title::Ref& title) const;
    void RefreshJKSVSaves(const pksm::titles::Title::Ref& title) const;
    std::vector<pksm::saves::Save::Ref> ListDiscoveredSaves(u64 titleId) const;
    std::vector<pksm::saves::Save::Ref> ListConfiguredSaves(u64 titleId) const;
    bool ValidateWithCore(const std::string& path) const;

    // Filesystem mounting helpers
    Result MountSaveData(FsFileSystem* fs, u64 titleId, AccountUid userId) const;
    void UnmountSaveData() const;

public:
    SwitchSaveDataProvider();
    ~SwitchSaveDataProvider();
    PU_SMART_CTOR(SwitchSaveDataProvider)

    // Validate every discovered and configured save on a background thread,
    // so the first landing on a title finds the cache already warm.
    // Main-thread-only, call once
    void PrewarmValidationCache();

    // Implementation of ISaveDataProvider
    std::vector<pksm::saves::Save::Ref> GetSavesForTitle(
        const pksm::titles::Title::Ref& title,
        const std::optional<AccountUid>& currentUser = std::nullopt
    ) const override;

    std::optional<pksm::saves::PendingLoad> ResolveLoad(
        const pksm::titles::Title::Ref& title,
        const std::string& saveName,
        const AccountUid* userId = nullptr
    ) override;
    std::optional<pksm::saves::LoadedSave> ExecuteLoad(const pksm::saves::PendingLoad& pending) override;
    void FinishLoad(const pksm::saves::PendingLoad& pending) override;

    std::optional<pksm::saves::LoadedSave> LoadSave(
        const pksm::titles::Title::Ref& title,
        const std::string& saveName,
        const AccountUid* userId = nullptr
    ) override;

    bool HasConsoleSaveData(u64 titleId, const AccountUid& userId) const override;

    bool HasDiscoveredEmulatorSaves(u64 titleId) const override;

    bool HasConfiguredEmulatorSaves(u64 titleId) const override;

    // Load save from different sources
    std::optional<pksm::saves::PendingLoad> ResolveConsoleSave(
        const pksm::titles::Title::Ref& title,
        const AccountUid& userId
    );
    std::optional<pksm::saves::PendingLoad> ResolveCheckpointSave(
        const pksm::titles::Title::Ref& title,
        const std::string& saveName
    );
    std::optional<pksm::saves::PendingLoad> ResolveJKSVSave(
        const pksm::titles::Title::Ref& title,
        const std::string& saveName
    );

    // Refresh methods
    void
    RefreshSaves(const pksm::titles::Title::Ref& title, const std::optional<AccountUid>& userId = std::nullopt) const;
};