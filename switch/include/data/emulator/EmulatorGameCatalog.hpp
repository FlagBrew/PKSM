#pragma once

#include <optional>
#include <string>
#include <switch/types.h>
#include <unordered_map>
#include <vector>

#include "data/emulator/EmulatorSaveConfig.hpp"

namespace pksm::data::emulator {

struct EmulatorGameEntry {
    std::string name;
    u64 titleId = 0;
    // Save-file family shared with sibling games (Ruby/Sapphire, Gold/Silver...)
    std::string saveFamily;
    // Lowercase path substrings that pick this game out of its family
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

    static std::vector<std::string> CandidatePaths(
        u64 titleId,
        const std::unordered_map<u64, EmulatorSaveSelection>& config
    );
};

}  // namespace pksm::data::emulator
