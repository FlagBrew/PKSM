#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "data/saves/LoadedSave.hpp"
#include "data/saves/Save.hpp"
#include "data/titles/Title.hpp"

// Backup saves kept on the SD card by Checkpoint and JKSV: each tool stores
// one directory per title holding one subdirectory per backup.
class BackupSaveSource {
public:
    // Rescan both tools' directories for this title (cheap directory reads)
    void Refresh(const pksm::titles::Title::Ref& title);

    // Cached backups for the title, Checkpoint's before JKSV's
    std::vector<pksm::saves::Save::Ref> List(u64 titleId) const;

    // Resolve a listed backup name into its directory's candidate files
    std::optional<pksm::saves::PendingLoad> Resolve(u64 titleId, const std::string& saveName) const;

private:
    // Path where Checkpoint stores saves
    static constexpr const char* CHECKPOINT_BASE_PATH = "sdmc:/switch/Checkpoint/saves/";

    // Path where JKSV stores backups
    static constexpr const char* JKSV_BASE_PATH = "sdmc:/JKSV/";

    void RefreshCheckpoint(const pksm::titles::Title::Ref& title);
    void RefreshJKSV(const pksm::titles::Title::Ref& title);

    std::unordered_map<u64, std::vector<pksm::saves::Save::Ref>> checkpointSaves;
    std::unordered_map<u64, std::vector<pksm::saves::Save::Ref>> jksvSaves;
};
