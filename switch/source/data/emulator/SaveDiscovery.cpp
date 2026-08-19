#include "data/emulator/SaveDiscovery.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <set>

#include "data/saves/SaveValidator.hpp"
#include "enums/GameVersion.hpp"
#include "utils/Logger.hpp"

namespace fs = std::filesystem;

namespace {

// Locations emulators write saves to on this console. RetroArch's own
// save root and the common ROM tree cover its four path modes: an explicit
// savefile_directory (with optional per-core/per-content subdirs) resolves
// under the save root or the configured directory, and
// savefiles_in_content_dir puts saves next to the ROM - as do standalone
// mGBA's default and the sort-by-core layout (<romdir>/<CoreName>/x.srm).
constexpr const char* RETROARCH_CFG = "sdmc:/retroarch/retroarch.cfg";
constexpr const char* RETROARCH_SAVE_ROOT = "sdmc:/retroarch/cores/savefiles/";
constexpr const char* MGBA_CONFIG = "sdmc:/mGBA/config.ini";
constexpr const char* ROM_TREE_ROOT = "sdmc:/roms";

// Citra-lineage 3DS emulators (the Dekopon/Raika Azahar Switch ports; the
// same tree shape as desktop Citra/Azahar copied to the card) keep saves in
// a virtual SD card:
//   <user dir>/sdmc/Nintendo 3DS/<id0>/<id1>/title/00040000/<title-lo>/data/00000001/main
// where id0/id1 are 32 zeros when the emulator made the tree but real hex
// when it was copied off a console's SD card, and gen 1/2 Virtual Console
// titles store sav.dat in place of main
constexpr const char* CITRA_USER_DIRS[] = {"sdmc:/switch/dekopon/", "sdmc:/switch/azahar/"};
constexpr const char* CITRA_TREE_NAME = "Nintendo 3DS";

// A scan is a boot-time cost; keep it bounded whatever the card holds
constexpr int MAX_SCAN_DEPTH = 4;
constexpr int MAX_SCAN_DIRS = 512;

const std::set<std::string> SAVE_EXTENSIONS = {".srm", ".sav", ".dsv"};

std::string ToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
    return s;
}

// `key = "value"` (RetroArch) and `key=value` (mGBA) both reduce to this
std::string ConfigValue(const std::string& configPath, const std::string& key) {
    std::ifstream f(configPath);
    if (!f.is_open()) {
        return "";
    }
    std::string line;
    while (std::getline(f, line)) {
        const auto eq = line.find('=');
        if (eq == std::string::npos) {
            continue;
        }
        std::string k = line.substr(0, eq);
        k.erase(std::remove_if(k.begin(), k.end(), [](unsigned char c) { return std::isspace(c); }), k.end());
        if (k != key) {
            continue;
        }
        std::string v = line.substr(eq + 1);
        const auto strip = [&v](char c) {
            v.erase(std::remove(v.begin(), v.end(), c), v.end());
        };
        strip('"');
        strip('\r');
        while (!v.empty() && std::isspace(static_cast<unsigned char>(v.front()))) {
            v.erase(v.begin());
        }
        while (!v.empty() && std::isspace(static_cast<unsigned char>(v.back()))) {
            v.pop_back();
        }
        return v;
    }
    return "";
}

// RetroArch writes SD-relative ("/retroarch/...") and app-relative
// (":/savefiles") directory values; give them their device back
std::string NormalizeConfiguredPath(const std::string& path) {
    if (path.rfind(":/", 0) == 0) {
        return "sdmc:/retroarch" + path.substr(1);
    }
    if (!path.empty() && path.front() == '/') {
        return "sdmc:" + path;
    }
    return path;
}

std::vector<std::string> CollectScanRoots() {
    // Configured directories are small and explicit user intent; scan them
    // before the ROM tree so a large library cannot starve them of the
    // directory budget
    std::vector<std::string> roots = {RETROARCH_SAVE_ROOT};

    // An explicit RetroArch save directory only applies when saves are not
    // redirected into the content dir (the ROM tree covers that mode)
    const std::string savefileDir = ConfigValue(RETROARCH_CFG, "savefile_directory");
    const bool inContentDir = ConfigValue(RETROARCH_CFG, "savefiles_in_content_dir") == "true";
    if (!savefileDir.empty() && savefileDir != "default" && !inContentDir) {
        roots.push_back(NormalizeConfiguredPath(savefileDir));
    }

    const std::string mgbaDir = ConfigValue(MGBA_CONFIG, "savegamePath");
    if (!mgbaDir.empty()) {
        roots.push_back(NormalizeConfiguredPath(mgbaDir));
    }

    roots.push_back(ROM_TREE_ROOT);
    return roots;
}

