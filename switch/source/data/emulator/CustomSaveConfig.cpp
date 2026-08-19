#include "data/emulator/CustomSaveConfig.hpp"

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

std::unordered_map<u64, CustomSaveSelection> CustomSaveConfig::Load(const std::string& path) {
    std::unordered_map<u64, CustomSaveSelection> out;

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

        CustomSaveSelection sel;

        const auto& v = it.value();
        if (v.contains("primary") && v["primary"].is_array()) {
            for (const auto& p : v["primary"]) {
                if (p.is_string()) {
                    UniquePush(sel.primary, p.get<std::string>());
                }
            }
        }

        if (!sel.primary.empty()) {
            out.emplace(titleId, std::move(sel));
        }
    }

    return out;
}

bool CustomSaveConfig::Save(const std::unordered_map<u64, CustomSaveSelection>& data, const std::string& path) {
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

void CustomSaveConfig::AddPrimaryPath(std::unordered_map<u64, CustomSaveSelection>& data, u64 titleId, const std::string& path) {
    if (titleId == 0) {
        return;
    }
    UniquePush(data[titleId].primary, path);
}


}  // namespace pksm::data::emulator
