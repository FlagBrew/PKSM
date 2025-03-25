#include "gui/screens/storage-screen/StorageScreen.hpp"

#include <ctime>

#include "gui/screens/main-menu/sub-components/menu-grid/MenuButtonGrid.hpp"
#include "utils/Logger.hpp"

namespace pksm::layout {

StorageScreen::StorageScreen(
    std::function<void()> onBack,
    std::function<void(pu::ui::Overlay::Ref)> onShowOverlay,
    std::function<void()> onHideOverlay,
    ISaveDataAccessor::Ref saveDataAccessor,
    IBoxDataProvider::Ref boxDataProvider
)
  : BaseLayout(onShowOverlay, onHideOverlay),
    onBack(onBack),
    saveDataAccessor(saveDataAccessor),
    boxDataProvider(boxDataProvider) {
    LOG_DEBUG("Initializing StorageScreen...");

    this->SetBackgroundColor(bgColor);
    background = ui::AnimatedBackground::New();
    this->Add(background);

    // Initialize focus management
    InitializeFocusManagement();

    // Initialize BoxGrid
    InitializePokemonBox();

    // Initialize help footer
    InitializeHelpFooter();

    // Set up button input handler for Back button
    buttonHandler.RegisterButton(HidNpadButton_B, nullptr, [this]() {
        LOG_DEBUG("B button pressed, returning to main menu");
        if (this->onBack) {
            this->onBack();
        }
    });

    // Set initial help items
    std::vector<pksm::ui::HelpItem> helpItems = {
        {{{pksm::ui::global::ButtonGlyph::A}}, "Select"},
        {{{pksm::ui::global::ButtonGlyph::B}}, "Back to Main Menu"},
        {{{pksm::ui::global::ButtonGlyph::L}, {pksm::ui::global::ButtonGlyph::R}}, "Switch Box"},
        {{{pksm::ui::global::ButtonGlyph::DPad}}, "Navigate Box"},
    };
    helpFooter->SetHelpItems(helpItems);

    // Set up input handling
    this->SetOnInput(
        std::bind(&StorageScreen::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    );

    LOG_DEBUG("StorageScreen initialization complete");
    PreRender();
}

void StorageScreen::InitializeFocusManagement() {
    LOG_DEBUG("Initializing focus and selection management...");

    // Initialize focus managers
    storageScreenFocusManager = pksm::input::FocusManager::New("StorageScreen Manager");
    storageScreenFocusManager->SetActive(true);  // since this is the root manager
    pokemonBoxFocusManager = pksm::input::FocusManager::New("PokemonBox Manager");

    // Initialize selection managers
    storageScreenSelectionManager = pksm::input::SelectionManager::New("StorageScreen Manager");
    storageScreenSelectionManager->SetActive(true);  // since this is the root manager
    pokemonBoxSelectionManager = pksm::input::SelectionManager::New("PokemonBox Manager");

    storageScreenFocusManager->RegisterChildManager(pokemonBoxFocusManager);
    storageScreenSelectionManager->RegisterChildManager(pokemonBoxSelectionManager);

    // Set up directional input handlers
    pokemonBoxDirectionalHandler.SetOnMoveUp([this]() {
        // Handle up navigation from box grid if needed
    });

    LOG_DEBUG("Focus and selection management initialization complete");
}

void StorageScreen::InitializePokemonBox() {
    LOG_DEBUG("Initializing PokemonBox...");

    // Create the PokemonBox with the same parameters as we would BoxGrid
    pokemonBox = pksm::ui::PokemonBox::New(
        BOX_GRID_SIDE_MARGIN,
        BOX_GRID_TOP_MARGIN,
        BOX_ITEM_SIZE,
        pokemonBoxFocusManager,
        pokemonBoxSelectionManager
    );
    this->Add(pokemonBox);
    pokemonBox->SetName("PokemonBox Element");
    pokemonBox->EstablishOwningRelationship();

    // Load box data for the current save
    LoadBoxData();

    // Set up selection changed callback
    pokemonBox->SetOnSelectionChanged([this](int boxIndex, int slotIndex) {
        LOG_DEBUG("Box selection changed: Box " + std::to_string(boxIndex) + ", Slot " + std::to_string(slotIndex));
    });

    // Set initial focus
    pokemonBox->RequestFocus();

    LOG_DEBUG("PokemonBox initialization complete");
}

void StorageScreen::LoadBoxData() {
    LOG_DEBUG("Loading box data from provider...");

    auto currentSave = saveDataAccessor->getCurrentSaveData();
    if (!currentSave) {
        LOG_DEBUG("No save data available, using fallback box data");
        // Set a default box count if no save data available
        pokemonBox->SetBoxCount(1);
        // Start at box 0
        pokemonBox->SetCurrentBox(0);
        LOG_DEBUG("Fallback box data loaded");
        return;
    }

    // Get box count from the provider
    size_t boxCount = boxDataProvider->GetBoxCount(currentSave);
    LOG_DEBUG("Setting box count to " + std::to_string(boxCount));
    pokemonBox->SetBoxCount(boxCount);

    // Load all boxes at once to ensure the box data provider knows about them
    for (size_t i = 0; i < boxCount; ++i) {
        auto boxData = boxDataProvider->GetBoxData(currentSave, i);
        pokemonBox->SetBoxData(i, boxData);
    }

    // Start at box 0
    pokemonBox->SetCurrentBox(0);

    LOG_DEBUG("Box data loaded successfully");
}

StorageScreen::~StorageScreen() = default;

void StorageScreen::OnInput(u64 down, u64 up, u64 held) {
    // First handle help-related input
    if (HandleHelpInput(down)) {
        return;  // Input was handled by help system
    }

    // Process button inputs
    pokemonBoxDirectionalHandler.HandleInput(down, held);
    buttonHandler.HandleInput(down, up, held);
}

std::vector<pksm::ui::HelpItem> StorageScreen::GetHelpOverlayItems() const {
    return {
        {{{pksm::ui::global::ButtonGlyph::A}}, "Select Pokémon"},
        {{{pksm::ui::global::ButtonGlyph::B}}, "Back to Main Menu"},
        {{{pksm::ui::global::ButtonGlyph::DPad}, {pksm::ui::global::ButtonGlyph::AnalogStick}}, "Navigate Box"},
        {{{pksm::ui::global::ButtonGlyph::L}}, "Previous Box"},
        {{{pksm::ui::global::ButtonGlyph::R}}, "Next Box"},
        {{{pksm::ui::global::ButtonGlyph::Minus}}, "Close Help"}
    };
}

void StorageScreen::OnHelpOverlayShown() {
    LOG_DEBUG("Help overlay shown, disabling UI elements");
    pokemonBox->SetDisabled(true);
}

void StorageScreen::OnHelpOverlayHidden() {
    LOG_DEBUG("Help overlay hidden, re-enabling UI elements");
    pokemonBox->SetDisabled(false);
}

}  // namespace pksm::layout