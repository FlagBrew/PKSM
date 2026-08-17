#pragma once

#include <optional>
#include <string>
#include <switch/types.h>
#include <unordered_map>
#include <vector>

#include "data/emulator/EmulatorSaveConfig.hpp"

namespace pksm::data::emulator {

struct ExtraSaveSlot {
    std::string name;
    std::vector<std::string> paths;
};

// One game from the bundled catalog (romfs data.json "emulator_games"):
// a non-installed game PKSM knows about, with candidate save locations.
struct EmulatorGameEntry {
    std::string name;
    std::string iconPath;
    u64 titleId = 0;
    std::vector<std::string> saveProbes;
    std::vector<ExtraSaveSlot> extraSaves;
    // Save-file family shared with sibling games whose saves are
    // byte-identical in kind (Ruby/Sapphire, Gold/Silver...); empty for
    // games whose saves discovery does not recognize
    std::string saveFamily;
    // Lowercase substrings that pick this game out of its family when a
    // discovered file's path names it (serials like "bpee" included)
    std::vector<std::string> keywords;
};

class EmulatorGameCatalog {
public:
    static constexpr const char* DefaultCatalogPath = "romfs:/gfx/data/data.json";

    static std::vector<EmulatorGameEntry> LoadFromDataJson(const std::string& jsonPath = DefaultCatalogPath);
    static std::optional<EmulatorGameEntry> FindByTitleId(
        const std::vector<EmulatorGameEntry>& entries,
        u64 titleId
    );
    static std::unordered_map<u64, EmulatorGameEntry> BuildIndexByTitleId(
        const std::vector<EmulatorGameEntry>& entries
    );

    // Every place a save for this game may live: catalog probes plus the
    // user's configured paths (order-preserving, deduplicated). Existence
    // and validity are the caller's concern.
    static std::vector<std::string> CandidatePaths(
        const EmulatorGameEntry& entry,
        const std::unordered_map<u64, EmulatorSaveSelection>& config
    );
};

}  // namespace pksm::data::emulator
