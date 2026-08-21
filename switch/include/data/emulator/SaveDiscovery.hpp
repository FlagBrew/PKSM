#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "data/emulator/EmulatorGameCatalog.hpp"
#include "enums/GameVersion.hpp"

namespace pksm::data::emulator {

// Finds emulator save files in the SD card's known locations and matches
// them to catalog games, so the Emulator tab populates without hand-edited
// configuration.
class SaveDiscovery {
public:
    // Game Boy Advance and Game Boy - Nintendo Switch Online
    static constexpr u64 NSO_APP_TITLES[] = {0x010012F017576000ULL, 0x0100C62011050000ULL};

    static std::vector<const EmulatorGameEntry*>
    MatchGames(const std::vector<EmulatorGameEntry>& games, const std::string& path, ::pksm::GameVersion version);

    static std::optional<std::set<std::string>> InstalledNSOCodes(u64 nsoTitleId);

    // A file inside an NSO app backup: <JKSV or Checkpoint title dir>/<backup>/saves/<code>/<file>
    struct NSOBackupRef {
        u64 nsoTitleId = 0;
        std::string source;
        std::string backup;
        std::string code;
    };
    static std::optional<NSOBackupRef> ParseNSOBackupPath(const std::filesystem::path& path);

    // Reports each candidate's parse verdict so callers can seed validation caches
    using ParseReport = std::function<
        void(const std::string& path, std::filesystem::file_time_type mtime, std::uintmax_t size, bool valid)>;

    // titleId -> save paths, most recently written first
    static std::unordered_map<u64, std::vector<std::string>>
    Discover(const std::vector<EmulatorGameEntry>& games, const ParseReport& onParsed = {});
};

}  // namespace pksm::data::emulator
