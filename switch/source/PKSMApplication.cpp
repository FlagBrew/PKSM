#include "PKSMApplication.hpp"

#include <sstream>

#include "data/providers/SaveDataAccessor.hpp"
#include "data/providers/SwitchSaveDataProvider.hpp"
#include "data/providers/SwitchTitleDataProvider.hpp"
#include "data/providers/BagDataProvider.hpp"
#include "data/providers/BoxDataProvider.hpp"
#include "data/providers/SwitchSaveDataWriter.hpp"
#include "gui/boot/BootFlow.hpp"
#include "gui/boot/RendererSetup.hpp"
#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"
#include "utils/TextureCaches.hpp"

namespace pksm {

PKSMApplication::PKSMApplication(
    pu::ui::render::Renderer::Ref renderer,
    std::unique_ptr<data::AccountManager> accountManager,
    ITitleDataProvider::Ref titleProvider,
    ISaveDataProvider::Ref saveProvider,
    ISaveDataAccessor::Ref saveDataAccessor,
    IBoxDataProvider::Ref boxDataProvider,
    IStorageHand::Ref storageHand,
    IBoxNameEditor::Ref boxNameEditor,
    IBagDataProvider::Ref bagDataProvider
)
  : pu::ui::Application(renderer),
    accountManager(std::move(accountManager)),
    titleProvider(std::move(titleProvider)),
    saveProvider(std::move(saveProvider)),
    saveDataAccessor(std::move(saveDataAccessor)),
    boxDataProvider(std::move(boxDataProvider)),
    storageHand(std::move(storageHand)),
    boxNameEditor(std::move(boxNameEditor)),
    bagDataProvider(std::move(bagDataProvider)) {
    saveSession = std::make_unique<SaveSession>(
        this->saveProvider,
        this->saveDataAccessor,
        *this->accountManager,
        SaveSession::Hooks{
            .showBlockingToast = [this](const std::string& message) { this->ShowBlockingToast(message); },
            .keepInputBlocked = [this]() { this->KeepInputBlocked(); },
            .endOverlay = [this]() { this->EndOverlay(); },
            .showErrorToast = [this](const std::string& message) { this->ShowErrorToast(message); },
            .requestChoice =
                [this](const std::string& title, const std::string& message, const std::vector<std::string>& options) {
                    return this->CreateShowDialog(title, message, options, true);
                },
            .onSaveLoaded = [this]() { screens->ShowMainMenu(); },
            .onSaveLeft = [this]() { screens->ShowTitleLoadScreen(); },
        }
    );
    // Add render callback to process account updates
    AddRenderCallback([this]() { this->accountManager->ProcessPendingUpdates(); });
    AddRenderCallback([this]() { this->saveSession->Poll(); });
    // A title return's caches, spread thin; a frame nobody can interact with can spare more
    AddRenderCallback([this]() { utils::TextureCaches::Drain(this->in_render_over ? 8 : 2); });
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

        boot::PhaseTimer phases;
        renderer = boot::CreateRenderer();
        phases.Log("renderer + fonts");

        boot::BootProgress bootProgress(renderer);
        if (!boot::RunAssetBootstrap(bootProgress)) {
            // No Application exists yet to run renderer teardown; skipping
            // Finalize hands hbmenu a live SDL/romfs stack, which crashes it
            bootProgress.Release();
            renderer->Finalize();
            return nullptr;
        }

        phases.Log("asset bootstrap");

        if (!utils::TextureCaches::Initialize()) {
            LOG_ERROR("Failed to initialize the sprite sheets");
            bootProgress.Release();
            renderer->Finalize();
            return nullptr;
        }
        phases.Log("sprite sheets");

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
        auto bagDataProvider = BagDataProvider::New(saveDataAccessor);
        phases.Log("data providers");
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
            boxDataProvider,
            bagDataProvider
        );

        LOG_DEBUG("Preparing application...");
        app->Prepare();
        phases.Log("screens");

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
    KeepInputBlocked();
}

void PKSMApplication::KeepInputBlocked() {
    this->in_render_over = true;
    this->render_over_fn = [](pu::ui::render::Renderer::Ref&) { return true; };
}

void PKSMApplication::ShowErrorToast(const std::string& message) {
    // Ends after 3s or any button press (see the SetOnInput hook)
    this->StartOverlayWithTimeout(MakeToastOverlay(message), 3000);
    errorToastActive = true;
}

void PKSMApplication::OnLoad() {
    try {
        LOG_DEBUG("Loading title screen...");
        LOG_MEMORY();

        screens = std::make_unique<ScreenRouter>(
            *this,
            *accountManager,
            titleProvider,
            saveProvider,
            saveDataAccessor,
            boxDataProvider,
            storageHand,
            boxNameEditor,
            bagDataProvider,
            [this](pksm::titles::Title::Ref title, pksm::saves::Save::Ref save) { saveSession->Load(title, save); },
            [this]() { saveSession->Leave(); }
        );

        // Register for save data changes in both MainMenu and StorageScreen
        LOG_DEBUG("Setting up save data change callbacks...");
        saveDataAccessor->setOnSaveDataChanged([this](pksm::saves::SaveData::Ref saveData) {
            LOG_DEBUG("Save data changed, updating UI");
            screens->OnSaveDataChanged(saveData);
        });

        // Start with title load screen
        LOG_DEBUG("Loading initial screen...");
        screens->ShowTitleLoadScreen();

        LOG_DEBUG("Application loaded successfully");
        utils::Logger::Flush();
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to load application: " + std::string(e.what()));
        throw;
    }
}

}  // namespace pksm