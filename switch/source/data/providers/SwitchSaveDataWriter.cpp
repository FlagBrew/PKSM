#include "data/providers/SwitchSaveDataWriter.hpp"

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <sys/stat.h>
#include <vector>

#include "data/saves/ConsoleSaveMount.hpp"
#include "data/saves/SafeNames.hpp"
#include "utils/Logger.hpp"

namespace {

constexpr const char* BACKUP_BASE_DIR = "sdmc:/switch/PKSM/backups";

bool WriteFile(const std::string& path, const u8* data, size_t size) {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out.good()) {
        return false;
    }
    out.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));
    return out.good();
}

// mkdir one component at a time (the 3DS app's idiom - std::filesystem's
// recursive creation is unproven against device-prefixed paths here)
bool MakeDirTree(const std::string& path) {
    size_t pos = path.find(":/");
    pos = (pos == std::string::npos) ? 0 : pos + 2;
    while (true) {
        pos = path.find('/', pos);
        const std::string component = (pos == std::string::npos) ? path : path.substr(0, pos);
        if (mkdir(component.c_str(), 0777) != 0 && errno != EEXIST) {
            LOG_ERROR("mkdir " + component + " failed, errno " + std::to_string(errno));
            return false;
        }
        if (pos == std::string::npos) {
            return true;
        }
        pos++;
    }
}

// JKSV-style folder name for the title, title ID when it has none
std::string BackupDirName(const pksm::titles::Title::Ref& title) {
    if (!title) {
        return "unknown-title";
    }
    std::string dirName = pksm::saves::JKSVSafeName(title->getName());
    if (dirName.empty()) {
        char titleIdStr[17];
        snprintf(titleIdStr, sizeof(titleIdStr), "%016lX", title->getTitleId());
        dirName = titleIdStr;
    }
    return dirName;
}

// Backups are pruned per title, oldest first, so heavy savers stay bounded
constexpr size_t MAX_BACKUPS_PER_TITLE = 10;

// Strictly our "%Y-%m-%d_%H-%M-%S" folder names; anything else is never pruned
bool IsBackupStampDir(const std::string& name) {
    if (name.size() != 19) {
        return false;
    }
    for (size_t i = 0; i < name.size(); i++) {
        const char c = name[i];
        if (i == 4 || i == 7 || i == 13 || i == 16) {
            if (c != '-') {
                return false;
            }
        } else if (i == 10) {
            if (c != '_') {
                return false;
            }
        } else if (c < '0' || c > '9') {
            return false;
        }
    }
    return true;
}

void PruneOldBackups(const std::string& titleDir) {
    std::vector<std::string> stamps;
    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(titleDir, ec)) {
        const std::string name = entry.path().filename().string();
        if (IsBackupStampDir(name)) {
            stamps.push_back(name);
        }
    }
    if (ec || stamps.size() <= MAX_BACKUPS_PER_TITLE) {
        return;
    }
    // Zero-padded stamps sort chronologically
    std::sort(stamps.begin(), stamps.end());
    const size_t excess = stamps.size() - MAX_BACKUPS_PER_TITLE;
    for (size_t i = 0; i < excess; i++) {
        const std::string victim = titleDir + "/" + stamps[i];
        std::error_code rmEc;
        std::filesystem::remove_all(victim, rmEc);
        if (rmEc) {
            LOG_ERROR("Failed to prune old backup " + victim);
        } else {
            LOG_INFO("Pruned old backup " + victim);
        }
    }
}

