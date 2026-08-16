#include "data/emulator/EmulatorSaveConfig.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

namespace pksm::data::emulator {

namespace {

bool EnsureParentDir(const std::string& path) {
    try {
        std::filesystem::path p(path);
        const auto parent = p.parent_path();
        if (!parent.empty()) {
            std::filesystem::create_directories(parent);
        }
        return true;
    } catch (...) {
        return false;
    }
}

void UniquePush(std::vector<std::string>& v, const std::string& s) {
    if (s.empty()) {
        return;
    }
    if (std::find(v.begin(), v.end(), s) == v.end()) {
        v.push_back(s);
    }
}

}  // namespace

std::unordered_map<u64, EmulatorSaveSelection> EmulatorSaveConfig::Load(const std::string& path) {
    std::unordered_map<u64, EmulatorSaveSelection> out;

    std::ifstream f(path);
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

    if (!j.contains("games") || !j["games"].is_object()) {
        return out;
    }

    for (auto it = j["games"].begin(); it != j["games"].end(); ++it) {
        const std::string titleIdKey = it.key();
        u64 titleId = 0;
        try {
            titleId = std::stoull(titleIdKey, nullptr, 16);
        } catch (...) {
            continue;
        }

        if (!it.value().is_object()) {
            continue;
        }

        EmulatorSaveSelection sel;

        const auto& v = it.value();
        if (v.contains("primary") && v["primary"].is_array()) {
            for (const auto& p : v["primary"]) {
                if (p.is_string()) {
                    UniquePush(sel.primary, p.get<std::string>());
                }
            }
        }

        if (v.contains("extra") && v["extra"].is_object()) {
            for (auto eit = v["extra"].begin(); eit != v["extra"].end(); ++eit) {
                if (!eit.value().is_array()) {
                    continue;
                }

                std::vector<std::string> paths;
                for (const auto& p : eit.value()) {
                    if (p.is_string()) {
                        UniquePush(paths, p.get<std::string>());
                    }
                }

                if (!paths.empty()) {
                    sel.extra.emplace(eit.key(), std::move(paths));
                }
            }
        }

        if (!sel.primary.empty() || !sel.extra.empty()) {
            out.emplace(titleId, std::move(sel));
        }
    }

    return out;
}

bool EmulatorSaveConfig::Save(const std::unordered_map<u64, EmulatorSaveSelection>& data, const std::string& path) {
    if (!EnsureParentDir(path)) {
        return false;
    }

    nlohmann::json out;
    out["games"] = nlohmann::json::object();

    for (const auto& [titleId, sel] : data) {
        if (titleId == 0) {
            continue;
        }

        char key[17];
        std::snprintf(key, sizeof(key), "%016llx", static_cast<unsigned long long>(titleId));

        nlohmann::json g;
        g["primary"] = sel.primary;
        g["extra"] = nlohmann::json::object();
        for (const auto& [slotName, paths] : sel.extra) {
            g["extra"][slotName] = paths;
        }

        out["games"][key] = std::move(g);
    }

    std::ofstream f(path, std::ios::trunc);
    if (!f.is_open()) {
        return false;
    }

    try {
        f << out.dump(2);
    } catch (...) {
        return false;
    }

    return f.good();
}

void EmulatorSaveConfig::AddPrimaryPath(std::unordered_map<u64, EmulatorSaveSelection>& data, u64 titleId, const std::string& path) {
    if (titleId == 0) {
        return;
    }
    UniquePush(data[titleId].primary, path);
}

void EmulatorSaveConfig::AddExtraPath(
    std::unordered_map<u64, EmulatorSaveSelection>& data,
    u64 titleId,
    const std::string& slotName,
    const std::string& path
) {
    if (titleId == 0 || slotName.empty()) {
        return;
    }

    UniquePush(data[titleId].extra[slotName], path);
}

}  // namespace pksm::data::emulator
