#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "data/emulator/EmulatorGameCatalog.hpp"

namespace pksm::data::emulator {

// Finds emulator save files in the SD card's known locations and matches
// them to catalog games, so the Emulator tab populates without hand-edited
// configuration. Matching is content-first: core parses the file and is
// the authority on its save family; path keywords then pick a game out of
// a family with several (Ruby vs Sapphire), and a file no keyword resolves
// lists under every game in its family.
class SaveDiscovery {
public:
    // Scan once and match: titleId -> save paths, most recently written
    // first. Costs one directory sweep plus a core parse per candidate
    // file; callers keep the result for the session.
    static std::unordered_map<u64, std::vector<std::string>> Discover(const std::vector<EmulatorGameEntry>& games);
};

}  // namespace pksm::data::emulator
