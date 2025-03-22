#pragma once

#include <SDL2/SDL.h>
#include <pu/Plutonium>

#include "utils/PokemonSpriteManager.hpp"

namespace pksm::ui {

class SpriteImage : public pu::ui::elm::Element {
private:
    pu::i32 x;
    pu::i32 y;
    pu::i32 width;
    pu::i32 height;
    SDL_Rect clipRect;
    pu::sdl2::TextureHandle::Ref texture;
    bool usingSpritesheet;

public:
    // Constructor for a clipped sprite image
    SpriteImage(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        pu::sdl2::TextureHandle::Ref texture,
        const SDL_Rect& clipRect
    );

    // Constructor for Pokemon sprite from individual files
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

    ~SpriteImage();

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
    void SetWidth(const pu::i32 width);
    void SetHeight(const pu::i32 height);
    void SetClipRect(const SDL_Rect& clipRect);
    void SetImage(pu::sdl2::TextureHandle::Ref texture, const SDL_Rect* clipRect = nullptr);

    // Set Pokemon sprite
    void SetPokemonSprite(u16 species, u8 form = 0, bool shiny = false);

    // Get the texture
    pu::sdl2::TextureHandle::Ref GetTexture();
};

}  // namespace pksm::ui