#pragma once

#include <pu/Plutonium>

namespace pksm::ui {

// Shared party-number badge texture (1-6, null outside that range),
// loaded once and kept for the app's lifetime
pu::sdl2::TextureHandle::Ref GetPartyBadgeTexture(u8 partyNumber);

}  // namespace pksm::ui
