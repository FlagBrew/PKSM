#include "data/saves/SaveValidator.hpp"

#include <cstdio>
#include <filesystem>
#include <sstream>

#include "utils/Logger.hpp"

namespace pksm::saves {

std::string SaveValidator::Summary::Describe() const {
    std::stringstream ss;
    ss << "gen " << generation << ", OT " << otName << " [" << tid << "/" << sid << "], " << int(badges)
       << " badges, " << playedHours << "h";
    return ss.str();
}

std::unique_ptr<::pksm::Sav> SaveValidator::Load(const std::string& path) {
    std::error_code ec;
    const auto size = std::filesystem::file_size(std::filesystem::path(path), ec);
    if (ec || size == 0 || size > MAX_SAVE_SIZE) {
        return nullptr;
    }

    FILE* f = fopen(path.c_str(), "rb");
    if (!f) {
        return nullptr;
    }

    std::shared_ptr<u8[]> data(new u8[size]);
    const size_t read = fread(data.get(), 1, size, f);
    fclose(f);
    if (read != size) {
        return nullptr;
    }

    // The input is an arbitrary user file; a parse throwing on malformed
    // content means "not a save", not a crash
    try {
        return ::pksm::Sav::getSave(data, size);
    } catch (const std::exception& e) {
        LOG_ERROR("Core threw while parsing " + path + ": " + e.what());
        return nullptr;
    }
}

SaveValidator::Summary SaveValidator::Summarize(const ::pksm::Sav& sav) {
    Summary out;
    out.generation = static_cast<std::string>(sav.generation());
    out.otName = sav.otName();
    out.tid = sav.TID();
    out.sid = sav.SID();
    out.badges = sav.badges();
    out.playedHours = sav.playedHours();
    return out;
}

std::optional<SaveValidator::Summary> SaveValidator::Validate(const std::string& path) {
    const auto sav = Load(path);
    if (!sav) {
        return std::nullopt;
    }
    return Summarize(*sav);
}

}  // namespace pksm::saves
