#pragma once

#include <pu/Plutonium>

#include "enums/Gender.hpp"

namespace pksm::ui {

// Shared gender badge texture, loaded once and kept for the app's lifetime
pu::sdl2::TextureHandle::Ref GetGenderIconTexture(::pksm::Gender gender);

}  // namespace pksm::ui
