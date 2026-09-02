#include "PKSMApplication.hpp"

#include <sstream>

#include "data/providers/SaveDataAccessor.hpp"
#include "data/providers/SwitchSaveDataProvider.hpp"
#include "data/providers/SwitchTitleDataProvider.hpp"
#include "data/providers/BoxDataProvider.hpp"
#include "data/providers/SwitchSaveDataWriter.hpp"
#include "gui/boot/BootFlow.hpp"
#include "gui/shared/FontManager.hpp"
#include "gui/shared/UIConstants.hpp"
#include "utils/AssetDownloader.hpp"
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

    renderer_opts.AddDefaultFontPath("romfs:/gfx/fonts/dinnextw1g_light.ttf");

    pksm::ui::FontManager::ConfigureRendererFontSizes(renderer_opts);

    LOG_DEBUG("Fonts configured successfully");
}

void PKSMApplication::ConfigureInput(pu::ui::render::RendererInitOptions& renderer_opts) {
    LOG_DEBUG("Configuring input...");

    renderer_opts.SetInputPlayerCount(1);
    renderer_opts.AddInputNpadStyleTag(HidNpadStyleSet_NpadStandard);
    renderer_opts.AddInputNpadIdType(HidNpadIdType_Handheld);
    renderer_opts.AddInputNpadIdType(HidNpadIdType_No1);

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
    AddRenderCallback([this]() { this->ProcessPendingSaveLoad(); });
    // The error toast also ends on any button press; a no-op once the 3s timeout ended it
    SetOnInput([this](const u64 down, const u64, const u64, const pu::ui::TouchPoint) {
        if (down != 0 && errorToastActive) {
            this->EndOverlay();
            errorToastActive = false;
        }
    });
}

