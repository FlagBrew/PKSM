#pragma once

#include <memory>
#include <pu/Plutonium>
#include <switch.h>

#include "data/AccountManager.hpp"
#include "data/providers/interfaces/IBagDataProvider.hpp"
#include "data/providers/interfaces/IBoxDataProvider.hpp"
#include "data/providers/interfaces/IBoxNameEditor.hpp"
#include "data/providers/interfaces/IStorageHand.hpp"
#include "gui/screens/bag-screen/BagScreen.hpp"
#include "gui/screens/main-menu/MainMenu.hpp"
#include "gui/screens/storage-screen/StorageScreen.hpp"
#include "gui/screens/title-load-screen/TitleLoadScreen.hpp"
#include "SaveSession.hpp"

namespace pksm {

class PKSMApplication : public pu::ui::Application {
private:
    // Screens
    pksm::layout::MainMenu::Ref mainMenu;
    pksm::layout::TitleLoadScreen::Ref titleLoadScreen;
    pksm::layout::StorageScreen::Ref storageScreen;
    pksm::layout::BagScreen::Ref bagScreen;

    // Data providers and managers
    std::unique_ptr<pksm::data::AccountManager> accountManager;
    ITitleDataProvider::Ref titleProvider;
    ISaveDataProvider::Ref saveProvider;
    ISaveDataAccessor::Ref saveDataAccessor;
    IBoxDataProvider::Ref boxDataProvider;
    IStorageHand::Ref storageHand;
    IBoxNameEditor::Ref boxNameEditor;
    IBagDataProvider::Ref bagDataProvider;

    std::unique_ptr<SaveSession> saveSession;

    // Navigation methods
    void ShowMainMenu();
    void ShowTitleLoadScreen();
    void ShowStorageScreen();
    void ShowBagScreen();

    pu::ui::Overlay::Ref MakeToastOverlay(const std::string& message);
    // Save-exit-style overlay; input stays blocked until EndOverlay
    void ShowBlockingToast(const std::string& message);
    // The block is consumed each frame; whoever waits on a worker re-arms it until it finishes
    void KeepInputBlocked();
    // Non-blocking notice that ends after 3s or any button press
    void ShowErrorToast(const std::string& message);

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
        IBoxNameEditor::Ref boxNameEditor,
        IBagDataProvider::Ref bagDataProvider
    );
    PU_SMART_CTOR(PKSMApplication)

    // Initialize the application with all necessary configuration
    static PKSMApplication::Ref Initialize();

    void OnLoad() override;
};

}  // namespace pksm