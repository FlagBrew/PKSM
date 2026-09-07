#include "ScreenRouter.hpp"

#include "gui/shared/DialogStyle.hpp"
#include "utils/Logger.hpp"
#include "utils/TextureCaches.hpp"

namespace pksm {

ScreenRouter::ScreenRouter(
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
)
  : app(app),
    saveDataAccessor(std::move(saveDataAccessor)),
    boxDataProvider(std::move(boxDataProvider)),
    storageHand(std::move(storageHand)),
    boxNameEditor(std::move(boxNameEditor)),
    bagDataProvider(std::move(bagDataProvider)) {
    LOG_DEBUG("Creating title load screen...");
    titleLoadScreen = layout::TitleLoadScreen::New(
        titleProvider,
        saveProvider,
        accountManager,
        [this](pu::ui::Overlay::Ref overlay) { this->app.StartOverlay(overlay); },
        [this]() { this->app.EndOverlay(); },
        std::move(onSaveSelected)
    );

    LOG_DEBUG("Creating navigation callbacks...");
    std::map<ui::MenuButtonType, std::function<void()>> navigationCallbacks = {
        {ui::MenuButtonType::Storage, [this]() { this->ShowStorageScreen(); }},
        {ui::MenuButtonType::Editor, [this]() { LOG_DEBUG("Editor button pressed (not implemented)"); }},
        {ui::MenuButtonType::Events, [this]() { LOG_DEBUG("Events button pressed (not implemented)"); }},
        {ui::MenuButtonType::Bag, [this]() { this->ShowBagScreen(); }},
        {ui::MenuButtonType::Scripts, [this]() { LOG_DEBUG("Scripts button pressed (not implemented)"); }},
        {ui::MenuButtonType::Settings, [this]() { LOG_DEBUG("Settings button pressed (not implemented)"); }}
    };

    LOG_DEBUG("Creating main menu...");
    mainMenu = layout::MainMenu::New(
        std::move(onMainMenuBack),
        [this](pu::ui::Overlay::Ref overlay) { this->app.StartOverlay(overlay); },
        [this]() { this->app.EndOverlay(); },
        this->saveDataAccessor,
        navigationCallbacks
    );
}

void ScreenRouter::ShowMainMenu() {
    LOG_DEBUG("Switching to main menu");
    app.LoadLayout(mainMenu);
}

void ScreenRouter::ShowTitleLoadScreen() {
    // Release the per-save screens' textures back to the applet heap; everything rebuilds on
    // demand. Freeing hundreds of textures at once is the title return's cost, so it is measured
    const auto ms = [](u64 from, u64 to) { return std::to_string(armTicksToNs(to - from) / 1000000); };
    const u64 t1 = armGetSystemTick();
    if (storageScreen || bagScreen) {
        // Screens before caches: the rows let go of shared handles cheaply while the caches still
        // own them, so the caches bury sole-owned textures and the burst never lands here
        storageScreen = nullptr;
        bagScreen = nullptr;
        const u64 t2 = armGetSystemTick();
        const auto cleared = utils::TextureCaches::Clear();
        const u64 t3 = armGetSystemTick();
        LOG_DEBUG(
            "Title return: screens " + ms(t1, t2) + " ms, caches " + ms(t2, t3) + " ms (" +
            std::to_string(cleared.sprites) + " sprites, " + std::to_string(cleared.items) + " items, " +
            std::to_string(cleared.glyphs) + " glyphs, " + std::to_string(cleared.texts) + " texts)"
        );
        utils::Logger::Flush();  // a milestone worth having even if the app exits right after
    }
    LOG_MEMORY();
    LOG_DEBUG("Switching to title load screen");
    app.LoadLayout(titleLoadScreen);
}

void ScreenRouter::ShowStorageScreen() {
    // Built on first entry: the screen's textures cost ~3 MB of the applet heap
    if (!storageScreen) {
        const u64 t0 = armGetSystemTick();
        LOG_DEBUG("Creating storage screen on first use...");
        storageScreen = layout::StorageScreen::New(
            [this]() { this->ShowMainMenu(); },
            [this](pu::ui::Overlay::Ref overlay) { this->app.StartOverlay(overlay); },
            [this]() { this->app.EndOverlay(); },
            [this](const std::string& title, const std::string& message, const std::string& confirmLabel) {
                return this->app.CreateShowDialog(title, message, {confirmLabel, "Cancel"}, true) == 0;
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
    app.LoadLayout(storageScreen);
}

void ScreenRouter::ShowBagScreen() {
    // Built per loaded save, like the storage screen
    if (!bagScreen) {
        const u64 t0 = armGetSystemTick();
        bagScreen = layout::BagScreen::New(
            [this]() { this->ShowMainMenu(); },
            [this](pu::ui::Overlay::Ref overlay) { this->app.StartOverlay(overlay); },
            [this]() { this->app.EndOverlay(); },
            [this](const pksm::ui::ChoiceDialog& choice) {
                return this->app.CreateShowDialog(
                    choice.title,
                    choice.message,
                    choice.options,
                    true,
                    {},
                    [&choice](pu::ui::Dialog::Ref& dialog) {
                        if (choice.vertical) {
                            pksm::ui::StyleListDialog(dialog, choice.notes);
                        }
                    }
                );
            },
            saveDataAccessor,
            bagDataProvider
        );
        LOG_MEMORY();
        LOG_DEBUG("Bag screen construct: " + std::to_string(armTicksToNs(armGetSystemTick() - t0) / 1000000) + " ms");
    }
    LOG_DEBUG("Switching to bag screen");
    app.LoadLayout(bagScreen);
}

void ScreenRouter::OnSaveDataChanged(saves::SaveData::Ref saveData) {
    LOG_DEBUG("Updating MainMenu with new save data");
    mainMenu->UpdateTrainerInfo();

    // Preload box data for storage screen
    if (storageScreen && saveData) {
        LOG_DEBUG("Preloading box data for StorageScreen");
        storageScreen->LoadBoxData();
    }
}

}  // namespace pksm
