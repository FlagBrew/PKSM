#include "PKSMApplication.hpp"

#include <sstream>

#include "data/providers/SaveDataAccessor.hpp"
#include "data/providers/SwitchSaveDataProvider.hpp"
#include "data/providers/SwitchTitleDataProvider.hpp"
#include "data/providers/BoxDataProvider.hpp"
#include "data/providers/SwitchSaveDataWriter.hpp"
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
    IBoxDataProvider::Ref boxDataProvider,
    IStorageHand::Ref storageHand,
    IBoxNameEditor::Ref boxNameEditor
)
  : pu::ui::Application(renderer),
    accountManager(std::move(accountManager)),
    titleProvider(std::move(titleProvider)),
    saveProvider(std::move(saveProvider)),
    saveDataAccessor(std::move(saveDataAccessor)),
    boxDataProvider(std::move(boxDataProvider)),
    storageHand(std::move(storageHand)),
    boxNameEditor(std::move(boxNameEditor)) {
    // Add render callback to process account updates
    AddRenderCallback([this]() { this->accountManager->ProcessPendingUpdates(); });
    AddRenderCallback([this]() { this->ProcessPendingSaveAndExit(); });
}

PKSMApplication::Ref PKSMApplication::Initialize() {
    try {
        // Initialize logger first
        utils::Logger::Initialize();
        utils::Logger::LogOutputMode();
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
            // Not fatal: the app still works for emulator/backup saves;
            // console-save listing just comes up empty without an account
            std::stringstream ss;
            ss << "Failed to initialize account manager: 0x" << std::hex << res;
            LOG_ERROR(ss.str());
        }

        LOG_DEBUG("Creating data providers...");
        auto saveProvider = SwitchSaveDataProvider::New();
        auto saveWriter = SwitchSaveDataWriter::New();
        auto titleProvider = SwitchTitleDataProvider::New(saveProvider);
        auto saveDataAccessor = SaveDataAccessor::New(saveProvider, saveWriter);
        auto boxDataProvider = BoxDataProvider::New(saveDataAccessor);
        LOG_MEMORY();  // Memory after data provider initialization

        // Create and prepare application
        LOG_DEBUG("Creating application...");
        // The box provider doubles as the storage hand and box-name editor:
        // one object owns the read model and every edit over the same Sav
        auto app = PKSMApplication::New(
            renderer,
            std::move(accountManager),
            titleProvider,
            saveProvider,
            saveDataAccessor,
            boxDataProvider,
            boxDataProvider,
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
    // Leaving a save session: release the loaded Sav (a console SV save holds
    // ~4.4MB) and drop the storage screen and sprite cache so their textures
    // go back to the applet heap. Everything rebuilds on demand.
    saveDataAccessor->unloadSave();
    if (storageScreen) {
        storageScreen = nullptr;
        utils::PokemonSpriteManager::ClearCache();
    }
    LOG_MEMORY();
    LOG_DEBUG("Switching to title load screen");
    this->LoadLayout(this->titleLoadScreen);
}

void PKSMApplication::HandleMainMenuBack() {
    if (saveDataAccessor->hasUnsavedChanges()) {
        const int choice = this->CreateShowDialog(
            "Unsaved Changes",
            "Save the changes to this game's save file?",
            {"Save", "Discard", "Cancel"},
            true
        );
        if (choice == 0) {
            auto savingText =
                pu::ui::elm::TextBlock::New(0, 0, "Saving... Do not close the app or power off.");
            savingText->SetFont(pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_HEADER));
            savingText->SetColor(pksm::ui::global::TEXT_WHITE);
            constexpr pu::i32 SAVING_OVERLAY_PADDING = 60;
            const pu::i32 overlayWidth = savingText->GetWidth() + 2 * SAVING_OVERLAY_PADDING;
            const pu::i32 overlayHeight = savingText->GetHeight() + 2 * SAVING_OVERLAY_PADDING;
            auto savingOverlay = pu::ui::Overlay::New(
                (static_cast<pu::i32>(pu::ui::render::ScreenWidth) - overlayWidth) / 2,
                (static_cast<pu::i32>(pu::ui::render::ScreenHeight) - overlayHeight) / 2,
                overlayWidth,
                overlayHeight,
                pu::ui::Color(30, 30, 30, 255)
            );
            savingOverlay->SetFadeAlphaVariation(pu::ui::Overlay::DefaultMaxFadeAlpha);
            savingText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
            savingText->SetVerticalAlign(pu::ui::elm::VerticalAlign::Center);
            savingOverlay->Add(savingText);
            this->StartOverlay(savingOverlay);
            // Write on a worker thread so the render loop keeps animating.
            // Block input meanwhile via Plutonium's render-over flag - OnRender
            // consumes it at the end of every frame, so ProcessPendingSaveAndExit
            // re-arms it each frame until the write completes.
            this->in_render_over = true;
            this->render_over_fn = [](pu::ui::render::Renderer::Ref&) { return true; };
            LOG_DEBUG("Starting save write...");
            LOG_MEMORY();
            saveWriteResult = std::async(std::launch::async, [this]() { return saveDataAccessor->saveChanges(); });
            return;
        }
        if (choice != 1) {
            return;
        }
    }
    this->ShowTitleLoadScreen();
}

