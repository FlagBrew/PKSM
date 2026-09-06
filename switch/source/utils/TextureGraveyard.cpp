#include "utils/TextureGraveyard.hpp"

#include <switch.h>

#include "utils/Logger.hpp"

namespace pksm::utils {

size_t TextureGraveyard::drained = 0;
u64 TextureGraveyard::drainTicks = 0;

std::vector<pu::sdl2::TextureHandle::Ref>& TextureGraveyard::Buried() {
    static auto* buried = new std::vector<pu::sdl2::TextureHandle::Ref>();
    return *buried;
}

void TextureGraveyard::Bury(pu::sdl2::TextureHandle::Ref texture) {
    if (texture) {
        Buried().push_back(std::move(texture));
    }
}

void TextureGraveyard::Drain(u64 budgetMs) {
    auto& buried = Buried();
    if (buried.empty()) {
        return;
    }
    const u64 t0 = armGetSystemTick();
    const u64 budgetTicks = armNsToTicks(budgetMs * 1000000);
    while (!buried.empty() && armGetSystemTick() - t0 < budgetTicks) {
        buried.pop_back();
        drained++;
    }
    drainTicks += armGetSystemTick() - t0;
    if (buried.empty()) {
        LOG_DEBUG(
            "Deferred release of " + std::to_string(drained) + " textures done, " +
            std::to_string(armTicksToNs(drainTicks) / 1000000) + " ms of frame time"
        );
        drained = 0;
        drainTicks = 0;
    }
}

}  // namespace pksm::utils
