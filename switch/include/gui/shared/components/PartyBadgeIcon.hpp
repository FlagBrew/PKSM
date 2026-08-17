#pragma once

#include <pu/Plutonium>

namespace pksm::ui {

// Shared badge texture for a 1-based party slot number (1-6). The six
// textures are tiny and shared by every caller, loaded once and kept for
// the lifetime of the app. Null for numbers outside the party range.
pu::sdl2::TextureHandle::Ref GetPartyBadgeTexture(u8 partyNumber);

}  // namespace pksm::ui
