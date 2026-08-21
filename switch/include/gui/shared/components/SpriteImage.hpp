#pragma once

#include <SDL2/SDL.h>
#include <pu/Plutonium>

#include "utils/SpriteSheet.hpp"

namespace pksm::ui {

// Draws a region of a shared spritesheet page; the sprite equivalent of
// Plutonium's Image
class SpriteImage : public pu::ui::elm::Element {
private:
    pu::i32 x;
    pu::i32 y;
    pu::i32 width;
    pu::i32 height;
    SDL_Rect clipRect;
    pu::sdl2::TextureHandle::Ref texture;

public:
    SpriteImage(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        const utils::SpriteRef& sprite
    );

    // Constructor resolving a Pokemon's sprite from the sheet
    SpriteImage(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        u16 species,
        u8 form = 0,
        bool shiny = false
    );

    PU_SMART_CTOR(SpriteImage)

    // Element implementation
    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void
    OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;

    // Getters/setters
    void SetX(const pu::i32 x);
    void SetY(const pu::i32 y);
    void SetImage(const utils::SpriteRef& sprite);

    // Set Pokemon sprite
    void SetPokemonSprite(u16 species, u8 form = 0, bool shiny = false);
};

}  // namespace pksm::ui
