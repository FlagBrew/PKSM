#pragma once

#include <cstdint>
#include <filesystem>
#include <map>
#include <optional>
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

    // Cache for save data
    struct SaveCache {
        std::vector<pksm::saves::Save::Ref> consoleSaves;
        bool isLoaded = false;
    };

    mutable std::unordered_map<u64, std::unordered_map<AccountUid, SaveCache, AccountUidHash>> consoleSaveCache;
    mutable std::unordered_map<u64, std::vector<pksm::saves::Save::Ref>> checkpointSaveCache;
    mutable std::unordered_map<u64, std::vector<pksm::saves::Save::Ref>> customSaveCache;

    // Catalog of non-installed games (loaded once; romfs is immutable)
    std::unordered_map<u64, pksm::data::emulator::EmulatorGameEntry> emulatorCatalog;

    // Core-validation results per file, invalidated when the file changes
    struct ValidatedFile {
        std::filesystem::file_time_type mtime;
        std::uintmax_t size = 0;
        bool valid = false;
    };
    mutable std::unordered_map<std::string, ValidatedFile> validationCache;

    // Helper methods
    void RefreshConsoleSaves(const pksm::titles::Title::Ref& title, const AccountUid& userId) const;
    void RefreshCheckpointSaves(const pksm::titles::Title::Ref& title) const;
    bool IsEmulatorTitle(u64 titleId) const { return emulatorCatalog.count(titleId) > 0; }
    std::vector<pksm::saves::Save::Ref> ListEmulatorSaves(u64 titleId) const;
    bool ValidateWithCore(const std::string& path) const;
    std::string GetSaveTitleName(const std::string& name) const;

    // Filesystem mounting helpers
    Result MountSaveData(FsFileSystem* fs, u64 titleId, AccountUid userId) const;
    void UnmountSaveData() const;

public:
    SwitchSaveDataProvider();
    PU_SMART_CTOR(SwitchSaveDataProvider)

    // Implementation of ISaveDataProvider
    std::vector<pksm::saves::Save::Ref> GetSavesForTitle(
        const pksm::titles::Title::Ref& title,
        const std::optional<AccountUid>& currentUser = std::nullopt
    ) const override;

    bool LoadSave(
        const pksm::titles::Title::Ref& title,
        const std::string& saveName,
        const AccountUid* userId = nullptr
    ) override;

    bool HasConsoleSaveData(u64 titleId, const AccountUid& userId) const override;

    // Load save from different sources
    bool LoadConsoleSave(const pksm::titles::Title::Ref& title, const AccountUid& userId);
    bool LoadCheckpointSave(const pksm::titles::Title::Ref& title, const std::string& saveName);
    bool LoadCustomSave(const pksm::titles::Title::Ref& title, const std::string& saveName);

    // Refresh methods
    void
    RefreshSaves(const pksm::titles::Title::Ref& title, const std::optional<AccountUid>& userId = std::nullopt) const;
};