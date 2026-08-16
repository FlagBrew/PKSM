#include "data/providers/SwitchSaveDataProvider.hpp"

#include <cstring>
#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <switch.h>

#include "data/emulator/EmulatorSaveConfig.hpp"
#include "data/providers/SwitchTitleDataProvider.hpp"
#include "data/saves/SaveValidator.hpp"
#include "utils/Logger.hpp"

// Utility to get a safe title name for filesystem operations
std::string SwitchSaveDataProvider::GetSaveTitleName(const std::string& name) const {
    // Replace characters that aren't valid in filenames
    std::string safeName = name;
    static const std::regex invalidChars("[\\\\/:*?\"<>|]");
    safeName = std::regex_replace(safeName, invalidChars, "_");
    return safeName;
}

SwitchSaveDataProvider::SwitchSaveDataProvider() {
    emulatorCatalog = pksm::data::emulator::EmulatorGameCatalog::BuildIndexByTitleId(
        pksm::data::emulator::EmulatorGameCatalog::LoadFromDataJson()
    );
}

Result SwitchSaveDataProvider::MountSaveData(FsFileSystem* fs, u64 titleId, AccountUid userId) const {
    // Defensively clear any leftover mount under the same device name
    fsdevUnmountDevice("save");

    // Open save data filesystem
    Result res = fsOpen_SaveData(fs, titleId, userId);
    if (R_SUCCEEDED(res)) {
        // Mount the filesystem to "save". fsdev closes fs itself on failure
        // (see libnx fs_dev.h) - do NOT fsFsClose here.
        int mountResult = fsdevMountDevice("save", *fs);
        if (mountResult == -1) {
            LOG_ERROR("Failed to mount save filesystem device");
            return MAKERESULT(Module_Libnx, LibnxError_IoError);
        }
    } else {
        std::stringstream ss;
        ss << "Failed to open save data filesystem: 0x" << std::hex << res;
        LOG_ERROR(ss.str());
    }

    return res;
}

void SwitchSaveDataProvider::UnmountSaveData() const {
    fsdevUnmountDevice("save");
}

bool SwitchSaveDataProvider::HasConsoleSaveData(u64 titleId, const AccountUid& userId) const {
    // A save container is created the moment a profile launches a game, even
    // if nothing was ever saved. Peek inside at the raw fs level (no fsdev
    // mount needed) so such empty containers don't count as save data.
    FsFileSystem fs;
    if (R_FAILED(fsOpen_SaveData(&fs, titleId, userId))) {
        return false;
    }

    bool hasContent = false;
    FsDir dir;
    if (R_SUCCEEDED(fsFsOpenDirectory(&fs, "/", FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles, &dir))) {
        FsDirectoryEntry entry;
        s64 count = 0;
        hasContent = R_SUCCEEDED(fsDirRead(&dir, &count, 1, &entry)) && count > 0;
        fsDirClose(&dir);
    }
    fsFsClose(&fs);
    return hasContent;
}

namespace {

// List the mounted save's root. Returns entry names (bounded), empty if none.
// A save container can exist but be empty when a profile launched the game
// without ever playing - such containers must not surface as loadable saves.
std::vector<std::string> ListMountedSaveRoot() {
    std::vector<std::string> names;
    DIR* dir = opendir("save:/");
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr && names.size() < 16) {
            names.emplace_back(entry->d_name);
        }
        closedir(dir);
    }
    return names;
}

std::string JoinNames(const std::vector<std::string>& names) {
    std::string joined;
    for (const auto& n : names) {
        if (!joined.empty()) {
            joined += ", ";
        }
        joined += n;
    }
    return joined.empty() ? "(empty)" : joined;
}

}  // namespace

bool SwitchSaveDataProvider::ValidateWithCore(const std::string& path) const {
    std::error_code ec;
    const auto mtime = std::filesystem::last_write_time(std::filesystem::path(path), ec);
    const auto size = ec ? 0 : std::filesystem::file_size(std::filesystem::path(path), ec);
    if (ec) {
        return false;
    }

    auto it = validationCache.find(path);
    if (it != validationCache.end() && it->second.mtime == mtime && it->second.size == size) {
        return it->second.valid;
    }

    const auto summary = pksm::saves::SaveValidator::Validate(path);
    if (summary) {
        LOG_INFO("Validated save " + path + ": " + summary->Describe());
    } else {
        LOG_INFO("Rejected save candidate " + path + ": core cannot parse it");
    }

    validationCache[path] = {mtime, size, summary.has_value()};
    return summary.has_value();
}

