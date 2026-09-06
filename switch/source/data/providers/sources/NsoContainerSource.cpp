#include "data/providers/sources/NsoContainerSource.hpp"

#include <algorithm>
#include <filesystem>

#include "data/emulator/SaveDiscovery.hpp"
#include "data/saves/ConsoleSaveMount.hpp"
#include "data/saves/NSOPathScheme.hpp"
#include "data/saves/SaveValidator.hpp"
#include "utils/Logger.hpp"

void NsoContainerSource::Scan(const AccountUid& userId) {
    saves.clear();
    int found = 0;

    for (const u64 nsoTitleId : pksm::data::emulator::SaveDiscovery::NSO_APP_TITLES) {
        FsFileSystem fs;
        if (R_FAILED(pksm::saves::MountConsoleSave(&fs, nsoTitleId, userId))) {
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
                const auto matched = pksm::data::emulator::SaveDiscovery::MatchGames(
                    games,
                    file.path().string(),
                    sav->version()
                );
                if (matched.empty()) {
                    continue;
                }

                const std::string filename = file.path().filename().string();
                const std::string path =
                    pksm::saves::MakeNSOSavePath(nsoTitleId, "/saves/" + code + "/" + filename);
                // "[NSO] main" is the save the app itself plays from; orphans
                // are labeled by their removed entry's code
                const bool orphan = installedCodes && installedCodes->find(code) == installedCodes->end();
                // Same rule as discovery: orphans list only where identification is exact
                if (orphan && matched.size() > 1) {
                    continue;
                }
                found++;
                for (const auto* game : matched) {
                    auto& list = saves[game->titleId];
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
        pksm::saves::UnmountConsoleSave();
    }
    // Live saves before orphans on every tile
    for (auto& [titleId, list] : saves) {
        std::stable_partition(list.begin(), list.end(), [](const pksm::saves::Save::Ref& s) {
            return s->getName().rfind("[NSO]", 0) == 0;
        });
    }
    LOG_INFO("NSO container scan: " + std::to_string(found) + " saves matched to catalog games");
}

std::vector<pksm::saves::Save::Ref> NsoContainerSource::List(u64 titleId, const std::optional<AccountUid>& user) {
    if (user) {
        const bool sameUser = scanUser && scanUser->uid[0] == user->uid[0] && scanUser->uid[1] == user->uid[1];
        if (!sameUser) {
            Scan(*user);
            scanUser = *user;
        }
    }
    const auto it = saves.find(titleId);
    return it != saves.end() ? it->second : std::vector<pksm::saves::Save::Ref>{};
}

std::optional<pksm::saves::PendingLoad>
NsoContainerSource::Resolve(const std::string& nsoPath, const AccountUid& userId) {
    const auto parsed = pksm::saves::ParseNSOSavePath(nsoPath);
    if (!parsed) {
        LOG_ERROR("Malformed NSO save path: " + nsoPath);
        return std::nullopt;
    }

    FsFileSystem fs;
    if (R_FAILED(pksm::saves::MountConsoleSave(&fs, parsed->nsoTitleId, userId))) {
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
