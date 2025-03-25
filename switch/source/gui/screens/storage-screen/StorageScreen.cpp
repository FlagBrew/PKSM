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
    InitializeBoxGrid();

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
    boxGridFocusManager = pksm::input::FocusManager::New("BoxGrid Manager");

    // Initialize selection managers
    storageScreenSelectionManager = pksm::input::SelectionManager::New("StorageScreen Manager");
    storageScreenSelectionManager->SetActive(true);  // since this is the root manager
    boxGridSelectionManager = pksm::input::SelectionManager::New("BoxGrid Manager");

    // Set up focus manager hierarchy
    storageScreenFocusManager->RegisterChildManager(boxGridFocusManager);

    // Set up selection manager hierarchy
    storageScreenSelectionManager->RegisterChildManager(boxGridSelectionManager);

    // Set up directional input handlers
    boxGridHandler.SetOnMoveUp([this]() {
        // Handle up navigation from box grid if needed
    });

    boxGridHandler.SetOnMoveDown([this]() {
        // Handle down navigation from box grid if needed
    });

    LOG_DEBUG("Focus and selection management initialization complete");
}

void StorageScreen::InitializeBoxGrid() {
    LOG_DEBUG("Initializing BoxGrid...");

    // Create the BoxGrid with default rows/columns
    boxGrid = pksm::ui::BoxGrid::New(
        BOX_GRID_SIDE_MARGIN,
        BOX_GRID_TOP_MARGIN,
        BOX_ITEM_SIZE,
        boxGridFocusManager,
        boxGridSelectionManager
    );

    // Set up BoxGrid
    boxGrid->IFocusable::SetName("BoxGrid Element");  // Specify which SetName we're calling

    // First add the grid to the layout
    this->Add(boxGrid);

    // THEN establish the owning relationship
    boxGrid->EstablishOwningRelationship();

    // Load box data for the current save
    LoadBoxData();

    // Set up selection changed callback
    boxGrid->SetOnSelectionChanged([this](int boxIndex, int slotIndex) {
        LOG_DEBUG("Box selection changed: Box " + std::to_string(boxIndex) + ", Slot " + std::to_string(slotIndex));
    });

    // Set initial focus
    boxGrid->RequestFocus();

    LOG_DEBUG("BoxGrid initialization complete");
}

void StorageScreen::LoadBoxData() {
    LOG_DEBUG("Loading box data from provider...");

    auto currentSave = saveDataAccessor->getCurrentSaveData();
    if (!currentSave) {
        LOG_DEBUG("No save data available, using fallback box data");
        // Set a default box count if no save data available
        boxGrid->SetBoxCount(1);
        // Start at box 0
        boxGrid->SetCurrentBox(0);
        LOG_DEBUG("Fallback box data loaded");
        return;
    }

    // Get box count from the provider
    size_t boxCount = boxDataProvider->GetBoxCount(currentSave);
    LOG_DEBUG("Setting box count to " + std::to_string(boxCount));
    boxGrid->SetBoxCount(boxCount);

    // Load all boxes at once to ensure the box data provider knows about them
    // The BoxGrid component will handle which ones to actually render
    for (size_t i = 0; i < boxCount; ++i) {
        auto boxData = boxDataProvider->GetBoxData(currentSave, i);
        boxGrid->SetBoxData(i, boxData);
    }

    // Start at box 0
    boxGrid->SetCurrentBox(0);

    LOG_DEBUG("Box data loaded successfully");
}

StorageScreen::~StorageScreen() = default;

void StorageScreen::OnInput(u64 down, u64 up, u64 held) {
    // First handle help-related input
    if (HandleHelpInput(down)) {
        return;  // Input was handled by help system
    }

    // Process button inputs
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
    boxGrid->SetDisabled(true);
}

void StorageScreen::OnHelpOverlayHidden() {
    LOG_DEBUG("Help overlay hidden, re-enabling UI elements");
    boxGrid->SetDisabled(false);
}

}  // namespace pksm::layout