std::vector<pksm::saves::Save::Ref> SwitchSaveDataProvider::ListEmulatorSaves(u64 titleId) const {
    std::vector<pksm::saves::Save::Ref> saves;

    auto catalogIt = emulatorCatalog.find(titleId);
    if (catalogIt == emulatorCatalog.end()) {
        return saves;
    }

    // The config file is tiny and hand-editable, so reload it on every
    // listing to pick up changes without a restart
    const auto cfg = pksm::data::emulator::EmulatorSaveConfig::Load();
    const auto paths = pksm::data::emulator::EmulatorGameCatalog::CandidatePaths(catalogIt->second, cfg);

    for (const auto& path : paths) {
        std::error_code ec;
        if (!std::filesystem::is_regular_file(std::filesystem::path(path), ec) || ec) {
            continue;
        }
        if (!ValidateWithCore(path)) {
            continue;
        }

        // Name saves by filename; qualify with the parent directory when two
        // candidates share one
        std::string name = std::filesystem::path(path).filename().string();
        for (const auto& existing : saves) {
            if (existing->getName() == name) {
                name += " (" + std::filesystem::path(path).parent_path().filename().string() + ")";
                break;
            }
        }
        saves.push_back(pksm::saves::Save::New(name, path));
    }

    return saves;
}

void SwitchSaveDataProvider::RefreshConsoleSaves(const pksm::titles::Title::Ref& title, const AccountUid& userId) const {
    if (!title) {
        return;
    }
    LOG_INFO("RefreshConsoleSaves: " + title->getName());

    // Clear current saves for this title and user
    u64 titleId = title->getTitleId();
    consoleSaveCache[titleId][userId].consoleSaves.clear();

    // The emptiness policy lives in HasConsoleSaveData; don't bother
    // mounting a container that holds no save data
    if (!HasConsoleSaveData(titleId, userId)) {
        std::stringstream ss;
        ss << "Empty save container for title " << std::hex << titleId << ", ignoring";
        LOG_INFO(ss.str());
        consoleSaveCache[titleId][userId].isLoaded = true;
        return;
    }

    // Attempt to mount the save data
    FsFileSystem fs;
    Result res = MountSaveData(&fs, titleId, userId);

    if (R_SUCCEEDED(res)) {
        auto rootEntries = ListMountedSaveRoot();
        UnmountSaveData();

        auto save = pksm::saves::Save::New("Console Save", "save:/");
        consoleSaveCache[titleId][userId].consoleSaves.push_back(save);
        std::stringstream ss;
        ss << "Found console save for title " << std::hex << titleId << " [" << JoinNames(rootEntries) << "]";
        LOG_INFO(ss.str());
    } else {
        std::stringstream ss;
        ss << "No console save found for title " << std::hex << titleId << ", user " << std::hex << userId.uid[0];
        LOG_INFO(ss.str());
    }

    // The cache entry is now current either way (possibly with zero saves)
    consoleSaveCache[titleId][userId].isLoaded = true;
}

void SwitchSaveDataProvider::RefreshCheckpointSaves(const pksm::titles::Title::Ref& title) const {
    if (!title) {
        return;
    }

    u64 titleId = title->getTitleId();
    std::vector<pksm::saves::Save::Ref> saves;

    // Format the title ID as a hex string
    char titleIdStr[20];
    snprintf(titleIdStr, sizeof(titleIdStr), "0x%016lX", titleId);

    // Checkpoint names its per-title folder with its own title string
    // ("0x<ID> <name>"), which won't always match how we render the name.
    // Scan the saves dir and substring-match the 16-hex ID instead.
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

        checkpointSaveCache[titleId] = saves;
        return;
    }

    // Scan for subdirectories
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

    // Cache the results
    checkpointSaveCache[titleId] = saves;
}

