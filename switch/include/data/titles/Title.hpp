#pragma once

#include <memory>
#include <pu/pu_Include.hpp>
#include <pu/ui/render/render_Renderer.hpp>
#include <string>

namespace pksm::titles {

// Which save universe a title belongs to. The same catalog game can hold a
// tile on both the Emulator tab (discovered saves) and the Custom tab
// (configured saves) with different save lists, so the title id alone can't
// identify a tile's saves - each title carries its context instead.
enum class TitleContext { Console, Emulator, Custom };

class Title {
public:
    Title(
        const std::string& name,
        const std::string& iconPath,
        u64 titleId,
        TitleContext context = TitleContext::Console
    )
      : name(name), titleId(titleId), context(context) {
        // Load icon texture
        auto image = pu::ui::render::LoadImage(iconPath);
        iconTexture = pu::sdl2::TextureHandle::New(image);
    }

    // New constructor that accepts a texture directly
    Title(const std::string& name, SDL_Texture* texture, u64 titleId, TitleContext context = TitleContext::Console)
      : name(name), titleId(titleId), context(context) {
        // Use the provided texture directly
        iconTexture = pu::sdl2::TextureHandle::New(texture);
    }

    // Constructor sharing an already-loaded texture (a game on both the
    // Emulator and Custom tabs keeps one icon in memory, not two)
    Title(
        const std::string& name,
        pu::sdl2::TextureHandle::Ref texture,
        u64 titleId,
        TitleContext context = TitleContext::Console
    )
      : name(name), titleId(titleId), context(context), iconTexture(std::move(texture)) {}

    PU_SMART_CTOR(Title)

    // Getters
    const std::string& getName() const { return name; }
    u64 getTitleId() const { return titleId; }
    TitleContext getContext() const { return context; }
    pu::sdl2::TextureHandle::Ref getIcon() const { return iconTexture; }

private:
    std::string name;
    u64 titleId;
    TitleContext context;
    pu::sdl2::TextureHandle::Ref iconTexture;
};
}  // namespace pksm::titles