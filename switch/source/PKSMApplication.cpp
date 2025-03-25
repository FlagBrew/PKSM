#include "PKSMApplication.hpp"

#include <sstream>

#include "data/providers/mock/MockBoxDataProvider.hpp"
#include "data/providers/mock/MockSaveDataAccessor.hpp"
#include "data/providers/mock/MockSaveDataProvider.hpp"
#include "data/providers/mock/MockTitleDataProvider.hpp"
#include "gui/shared/FontManager.hpp"
#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"
#include "utils/PokemonSpriteManager.hpp"

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

PKSMApplication::PKSMApplication(
    pu::ui::render::Renderer::Ref renderer,
    std::unique_ptr<data::AccountManager> accountManager,
    ITitleDataProvider::Ref titleProvider,
    ISaveDataProvider::Ref saveProvider,
    ISaveDataAccessor::Ref saveDataAccessor,
    IBoxDataProvider::Ref boxDataProvider
)
  : pu::ui::Application(renderer),
    accountManager(std::move(accountManager)),
    titleProvider(std::move(titleProvider)),
    saveProvider(std::move(saveProvider)),
    saveDataAccessor(std::move(saveDataAccessor)),
    boxDataProvider(std::move(boxDataProvider)) {
    // Add render callback to process account updates
    AddRenderCallback([this]() { this->accountManager->ProcessPendingUpdates(); });
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

        if (!utils::PokemonSpriteManager::Initialize("romfs:/gfx/pokesprites/pokesprite.json")) {
            LOG_ERROR("Failed to initialize Pokemon sprite manager");
            return nullptr;
        }

        auto recordingInitResult = appletInitializeGamePlayRecording();
        if (R_FAILED(recordingInitResult)) {
            LOG_ERROR("Failed to initialize game play recording");
        } else {
            appletSetGamePlayRecordingState(true);
        }

        // Initialize account manager and data providers
        LOG_DEBUG("Initializing account manager and data providers...");
        auto accountManager = std::make_unique<data::AccountManager>();
        Result res = accountManager->Initialize();
        if (R_FAILED(res)) {
            LOG_ERROR("Failed to initialize account manager");
            throw std::runtime_error("Account manager initialization failed");
        }

        LOG_DEBUG("Creating data providers...");
        auto titleProvider = std::make_shared<MockTitleDataProvider>(accountManager->GetCurrentAccount());
        auto saveProvider = std::make_shared<MockSaveDataProvider>(accountManager->GetCurrentAccount());
        auto saveDataAccessor = std::make_shared<MockSaveDataAccessor>();
        auto boxDataProvider = std::make_shared<MockBoxDataProvider>();
        LOG_MEMORY();  // Memory after data provider initialization

        // Create and prepare application
        LOG_DEBUG("Creating application...");
        auto app = PKSMApplication::New(
            renderer,
            std::move(accountManager),
            titleProvider,
            saveProvider,
            saveDataAccessor,
            boxDataProvider
        );

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

void PKSMApplication::ShowStorageScreen() {
    LOG_DEBUG("Switching to storage screen");
    this->LoadLayout(this->storageScreen);
}

void PKSMApplication::OnSaveSelected(pksm::titles::Title::Ref title, pksm::saves::Save::Ref save) {
    LOG_DEBUG("Save selected: " + save->getName() + " for title: " + title->getName());
    // Load the mock save data based on the title and save name
    if (saveDataAccessor->loadSave(title, save->getName())) {
        LOG_DEBUG("Successfully loaded mock save data");

        // Now that the save is loaded, show the main menu
        this->ShowMainMenu();
    } else {
        LOG_ERROR("Failed to load mock save data");
    }
}

void PKSMApplication::OnLoad() {
    try {
        LOG_DEBUG("Loading title screen...");
        LOG_MEMORY();

        // Create title load screen
        LOG_DEBUG("Creating title load screen...");
        titleLoadScreen = pksm::layout::TitleLoadScreen::New(
            titleProvider,
            saveProvider,
            *accountManager,
            [this](pu::ui::Overlay::Ref overlay) { this->StartOverlay(overlay); },
            [this]() { this->EndOverlay(); },
            [this](pksm::titles::Title::Ref title, pksm::saves::Save::Ref save) { this->OnSaveSelected(title, save); }
        );

        // Create main menu with back callback and overlay handlers
        // Create navigation callbacks for menu buttons
        LOG_DEBUG("Creating navigation callbacks...");
        std::map<pksm::ui::MenuButtonType, std::function<void()>> navigationCallbacks = {
            {pksm::ui::MenuButtonType::Storage, [this]() { this->ShowStorageScreen(); }},
            {pksm::ui::MenuButtonType::Editor, [this]() { LOG_DEBUG("Editor button pressed (not implemented)"); }},
            {pksm::ui::MenuButtonType::Events, [this]() { LOG_DEBUG("Events button pressed (not implemented)"); }},
            {pksm::ui::MenuButtonType::Bag, [this]() { LOG_DEBUG("Bag button pressed (not implemented)"); }},
            {pksm::ui::MenuButtonType::Scripts, [this]() { LOG_DEBUG("Scripts button pressed (not implemented)"); }},
            {pksm::ui::MenuButtonType::Settings, [this]() { LOG_DEBUG("Settings button pressed (not implemented)"); }}
        };

        LOG_DEBUG("Creating main menu...");
        mainMenu = pksm::layout::MainMenu::New(
            [this]() { this->ShowTitleLoadScreen(); },
            [this](pu::ui::Overlay::Ref overlay) { this->StartOverlay(overlay); },
            [this]() { this->EndOverlay(); },
            saveDataAccessor,  // Pass the save data accessor to the main menu
            navigationCallbacks  // Pass navigation callbacks to the main menu
        );

        // Create storage screen
        LOG_DEBUG("Creating storage screen...");
        storageScreen = pksm::layout::StorageScreen::New(
            [this]() { this->ShowMainMenu(); },  // Back handler goes to main menu
            [this](pu::ui::Overlay::Ref overlay) { this->StartOverlay(overlay); },
            [this]() { this->EndOverlay(); },
            saveDataAccessor,  // Pass the save data accessor
            boxDataProvider  // Pass the box data provider
        );

        // Register for save data changes in both MainMenu and StorageScreen
        LOG_DEBUG("Setting up save data change callbacks...");
        saveDataAccessor->setOnSaveDataChanged([this](pksm::saves::SaveData::Ref saveData) {
            LOG_DEBUG("Save data changed, updating UI");

            // Update main menu with new save data
            if (mainMenu) {
                LOG_DEBUG("Updating MainMenu with new save data");
                mainMenu->UpdateTrainerInfo();
            }

            // Preload box data for storage screen
            if (storageScreen && saveData) {
                LOG_DEBUG("Preloading box data for StorageScreen");
                storageScreen->LoadBoxData();
            }
        });

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