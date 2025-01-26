#include <switch.h>
#include <pu/Plutonium>
#include "gui/screen/TitleLoadScreen.hpp"

// Main application class
class PKSMApplication : public pu::ui::Application {
private:
    std::shared_ptr<TitleLoadScreen> saveLoadScreen;

public:
    using Application::Application;  // Inherit constructor

    PU_SMART_CTOR(PKSMApplication)

    void OnLoad() override {
        // Create and add the save load screen
        this->saveLoadScreen = std::make_shared<TitleLoadScreen>();
        this->LoadLayout(this->saveLoadScreen);
    }
};

int main(int argc, char* argv[]) {
    // Create and run the application with proper input configuration
    auto renderer_opts = pu::ui::render::RendererInitOptions(SDL_INIT_EVERYTHING, pu::ui::render::RendererHardwareFlags);
    renderer_opts.init_img = true;
    renderer_opts.sdl_img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
    renderer_opts.init_romfs = true;
    renderer_opts.AddDefaultAllSharedFonts();
    
    // Configure input handling
    renderer_opts.SetInputPlayerCount(1);  // Accept input from one player
    renderer_opts.AddInputNpadStyleTag(HidNpadStyleSet_NpadStandard);  // Accept standard controller input
    renderer_opts.AddInputNpadIdType(HidNpadIdType_Handheld);  // Accept handheld mode input
    renderer_opts.AddInputNpadIdType(HidNpadIdType_No1);  // Accept controller 1 input
    
    auto renderer = pu::ui::render::Renderer::New(renderer_opts);
    auto app = PKSMApplication::New(renderer);
    
    app->Prepare();
    app->ShowWithFadeIn();

    return 0;
} 