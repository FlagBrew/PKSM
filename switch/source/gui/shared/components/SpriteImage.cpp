#include "gui/shared/components/SpriteImage.hpp"

#include "utils/PokemonSpriteManager.hpp"

namespace pksm::ui {

SpriteImage::SpriteImage(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    const utils::SpriteRef& sprite
)
  : Element(), x(x), y(y), width(width), height(height), clipRect(sprite.src), texture(sprite.texture) {}

SpriteImage::SpriteImage(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    u16 species,
    u8 form,
    bool shiny
)
  : Element(), x(x), y(y), width(width), height(height), clipRect{0, 0, 0, 0}, texture(nullptr) {
    SetPokemonSprite(species, form, shiny);
}

pu::i32 SpriteImage::GetX() {
    return this->x;
}

pu::i32 SpriteImage::GetY() {
    return this->y;
}

pu::i32 SpriteImage::GetWidth() {
    return this->width;
}

pu::i32 SpriteImage::GetHeight() {
    return this->height;
}

void SpriteImage::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    if (texture) {
        SDL_Rect destRect = {x, y, width, height};
        SDL_RenderCopy(pu::ui::render::GetMainRenderer(), texture->Get(), &clipRect, &destRect);
    }
}

void SpriteImage::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    // No input handling needed
}

void SpriteImage::SetX(const pu::i32 x) {
    this->x = x;
}

void SpriteImage::SetY(const pu::i32 y) {
    this->y = y;
}

void SpriteImage::SetImage(const utils::SpriteRef& sprite) {
    this->texture = sprite.texture;
    this->clipRect = sprite.src;
}

void SpriteImage::SetPokemonSprite(u16 species, u8 form, bool shiny) {
    SetImage(species == 0 ? utils::SpriteRef{} : utils::PokemonSpriteManager::GetPokemonSprite(species, form, shiny));
}

}  // namespace pksm::ui
