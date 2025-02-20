#include "PKSMApplication.hpp"

#include <sstream>
#include <switch.h>

#include "gui/shared/FontManager.hpp"
#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"

namespace pksm {

pu::ui::render::RendererInitOptions PKSMApplication::CreateRendererOptions() {
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

    LOG_DEBUG("Renderer options created successfully");
    return renderer_opts;
}

void PKSMApplication::ConfigureFonts(pu::ui::render::RendererInitOptions& renderer_opts) {
    LOG_DEBUG("Configuring fonts...");

    // Register default (light) font
    renderer_opts.AddDefaultFontPath("romfs:/gfx/fonts/dinnextw1g_light.ttf");

    // Register all custom font sizes
    pksm::ui::FontManager::ConfigureRendererFontSizes(renderer_opts);

    LOG_DEBUG("Fonts configured successfully");
}

void PKSMApplication::ConfigureInput(pu::ui::render::RendererInitOptions& renderer_opts) {
    LOG_DEBUG("Configuring input...");

    renderer_opts.SetInputPlayerCount(1);  // Accept input from one player
    renderer_opts.AddInputNpadStyleTag(HidNpadStyleSet_NpadStandard);  // Accept standard controller input
    renderer_opts.AddInputNpadIdType(HidNpadIdType_Handheld);  // Accept handheld mode input
    renderer_opts.AddInputNpadIdType(HidNpadIdType_No1);  // Accept controller 1 input

    LOG_DEBUG("Input configured successfully");
}

void PKSMApplication::RegisterAdditionalFonts() {
    LOG_DEBUG("Registering additional fonts...");

    try {
        // Register heavy font for all custom sizes
        pksm::ui::FontManager::RegisterFont(
            "romfs:/gfx/fonts/dinnextw1g_heavy.ttf",
            pksm::ui::global::MakeHeavyFontName
        );

        // Register medium font for all custom sizes
        pksm::ui::FontManager::RegisterFont(
            "romfs:/gfx/fonts/dinnextw1g_medium.ttf",
            pksm::ui::global::MakeMediumFontName
        );

        // Register switch button font for all custom sizes
        pksm::ui::FontManager::RegisterFont(
            "romfs:/gfx/fonts/NintendoExtLE003-M.ttf",
            pksm::ui::global::MakeSwitchButtonFontName
        );

        LOG_DEBUG("Additional fonts registered successfully");
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to register additional fonts: " + std::string(e.what()));
        throw;
    }
}

PKSMApplication::PKSMApplication(pu::ui::render::Renderer::Ref renderer)
  : pu::ui::Application(renderer), accountManager() {
    // Add render callback to process account updates
    AddRenderCallback([this]() { accountManager.ProcessPendingUpdates(); });
}

PKSMApplication::Ref PKSMApplication::Initialize() {
    try {
        // Initialize logger first
        utils::Logger::Initialize();
        LOG_INFO("Initializing PKSM...");
        LOG_MEMORY();  // Initial memory state

        // Initialize renderer with all configurations
        auto renderer_opts = CreateRendererOptions();
        ConfigureFonts(renderer_opts);
        ConfigureInput(renderer_opts);

        LOG_DEBUG("Creating renderer...");
        auto renderer = pu::ui::render::Renderer::New(renderer_opts);

        LOG_DEBUG("Initializing renderer...");
        renderer->Initialize();
        LOG_MEMORY();  // Memory after renderer initialization

        // Register additional fonts after romfs is mounted
        RegisterAdditionalFonts();

        // Create and prepare application
        LOG_DEBUG("Creating application...");
        auto app = PKSMApplication::New(renderer);

        LOG_DEBUG("Preparing application...");
        app->Prepare();

        LOG_INFO("PKSM initialization complete");
        LOG_MEMORY();  // Final initialization memory state
        return app;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to initialize application: " + std::string(e.what()));
        throw;
    }
}

void PKSMApplication::ShowMainMenu() {
    LOG_DEBUG("Switching to main menu");
    this->LoadLayout(this->mainMenu);
}

void PKSMApplication::ShowTitleLoadScreen() {
    LOG_DEBUG("Switching to title load screen");
    this->LoadLayout(this->titleLoadScreen);
}

void PKSMApplication::OnLoad() {
    try {
        LOG_DEBUG("Loading title screen...");
        LOG_MEMORY();

        // Initialize account manager
        LOG_DEBUG("Initializing account manager...");
        Result res = accountManager.Initialize();
        if (R_FAILED(res)) {
            LOG_ERROR("Failed to initialize account manager");
            throw std::runtime_error("Account manager initialization failed");
        }

        // Create data providers with initial account
        LOG_DEBUG("Creating data providers...");
        titleProvider = std::make_shared<MockTitleDataProvider>(accountManager.GetCurrentAccount());
        saveProvider = std::make_shared<MockSaveDataProvider>(accountManager.GetCurrentAccount());

        // Create main menu with back callback
        LOG_DEBUG("Creating main menu...");
        mainMenu = pksm::layout::MainMenu::New([this]() { this->ShowTitleLoadScreen(); });

        // Create title load screen
        LOG_DEBUG("Creating title load screen...");
        titleLoadScreen = pksm::layout::TitleLoadScreen::New(
            titleProvider,
            saveProvider,
            accountManager,
            [this](pu::ui::Overlay::Ref overlay) { this->StartOverlay(overlay); },
            [this]() { this->EndOverlay(); },
            [this]() { this->ShowMainMenu(); }
        );

        // Start with title load screen
        LOG_DEBUG("Loading initial screen...");
        this->ShowTitleLoadScreen();

        LOG_DEBUG("Application loaded successfully");
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to load application: " + std::string(e.what()));
        throw;
    }
}

}  // namespace pksm