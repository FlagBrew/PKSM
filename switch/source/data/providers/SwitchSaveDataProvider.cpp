#include "data/providers/SwitchSaveDataProvider.hpp"

#include <algorithm>
#include <cstring>
#include <dirent.h>
#include <filesystem>
#include <sstream>
#include <switch.h>

#include "data/emulator/CustomSaveConfig.hpp"
#include "data/emulator/SaveDiscovery.hpp"
#include "data/saves/ConsoleSaveMount.hpp"
#include "data/saves/NSOPathScheme.hpp"
#include "data/saves/SaveValidator.hpp"
#include "utils/Logger.hpp"

SwitchSaveDataProvider::SwitchSaveDataProvider() {
    emulatorGames = pksm::data::emulator::EmulatorGameCatalog::LoadFromDataJson();
    emulatorCatalog = pksm::data::emulator::EmulatorGameCatalog::BuildIndexByTitleId(emulatorGames);
}

void SwitchSaveDataProvider::PrewarmValidationCache() {
    validationCache.Prewarm([this]() {
        std::vector<std::string> paths;
        for (const auto& [titleId, list] : DiscoveredSaves()) {
            paths.insert(paths.end(), list.begin(), list.end());
        }
        const auto cfg = pksm::data::emulator::CustomSaveConfig::Load();
        for (const auto& [titleId, entry] : emulatorCatalog) {
            for (const auto& path : pksm::data::emulator::EmulatorGameCatalog::CandidatePaths(entry.titleId, cfg)) {
                std::error_code ec;
                if (std::filesystem::is_regular_file(std::filesystem::path(path), ec) && !ec) {
                    paths.push_back(path);
                }
            }
        }
        return paths;
    });
}

const std::unordered_map<u64, std::vector<std::string>>& SwitchSaveDataProvider::DiscoveredSaves() const {
    // Prewarm worker and UI thread can both trigger the lazy build; its own
    // mutex so the seconds-long scan never blocks validationCache hits
    std::lock_guard<std::mutex> lg(discoveryMutex);
    if (!discoveredSaves) {
        // The scan already core-parses every candidate; capture the verdicts
        // so nothing parses these files a second time
        discoveredSaves = pksm::data::emulator::SaveDiscovery::Discover(
            emulatorGames,
            [this](const std::string& path, std::filesystem::file_time_type mtime, std::uintmax_t size, bool valid) {
                validationCache.Record(path, mtime, size, valid);
            }
        );
    }
    return *discoveredSaves;
}

