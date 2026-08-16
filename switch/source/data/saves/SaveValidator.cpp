#include "data/saves/SaveValidator.hpp"

#include <cstdio>
#include <filesystem>
#include <memory>
#include <sstream>

#include "sav/Sav.hpp"
#include "utils/Logger.hpp"

namespace pksm::saves {

std::string SaveValidator::Summary::Describe() const {
    std::stringstream ss;
    ss << "gen " << generation << ", OT " << otName << " [" << tid << "/" << sid << "], " << int(badges)
       << " badges, " << playedHours << "h";
    return ss.str();
}

std::optional<SaveValidator::Summary> SaveValidator::Validate(const std::string& path) {
    std::error_code ec;
    const auto size = std::filesystem::file_size(std::filesystem::path(path), ec);
    if (ec || size == 0 || size > MAX_SAVE_SIZE) {
        return std::nullopt;
    }

    FILE* f = fopen(path.c_str(), "rb");
    if (!f) {
        return std::nullopt;
    }

    std::shared_ptr<u8[]> data(new u8[size]);
    const size_t read = fread(data.get(), 1, size, f);
    fclose(f);
    if (read != size) {
        return std::nullopt;
    }

    auto sav = ::pksm::Sav::getSave(data, size);
    if (!sav) {
        return std::nullopt;
    }

    Summary out;
    out.generation = static_cast<std::string>(sav->generation());
    out.otName = sav->otName();
    out.tid = sav->TID();
    out.sid = sav->SID();
    out.badges = sav->badges();
    out.playedHours = sav->playedHours();
    return out;
}

}  // namespace pksm::saves
