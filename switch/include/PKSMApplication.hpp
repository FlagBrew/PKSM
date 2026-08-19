#pragma once

#include <future>
#include <memory>
#include <pu/Plutonium>
#include <switch.h>

#include "data/AccountManager.hpp"
#include "data/providers/interfaces/IBoxDataProvider.hpp"
#include "data/providers/interfaces/IBoxNameEditor.hpp"
#include "data/providers/interfaces/IStorageHand.hpp"
#include "gui/screens/main-menu/MainMenu.hpp"
#include "gui/screens/storage-screen/StorageScreen.hpp"
#include "gui/screens/title-load-screen/TitleLoadScreen.hpp"

namespace pksm {

class PKSMApplication : public pu::ui::Application {
private:
    // Screens
    pksm::layout::MainMenu::Ref mainMenu;
    pksm::layout::TitleLoadScreen::Ref titleLoadScreen;
    pksm::layout::StorageScreen::Ref storageScreen;

    // Data providers and managers
    std::unique_ptr<pksm::data::AccountManager> accountManager;
    ITitleDataProvider::Ref titleProvider;
    ISaveDataProvider::Ref saveProvider;
    ISaveDataAccessor::Ref saveDataAccessor;
    IBoxDataProvider::Ref boxDataProvider;
    IStorageHand::Ref storageHand;
    IBoxNameEditor::Ref boxNameEditor;

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
    void ShowStorageScreen();
    void HandleMainMenuBack();

    // Save handling
    void OnSaveSelected(pksm::titles::Title::Ref title, pksm::saves::Save::Ref save);
    void ProcessPendingSaveAndExit();
    void ProcessPendingSaveLoad();
    // Save-exit-style overlay; input stays blocked until EndOverlay
    void ShowBlockingToast(const std::string& message);
    // Non-blocking notice that ends after 3s or any button press
    void ShowErrorToast(const std::string& message);

    // In-flight save-and-exit write: runs on a worker thread so the render
    // loop keeps animating; ProcessPendingSaveAndExit polls it each frame,
    // keeping input blocked until it completes
    std::future<bool> saveWriteResult;

    // An in-flight save load: the provider resolved (and possibly mounted)
    // on the UI thread, the worker reads+parses, and ProcessPendingSaveLoad
    // finishes the mount and commits on the UI thread
    std::future<std::optional<pksm::saves::LoadedSave>> saveLoadResult;
    std::optional<pksm::saves::PendingLoad> pendingSaveLoad;
    pksm::titles::Title::Ref pendingLoadTitle;
    std::string pendingLoadSaveName;
    AccountUid pendingLoadUserId{};
    u64 pendingLoadStartTick = 0;

    bool errorToastActive = false;

public:
    PKSMApplication(
        pu::ui::render::Renderer::Ref renderer,
        std::unique_ptr<data::AccountManager> accountManager,
        ITitleDataProvider::Ref titleProvider,
        ISaveDataProvider::Ref saveProvider,
        ISaveDataAccessor::Ref saveDataAccessor,
        IBoxDataProvider::Ref boxDataProvider,
        IStorageHand::Ref storageHand,
        IBoxNameEditor::Ref boxNameEditor
    );
    PU_SMART_CTOR(PKSMApplication)

    // Initialize the application with all necessary configuration
    static PKSMApplication::Ref Initialize();

    void OnLoad() override;
};

}  // namespace pksm