struct FoundFile {
    std::string path;
    fs::file_time_type mtime;
};

std::vector<FoundFile> ScanForSaveFiles(const std::vector<std::string>& roots) {
    std::vector<FoundFile> found;
    int dirBudget = MAX_SCAN_DIRS;

    for (const auto& root : roots) {
        std::error_code ec;
        fs::recursive_directory_iterator it(root, fs::directory_options::skip_permission_denied, ec);
        if (ec) {
            continue;
        }
        for (const fs::recursive_directory_iterator end; it != end && dirBudget > 0; it.increment(ec)) {
            if (ec) {
                break;
            }
            const auto& entry = *it;
            if (entry.is_directory(ec)) {
                dirBudget--;
                if (it.depth() >= MAX_SCAN_DEPTH) {
                    it.disable_recursion_pending();
                }
                continue;
            }
            if (!entry.is_regular_file(ec)) {
                continue;
            }
            const std::string ext = ToLower(entry.path().extension().string());
            if (SAVE_EXTENSIONS.find(ext) == SAVE_EXTENSIONS.end()) {
                continue;
            }
            const auto size = entry.file_size(ec);
            if (ec || size == 0 || size > pksm::saves::SaveValidator::MAX_SAVE_SIZE) {
                continue;
            }
            auto mtime = entry.last_write_time(ec);
            if (ec) {
                mtime = fs::file_time_type{};
            }
            found.push_back({entry.path().string(), mtime});
        }
    }
    if (dirBudget <= 0) {
        LOG_INFO("Save discovery stopped early: directory budget exhausted, some locations were not scanned");
    }
    return found;
}

bool IsHex32(const std::string& name) {
    return name.size() == 32 &&
        std::all_of(name.begin(), name.end(), [](unsigned char c) { return std::isxdigit(c) != 0; });
}

