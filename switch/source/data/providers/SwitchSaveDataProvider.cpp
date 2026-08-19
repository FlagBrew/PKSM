#include "data/providers/SwitchSaveDataProvider.hpp"

#include <algorithm>
#include <cstring>
#include <dirent.h>
#include <filesystem>
#include <sstream>
#include <switch.h>

#include "data/emulator/EmulatorSaveConfig.hpp"
#include "data/emulator/SaveDiscovery.hpp"
#include "data/saves/ConsoleSaveMount.hpp"
#include "data/saves/SafeNames.hpp"
#include "data/saves/SaveValidator.hpp"
#include "utils/Logger.hpp"

SwitchSaveDataProvider::SwitchSaveDataProvider() {
    emulatorGames = pksm::data::emulator::EmulatorGameCatalog::LoadFromDataJson();
    emulatorCatalog = pksm::data::emulator::EmulatorGameCatalog::BuildIndexByTitleId(emulatorGames);
}

SwitchSaveDataProvider::~SwitchSaveDataProvider() {
    prewarmStop = true;
    if (prewarmThread.joinable()) {
        prewarmThread.join();
    }
}

void SwitchSaveDataProvider::PrewarmValidationCache() {
    if (prewarmThread.joinable()) {
        return;
    }
    prewarmThread = std::thread([this]() {
        // An exception escaping a std::thread is std::terminate with no log;
        // a failed prewarm must degrade to cold landings instead
        try {
            const u64 t0 = armGetSystemTick();

            std::vector<std::string> paths;
            for (const auto& [titleId, list] : DiscoveredSaves()) {
                paths.insert(paths.end(), list.begin(), list.end());
            }
            const auto cfg = pksm::data::emulator::EmulatorSaveConfig::Load();
            for (const auto& [titleId, entry] : emulatorCatalog) {
                for (const auto& path : pksm::data::emulator::EmulatorGameCatalog::CandidatePaths(entry, cfg)) {
                    std::error_code ec;
                    if (std::filesystem::is_regular_file(std::filesystem::path(path), ec) && !ec) {
                        paths.push_back(path);
                    }
                }
            }
            // Save families share files (one .srm can match several games)
            std::sort(paths.begin(), paths.end());
            paths.erase(std::unique(paths.begin(), paths.end()), paths.end());

            size_t validated = 0;
            for (const auto& path : paths) {
                if (prewarmStop) {
                    return;
                }
                ValidateWithCore(path);
                validated++;
            }
            LOG_DEBUG(
                "Prewarmed save validation: " + std::to_string(validated) + " files in " +
                std::to_string(armTicksToNs(armGetSystemTick() - t0) / 1000000) + " ms"
            );
        } catch (const std::exception& e) {
            LOG_ERROR("Save prewarm aborted: " + std::string(e.what()));
        }
    });
}

const std::unordered_map<u64, std::vector<std::string>>& SwitchSaveDataProvider::DiscoveredSaves() const {
    // The prewarm worker and the UI thread can both trigger the lazy build;
    // the map is immutable once built. Its own mutex: the scan is seconds
    // long and must never make validationCache hits wait on it
    std::lock_guard<std::mutex> lg(discoveryMutex);
    if (!discoveredSaves) {
        // The scan already core-parses every candidate; capture the verdicts
        // so nothing (prewarm included) parses these files a second time
        discoveredSaves = pksm::data::emulator::SaveDiscovery::Discover(
            emulatorGames,
            [this](const std::string& path, std::filesystem::file_time_type mtime, std::uintmax_t size, bool valid) {
                std::lock_guard<std::mutex> cacheLock(validationMutex);
                validationCache[path] = {mtime, size, valid};
            }
        );
    }
    return *discoveredSaves;
}

Result SwitchSaveDataProvider::MountSaveData(FsFileSystem* fs, u64 titleId, AccountUid userId) const {
    return pksm::saves::MountConsoleSave(fs, titleId, userId);
}

