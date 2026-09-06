#pragma once

#include <pu/Plutonium>
#include <vector>

namespace pksm::utils {

// Textures that are done with but not yet destroyed. Destroying one costs a quarter of a
// millisecond on the console, so a cache that dropped hundreds at once stalled a frame for
// hundreds of milliseconds; buried here they die a few per frame instead, within a budget.
// Render thread only.
class TextureGraveyard {
public:
    // Takes the handle; the texture dies on a later frame, or when its last other owner lets go
    static void Bury(pu::sdl2::TextureHandle::Ref texture);
    // Destroys buried textures for at most budgetMs; call once per frame rendered
    static void Drain(u64 budgetMs);

private:
    // Deliberately leaked, like the sprite sheets: a static destructor would destroy textures
    // after the renderer is gone
    static std::vector<pu::sdl2::TextureHandle::Ref>& Buried();
    static size_t drained;  // since the graveyard was last empty, for the log
    static u64 drainTicks;
};

}  // namespace pksm::utils
