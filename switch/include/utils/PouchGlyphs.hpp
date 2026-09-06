#pragma once

#include <string>

#include "enums/Generation.hpp"
#include "sav/Sav.hpp"
#include "utils/SpriteSheet.hpp"

namespace pksm::utils {

// The bag's pouch glyphs: 64px white masks from pouchglyphs.pkss, one per pouch look
class PouchGlyphs {
public:
    // Load the PKSS glyph sheet; the pouch column draws no glyphs without it
    static bool Initialize(const std::string& sheetPath);

    // A pouch's glyph; storageFormat settles the pouches whose name changes by game
    static SpriteRef Get(::pksm::Sav::Pouch pouch, ::pksm::Generation storageFormat);

    // Drop the warm textures (session teardown)
    static size_t ClearCache();  // returns how many sprites it dropped

private:
    static SpriteSheet& Sheet();
};

}  // namespace pksm::utils
