#pragma once

#include <string>
#include <switch/types.h>
#include <unordered_map>
#include <vector>

namespace pksm::data::emulator {

// User-selected save locations for one catalog game (arbitrary sdmc paths)
struct CustomSaveSelection {
    std::vector<std::string> primary;
};

class CustomSaveConfig {
public:
    static constexpr const char* DefaultConfigPath = "sdmc:/switch/PKSM/emulator_saves.json";

    static std::unordered_map<u64, CustomSaveSelection> Load(const std::string& path = DefaultConfigPath);
    static bool Save(
        const std::unordered_map<u64, CustomSaveSelection>& data,
        const std::string& path = DefaultConfigPath
    );

    static void AddPrimaryPath(
        std::unordered_map<u64, CustomSaveSelection>& data,
        u64 titleId,
        const std::string& path
    );
};

}  // namespace pksm::data::emulator
