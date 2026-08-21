#include "gui/shared/components/PartyBadgeIcon.hpp"

#include <string>

namespace pksm::ui {

pu::sdl2::TextureHandle::Ref GetPartyBadgeTexture(u8 partyNumber) {
    static pu::sdl2::TextureHandle::Ref badges[6];

    if (partyNumber < 1 || partyNumber > 6) {
        return nullptr;
    }
    auto& slot = badges[partyNumber - 1];
    if (!slot) {
        pu::sdl2::Texture tex = pu::ui::render::LoadImage(
            "romfs:/gfx/ui/icon_party_" + std::to_string(partyNumber) + ".png"
        );
        if (tex) {
            slot = pu::sdl2::TextureHandle::New(tex);
        }
    }
    return slot;
}

}  // namespace pksm::ui