// Copy the current on-disk save aside before it gets overwritten, the same
// habit as 3DS PKSM's auto-backup. Best-effort: a failure is logged but
// never blocks the user's explicit save.
void BackupBeforeWrite(const pksm::titles::Title::Ref& title, const std::string& savePath) {
    // Bounded read, the same discipline as SaveValidator::Load - never trust
    // a device file to report EOF or a sane size
    constexpr u64 MAX_BACKUP_SIZE = 0x4000000;  // 64MB, far above any save container file
    std::error_code ec;
    const u64 srcSize = std::filesystem::file_size(std::filesystem::path(savePath), ec);
    if (ec || srcSize == 0) {
        LOG_INFO("No existing file to back up at " + savePath);
        return;
    }
    if (srcSize > MAX_BACKUP_SIZE) {
        LOG_ERROR("Backup skipped: " + savePath + " reports absurd size " + std::to_string(srcSize));
        return;
    }
    std::vector<char> bytes(srcSize);
    FILE* in = fopen(savePath.c_str(), "rb");
    if (!in) {
        LOG_ERROR("Backup skipped: cannot open " + savePath);
        return;
    }
    const size_t got = fread(bytes.data(), 1, srcSize, in);
    fclose(in);
    if (got != srcSize) {
        LOG_ERROR("Backup skipped: short read of " + savePath);
        return;
    }

    // Saves within the same second share a stamp dir; the later backup wins
    char stamp[32];
    const std::time_t now = std::time(nullptr);
    std::strftime(stamp, sizeof(stamp), "%Y-%m-%d_%H-%M-%S", std::localtime(&now));
    const std::string titleDir = std::string(BACKUP_BASE_DIR) + "/" + BackupDirName(title);
    const std::string dir = titleDir + "/" + stamp;
    if (!MakeDirTree(dir)) {
        LOG_ERROR("Backup skipped: cannot create " + dir);
        return;
    }

    const std::string target = dir + "/" + savePath.substr(savePath.find_last_of('/') + 1);
    if (!WriteFile(target, reinterpret_cast<const u8*>(bytes.data()), bytes.size())) {
        LOG_ERROR("Backup failed writing " + target);
        std::remove(target.c_str());
        return;
    }
    LOG_INFO("Backed up " + savePath + " to " + target);
    PruneOldBackups(titleDir);
}

}  // namespace

bool SwitchSaveDataWriter::WriteSave(
    const pksm::titles::Title::Ref& title,
    const std::string& savePath,
    const AccountUid* userId,
    const u8* data,
    size_t size
) {
    if (!data || size == 0) {
        return false;
    }

    const bool isConsoleSave = savePath.rfind("save:/", 0) == 0;
    if (!isConsoleSave && savePath.rfind("sdmc:", 0) != 0) {
        LOG_ERROR("Write-back is not supported for " + savePath);
        return false;
    }

    if (isConsoleSave) {
        if (!title || !userId) {
            LOG_ERROR("Console save write-back needs the title and user it was loaded for");
            return false;
        }
        FsFileSystem fs;
        if (R_FAILED(pksm::saves::MountConsoleSave(&fs, title->getTitleId(), *userId))) {
            LOG_ERROR("Cannot mount console save for write-back: " + title->getName());
            return false;
        }

        BackupBeforeWrite(title, savePath);

        // The container is journaled: write in place, nothing persists
        // without the commit
        bool written = WriteFile(savePath, data, size);
        if (written) {
            Result rc = fsdevCommitDevice("save");
            if (R_FAILED(rc)) {
                LOG_ERROR("Failed to commit console save device for " + savePath);
                written = false;
            }
        }
        pksm::saves::UnmountConsoleSave();
        if (!written) {
            LOG_ERROR("Failed to write " + savePath);
        }
        return written;
    }

    BackupBeforeWrite(title, savePath);

    // sdmc files have no journal and the target is the only copy: write a
    // sibling temp file and swap it in, so an interrupted write can never
    // leave a truncated save behind
    const std::string tempPath = savePath + ".pksm-tmp";
    if (!WriteFile(tempPath, data, size)) {
        LOG_ERROR("Failed to write " + tempPath);
        std::remove(tempPath.c_str());
        return false;
    }
    std::remove(savePath.c_str());
    if (std::rename(tempPath.c_str(), savePath.c_str()) != 0) {
        LOG_ERROR("Failed to move " + tempPath + " into place; the written save is intact there");
        return false;
    }
    return true;
}
