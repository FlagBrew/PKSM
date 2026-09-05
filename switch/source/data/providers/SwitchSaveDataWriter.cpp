#include "data/providers/SwitchSaveDataWriter.hpp"

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <sys/stat.h>
#include <vector>

#include "data/saves/ConsoleSaveMount.hpp"
#include "data/saves/NSOContainer.hpp"
#include "data/saves/NSOPathScheme.hpp"
#include "data/saves/SafeNames.hpp"
#include "data/saves/SaveValidator.hpp"
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

// mkdir one component at a time; std::filesystem recursion is unproven on device-prefixed paths
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

// Best-effort auto-backup; failure is logged but never blocks the user's save
void BackupBeforeWrite(const pksm::titles::Title::Ref& title, const std::string& savePath) {
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
    LOG_DEBUG("Backing up " + savePath + " (" + std::to_string(srcSize) + " bytes)");

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

    FILE* in = fopen(savePath.c_str(), "rb");
    if (!in) {
        LOG_ERROR("Backup skipped: cannot open " + savePath);
        return;
    }
    const std::string target = dir + "/" + savePath.substr(savePath.find_last_of('/') + 1);
    FILE* out = fopen(target.c_str(), "wb");
    if (!out) {
        fclose(in);
        LOG_ERROR("Backup skipped: cannot create " + target);
        return;
    }

    // Chunked copy: a save-sized contiguous buffer has failed to allocate under peak heap here
    std::vector<char> chunk(0x40000);
    u64 copied = 0;
    bool writeOk = true;
    while (copied < srcSize) {
        const size_t got = fread(chunk.data(), 1, chunk.size(), in);
        if (got == 0) {
            break;
        }
        if (fwrite(chunk.data(), 1, got, out) != got) {
            writeOk = false;
            break;
        }
        copied += got;
    }
    fclose(in);
    fclose(out);
    if (!writeOk || copied != srcSize) {
        LOG_ERROR("Backup failed copying " + savePath + " to " + target);
        std::remove(target.c_str());
        std::remove(dir.c_str());
        return;
    }
    LOG_INFO("Backed up " + savePath + " to " + target);
    PruneOldBackups(titleDir);
}

// Splice a raw save back into its NSO container (header kept, SHA-1 re-signed);
// plain files pass through untouched. False means the write must abort.
bool PrepareWriteBuffer(const std::string& path, const u8*& data, size_t& size, std::vector<u8>& container) {
    std::error_code ec;
    const auto existingSize = std::filesystem::file_size(std::filesystem::path(path), ec);
    if (ec || existingSize <= size || existingSize > pksm::saves::SaveValidator::MAX_SAVE_SIZE) {
        return true;
    }

    container.resize(existingSize);
    FILE* in = fopen(path.c_str(), "rb");
    const bool readOk = in && fread(container.data(), 1, existingSize, in) == existingSize;
    if (in) {
        fclose(in);
    }
    if (!readOk) {
        // Writing raw bytes over an unreadable container would destroy its header
        LOG_ERROR("Cannot read existing save file before write-back: " + path);
        return false;
    }
    const auto region = pksm::saves::ProbeNSOContainer(container.data(), container.size());
    if (region && region->rawSize == size) {
        std::memcpy(container.data() + region->rawOffset, data, size);
        pksm::saves::ResignNSOContainer(container.data(), *region);
        data = container.data();
        size = container.size();
    } else if (region) {
        LOG_ERROR(
            "Refusing to write " + std::to_string(size) + " bytes into a container holding " +
            std::to_string(region->rawSize) + ": " + path
        );
        return false;
    } else {
        container.clear();
    }
    return true;
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

    // Save inside an NSO app's console container
    if (const auto nso = pksm::saves::ParseNSOSavePath(savePath)) {
        if (!userId) {
            LOG_ERROR("NSO container write-back needs the account it was loaded for");
            return false;
        }
        FsFileSystem fs;
        if (R_FAILED(pksm::saves::MountConsoleSave(&fs, nso->nsoTitleId, *userId))) {
            LOG_ERROR("Cannot mount NSO save container for write-back: " + savePath);
            return false;
        }
        const std::string mountedPath = "save:" + nso->innerPath;
        BackupBeforeWrite(title, mountedPath);
        std::vector<u8> container;
        bool written = PrepareWriteBuffer(mountedPath, data, size, container) && WriteFile(mountedPath, data, size);
        if (written) {
            Result rc = fsdevCommitDevice("save");
            if (R_FAILED(rc)) {
                LOG_ERROR("Failed to commit NSO save container for " + savePath);
                written = false;
            }
        }
        pksm::saves::UnmountConsoleSave();
        if (!written) {
            LOG_ERROR("Failed to write " + savePath);
        }
        return written;
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

        // The container is journaled: nothing persists without the commit
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

    std::vector<u8> container;
    if (!PrepareWriteBuffer(savePath, data, size, container)) {
        return false;
    }

    // sdmc has no journal: write a sibling temp file and swap, so interruption never truncates the save
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
