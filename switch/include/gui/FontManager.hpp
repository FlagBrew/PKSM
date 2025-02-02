#pragma once

#include <pu/Plutonium>
#include <vector>
#include <string>
#include <functional>
#include "gui/UIConstants.hpp"

namespace gui {

class FontManager {
public:
    // Function type for font name generators (e.g., MakeHeavyFontName)
    using FontNameGenerator = std::function<std::string(u32)>;

    // Register a font with all custom sizes
    static void RegisterFont(const std::string& fontPath, const FontNameGenerator& nameGenerator) {
        const std::vector<u32> sizes = {
            UIConstants::FONT_SIZE_TITLE,
            UIConstants::FONT_SIZE_HEADER,
            UIConstants::FONT_SIZE_BUTTON
            // New sizes will automatically be included when added to UIConstants
        };

        for (const auto& size : sizes) {
            auto font = std::make_shared<pu::ttf::Font>(size);
            font->LoadFromFile(fontPath);
            pu::ui::render::AddFont(nameGenerator(size), font);
        }
    }

    // Configure renderer options with all custom font sizes
    static void ConfigureRendererFontSizes(pu::ui::render::RendererInitOptions& renderer_opts) {
        // Register all custom font sizes
        renderer_opts.AddExtraDefaultFontSize(UIConstants::FONT_SIZE_TITLE);
        renderer_opts.AddExtraDefaultFontSize(UIConstants::FONT_SIZE_HEADER);
        renderer_opts.AddExtraDefaultFontSize(UIConstants::FONT_SIZE_BUTTON);
        // New sizes will automatically be included when added to UIConstants
    }
};

} // namespace gui 