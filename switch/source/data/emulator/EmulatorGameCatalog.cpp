#include "data/emulator/EmulatorGameCatalog.hpp"

#include <algorithm>
#include <fstream>

#include <nlohmann/json.hpp>

namespace pksm::data::emulator {

namespace {

bool ParseTitleIdHex(const std::string& hex, u64& out) {
    try {
        out = std::stoull(hex, nullptr, 16);
        return true;
    } catch (...) {
        return false;
    }
}

}  // namespace

std::vector<EmulatorGameEntry> EmulatorGameCatalog::LoadFromDataJson(const std::string& jsonPath) {
    std::vector<EmulatorGameEntry> out;

    std::ifstream f(jsonPath);
    if (!f.is_open()) {
        return out;
    }

    nlohmann::json j;
    try {
        f >> j;
    } catch (...) {
        return out;
    }

    if (!j.is_object()) {
        return out;
    }

    if (!j.contains("emulator_games") || !j["emulator_games"].is_array()) {
        return out;
    }

    for (const auto& eg : j["emulator_games"]) {
        if (!eg.is_object()) {
            continue;
        }

        if (!eg.contains("name") || !eg["name"].is_string()) {
            continue;
        }
        if (!eg.contains("title_id") || !eg["title_id"].is_string()) {
            continue;
        }

        EmulatorGameEntry entry;
        entry.name = eg["name"].get<std::string>();

        const std::string titleIdHex = eg["title_id"].get<std::string>();
        if (!ParseTitleIdHex(titleIdHex, entry.titleId) || entry.titleId == 0) {
            continue;
        }

        if (eg.contains("save_family") && eg["save_family"].is_string()) {
            entry.saveFamily = eg["save_family"].get<std::string>();
        }

        if (eg.contains("keywords") && eg["keywords"].is_array()) {
            for (const auto& k : eg["keywords"]) {
                if (k.is_string()) {
                    entry.keywords.push_back(k.get<std::string>());
                }
            }
        }

        out.push_back(std::move(entry));
    }

    return out;
}

std::optional<EmulatorGameEntry> EmulatorGameCatalog::FindByTitleId(const std::vector<EmulatorGameEntry>& entries, u64 titleId) {
    for (const auto& e : entries) {
        if (e.titleId == titleId) {
            return e;
        }
    }
    return std::nullopt;
}

std::vector<std::string> EmulatorGameCatalog::CandidatePaths(
    u64 titleId,
    const std::unordered_map<u64, EmulatorSaveSelection>& config
) {
    std::vector<std::string> paths;
    auto it = config.find(titleId);
    if (it != config.end()) {
        for (const auto& p : it->second.primary) {
            if (!p.empty() && std::find(paths.begin(), paths.end(), p) == paths.end()) {
                paths.push_back(p);
            }
        }
    }
    return paths;
}

std::unordered_map<u64, EmulatorGameEntry> EmulatorGameCatalog::BuildIndexByTitleId(const std::vector<EmulatorGameEntry>& entries) {
    std::unordered_map<u64, EmulatorGameEntry> idx;
    for (const auto& e : entries) {
        if (e.titleId == 0) {
            continue;
        }
        idx.emplace(e.titleId, e);
    }
    return idx;
}

}  // namespace pksm::data::emulator