void SwitchSaveDataProvider::UnmountSaveData() const {
    pksm::saves::UnmountConsoleSave();
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

// Try candidate files in order and load the first one core parses. Known
// live-save names are tried first; a container may also hold the game's own
// rolling copy (SWSH's "backup"), which should only win when nothing else
// parses.
std::optional<pksm::saves::LoadedSave> ProbeSaveFiles(
    std::vector<std::string> paths,
    const std::string& logContext
) {
    const auto rank = [](const std::string& path) {
        const std::string name = std::filesystem::path(path).filename().string();
        if (name == "main") {
            return 0;
        }
        if (name == "savedata.bin") {
            return 1;
        }
        return name.find("backup") != std::string::npos ? 3 : 2;
    };
    std::stable_sort(paths.begin(), paths.end(), [&rank](const std::string& a, const std::string& b) {
        return rank(a) < rank(b);
    });

    for (const auto& path : paths) {
        std::error_code ec;
        if (!std::filesystem::is_regular_file(std::filesystem::path(path), ec) || ec) {
            continue;
        }
        auto sav = pksm::saves::SaveValidator::Load(path);
        if (sav) {
            LOG_INFO(
                "Loaded " + logContext + " " + path + ": " +
                pksm::saves::SaveValidator::Summarize(*sav).Describe()
            );
            return pksm::saves::LoadedSave{std::move(sav), path};
        }
    }
    return std::nullopt;
}

// A save inside a Citra-style virtual SD ends in .../data/00000001/main,
// so its parent directories say nothing; label it with the emulator's user
// dir (the directory owning the tree) instead, e.g. "main (dekopon)"
struct CitraTreeLabel {
    std::string owner;
    std::string id0;
};
std::optional<CitraTreeLabel> LabelCitraTreePath(const std::filesystem::path& p) {
    std::vector<std::string> parts;
    for (const auto& part : p) {
        parts.push_back(part.string());
    }
    const auto it = std::find(parts.begin(), parts.end(), "Nintendo 3DS");
    if (it == parts.end() || it + 1 == parts.end()) {
        return std::nullopt;
    }
    CitraTreeLabel label;
    label.id0 = *(it + 1);
    // Walk back over the virtual "sdmc" dir to the emulator's own dir; a
    // tree at the card root has no owner
    auto owner = it;
    if (owner != parts.begin() && *(owner - 1) == "sdmc") {
        owner--;
    }
    if (owner != parts.begin() && (owner - 1)->find(':') == std::string::npos) {
        label.owner = *(owner - 1);
    } else {
        label.owner = "3DS card";
    }
    return label;
}

// A backup (Checkpoint, JKSV) is a directory; probe its files and let core
// decide which one is the save
std::optional<pksm::saves::LoadedSave> LoadBackupDirectory(
    const std::string& dirPath,
    const std::string& logContext
) {
    std::vector<std::string> candidates;
    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(dirPath, ec)) {
        candidates.push_back(entry.path().string());
    }
    if (auto loaded = ProbeSaveFiles(candidates, logContext)) {
        return loaded;
    }
    LOG_ERROR("No file in backup " + dirPath + " parses as a save");
    return std::nullopt;
}

}  // namespace

bool SwitchSaveDataProvider::ValidateWithCore(const std::string& path) const {
    std::error_code ec;
    const auto mtime = std::filesystem::last_write_time(std::filesystem::path(path), ec);
    const auto size = ec ? 0 : std::filesystem::file_size(std::filesystem::path(path), ec);
    if (ec) {
        return false;
    }

    {
        std::lock_guard<std::mutex> lg(validationMutex);
        auto it = validationCache.find(path);
        if (it != validationCache.end() && it->second.mtime == mtime && it->second.size == size) {
            return it->second.valid;
        }
    }

    // Validation itself runs unlocked - it reads and parses the whole file,
    // and must not block the UI thread's cache hits
    const auto summary = pksm::saves::SaveValidator::Validate(path);
    if (summary) {
        LOG_INFO("Validated save " + path + ": " + summary->Describe());
    } else {
        LOG_INFO("Rejected save candidate " + path + ": core cannot parse it");
    }

    std::lock_guard<std::mutex> lg(validationMutex);
    validationCache[path] = {mtime, size, summary.has_value()};
    return summary.has_value();
}

std::vector<pksm::saves::Save::Ref> SwitchSaveDataProvider::ListDiscoveredSaves(u64 titleId) const {
    std::vector<pksm::saves::Save::Ref> saves;

    const auto& discovered = DiscoveredSaves();
    const auto it = discovered.find(titleId);
    if (it == discovered.end()) {
        return saves;
    }

    const auto taken = [&saves](const std::string& name) {
        return std::any_of(saves.begin(), saves.end(), [&name](const pksm::saves::Save::Ref& s) {
            return s->getName() == name;
        });
    };

    for (const auto& path : it->second) {
        std::error_code ec;
        if (!std::filesystem::is_regular_file(std::filesystem::path(path), ec) || ec) {
            continue;
        }
        if (!ValidateWithCore(path)) {
            continue;
        }

        // A discovered file's directory is what identifies it (the emulator
        // or ROM folder it lives in), so always qualify with the parent;
        // when two files would still read identically, add the grandparent
        const std::filesystem::path p(path);
        const std::string filename = p.filename().string();
        std::string name;
        if (const auto citra = LabelCitraTreePath(p)) {
            name = filename + " (" + citra->owner + ")";
            if (taken(name)) {
                name = filename + " (" + citra->owner + "/" + citra->id0.substr(0, 8) + ")";
            }
        } else if (const auto nso = pksm::data::emulator::SaveDiscovery::ParseNSOBackupPath(p)) {
            name = "[" + nso->source + "] " + nso->backup + " (" + nso->code + ")";
            if (taken(name)) {
                name += " " + filename;
            }
        } else {
            name = filename + " (" + p.parent_path().filename().string() + ")";
            if (taken(name)) {
                name = filename + " (" + p.parent_path().parent_path().filename().string() + "/" +
                    p.parent_path().filename().string() + ")";
            }
        }
        saves.push_back(pksm::saves::Save::New(name, path));
    }

    return saves;
}

