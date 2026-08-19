#pragma once

#include <cstdio>
#include <optional>
#include <string>
#include <switch.h>

namespace pksm::saves {

// Path of a save living inside an NSO emulator app's console save
// container: "nsosave:<16-hex title id>/saves/<code>/<file>". The scheme
// carries the container's own title id, so loading and write-back mount
// the right save data no matter which catalog game the save listed under;
// the remainder is the path inside the mounted container.
struct NSOSavePath {
    u64 nsoTitleId = 0;
    std::string innerPath;
};

inline std::string MakeNSOSavePath(u64 nsoTitleId, const std::string& innerPath) {
    char hex[17];
    snprintf(hex, sizeof(hex), "%016lX", nsoTitleId);
    return std::string("nsosave:") + hex + innerPath;
}

inline std::optional<NSOSavePath> ParseNSOSavePath(const std::string& path) {
    constexpr size_t SCHEME_LEN = 8;  // "nsosave:"
    if (path.rfind("nsosave:", 0) != 0 || path.size() <= SCHEME_LEN + 16) {
        return std::nullopt;
    }
    NSOSavePath out;
    try {
        out.nsoTitleId = std::stoull(path.substr(SCHEME_LEN, 16), nullptr, 16);
    } catch (...) {
        return std::nullopt;
    }
    out.innerPath = path.substr(SCHEME_LEN + 16);
    if (out.nsoTitleId == 0 || out.innerPath.front() != '/') {
        return std::nullopt;
    }
    return out;
}

}  // namespace pksm::saves
