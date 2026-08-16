#include "data/providers/SwitchSaveDataProvider.hpp"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <switch.h>

#include "data/providers/SwitchTitleDataProvider.hpp"
#include "utils/Logger.hpp"

// Utility to get a safe title name for filesystem operations
std::string SwitchSaveDataProvider::GetSaveTitleName(const std::string& name) const {
    // Replace characters that aren't valid in filenames
    std::string safeName = name;
    std::regex invalidChars("[\\\\/:*?\"<>|]");
    safeName = std::regex_replace(safeName, invalidChars, "_");
    return safeName;
}

SwitchSaveDataProvider::SwitchSaveDataProvider() {
    // Initialize empty state
}

Result SwitchSaveDataProvider::MountSaveData(FsFileSystem* fs, u64 titleId, AccountUid userId) const {
    // Open save data filesystem
    Result res = fsOpen_SaveData(fs, titleId, userId);
    if (R_SUCCEEDED(res)) {
        // Mount the filesystem to "save"
        int mountResult = fsdevMountDevice("save", *fs);
        if (mountResult == -1) {
            LOG_ERROR("Failed to mount save filesystem");
            fsFsClose(fs);
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

void SwitchSaveDataProvider::RefreshConsoleSaves(const pksm::titles::Title::Ref& title, const AccountUid& userId) const {
    LOG_INFO("RefreshConsoleSaves" + title->getName());

    if (!title) {
        return;
    }

    // Clear current saves for this title and user
    u64 titleId = title->getTitleId();
    consoleSaveCache[titleId][userId].consoleSaves.clear();

    // Attempt to mount the save data
    FsFileSystem fs;
    Result res = MountSaveData(&fs, titleId, userId);

    if (R_SUCCEEDED(res)) {
        // Create a save entry for the console save
        std::string saveName = "Console Save";
        std::string savePath = "save:/";
        auto save = pksm::saves::Save::New(saveName, savePath);
        consoleSaveCache[titleId][userId].consoleSaves.push_back(save);

        // Mark as loaded
        consoleSaveCache[titleId][userId].isLoaded = true;

        // Unmount when done
        UnmountSaveData();

        std::stringstream ss;
        ss << "Found console save for title " << std::hex << titleId << ", user " << std::hex << userId.uid[0];
        LOG_INFO(ss.str());
    } else {
        std::stringstream ss;
        ss << "No console save found for title " << std::hex << titleId << ", user " << std::hex << userId.uid[0];
        LOG_INFO(ss.str());
    }
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

    // Look for a directory with the title ID
    std::string basePath = CHECKPOINT_BASE_PATH;
    std::string titlePath;

    // Format 1: ID with name
    std::string safeName = GetSaveTitleName(title->getName());
    std::string path1 = basePath + titleIdStr + " " + safeName;

    // Format 2: just ID
    std::string path2 = basePath + titleIdStr;

    if (std::filesystem::exists(path1)) {
        titlePath = path1;
    } else if (std::filesystem::exists(path2)) {
        titlePath = path2;
    } else {
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
    // Refresh the saves data for this title and user
    RefreshSaves(title, currentUser);

    if (!title) {
        return {};
    }

    u64 titleId = title->getTitleId();
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

    // Mount the save filesystem
    FsFileSystem fs;
    Result res = MountSaveData(&fs, titleId, userId);

    if (R_FAILED(res)) {
        std::stringstream ss;
        ss << "Failed to mount save filesystem: 0x" << std::hex << res;
        LOG_ERROR(ss.str());
        return false;
    }

    // At this point, the save is mounted at "save:/"
    // We can now access it through standard file operations

    // Check if the save file exists
    std::string savePath = "save:/main";
    if (!std::filesystem::exists(savePath)) {
        std::stringstream ss;
        ss << "Save file not found at " << savePath;
        LOG_ERROR(ss.str());
        UnmountSaveData();
        return false;
    }

    // Here you would typically load the save data into memory
    // For demonstration, we'll just check that we can access it

    LOG_INFO("Successfully mounted console save");

    // Keep the save mounted for further operations
    // The save will be accessible at "save:/" in the filesystem

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