#pragma once

#include <memory>
#include <optional>
#include <string>
#include <switch/types.h>

#include "sav/Sav.hpp"

namespace pksm::saves {

// Fail-closed: a file only counts as a save if core can parse trainer data from it
class SaveValidator {
public:
    struct Summary {
        std::string generation;  // "1".."9" / "LGPE"
        std::string otName;
        u16 tid = 0;
        u16 sid = 0;
        u8 badges = 0;
        u16 playedHours = 0;

        std::string Describe() const;
    };

    // The returned Sav owns a full in-memory copy, so the source may be unmounted afterwards
    static std::unique_ptr<::pksm::Sav> Load(const std::string& path);

    static Summary Summarize(const ::pksm::Sav& sav);

    static std::optional<Summary> Validate(const std::string& path);

    // Largest known main-series save (SV DLC ~4.4MB) with headroom
    static constexpr size_t MAX_SAVE_SIZE = 0x500000;
};

}  // namespace pksm::saves
