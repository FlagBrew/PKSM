#pragma once

#include <functional>
#include <pu/Plutonium>

#include "data/AccountManager.hpp"
#include "data/providers/interfaces/IBagDataProvider.hpp"
#include "data/providers/interfaces/IBoxDataProvider.hpp"
#include "data/providers/interfaces/IBoxNameEditor.hpp"
#include "data/providers/interfaces/ISaveDataAccessor.hpp"
#include "data/providers/interfaces/ISaveDataProvider.hpp"
#include "data/providers/interfaces/IStorageHand.hpp"
#include "data/providers/interfaces/ITitleDataProvider.hpp"
#include "gui/screens/bag-screen/BagScreen.hpp"
#include "gui/screens/main-menu/MainMenu.hpp"
#include "gui/screens/storage-screen/StorageScreen.hpp"
#include "gui/screens/title-load-screen/TitleLoadScreen.hpp"

namespace pksm {

// The screens and which one is up. The title screen and main menu live for the run; the storage
// and bag screens are built on first entry into a loaded save and torn down on the title return.
class ScreenRouter {
public:
    ScreenRouter(
        pu::ui::Application& app,
        data::AccountManager& accountManager,
        ITitleDataProvider::Ref titleProvider,
        ISaveDataProvider::Ref saveProvider,
        ISaveDataAccessor::Ref saveDataAccessor,
        IBoxDataProvider::Ref boxDataProvider,
        IStorageHand::Ref storageHand,
        IBoxNameEditor::Ref boxNameEditor,
        IBagDataProvider::Ref bagDataProvider,
        std::function<void(titles::Title::Ref, saves::Save::Ref)> onSaveSelected,
        std::function<void()> onMainMenuBack
    );

    void ShowMainMenu();
    // Releases the per-save screens and the texture caches on the way; the session has let the Sav go
    void ShowTitleLoadScreen();
    void ShowStorageScreen();
    void ShowBagScreen();

    // Keeps the screens in step with a loaded, reloaded or cleared save
    void OnSaveDataChanged(saves::SaveData::Ref saveData);

private:
    pu::ui::Application& app;
    ISaveDataAccessor::Ref saveDataAccessor;
    IBoxDataProvider::Ref boxDataProvider;
    IStorageHand::Ref storageHand;
    IBoxNameEditor::Ref boxNameEditor;
    IBagDataProvider::Ref bagDataProvider;

    layout::TitleLoadScreen::Ref titleLoadScreen;
    layout::MainMenu::Ref mainMenu;
    layout::StorageScreen::Ref storageScreen;
    layout::BagScreen::Ref bagScreen;
};

}  // namespace pksm
