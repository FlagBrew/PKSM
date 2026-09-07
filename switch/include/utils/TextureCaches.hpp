#pragma once

#include <cstddef>
#include <switch.h>

#include "utils/TextureGraveyard.hpp"

namespace pksm::utils {

// The texture caches a loaded save warms: the three sprite sheets, the shared text, and the
// graveyard they retire into. Render thread only, like the caches themselves.
class TextureCaches {
public:
    // Load the sprite sheets from SD; nothing draws sprites without all three
    static bool Initialize();

    struct Cleared {
        size_t sprites;
        size_t items;
        size_t glyphs;
        size_t texts;
    };
    // Hand every warm texture to the graveyard (title return); how many, per cache
    static Cleared Clear();

    // Destroys retired textures for at most budgetMs; call once per frame rendered
    static void Drain(u64 budgetMs) { TextureGraveyard::Drain(budgetMs); }
};

}  // namespace pksm::utils
