#pragma once

#include <memory>
#include <pu/Plutonium>

#include "data/AccountManager.hpp"
#include "data/providers/mock/MockSaveDataProvider.hpp"
#include "data/providers/mock/MockTitleDataProvider.hpp"
#include "gui/screens/main-menu/MainMenu.hpp"
#include "gui/screens/title-load-screen/TitleLoadScreen.hpp"

namespace pksm {

class PKSMApplication : public pu::ui::Application {
private:
    // Screens
    pksm::layout::MainMenu::Ref mainMenu;
    pksm::layout::TitleLoadScreen::Ref titleLoadScreen;

    // Data providers and managers
    pksm::data::AccountManager accountManager;
    ITitleDataProvider::Ref titleProvider;
    ISaveDataProvider::Ref saveProvider;

    // Initialize renderer options with basic configuration
    static pu::ui::render::RendererInitOptions CreateRendererOptions();

    // Configure font settings
    static void ConfigureFonts(pu::ui::render::RendererInitOptions& renderer_opts);

    // Configure input settings
    static void ConfigureInput(pu::ui::render::RendererInitOptions& renderer_opts);

    // Register additional fonts that require romfs to be mounted
    static void RegisterAdditionalFonts();

    // Navigation methods
    void ShowMainMenu();
    void ShowTitleLoadScreen();

public:
    PKSMApplication(pu::ui::render::Renderer::Ref renderer);
    PU_SMART_CTOR(PKSMApplication)

    // Initialize the application with all necessary configuration
    static PKSMApplication::Ref Initialize();

    void OnLoad() override;
};

}  // namespace pksm