void SwitchSaveDataProvider::ScanNSOContainers(const AccountUid& userId) const {
    nsoConsoleSaves.clear();
    int found = 0;

    for (const u64 nsoTitleId : pksm::data::emulator::SaveDiscovery::NSO_APP_TITLES) {
        FsFileSystem fs;
        if (R_FAILED(MountSaveData(&fs, nsoTitleId, userId))) {
            // App not installed, or this account never launched it
            continue;
        }

        // The app's game list separates saves it still opens from orphans of removed entries
        const auto installedCodes = pksm::data::emulator::SaveDiscovery::InstalledNSOCodes(nsoTitleId);

        // Non-throwing iteration: a throw mid-scan would leave the save
        // device mounted, poisoning every later mount
        std::error_code ec;
        std::filesystem::directory_iterator codeIt("save:/saves", ec);
        for (const std::filesystem::directory_iterator end; !ec && codeIt != end; codeIt.increment(ec)) {
            const auto& codeDir = *codeIt;
            std::error_code typeEc;
            if (!codeDir.is_directory(typeEc) || typeEc) {
                continue;
            }
            const std::string code = codeDir.path().filename().string();
            std::filesystem::directory_iterator fileIt(codeDir.path(), typeEc);
            for (const std::filesystem::directory_iterator fend; !typeEc && fileIt != fend;
                 fileIt.increment(typeEc)) {
                const auto& file = *fileIt;
                std::error_code fileEc;
                if (!file.is_regular_file(fileEc) || fileEc) {
                    continue;
                }
                auto sav = pksm::saves::SaveValidator::Load(file.path().string());
                if (!sav) {
                    continue;
                }
                const auto games = pksm::data::emulator::SaveDiscovery::MatchGames(
                    emulatorGames,
                    file.path().string(),
                    sav->version()
                );
                if (games.empty()) {
                    continue;
                }

                const std::string filename = file.path().filename().string();
                const std::string path =
                    pksm::saves::MakeNSOSavePath(nsoTitleId, "/saves/" + code + "/" + filename);
                // "[NSO] main" is the save the app itself plays from; orphans
                // are labeled by their removed entry's code
                const bool orphan = installedCodes && installedCodes->find(code) == installedCodes->end();
                // Same rule as discovery: orphans list only where identification is exact
                if (orphan && games.size() > 1) {
                    continue;
                }
                found++;
                for (const auto* game : games) {
                    auto& list = nsoConsoleSaves[game->titleId];
                    const auto taken = [&list](const std::string& n) {
                        return std::any_of(list.begin(), list.end(), [&n](const pksm::saves::Save::Ref& s) {
                            return s->getName() == n;
                        });
                    };
                    std::string name = orphan ? "[Orphaned NSO] " + code : "[NSO] main";
                    if (taken(name)) {
                        name = orphan ? "[Orphaned NSO] " + code + "/" + filename : "[NSO] main (" + code + ")";
                    }
                    if (taken(name)) {
                        name = "[NSO] main (" + code + "/" + filename + ")";
                    }
                    list.push_back(pksm::saves::Save::New(name, path));
                }
            }
        }
        UnmountSaveData();
    }
    // Live saves before orphans on every tile
    for (auto& [titleId, list] : nsoConsoleSaves) {
        std::stable_partition(list.begin(), list.end(), [](const pksm::saves::Save::Ref& s) {
            return s->getName().rfind("[NSO]", 0) == 0;
        });
    }
    LOG_INFO("NSO container scan: " + std::to_string(found) + " saves matched to catalog games");
}

std::vector<pksm::saves::Save::Ref>
SwitchSaveDataProvider::ListNSOConsoleSaves(u64 titleId, const std::optional<AccountUid>& user) const {
    if (user) {
        const bool sameUser = nsoScanUser && nsoScanUser->uid[0] == user->uid[0] && nsoScanUser->uid[1] == user->uid[1];
        if (!sameUser) {
            ScanNSOContainers(*user);
            nsoScanUser = *user;
        }
    }
    const auto it = nsoConsoleSaves.find(titleId);
    return it != nsoConsoleSaves.end() ? it->second : std::vector<pksm::saves::Save::Ref>{};
}

std::optional<pksm::saves::PendingLoad>
SwitchSaveDataProvider::ResolveNSOConsoleSave(const std::string& nsoPath, const AccountUid& userId) {
    const auto parsed = pksm::saves::ParseNSOSavePath(nsoPath);
    if (!parsed) {
        LOG_ERROR("Malformed NSO save path: " + nsoPath);
        return std::nullopt;
    }

    FsFileSystem fs;
    if (R_FAILED(MountSaveData(&fs, parsed->nsoTitleId, userId))) {
        return std::nullopt;
    }
    // The Sav holds a full in-memory copy; write-back remounts by the title id in the recorded path
    pksm::saves::PendingLoad pending;
    pending.candidates = {"save:" + parsed->innerPath};
    pending.recordedPath = nsoPath;
    pending.description = "NSO container save";
    pending.mounted = true;
    return pending;
}

Result SwitchSaveDataProvider::MountSaveData(FsFileSystem* fs, u64 titleId, AccountUid userId) const {
    return pksm::saves::MountConsoleSave(fs, titleId, userId);
}

void SwitchSaveDataProvider::UnmountSaveData() const {
    pksm::saves::UnmountConsoleSave();
}

