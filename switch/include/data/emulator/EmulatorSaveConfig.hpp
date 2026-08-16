#pragma once

#include <string>
#include <switch/types.h>
#include <unordered_map>
#include <vector>

namespace pksm::data::emulator {

// User-selected save locations for one catalog game: primary saves plus
// named extra slots, all arbitrary sdmc paths.
struct EmulatorSaveSelection {
    std::vector<std::string> primary;
    std::unordered_map<std::string, std::vector<std::string>> extra;
};

class EmulatorSaveConfig {
public:
    static constexpr const char* DefaultConfigPath = "sdmc:/switch/PKSM/emulator_saves.json";

    static std::unordered_map<u64, EmulatorSaveSelection> Load(const std::string& path = DefaultConfigPath);
    static bool Save(
        const std::unordered_map<u64, EmulatorSaveSelection>& data,
        const std::string& path = DefaultConfigPath
    );

    static void AddPrimaryPath(
        std::unordered_map<u64, EmulatorSaveSelection>& data,
        u64 titleId,
        const std::string& path
    );
    static void AddExtraPath(
        std::unordered_map<u64, EmulatorSaveSelection>& data,
        u64 titleId,
        const std::string& slotName,
        const std::string& path
    );
};

}  // namespace pksm::data::emulator