void SwitchSaveDataProvider::RefreshSaves(
    const pksm::titles::Title::Ref& title,
    const std::optional<AccountUid>& userId
) const {
    if (!title) {
        return;
    }

    u64 titleId = title->getTitleId();

    // Emulator titles have no console/Checkpoint saves to refresh; their
    // listing is rebuilt (and revalidated on change) in GetSavesForTitle
    if (IsEmulatorTitle(titleId)) {
        return;
    }

    // If a user ID is provided, refresh console saves
    if (userId) {
        RefreshConsoleSaves(title, *userId);
    }

    // Refresh Checkpoint saves (these are independent of the user)
    RefreshCheckpointSaves(title);

    std::stringstream ss;
    ss << "Refreshed saves for title " << std::hex << titleId;
    LOG_INFO(ss.str());
}

std::vector<pksm::saves::Save::Ref> SwitchSaveDataProvider::GetSavesForTitle(
    const pksm::titles::Title::Ref& title,
    const std::optional<AccountUid>& currentUser
) const {
    if (!title) {
        return {};
    }

    u64 titleId = title->getTitleId();

    // Catalog (emulator) games never have console/Checkpoint saves; their
    // files live at arbitrary sdmc paths and are core-validated instead
    if (IsEmulatorTitle(titleId)) {
        return ListEmulatorSaves(titleId);
    }

    // Console saves: refresh only on cache miss (a refresh mounts the save
    // FS, too heavy for every selection change). Checkpoint saves: always
    // rescan - it's a cheap directory read, and backups can be created
    // mid-session.
    bool consoleCached = true;
    if (currentUser) {
        auto titleIt = consoleSaveCache.find(titleId);
        consoleCached = titleIt != consoleSaveCache.end() &&
            titleIt->second.count(*currentUser) > 0 && titleIt->second.at(*currentUser).isLoaded;
    }
    if (!consoleCached && currentUser) {
        RefreshConsoleSaves(title, *currentUser);
    }
    RefreshCheckpointSaves(title);
    std::vector<pksm::saves::Save::Ref> result;

    // Add console saves if a user is specified
    if (currentUser) {
        // Check if we have console saves for this title and user
        auto titleIt = consoleSaveCache.find(titleId);

        if (titleIt != consoleSaveCache.end()) {
            auto userIt = titleIt->second.find(*currentUser);
            if (userIt != titleIt->second.end() && userIt->second.isLoaded) {
                // Add all saves for this user
                result.insert(result.end(), userIt->second.consoleSaves.begin(), userIt->second.consoleSaves.end());
            }
        }
    }

    // Add Checkpoint saves
    auto checkpointIt = checkpointSaveCache.find(titleId);

    if (checkpointIt != checkpointSaveCache.end()) {
        result.insert(result.end(), checkpointIt->second.begin(), checkpointIt->second.end());
    }

    // Add custom saves
    auto customIt = customSaveCache.find(titleId);
    if (customIt != customSaveCache.end()) {
        result.insert(result.end(), customIt->second.begin(), customIt->second.end());
    }

    return result;
}

bool SwitchSaveDataProvider::LoadConsoleSave(const pksm::titles::Title::Ref& title, const AccountUid& userId) {
    if (!title) {
        return false;
    }

    u64 titleId = title->getTitleId();

    std::stringstream ss;
    ss << "Loading console save for title " << std::hex << titleId << ", user " << std::hex << userId.uid[0];
    LOG_INFO(ss.str());

    // Open the save filesystem, list its root at the raw fs level BEFORE
    // mounting (save FS handles are exclusive, so this must share the one
    // handle), then mount that same handle to devoptab
    fsdevUnmountDevice("save");
    FsFileSystem fs;
    Result res = fsOpen_SaveData(&fs, titleId, userId);
    if (R_FAILED(res)) {
        std::stringstream ss;
        ss << "Failed to open save data filesystem: 0x" << std::hex << res;
        LOG_ERROR(ss.str());
        return false;
    }

    if (fsdevMountDevice("save", fs) == -1) {
        // fsdev closes fs itself on failure - do not fsFsClose here
        LOG_ERROR("Failed to mount save filesystem device");
        return false;
    }

    // Save file names vary per game (main, savedata.bin, ...), so until the
    // real save accessor takes over, "loadable" means the container has
    // content. The listing doubles as the support-log breadcrumb.
    auto rootEntries = ListMountedSaveRoot();
    if (rootEntries.empty()) {
        LOG_ERROR("Save load failed for " + title->getName() + "; save root: (empty)");
        UnmountSaveData();
        return false;
    }

    LOG_INFO("Console save mounted for " + title->getName() + "; save root: " + JoinNames(rootEntries));

    // Unmount until the (still-mock) save accessor takes ownership of real
    // reads - leaving "save" mounted poisons every later mount attempt.
    UnmountSaveData();

    return true;
}