std::vector<pksm::saves::Save::Ref> SwitchSaveDataProvider::ListConfiguredSaves(u64 titleId) const {
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

bool SwitchSaveDataProvider::HasDiscoveredEmulatorSaves(u64 titleId) const {
    // Whether a discovered file parses as a save was already decided by the
    // scan itself
    const auto& discovered = DiscoveredSaves();
    const auto it = discovered.find(titleId);
    return it != discovered.end() && !it->second.empty();
}

bool SwitchSaveDataProvider::HasConfiguredEmulatorSaves(u64 titleId) const {
    auto catalogIt = emulatorCatalog.find(titleId);
    if (catalogIt == emulatorCatalog.end()) {
        return false;
    }

    // Whether a configured file parses as a save is decided at listing time;
    // a tile only needs one candidate to exist
    const auto cfg = pksm::data::emulator::EmulatorSaveConfig::Load();
    const auto paths = pksm::data::emulator::EmulatorGameCatalog::CandidatePaths(catalogIt->second, cfg);
    return std::any_of(paths.begin(), paths.end(), [](const std::string& path) {
        std::error_code ec;
        return std::filesystem::exists(std::filesystem::path(path), ec) && !ec;
    });
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

void SwitchSaveDataProvider::RefreshJKSVSaves(const pksm::titles::Title::Ref& title) const {
    if (!title) {
        return;
    }

    u64 titleId = title->getTitleId();
    std::vector<pksm::saves::Save::Ref> saves;

    // JKSV names a game's folder after its sanitized title, or after the
    // title ID when the name has no ASCII representation
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
        jksvSaveCache[titleId] = saves;
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

    jksvSaveCache[titleId] = saves;
}

void SwitchSaveDataProvider::RefreshSaves(
    const pksm::titles::Title::Ref& title,
    const std::optional<AccountUid>& userId
) const {
    if (!title) {
        return;
    }

    u64 titleId = title->getTitleId();

    // Emulator and Custom titles have no console/Checkpoint saves to
    // refresh; their listings are rebuilt (and revalidated on change) in
    // GetSavesForTitle
    if (title->getContext() != pksm::titles::TitleContext::Console) {
        return;
    }

    // If a user ID is provided, refresh console saves
    if (userId) {
        RefreshConsoleSaves(title, *userId);
    }

    // Refresh Checkpoint and JKSV backups (these are independent of the user)
    RefreshCheckpointSaves(title);
    RefreshJKSVSaves(title);

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

    // Catalog games never have console/Checkpoint saves; their files live
    // at arbitrary sdmc paths and are core-validated instead. The title's
    // context picks the save universe - the same game can sit on the
    // Emulator tab (discovered files) and the Custom tab (configured files)
    // with different lists.
    switch (title->getContext()) {
        case pksm::titles::TitleContext::Emulator:
            return ListDiscoveredSaves(titleId);
        case pksm::titles::TitleContext::Custom:
            return ListConfiguredSaves(titleId);
        case pksm::titles::TitleContext::Console:
            break;
    }

    // Console saves: refresh only on cache miss (a refresh mounts the save
    // FS, too heavy for every selection change). Checkpoint/JKSV backups:
    // always rescan - it's a cheap directory read, and backups can be
    // created mid-session.
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
    RefreshJKSVSaves(title);
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

    // Add JKSV backups
    auto jksvIt = jksvSaveCache.find(titleId);
    if (jksvIt != jksvSaveCache.end()) {
        result.insert(result.end(), jksvIt->second.begin(), jksvIt->second.end());
    }

    return result;
}

std::optional<pksm::saves::LoadedSave> SwitchSaveDataProvider::LoadConsoleSave(
    const pksm::titles::Title::Ref& title,
    const AccountUid& userId
) {
    if (!title) {
        return std::nullopt;
    }

    u64 titleId = title->getTitleId();

    std::stringstream ss;
    ss << "Loading console save for title " << std::hex << titleId << ", user " << std::hex << userId.uid[0];
    LOG_INFO(ss.str());

    fsdevUnmountDevice("save");
    FsFileSystem fs;
    Result res = fsOpen_SaveData(&fs, titleId, userId);
    if (R_FAILED(res)) {
        std::stringstream ss;
        ss << "Failed to open save data filesystem: 0x" << std::hex << res;
        LOG_ERROR(ss.str());
        return std::nullopt;
    }

    if (fsdevMountDevice("save", fs) == -1) {
        // fsdev closes fs itself on failure - do not fsFsClose here
        LOG_ERROR("Failed to mount save filesystem device");
        return std::nullopt;
    }

    // Save file names vary per game (main, savedata.bin, ...), so probe every
    // file in the container root and let core decide which one is the save
    auto rootEntries = ListMountedSaveRoot();
    std::vector<std::string> candidates;
    for (const auto& entry : rootEntries) {
        candidates.push_back("save:/" + entry);
    }
    auto loaded = ProbeSaveFiles(candidates, "console save for " + title->getName());

    // The Sav holds a full in-memory copy, so unmount immediately - leaving
    // "save" mounted poisons every later mount attempt. Write-back will
    // remount when it lands.
    UnmountSaveData();

    if (!loaded) {
        LOG_ERROR(
            "No file in the save container parses as a save for " + title->getName() +
            "; save root: " + JoinNames(rootEntries)
        );
    }
    return loaded;
}

std::optional<pksm::saves::LoadedSave> SwitchSaveDataProvider::LoadCheckpointSave(
    const pksm::titles::Title::Ref& title,
    const std::string& saveName
) {
    if (!title) {
        return std::nullopt;
    }

    u64 titleId = title->getTitleId();

    // A miss here is normal - LoadSave tries each save source in turn, so
    // only a claimed-but-failed load (in LoadBackupDirectory) is an error
    auto titleIt = checkpointSaveCache.find(titleId);
    if (titleIt == checkpointSaveCache.end()) {
        return std::nullopt;
    }

    // Find the save with the matching name
    for (const auto& save : titleIt->second) {
        if (save->getName() == saveName) {
            std::stringstream ss;
            ss << "Loading Checkpoint save " << saveName << " for title " << std::hex << titleId;
            LOG_INFO(ss.str());
            return LoadBackupDirectory(save->getPath(), "Checkpoint save");
        }
    }

    return std::nullopt;
}

std::optional<pksm::saves::LoadedSave> SwitchSaveDataProvider::LoadJKSVSave(
    const pksm::titles::Title::Ref& title,
    const std::string& saveName
) {
    if (!title) {
        return std::nullopt;
    }

    u64 titleId = title->getTitleId();

    auto titleIt = jksvSaveCache.find(titleId);
    if (titleIt == jksvSaveCache.end()) {
        return std::nullopt;
    }

    for (const auto& save : titleIt->second) {
        if (save->getName() == saveName) {
            return LoadBackupDirectory(save->getPath(), "JKSV save");
        }
    }

    return std::nullopt;
}

std::optional<pksm::saves::LoadedSave> SwitchSaveDataProvider::LoadSave(
    const pksm::titles::Title::Ref& title,
    const std::string& saveName,
    const AccountUid* userId
) {
    if (!title) {
        return std::nullopt;
    }

    // Emulator/Custom saves: re-resolve the name against the title's own
    // listing (its context picks discovered vs configured files) and parse
    // the file with core
    if (title->getContext() != pksm::titles::TitleContext::Console) {
        const bool fromDiscovery = title->getContext() == pksm::titles::TitleContext::Emulator;
        const auto saves =
            fromDiscovery ? ListDiscoveredSaves(title->getTitleId()) : ListConfiguredSaves(title->getTitleId());
        const char* context = fromDiscovery ? "discovered" : "configured";
        for (const auto& save : saves) {
            if (save->getName() == saveName) {
                auto sav = pksm::saves::SaveValidator::Load(save->getPath());
                if (sav) {
                    LOG_INFO(
                        "Loaded " + std::string(context) + " emulator save " + save->getPath() + ": " +
                        pksm::saves::SaveValidator::Summarize(*sav).Describe()
                    );
                    return pksm::saves::LoadedSave{std::move(sav), save->getPath()};
                }
                LOG_ERROR("Emulator save no longer parses: " + save->getPath());
                return std::nullopt;
            }
        }
        LOG_ERROR("No " + std::string(context) + " emulator save named " + saveName);
        return std::nullopt;
    }

    // Handle console save (which requires a user ID)
    if (saveName == "Console Save" && userId) {
        return LoadConsoleSave(title, *userId);
    }

    // Try to load from Checkpoint saves
    if (auto loaded = LoadCheckpointSave(title, saveName)) {
        return loaded;
    }

    // Try to load from JKSV backups
    if (auto loaded = LoadJKSVSave(title, saveName)) {
        return loaded;
    }

    std::stringstream ss;
    ss << "Unable to load save " << saveName << " for title " << std::hex << title->getTitleId();
    LOG_ERROR(ss.str());
    return std::nullopt;
}