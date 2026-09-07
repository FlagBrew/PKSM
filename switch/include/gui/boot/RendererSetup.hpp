#pragma once

#include <pu/Plutonium>

namespace pksm::boot {

// SDL, the renderer and every face the app draws with, ready for the first frame
pu::ui::render::Renderer::Ref CreateRenderer();

}  // namespace pksm::boot
