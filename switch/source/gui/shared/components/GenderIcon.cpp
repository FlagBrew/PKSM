#include "gui/shared/components/GenderIcon.hpp"

namespace {

constexpr const char* ICON_GENDER_MALE = "romfs:/gfx/ui/icon_gender_male.png";
constexpr const char* ICON_GENDER_FEMALE = "romfs:/gfx/ui/icon_gender_female.png";
constexpr const char* ICON_GENDER_NEUTRAL = "romfs:/gfx/ui/icon_gender_neutral.png";

}  // namespace

namespace pksm::ui {

pu::sdl2::TextureHandle::Ref GetGenderIconTexture(::pksm::Gender gender) {
    static pu::sdl2::TextureHandle::Ref male;
    static pu::sdl2::TextureHandle::Ref female;
    static pu::sdl2::TextureHandle::Ref neutral;

    const auto loadOnce = [](pu::sdl2::TextureHandle::Ref& slot, const char* path) {
        if (slot) {
            return;
        }
        pu::sdl2::Texture tex = pu::ui::render::LoadImage(path);
        if (tex) {
            slot = pu::sdl2::TextureHandle::New(tex);
        }
    };

    loadOnce(male, ICON_GENDER_MALE);
    loadOnce(female, ICON_GENDER_FEMALE);
    loadOnce(neutral, ICON_GENDER_NEUTRAL);

    if (gender == ::pksm::Gender{::pksm::Gender::Male}) {
        return male;
    }
    if (gender == ::pksm::Gender{::pksm::Gender::Female}) {
        return female;
    }
    return neutral;
}

}  // namespace pksm::ui
