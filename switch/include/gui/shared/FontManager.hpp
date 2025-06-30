#pragma once

#include <functional>
#include <pu/Plutonium>
#include <string>
#include <vector>

#include "gui/shared/UIConstants.hpp"

namespace pksm::ui {

class FontManager {
public:
    // Function type for font name generators (e.g., MakeHeavyFontName)
    using FontNameGenerator = std::function<std::string(u32)>;

    // Register a font with all custom sizes
    static void RegisterFont(const std::string& fontPath, const FontNameGenerator& nameGenerator) {
        const std::vector<u32> sizes = {
            pksm::ui::global::FONT_SIZE_TITLE,
            pksm::ui::global::FONT_SIZE_HEADER,
            pksm::ui::global::FONT_SIZE_BUTTON,
            pksm::ui::global::FONT_SIZE_BOX_BUTTON,
            pksm::ui::global::FONT_SIZE_TRIGGER_BUTTON_NAVIGATION,
            pksm::ui::global::FONT_SIZE_BOX_SPACES_BUTTON,
            pksm::ui::global::FONT_SIZE_ACCOUNT_NAME,
            pksm::ui::global::FONT_SIZE_TRAINER_INFO_STATS
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
        renderer_opts.AddExtraDefaultFontSize(pksm::ui::global::FONT_SIZE_TITLE);
        renderer_opts.AddExtraDefaultFontSize(pksm::ui::global::FONT_SIZE_HEADER);
        renderer_opts.AddExtraDefaultFontSize(pksm::ui::global::FONT_SIZE_BUTTON);
        renderer_opts.AddExtraDefaultFontSize(pksm::ui::global::FONT_SIZE_BOX_BUTTON);
        renderer_opts.AddExtraDefaultFontSize(pksm::ui::global::FONT_SIZE_TRIGGER_BUTTON_NAVIGATION);
        renderer_opts.AddExtraDefaultFontSize(pksm::ui::global::FONT_SIZE_BOX_SPACES_BUTTON);
        renderer_opts.AddExtraDefaultFontSize(pksm::ui::global::FONT_SIZE_ACCOUNT_NAME);
        renderer_opts.AddExtraDefaultFontSize(pksm::ui::global::FONT_SIZE_TRAINER_INFO_STATS);
    }
};

}  // namespace pksm::ui