// The Switch ports redirect their whole user dir through a plain-text
// pointer file next to the default location
std::string CitraUserDir(const std::string& defaultDir) {
    std::ifstream f(defaultDir + "user_dir.txt");
    if (!f.is_open()) {
        return defaultDir;
    }
    std::string redirected((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    // Trim the ends only; the emulator allows interior spaces in the path
    while (!redirected.empty() && std::isspace(static_cast<unsigned char>(redirected.front()))) {
        redirected.erase(redirected.begin());
    }
    while (!redirected.empty() && std::isspace(static_cast<unsigned char>(redirected.back()))) {
        redirected.pop_back();
    }
    if (redirected.empty()) {
        return defaultDir;
    }
    if (redirected.back() != '/') {
        redirected += '/';
    }
    return redirected;
}

std::vector<std::string> CitraTreeRoots() {
    // Both ports' pointer files can target the same user dir; a duplicate
    // root would list every save in it twice
    std::vector<std::string> roots;
    auto push = [&roots](const std::string& p) {
        if (std::find(roots.begin(), roots.end(), p) == roots.end()) {
            roots.push_back(p);
        }
    };
    for (const char* base : CITRA_USER_DIRS) {
        push(CitraUserDir(base) + "sdmc/" + CITRA_TREE_NAME + "/");
    }
    // A console's SD card copied onto this one wholesale
    push(std::string("sdmc:/") + CITRA_TREE_NAME + "/");
    return roots;
}

// The virtual-SD tree is too deep for the generic sweep, but its shape is
// fixed, so walk it level by level instead of recursing
void ScanCitraTrees(std::vector<FoundFile>& found) {
    int dirBudget = MAX_SCAN_DIRS;
    const auto eachSubdir = [&dirBudget](const fs::path& dir, const auto& visit) {
        std::error_code ec;
        fs::directory_iterator it(dir, fs::directory_options::skip_permission_denied, ec);
        if (ec) {
            return;
        }
        for (const fs::directory_iterator end; it != end && dirBudget > 0; it.increment(ec)) {
            if (ec) {
                break;
            }
            std::error_code typeEc;
            if (!it->is_directory(typeEc) || typeEc) {
                continue;
            }
            dirBudget--;
            visit(it->path());
        }
    };

    for (const auto& root : CitraTreeRoots()) {
        eachSubdir(fs::path(root), [&](const fs::path& id0) {
            if (!IsHex32(id0.filename().string())) {
                return;
            }
            eachSubdir(id0, [&](const fs::path& id1) {
                if (!IsHex32(id1.filename().string())) {
                    return;
                }
                eachSubdir(id1 / "title" / "00040000", [&](const fs::path& titleDir) {
                    for (const char* filename : {"main", "sav.dat"}) {
                        const fs::path savePath = titleDir / "data" / "00000001" / filename;
                        std::error_code ec;
                        if (!fs::is_regular_file(savePath, ec) || ec) {
                            continue;
                        }
                        const auto size = fs::file_size(savePath, ec);
                        if (ec || size == 0 || size > pksm::saves::SaveValidator::MAX_SAVE_SIZE) {
                            continue;
                        }
                        auto mtime = fs::last_write_time(savePath, ec);
                        if (ec) {
                            mtime = fs::file_time_type{};
                        }
                        found.push_back({savePath.string(), mtime});
                    }
                });
            });
        });
    }
    if (dirBudget <= 0) {
        LOG_INFO("Citra tree scan stopped early: directory budget exhausted, some locations were not scanned");
    }
}

// Core's version() collapses sibling games into one save family; the
// catalog's save_family keys pick up from here
std::string FamilyOf(::pksm::GameVersion version) {
    switch (version) {
        case ::pksm::GameVersion::RD:
            return "rby";
        case ::pksm::GameVersion::GD:
            return "gs";
        case ::pksm::GameVersion::C:
            return "c";
        case ::pksm::GameVersion::S:
            return "rs";
        case ::pksm::GameVersion::E:
            return "e";
        case ::pksm::GameVersion::FR:
            return "frlg";
        case ::pksm::GameVersion::D:
            return "dp";
        case ::pksm::GameVersion::Pt:
            return "pt";
        case ::pksm::GameVersion::HG:
            return "hgss";
        case ::pksm::GameVersion::B:
            return "b";
        case ::pksm::GameVersion::W:
            return "w";
        case ::pksm::GameVersion::B2:
            return "b2";
        case ::pksm::GameVersion::W2:
            return "w2";
        // Gen 6/7 saves carry their exact version, so each game is a
        // one-member family and never needs keyword disambiguation
        case ::pksm::GameVersion::X:
            return "x";
        case ::pksm::GameVersion::Y:
            return "y";
        case ::pksm::GameVersion::OR:
            return "or";
        case ::pksm::GameVersion::AS:
            return "as";
        case ::pksm::GameVersion::SN:
            return "sn";
        case ::pksm::GameVersion::MN:
            return "mn";
        case ::pksm::GameVersion::US:
            return "us";
        case ::pksm::GameVersion::UM:
            return "um";
        default:
            return "";
    }
}

}  // namespace

namespace pksm::data::emulator {

std::unordered_map<u64, std::vector<std::string>> SaveDiscovery::Discover(const std::vector<EmulatorGameEntry>& games) {
    std::unordered_map<u64, std::vector<std::string>> discovered;

    auto files = ScanForSaveFiles(CollectScanRoots());
    ScanCitraTrees(files);
    // Most recently written first, so each game lists its live save on top
    std::sort(files.begin(), files.end(), [](const FoundFile& a, const FoundFile& b) {
        return a.mtime > b.mtime;
    });

    int matched = 0;
    for (const auto& file : files) {
        const auto sav = pksm::saves::SaveValidator::Load(file.path);
        if (!sav) {
            continue;
        }
        const std::string family = FamilyOf(sav->version());
        if (family.empty()) {
            continue;
        }

        std::vector<const EmulatorGameEntry*> candidates;
        for (const auto& game : games) {
            if (game.saveFamily == family) {
                candidates.push_back(&game);
            }
        }
        if (candidates.empty()) {
            continue;
        }

        // Path keywords pick a game out of a multi-game family; a file
        // nothing names lists under the whole family
        if (candidates.size() > 1) {
            const std::string lowerPath = ToLower(file.path);
            std::vector<const EmulatorGameEntry*> named;
            for (const auto* game : candidates) {
                const bool hit = std::any_of(
                    game->keywords.begin(),
                    game->keywords.end(),
                    [&lowerPath](const std::string& k) { return lowerPath.find(k) != std::string::npos; }
                );
                if (hit) {
                    named.push_back(game);
                }
            }
            if (!named.empty()) {
                candidates = std::move(named);
            }
        }

        matched++;
        for (const auto* game : candidates) {
            discovered[game->titleId].push_back(file.path);
        }
    }

    LOG_INFO(
        "Save discovery: " + std::to_string(files.size()) + " candidate files, " + std::to_string(matched) +
        " matched to catalog games"
    );
    return discovered;
}

}  // namespace pksm::data::emulator
