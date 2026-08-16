#pragma once

#include <optional>
#include <string>
#include <switch/types.h>

namespace pksm::saves {

// Validates a save file by letting PKSM-Core parse it. A file only counts as
// a save if core recognizes it (by size/content) and can read trainer data
// from it; anything else is rejected before it ever reaches the UI.
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

    // Returns trainer info when core can parse the file, nullopt otherwise.
    static std::optional<Summary> Validate(const std::string& path);

private:
    // Largest known main-series save (SV DLC ~4.4MB) with headroom; larger
    // files can't be saves and are rejected without reading them
    static constexpr size_t MAX_SAVE_SIZE = 0x500000;
};

}  // namespace pksm::saves
