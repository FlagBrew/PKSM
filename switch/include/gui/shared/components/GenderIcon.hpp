#pragma once

#include <pu/Plutonium>

#include "enums/Gender.hpp"

namespace pksm::ui {

// Shared badge texture for a gender (male/female/neutral). The three
// textures are tiny and shared by every caller, loaded once and kept for
// the lifetime of the app.
pu::sdl2::TextureHandle::Ref GetGenderIconTexture(::pksm::Gender gender);

}  // namespace pksm::ui