PKSMApplication::Ref PKSMApplication::Initialize() {
    pu::ui::render::Renderer::Ref renderer;
    try {
        // Initialize logger first
        utils::Logger::Initialize();
        utils::Logger::LogOutputMode();
        utils::Logger::LogEnvironment();
        LOG_INFO("Initializing PKSM...");
        LOG_MEMORY();  // Initial memory state

        // The launch-to-first-interactive-frame gap lives in these steps; keep them measured
        u64 bootPhaseStart = armGetSystemTick();
        auto logBootPhase = [&bootPhaseStart](const char* phase) {
            const u64 now = armGetSystemTick();
            LOG_DEBUG(
                "Boot phase " + std::string(phase) + ": " +
                std::to_string(armTicksToNs(now - bootPhaseStart) / 1000000) + " ms"
            );
            bootPhaseStart = now;
        };

        // Initialize renderer with all configurations
        auto renderer_opts = CreateRendererOptions();
        ConfigureFonts(renderer_opts);
        ConfigureInput(renderer_opts);

        LOG_DEBUG("Creating renderer...");
        renderer = pu::ui::render::Renderer::New(renderer_opts);

        LOG_DEBUG("Initializing renderer...");
        renderer->Initialize();
        LOG_MEMORY();  // Memory after renderer initialization

        // Register additional fonts after romfs is mounted
        RegisterAdditionalFonts();
        logBootPhase("renderer + fonts");

        boot::BootProgress bootProgress(renderer);
        if (!boot::RunAssetBootstrap(bootProgress)) {
            // No Application exists yet to run renderer teardown; skipping
            // Finalize hands hbmenu a live SDL/romfs stack, which crashes it
            bootProgress.Release();
            renderer->Finalize();
            return nullptr;
        }

        logBootPhase("asset bootstrap");

        if (!utils::PokemonSpriteManager::Initialize(
                utils::AssetDownloader::ResolvedPath(utils::AssetDownloader::Asset::PokemonSprites)
            )) {
            LOG_ERROR("Failed to initialize Pokemon sprite manager");
            bootProgress.Release();
            renderer->Finalize();
            return nullptr;
        }
        logBootPhase("sprite sheet");

        auto recordingInitResult = appletInitializeGamePlayRecording();
        if (R_FAILED(recordingInitResult)) {
            LOG_ERROR("Failed to initialize game play recording");
        } else {
            appletSetGamePlayRecordingState(true);
        }

        // Initialize account manager and data providers
        bootProgress.ShowPhase("Scanning saves");
        LOG_DEBUG("Initializing account manager and data providers...");
        auto accountManager = std::make_unique<data::AccountManager>();
        Result res = accountManager->Initialize();
        if (R_FAILED(res)) {
            // Not fatal: emulator/backup saves still work; console listing just comes up empty
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
        logBootPhase("data providers");
        LOG_MEMORY();  // Memory after data provider initialization

        // Create and prepare application
        bootProgress.ShowPhase("Preparing screens");
        LOG_DEBUG("Creating application...");
        // The box provider doubles as storage hand and box-name editor: one object owns every edit
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
        logBootPhase("screens");

        // Warm the save-validation cache so first landing on a title doesn't pay it on input
        saveProvider->PrewarmValidationCache();

        LOG_INFO("PKSM initialization complete");
        LOG_MEMORY();  // Final initialization memory state
        // Boot milestones flush synchronously: a crash before the timed flush would lose them
        utils::Logger::Flush();
        return app;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to initialize application: " + std::string(e.what()));
        // The renderer outlives the try and still owes hbmenu its teardown
        if (renderer) {
            renderer->Finalize();
        }
        throw;
    }
}

void PKSMApplication::ShowMainMenu() {
    LOG_DEBUG("Switching to main menu");
    this->LoadLayout(this->mainMenu);
}

void PKSMApplication::ShowTitleLoadScreen() {
    // Release the Sav and storage textures back to the applet heap; everything rebuilds on demand
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
            ShowBlockingToast("Saving... Do not close the app or power off.");
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

pu::ui::Overlay::Ref PKSMApplication::MakeToastOverlay(const std::string& message) {
    // A lingering error toast would make StartOverlay a silent no-op
    if (errorToastActive) {
        this->EndOverlay();
        errorToastActive = false;
    }
    auto text = pu::ui::elm::TextBlock::New(0, 0, message);
    text->SetFont(pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_HEADER));
    text->SetColor(pksm::ui::global::TEXT_WHITE);
    constexpr pu::i32 TOAST_PADDING = 60;
    const pu::i32 overlayWidth = text->GetWidth() + 2 * TOAST_PADDING;
    const pu::i32 overlayHeight = text->GetHeight() + 2 * TOAST_PADDING;
    auto overlay = pu::ui::Overlay::New(
        (static_cast<pu::i32>(pu::ui::render::ScreenWidth) - overlayWidth) / 2,
        (static_cast<pu::i32>(pu::ui::render::ScreenHeight) - overlayHeight) / 2,
        overlayWidth,
        overlayHeight,
        pu::ui::Color(30, 30, 30, 255)
    );
    overlay->SetFadeAlphaVariation(pu::ui::Overlay::DefaultMaxFadeAlpha);
    text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
    text->SetVerticalAlign(pu::ui::elm::VerticalAlign::Center);
    overlay->Add(text);
    return overlay;
}

void PKSMApplication::ShowBlockingToast(const std::string& message) {
    this->StartOverlay(MakeToastOverlay(message));
    // OnRender consumes the render-over flag each frame, so waiters re-arm it until they finish
    this->in_render_over = true;
    this->render_over_fn = [](pu::ui::render::Renderer::Ref&) { return true; };
}

void PKSMApplication::ShowErrorToast(const std::string& message) {
    // Ends after 3s or any button press (see the SetOnInput hook)
    this->StartOverlayWithTimeout(MakeToastOverlay(message), 3000);
    errorToastActive = true;
}

void PKSMApplication::ProcessPendingSaveAndExit() {
    if (!saveWriteResult.valid()) {
        return;
    }
    if (saveWriteResult.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
        // Still writing: keep input blocked
        this->in_render_over = true;
        this->render_over_fn = [](pu::ui::render::Renderer::Ref&) { return true; };
        return;
    }

    // A worker death (allocation failure) is a failed save with changes still loaded, not a crash
    bool saved = false;
    try {
        saved = saveWriteResult.get();
    } catch (const std::exception& e) {
        LOG_ERROR("Save write threw: " + std::string(e.what()));
    }
    LOG_DEBUG(saved ? "Save write completed" : "Save write failed");
    LOG_MEMORY();
    // Flush now - a follow-up crash would lose the 3s flush window
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

void PKSMApplication::ProcessPendingSaveLoad() {
    if (!saveLoadResult.valid()) {
        return;
    }
    if (saveLoadResult.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
        // Still parsing: keep input blocked
        this->in_render_over = true;
        this->render_over_fn = [](pu::ui::render::Renderer::Ref&) { return true; };
        return;
    }

    // A worker death (allocation failure) is just a failed load
    std::optional<pksm::saves::LoadedSave> loaded;
    try {
        loaded = saveLoadResult.get();
    } catch (const std::exception& e) {
        LOG_ERROR("Save load threw: " + std::string(e.what()));
    }
    // Release the mount on every outcome
    saveProvider->FinishLoad(*pendingSaveLoad);
    pendingSaveLoad.reset();

    const bool ok = saveDataAccessor->applySaveLoadResult(
        pendingLoadTitle,
        pendingLoadSaveName,
        &pendingLoadUserId,
        std::move(loaded)
    );
    pendingLoadTitle = nullptr;
    this->EndOverlay();
    if (ok) {
        LOG_MEMORY();
        this->ShowMainMenu();
        LOG_DEBUG(
            "Save selection to main menu: " +
            std::to_string(armTicksToNs(armGetSystemTick() - pendingLoadStartTick) / 1000000) + " ms total"
        );
    } else {
        LOG_ERROR("Failed to load save data");
        ShowErrorToast("This save could not be loaded");
    }
}

void PKSMApplication::ShowStorageScreen() {
    // Built on first entry: the screen's textures cost ~25MB of the applet heap
    if (!storageScreen) {
        const u64 t0 = armGetSystemTick();
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
        LOG_MEMORY();
        LOG_DEBUG(
            "Storage screen construct: " + std::to_string(armTicksToNs(armGetSystemTick() - t0) / 1000000) + " ms"
        );
    }
    LOG_DEBUG("Switching to storage screen");
    this->LoadLayout(this->storageScreen);
}

void PKSMApplication::OnSaveSelected(pksm::titles::Title::Ref title, pksm::saves::Save::Ref save) {
    LOG_DEBUG("Save selected: " + save->getName() + " for title: " + title->getName());

    // Resolution and any console mount stay on the UI thread; the worker only reads+parses
    auto userId = accountManager->GetCurrentAccount();
    pendingLoadStartTick = armGetSystemTick();
    pendingSaveLoad = saveProvider->ResolveLoad(title, save, &userId);
    if (!pendingSaveLoad) {
        saveDataAccessor->applySaveLoadResult(title, save->getName(), &userId, std::nullopt);
        ShowErrorToast("This save could not be loaded");
        return;
    }
    pendingLoadTitle = title;
    pendingLoadSaveName = save->getName();
    pendingLoadUserId = userId;
    ShowBlockingToast("Loading save...");
    saveLoadResult = std::async(std::launch::async, [this]() { return saveProvider->ExecuteLoad(*pendingSaveLoad); });
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

        // The storage screen is built lazily in ShowStorageScreen

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
        utils::Logger::Flush();
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to load application: " + std::string(e.what()));
        throw;
    }
}

}  // namespace pksm