void PKSMApplication::ProcessPendingSaveAndExit() {
    if (!saveWriteResult.valid()) {
        return;
    }
    if (saveWriteResult.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
        // Still writing: keep input blocked (OnRender consumed the flag at the
        // end of the previous frame)
        this->in_render_over = true;
        this->render_over_fn = [](pu::ui::render::Renderer::Ref&) { return true; };
        return;
    }

    // The worker can die on allocation failure in a fragmented heap; that is
    // a failed save with changes still loaded, not a reason to bring the
    // whole app down past the failure dialog below
    bool saved = false;
    try {
        saved = saveWriteResult.get();
    } catch (const std::exception& e) {
        LOG_ERROR("Save write threw: " + std::string(e.what()));
    }
    LOG_DEBUG(saved ? "Save write completed" : "Save write failed");
    LOG_MEMORY();
    // Persist the tail now: a follow-up crash or the applet being torn down
    // would lose the 3s flush window
    utils::Logger::Flush();
    this->EndOverlay();
    if (!saved) {
        this->CreateShowDialog(
            "Save Failed",
            "The save file could not be written. Your changes are still loaded.",
            {"OK"},
            true
        );
        return;
    }
    this->ShowTitleLoadScreen();
}

void PKSMApplication::ShowStorageScreen() {
    // Constructed on first entry, not at boot: the screen's textures cost
    // ~25MB of the applet heap, paid only when storage is actually used
    if (!storageScreen) {
        LOG_DEBUG("Creating storage screen on first use...");
        storageScreen = pksm::layout::StorageScreen::New(
            [this]() { this->ShowMainMenu(); },
            [this](pu::ui::Overlay::Ref overlay) { this->StartOverlay(overlay); },
            [this]() { this->EndOverlay(); },
            [this](const std::string& title, const std::string& message, const std::string& confirmLabel) {
                return this->CreateShowDialog(title, message, {confirmLabel, "Cancel"}, true) == 0;
            },
            saveDataAccessor,
            boxDataProvider,
            storageHand,
            boxNameEditor
        );
        storageScreen->LoadBoxData();
        LOG_MEMORY();
    }
    LOG_DEBUG("Switching to storage screen");
    this->LoadLayout(this->storageScreen);
}

void PKSMApplication::OnSaveSelected(pksm::titles::Title::Ref title, pksm::saves::Save::Ref save) {
    LOG_DEBUG("Save selected: " + save->getName() + " for title: " + title->getName());

    // The main menu is only entered with a loaded save; a failed load stays
    // on the title screen, whose next listing drops the bad candidate
    auto userId = accountManager->GetCurrentAccount();
    if (saveDataAccessor->loadSave(title, save->getName(), &userId)) {
        LOG_MEMORY();
        this->ShowMainMenu();
    } else {
        LOG_ERROR("Failed to load save data");
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
            [this]() { this->HandleMainMenuBack(); },
            [this](pu::ui::Overlay::Ref overlay) { this->StartOverlay(overlay); },
            [this]() { this->EndOverlay(); },
            saveDataAccessor,  // Pass the save data accessor to the main menu
            navigationCallbacks  // Pass navigation callbacks to the main menu
        );

        // The storage screen is built lazily in ShowStorageScreen - its
        // textures are too heavy for the applet heap to pay at boot

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