bool SwitchSaveDataProvider::HasConsoleSaveData(u64 titleId, const AccountUid& userId) const {
    // A save container exists the moment a profile launches a game; peek at
    // the raw fs level so empty containers don't count as save data
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

// Try candidates in order, known live-save names first, so a container's own
// rolling copy (SWSH's "backup") only wins when nothing else parses
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

// A virtual-SD save's parent dirs say nothing; label it with the emulator's
// user dir instead, e.g. "main (dekopon)"
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
    // Walk back over the virtual "sdmc" dir to the emulator's own dir
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

}  // namespace

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
        if (!validationCache.Validate(path)) {
            continue;
        }

        // A discovered file's directory identifies it, so qualify with the
        // parent; add the grandparent when two still read identically
        const std::filesystem::path p(path);
        const std::string filename = p.filename().string();
        std::string name;
        if (const auto citra = LabelCitraTreePath(p)) {
            name = "[" + citra->owner + "] " + filename;
            if (taken(name)) {
                name = "[" + citra->owner + "/" + citra->id0.substr(0, 8) + "] " + filename;
            }
        } else if (const auto nso = pksm::data::emulator::SaveDiscovery::ParseNSOBackupPath(p)) {
            name = "[" + nso->source + "] " + nso->backup + " (" + nso->code + ")";
            if (taken(name)) {
                name += " " + filename;
            }
        } else {
            name = "[" + p.parent_path().filename().string() + "] " + filename;
            if (taken(name)) {
                name = "[" + p.parent_path().parent_path().filename().string() + "/" +
                    p.parent_path().filename().string() + "] " + filename;
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

    // The config file is tiny and hand-editable; reload it on every listing
    const auto cfg = pksm::data::emulator::CustomSaveConfig::Load();
    const auto paths = pksm::data::emulator::EmulatorGameCatalog::CandidatePaths(catalogIt->first, cfg);

    for (const auto& path : paths) {
        std::error_code ec;
        if (!std::filesystem::is_regular_file(std::filesystem::path(path), ec) || ec) {
            continue;
        }
        if (!validationCache.Validate(path)) {
            continue;
        }

        // Name saves by filename; qualify with the parent when two candidates share one
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
    // Whether a discovered file parses was already decided by the scan itself
    const auto& discovered = DiscoveredSaves();
    const auto it = discovered.find(titleId);
    return it != discovered.end() && !it->second.empty();
}

bool SwitchSaveDataProvider::HasConfiguredEmulatorSaves(u64 titleId) const {
    auto catalogIt = emulatorCatalog.find(titleId);
    if (catalogIt == emulatorCatalog.end()) {
        return false;
    }

    // Configured files are validated at listing time; a tile only needs one candidate to exist
    const auto cfg = pksm::data::emulator::CustomSaveConfig::Load();
    const auto paths = pksm::data::emulator::EmulatorGameCatalog::CandidatePaths(catalogIt->first, cfg);
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

    // Don't mount a container HasConsoleSaveData judged empty
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

std::vector<pksm::saves::Save::Ref> SwitchSaveDataProvider::GetSavesForTitle(
    const pksm::titles::Title::Ref& title,
    const std::optional<AccountUid>& currentUser
) const {
    if (!title) {
        return {};
    }

    u64 titleId = title->getTitleId();

    // Catalog games' files live at arbitrary sdmc paths; the title's context
    // picks discovered (Emulator tab) vs configured (Custom tab) lists
    switch (title->getContext()) {
        case pksm::titles::TitleContext::Emulator: {
            // The NSO apps' own containers hold the live saves; SD files list after them
            auto saves = ListNSOConsoleSaves(titleId, currentUser);
            const auto discovered = ListDiscoveredSaves(titleId);
            saves.insert(saves.end(), discovered.begin(), discovered.end());
            return saves;
        }
        case pksm::titles::TitleContext::Custom:
            return ListConfiguredSaves(titleId);
        case pksm::titles::TitleContext::Console:
            break;
    }

    // Console saves refresh only on cache miss (a refresh mounts the save FS);
    // Checkpoint/JKSV rescans are cheap directory reads, done every time
    bool consoleCached = true;
    if (currentUser) {
        auto titleIt = consoleSaveCache.find(titleId);
        consoleCached = titleIt != consoleSaveCache.end() &&
            titleIt->second.count(*currentUser) > 0 && titleIt->second.at(*currentUser).isLoaded;
    }
    if (!consoleCached && currentUser) {
        RefreshConsoleSaves(title, *currentUser);
    }
    backupSaves.Refresh(title);
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

    // Add Checkpoint saves and JKSV backups
    const auto backups = backupSaves.List(titleId);
    result.insert(result.end(), backups.begin(), backups.end());

    return result;
}

std::optional<pksm::saves::PendingLoad> SwitchSaveDataProvider::ResolveConsoleSave(
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

    // Probe every file in the container root and let core decide which is the
    // save. FinishLoad unmounts as soon as the read completes - leaving
    // "save" mounted poisons every later mount attempt.
    pksm::saves::PendingLoad pending;
    for (const auto& entry : ListMountedSaveRoot()) {
        pending.candidates.push_back("save:/" + entry);
    }
    pending.description = "console save for " + title->getName();
    pending.mounted = true;
    if (pending.candidates.empty()) {
        LOG_ERROR("Save container for " + title->getName() + " lists no files");
        UnmountSaveData();
        return std::nullopt;
    }
    return pending;
}

std::optional<pksm::saves::PendingLoad> SwitchSaveDataProvider::ResolveLoad(
    const pksm::titles::Title::Ref& title,
    const std::string& saveName,
    const AccountUid* userId
) {
    if (!title) {
        return std::nullopt;
    }

    // Re-resolve the name against the title's own listing (context picks
    // discovered vs configured files)
    if (title->getContext() != pksm::titles::TitleContext::Console) {
        if (title->getContext() == pksm::titles::TitleContext::Emulator && userId) {
            for (const auto& save : ListNSOConsoleSaves(title->getTitleId(), *userId)) {
                if (save->getName() == saveName) {
                    return ResolveNSOConsoleSave(save->getPath(), *userId);
                }
            }
        }
        const bool fromDiscovery = title->getContext() == pksm::titles::TitleContext::Emulator;
        const auto saves =
            fromDiscovery ? ListDiscoveredSaves(title->getTitleId()) : ListConfiguredSaves(title->getTitleId());
        const char* context = fromDiscovery ? "discovered" : "configured";
        for (const auto& save : saves) {
            if (save->getName() == saveName) {
                pksm::saves::PendingLoad pending;
                pending.candidates = {save->getPath()};
                pending.description = std::string(context) + " emulator save";
                return pending;
            }
        }
        LOG_ERROR("No " + std::string(context) + " emulator save named " + saveName);
        return std::nullopt;
    }

    // Handle console save (which requires a user ID)
    if (saveName == "Console Save" && userId) {
        return ResolveConsoleSave(title, *userId);
    }

    // Try Checkpoint saves, then JKSV backups
    if (auto pending = backupSaves.Resolve(title->getTitleId(), saveName)) {
        return pending;
    }

    std::stringstream ss;
    ss << "Unable to resolve save " << saveName << " for title " << std::hex << title->getTitleId();
    LOG_ERROR(ss.str());
    return std::nullopt;
}

std::optional<pksm::saves::LoadedSave> SwitchSaveDataProvider::ExecuteLoad(const pksm::saves::PendingLoad& pending) {
    auto loaded = ProbeSaveFiles(pending.candidates, pending.description);
    if (!loaded) {
        LOG_ERROR(
            "No candidate parses as a " + pending.description + " (" + std::to_string(pending.candidates.size()) +
            " files near " + (pending.candidates.empty() ? "?" : pending.candidates.front()) + ")"
        );
        return std::nullopt;
    }
    if (!pending.recordedPath.empty()) {
        loaded->path = pending.recordedPath;
    }
    return loaded;
}

void SwitchSaveDataProvider::FinishLoad(const pksm::saves::PendingLoad& pending) {
    if (pending.mounted) {
        UnmountSaveData();
    }
}