bool SwitchSaveDataProvider::LoadCheckpointSave(const pksm::titles::Title::Ref& title, const std::string& saveName) {
    if (!title) {
        return false;
    }

    u64 titleId = title->getTitleId();

    std::stringstream ss;
    ss << "Loading Checkpoint save " << saveName << " for title " << std::hex << titleId;
    LOG_INFO(ss.str());

    // Check if we have this title in our cache
    auto titleIt = checkpointSaveCache.find(titleId);
    if (titleIt == checkpointSaveCache.end()) {
        ss.str("");
        ss << "No Checkpoint saves found for title " << std::hex << titleId;
        LOG_ERROR(ss.str());
        return false;
    }

    // Find the save with the matching name
    for (const auto& save : titleIt->second) {
        if (save->getName() == saveName) {
            std::string savePath = save->getPath();

            // Here you would typically load the save data from the checkpoint directory
            // For demonstration, we'll just check that we can access the directory

            if (!std::filesystem::exists(savePath)) {
                ss.str("");
                ss << "Save directory not found at " << savePath;
                LOG_ERROR(ss.str());
                return false;
            }

            ss.str("");
            ss << "Found Checkpoint save at " << savePath;
            LOG_INFO(ss.str());

            // Now we would handle loading this save into the appropriate format

            return true;
        }
    }

    ss.str("");
    ss << "Checkpoint save " << saveName << " not found for title " << std::hex << titleId;
    LOG_ERROR(ss.str());
    return false;
}

bool SwitchSaveDataProvider::LoadCustomSave(const pksm::titles::Title::Ref& title, const std::string& saveName) {
    if (!title) {
        return false;
    }

    u64 titleId = title->getTitleId();

    std::stringstream ss;
    ss << "Loading custom save " << saveName << " for title " << std::hex << titleId;
    LOG_INFO(ss.str());

    // Custom save loading would go here
    // This could be for saves located in specific known locations outside
    // of the standard console or Checkpoint directories

    return false;  // Not implemented yet
}

bool SwitchSaveDataProvider::LoadSave(
    const pksm::titles::Title::Ref& title,
    const std::string& saveName,
    const AccountUid* userId
) {
    if (!title) {
        return false;
    }

    // Emulator saves: re-resolve the name to its path and parse it with
    // core as proof of loadability. Holding on to the parsed Sav comes with
    // the save-data accessor absorption.
    if (IsEmulatorTitle(title->getTitleId())) {
        for (const auto& save : ListEmulatorSaves(title->getTitleId())) {
            if (save->getName() == saveName) {
                const auto summary = pksm::saves::SaveValidator::Validate(save->getPath());
                if (summary) {
                    LOG_INFO("Loaded emulator save " + save->getPath() + ": " + summary->Describe());
                    return true;
                }
                LOG_ERROR("Emulator save no longer parses: " + save->getPath());
                return false;
            }
        }
        LOG_ERROR("Emulator save not found: " + saveName);
        return false;
    }

    // Handle console save (which requires a user ID)
    if (saveName == "Console Save" && userId) {
        return LoadConsoleSave(title, *userId);
    }

    // Try to load from Checkpoint saves
    if (LoadCheckpointSave(title, saveName)) {
        return true;
    }

    // Try to load from custom saves
    if (LoadCustomSave(title, saveName)) {
        return true;
    }

    std::stringstream ss;
    ss << "Unable to load save " << saveName << " for title " << std::hex << title->getTitleId();
    LOG_ERROR(ss.str());
    return false;
}