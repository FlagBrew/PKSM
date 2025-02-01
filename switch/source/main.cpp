#include <switch.h>
#include <pu/Plutonium>
#include "gui/screen/TitleLoadScreen.hpp"
#include "data/mock/MockTitleDataProvider.hpp"
#include "data/mock/MockSaveDataProvider.hpp"
#include "gui/UIConstants.hpp"

// Main application class
class PKSMApplication : public pu::ui::Application {
private:
    std::shared_ptr<TitleLoadScreen> saveLoadScreen;

    // Initialize renderer options with basic configuration
    static pu::ui::render::RendererInitOptions CreateRendererOptions() {
        // Initialize SDL with hardware acceleration and vsync
        // This enables proper display, audio, and controller support
        auto renderer_opts = pu::ui::render::RendererInitOptions(SDL_INIT_EVERYTHING, pu::ui::render::RendererHardwareFlags);

        // Enable PNG/JPG loading for UI assets
        renderer_opts.init_img = true;
        renderer_opts.sdl_img_flags = IMG_INIT_PNG | IMG_INIT_JPG;

        // Enable romfs for loading assets bundled with the NRO
        renderer_opts.init_romfs = true;

        return renderer_opts;
    }

    // Configure font settings
    static void ConfigureFonts(pu::ui::render::RendererInitOptions& renderer_opts) {
        // Register default (light) font
        renderer_opts.AddDefaultFontPath("romfs:/gfx/ui/dinnextw1g_light.ttf");

        // Register all custom font sizes
        renderer_opts.AddExtraDefaultFontSize(UIConstants::FONT_SIZE_TITLE);
        renderer_opts.AddExtraDefaultFontSize(UIConstants::FONT_SIZE_HEADER);
    }

    // Configure input settings
    static void ConfigureInput(pu::ui::render::RendererInitOptions& renderer_opts) {
        renderer_opts.SetInputPlayerCount(1);  // Accept input from one player
        renderer_opts.AddInputNpadStyleTag(HidNpadStyleSet_NpadStandard);  // Accept standard controller input
        renderer_opts.AddInputNpadIdType(HidNpadIdType_Handheld);  // Accept handheld mode input
        renderer_opts.AddInputNpadIdType(HidNpadIdType_No1);  // Accept controller 1 input
    }

    // Register additional fonts that require romfs to be mounted
    static void RegisterAdditionalFonts() {
        // Register heavy font
        auto heavy_font = std::make_shared<pu::ttf::Font>(UIConstants::FONT_SIZE_TITLE);
        heavy_font->LoadFromFile("romfs:/gfx/ui/dinnextw1g_heavy.ttf");
        pu::ui::render::AddFont(UIConstants::MakeHeavyFontName(UIConstants::FONT_SIZE_TITLE), heavy_font);
    }

public:
    using Application::Application;  // Inherit constructor

    PU_SMART_CTOR(PKSMApplication)

    // Initialize the application with all necessary configuration
    static std::shared_ptr<PKSMApplication> Initialize() {
        // Initialize renderer with all configurations
        auto renderer_opts = CreateRendererOptions();
        ConfigureFonts(renderer_opts);
        ConfigureInput(renderer_opts);
        
        // Create and initialize renderer
        auto renderer = pu::ui::render::Renderer::New(renderer_opts);
        renderer->Initialize();
        
        // Register additional fonts after romfs is mounted
        RegisterAdditionalFonts();
        
        // Create and prepare application
        auto app = PKSMApplication::New(renderer);
        app->Prepare();
        
        return app;
    }

    void OnLoad() override {
        // Create data providers
        auto titleProvider = std::make_shared<MockTitleDataProvider>();
        auto saveProvider = std::make_shared<MockSaveDataProvider>();
        
        // Create and load the title screen
        this->saveLoadScreen = TitleLoadScreen::New(titleProvider, saveProvider);
        this->LoadLayout(this->saveLoadScreen);
    }
};

int main(int argc, char* argv[]) {
    // Initialize and run application
    auto app = PKSMApplication::Initialize();
    app->ShowWithFadeIn();
    return 0;
} 