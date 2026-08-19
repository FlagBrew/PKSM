#include "data/providers/sources/BackupSaveSource.hpp"

#include <cstdio>
#include <filesystem>
#include <sstream>

#include "data/saves/SafeNames.hpp"
#include "utils/Logger.hpp"

namespace {

// A backup is a directory; probe its files and let core decide which is the save
std::optional<pksm::saves::PendingLoad> ResolveBackupDirectory(
    const std::string& dirPath,
    const std::string& logContext
) {
    pksm::saves::PendingLoad pending;
    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(dirPath, ec)) {
        pending.candidates.push_back(entry.path().string());
    }
    if (pending.candidates.empty()) {
        LOG_ERROR("Backup " + dirPath + " has no files to probe");
        return std::nullopt;
    }
    pending.description = logContext;
    return pending;
}

}  // namespace

void BackupSaveSource::Refresh(const pksm::titles::Title::Ref& title) {
    if (!title) {
        return;
    }
    RefreshCheckpoint(title);
    RefreshJKSV(title);
}

void BackupSaveSource::RefreshCheckpoint(const pksm::titles::Title::Ref& title) {
    u64 titleId = title->getTitleId();
    std::vector<pksm::saves::Save::Ref> saves;

    char titleIdStr[20];
    snprintf(titleIdStr, sizeof(titleIdStr), "0x%016lX", titleId);

    // Checkpoint names its per-title folder "0x<ID> <name>"; scan the saves
    // dir and substring-match the 16-hex ID
    std::string basePath = CHECKPOINT_BASE_PATH;
    std::string titlePath;
    const std::string idHex = titleIdStr + 2;  // skip "0x"

    try {
        for (const auto& entry : std::filesystem::directory_iterator(basePath)) {
            if (entry.is_directory() && entry.path().filename().string().find(idHex) != std::string::npos) {
                titlePath = entry.path().string();
                break;
            }
        }
    } catch (const std::exception&) {
        // No Checkpoint directory at all - fall through to the empty case
    }

    if (titlePath.empty()) {
        std::stringstream ss;
        ss << "No Checkpoint saves found for title " << titleIdStr;
        LOG_INFO(ss.str());

        checkpointSaves[titleId] = saves;
        return;
    }

    try {
        for (const auto& entry : std::filesystem::directory_iterator(titlePath)) {
            if (entry.is_directory()) {
                std::string saveName = entry.path().filename().string();
                std::string savePath = entry.path().string();

                // Add to list of saves
                auto save = pksm::saves::Save::New(saveName, savePath);
                saves.push_back(save);

                std::stringstream ss;
                ss << "Found Checkpoint save: " << saveName << " at " << savePath;
                LOG_INFO(ss.str());
            }
        }
    } catch (const std::exception& e) {
        std::stringstream ss;
        ss << "Error scanning Checkpoint saves: " << e.what();
        LOG_ERROR(ss.str());
    }

    checkpointSaves[titleId] = saves;
}

void BackupSaveSource::RefreshJKSV(const pksm::titles::Title::Ref& title) {
    u64 titleId = title->getTitleId();
    std::vector<pksm::saves::Save::Ref> saves;

    // JKSV names a game's folder after its sanitized title, or the title ID
    // when the name has no ASCII representation
    char titleIdStr[17];
    snprintf(titleIdStr, sizeof(titleIdStr), "%016lX", titleId);
    const std::string safeName = pksm::saves::JKSVSafeName(title->getName());

    std::string titlePath;
    for (const auto& dirName : {safeName, std::string(titleIdStr)}) {
        std::error_code ec;
        if (!dirName.empty() && std::filesystem::is_directory(JKSV_BASE_PATH + dirName, ec) && !ec) {
            titlePath = JKSV_BASE_PATH + dirName;
            break;
        }
    }

    if (titlePath.empty()) {
        jksvSaves[titleId] = saves;
        return;
    }

    // Zip backups are not supported yet; list directory backups only
    try {
        for (const auto& entry : std::filesystem::directory_iterator(titlePath)) {
            if (entry.is_directory()) {
                std::string saveName = "[JKSV] " + entry.path().filename().string();
                saves.push_back(pksm::saves::Save::New(saveName, entry.path().string()));

                std::stringstream ss;
                ss << "Found JKSV backup: " << saveName << " at " << entry.path().string();
                LOG_INFO(ss.str());
            }
        }
    } catch (const std::exception& e) {
        std::stringstream ss;
        ss << "Error scanning JKSV backups: " << e.what();
        LOG_ERROR(ss.str());
    }

    jksvSaves[titleId] = saves;
}

std::vector<pksm::saves::Save::Ref> BackupSaveSource::List(u64 titleId) const {
    std::vector<pksm::saves::Save::Ref> result;

    auto checkpointIt = checkpointSaves.find(titleId);
    if (checkpointIt != checkpointSaves.end()) {
        result.insert(result.end(), checkpointIt->second.begin(), checkpointIt->second.end());
    }

    auto jksvIt = jksvSaves.find(titleId);
    if (jksvIt != jksvSaves.end()) {
        result.insert(result.end(), jksvIt->second.begin(), jksvIt->second.end());
    }

    return result;
}

std::optional<pksm::saves::PendingLoad> BackupSaveSource::Resolve(u64 titleId, const std::string& saveName) const {
    // A miss is normal - the caller tries each save source in turn; only a
    // claimed-but-failed load is an error
    auto checkpointIt = checkpointSaves.find(titleId);
    if (checkpointIt != checkpointSaves.end()) {
        for (const auto& save : checkpointIt->second) {
            if (save->getName() == saveName) {
                std::stringstream ss;
                ss << "Loading Checkpoint save " << saveName << " for title " << std::hex << titleId;
                LOG_INFO(ss.str());
                return ResolveBackupDirectory(save->getPath(), "Checkpoint save");
            }
        }
    }

    auto jksvIt = jksvSaves.find(titleId);
    if (jksvIt != jksvSaves.end()) {
        for (const auto& save : jksvIt->second) {
            if (save->getName() == saveName) {
                return ResolveBackupDirectory(save->getPath(), "JKSV save");
            }
        }
    }

    return std::nullopt;
}
