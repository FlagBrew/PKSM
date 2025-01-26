#pragma once

#include <string>
#include <memory>
#include <pu/pu_Include.hpp>
#include <pu/ui/render/render_Renderer.hpp>

namespace titles {

class Title {
public:
    Title(const std::string& name, const std::string& iconPath, u64 titleId)
        : name(name), titleId(titleId) {
        // Load icon texture
        auto image = pu::ui::render::LoadImage(iconPath);
        iconTexture = pu::sdl2::TextureHandle::New(image);
    }

    // Getters
    const std::string& getName() const { return name; }
    u64 getTitleId() const { return titleId; }
    pu::sdl2::TextureHandle::Ref getIcon() const { return iconTexture; }

private:
    std::string name;
    u64 titleId;
    pu::sdl2::TextureHandle::Ref iconTexture;
};

using TitleRef = std::shared_ptr<Title>;

} // namespace titles 