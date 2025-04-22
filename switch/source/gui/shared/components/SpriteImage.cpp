#include "gui/shared/components/SpriteImage.hpp"

#include "utils/Logger.hpp"

namespace pksm::ui {

SpriteImage::SpriteImage(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    pu::sdl2::TextureHandle::Ref texture,
    const SDL_Rect& clipRect
)
  : Element(), x(x), y(y), width(width), height(height), clipRect(clipRect), texture(texture), usingSpritesheet(false) {
    LOG_DEBUG("Created SpriteImage with clipping rect");
}

SpriteImage::SpriteImage(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    u16 species,
    u8 form,
    bool shiny
)
  : Element(),
    x(x),
    y(y),
    width(width),
    height(height),
    clipRect{0, 0, 0, 0},
    texture(nullptr),
    usingSpritesheet(false) {
    // Set the Pokemon sprite
    SetPokemonSprite(species, form, shiny);
    LOG_DEBUG("Created SpriteImage for Pokemon species " + std::to_string(species));
}

SpriteImage::~SpriteImage() {
    // The texture handle will be destroyed automatically
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
        // Destination rectangle
        SDL_Rect destRect = {x, y, width, height};

        // Render the texture
        SDL_RenderCopy(
            pu::ui::render::GetMainRenderer(),
            texture->Get(),
            nullptr,  // Use the entire texture
            &destRect  // Destination rectangle
        );
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

void SpriteImage::SetWidth(const pu::i32 width) {
    this->width = width;
}

void SpriteImage::SetHeight(const pu::i32 height) {
    this->height = height;
}

void SpriteImage::SetClipRect(const SDL_Rect& clipRect) {
    this->clipRect = clipRect;
}

void SpriteImage::SetImage(pu::sdl2::TextureHandle::Ref texture, const SDL_Rect* clipRect) {
    this->texture = texture;

    if (clipRect) {
        this->clipRect = *clipRect;
    } else if (texture) {
        // Use the entire texture if no clip rect provided
        this->clipRect =
            {0, 0, pu::ui::render::GetTextureWidth(texture->Get()), pu::ui::render::GetTextureHeight(texture->Get())};
    } else {
        this->clipRect = {0, 0, 0, 0};
    }

    this->usingSpritesheet = false;
}

void SpriteImage::SetPokemonSprite(u16 species, u8 form, bool shiny) {
    if (species == 0) {
        // Empty slot
        this->texture = nullptr;
        this->clipRect = {0, 0, 0, 0};
        return;
    }

    // Get the sprite directly from PokemonSpriteManager
    this->texture = utils::PokemonSpriteManager::GetPokemonSprite(species, form, shiny);

    if (this->texture) {
        // Set clip rect to the full texture size
        this->clipRect = {
            0,
            0,
            pu::ui::render::GetTextureWidth(this->texture->Get()),
            pu::ui::render::GetTextureHeight(this->texture->Get())
        };
    } else {
        LOG_ERROR("Failed to find sprite for species " + std::to_string(species));
        this->clipRect = {0, 0, 0, 0};
    }
}

pu::sdl2::TextureHandle::Ref SpriteImage::GetTexture() {
    return this->texture;
}

}  // namespace pksm::ui