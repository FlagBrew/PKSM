#include "data/saves/SaveValidator.hpp"

#include <cstdio>
#include <filesystem>
#include <new>
#include <sstream>

#include "data/saves/NSOContainer.hpp"
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

    // A save-sized allocation can fail on a fragmented heap; treat as "cannot validate"
    std::shared_ptr<u8[]> data(new (std::nothrow) u8[size]);
    if (!data) {
        fclose(f);
        return nullptr;
    }
    const size_t read = fread(data.get(), 1, size, f);
    fclose(f);
    if (read != size) {
        return nullptr;
    }

    // GB NSO container: parse just the raw tail slice (the aliasing pointer
    // keeps the whole buffer alive)
    size_t parseSize = size;
    std::shared_ptr<u8[]> parseData = data;
    if (const auto container = ProbeNSOContainer(data.get(), size)) {
        parseData = std::shared_ptr<u8[]>(data, data.get() + container->rawOffset);
        parseSize = container->rawSize;
    }

    // A parse throwing on malformed content means "not a save", not a crash
    try {
        auto sav = ::pksm::Sav::getSave(parseData, parseSize);
        if (sav && !sav->checksumsValid()) {
            // The game rewrites these checksums on every in-game save, so a
            // healthy file never fails this - only torn copies or ROM-hack layouts
            LOG_INFO("Rejecting " + path + ": save checksums do not match its contents");
            return nullptr;
        }
        return sav;
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
