#include "gui/boot/RendererSetup.hpp"

#include "gui/shared/FontManager.hpp"
#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"

namespace pksm::boot {

namespace {

pu::ui::render::RendererInitOptions CreateRendererOptions() {
    LOG_DEBUG("Creating renderer options...");
    LOG_MEMORY();

    // Initialize SDL with hardware acceleration and vsync
    // This enables proper display, audio, and controller support
    auto renderer_opts = pu::ui::render::RendererInitOptions(SDL_INIT_EVERYTHING, pu::ui::render::RendererHardwareFlags);

    // Enable PNG/JPG loading for UI assets
    renderer_opts.init_img = true;
    renderer_opts.sdl_img_flags = IMG_INIT_PNG | IMG_INIT_JPG;

    // Enable romfs for loading assets bundled with the NRO
    renderer_opts.init_romfs = true;

    // The renderer's stages are where most of the boot heap goes; each one is measured
    renderer_opts.SetInitStageCallback([](const char* stage) {
        LOG_DEBUG(
            "Renderer init " + std::string(stage) + ": heap claim " + std::to_string(utils::Logger::HeapClaimMB()) +
            " MB"
        );
    });

    LOG_DEBUG("Renderer options created successfully");
    return renderer_opts;
}

void ConfigureFonts(pu::ui::render::RendererInitOptions& renderer_opts) {
    LOG_DEBUG("Configuring fonts...");

    renderer_opts.AddDefaultFontPath("romfs:/gfx/fonts/dinnextw1g_light.ttf");
    // The console's own font fills the glyphs DIN lacks (★, CJK); faces are tried in order
    renderer_opts.AddDefaultSharedFont(PlSharedFontType_Standard);
    renderer_opts.AddDefaultSharedFont(PlSharedFontType_NintendoExt);  // button glyphs in dialog text
    renderer_opts.SetDefaultSharedFontScale(pksm::ui::global::FALLBACK_FONT_SCALE);

    pksm::ui::FontManager::ConfigureRendererFontSizes(renderer_opts);

    LOG_DEBUG("Fonts configured successfully");
}

void ConfigureInput(pu::ui::render::RendererInitOptions& renderer_opts) {
    LOG_DEBUG("Configuring input...");

    renderer_opts.SetInputPlayerCount(1);
    renderer_opts.AddInputNpadStyleTag(HidNpadStyleSet_NpadStandard);
    renderer_opts.AddInputNpadIdType(HidNpadIdType_Handheld);
    renderer_opts.AddInputNpadIdType(HidNpadIdType_No1);

    LOG_DEBUG("Input configured successfully");
}

// Faces beyond the default: from romfs, so only once the renderer has mounted it
void RegisterAdditionalFonts() {
    LOG_DEBUG("Registering additional fonts...");

    try {
        // Register heavy font for all custom sizes
        pksm::ui::FontManager::RegisterFont(
            "romfs:/gfx/fonts/dinnextw1g_heavy.ttf",
            pksm::ui::global::MakeHeavyFontName,
            false
        );

        // Register medium font for all custom sizes
        pksm::ui::FontManager::RegisterFont(
            "romfs:/gfx/fonts/dinnextw1g_medium.ttf",
            pksm::ui::global::MakeMediumFontName,
            true
        );

        // Register switch button font for all custom sizes
        pksm::ui::FontManager::RegisterFont(
            "romfs:/gfx/fonts/NintendoExtLE003-M.ttf",
            pksm::ui::global::MakeSwitchButtonFontName,
            false
        );

        LOG_DEBUG("Additional fonts registered successfully");
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to register additional fonts: " + std::string(e.what()));
        throw;
    }
}

}  // namespace

pu::ui::render::Renderer::Ref CreateRenderer() {
    auto renderer_opts = CreateRendererOptions();
    ConfigureFonts(renderer_opts);
    ConfigureInput(renderer_opts);

    LOG_DEBUG("Creating renderer...");
    auto renderer = pu::ui::render::Renderer::New(renderer_opts);

    LOG_DEBUG("Initializing renderer...");
    renderer->Initialize();
    LOG_MEMORY();  // Memory after renderer initialization

    RegisterAdditionalFonts();
    return renderer;
}

}  // namespace pksm::boot
