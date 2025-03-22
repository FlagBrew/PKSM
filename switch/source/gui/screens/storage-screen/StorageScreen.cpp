#include "gui/screens/storage-screen/StorageScreen.hpp"

#include <ctime>

#include "gui/screens/main-menu/sub-components/menu-grid/MenuButtonGrid.hpp"
#include "utils/Logger.hpp"

namespace pksm::layout {

StorageScreen::StorageScreen(
    std::function<void()> onBack,
    std::function<void(pu::ui::Overlay::Ref)> onShowOverlay,
    std::function<void()> onHideOverlay
)
  : BaseLayout(onShowOverlay, onHideOverlay), onBack(onBack) {
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

    // Set initial help items
    std::vector<pksm::ui::HelpItem> helpItems = {
        {{{pksm::ui::HelpButton::A}}, "Select"},
        {{{pksm::ui::HelpButton::B}}, "Back to Main Menu"},
        {{{pksm::ui::HelpButton::L}, {pksm::ui::HelpButton::R}}, "Switch Box"},
        {{{pksm::ui::HelpButton::DPad}}, "Navigate Box"},
    };
    helpFooter->SetHelpItems(helpItems);

    // Set up input handling
    this->SetOnInput(
        std::bind(&StorageScreen::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    );

    LOG_DEBUG("StorageScreen initialization complete");
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
    boxGrid->SetName("BoxGrid Element");

    // First add the grid to the layout
    this->Add(boxGrid);

    // THEN establish the owning relationship
    boxGrid->EstablishOwningRelationship();

    // Load mock Pokémon data
    LoadMockPokemonData();

    // Set up selection changed callback
    boxGrid->SetOnSelectionChanged([this](int boxIndex, int slotIndex) {
        LOG_DEBUG("Box selection changed: Box " + std::to_string(boxIndex) + ", Slot " + std::to_string(slotIndex));
    });

    // Set initial focus
    boxGrid->RequestFocus();

    LOG_DEBUG("BoxGrid initialization complete");
}

void StorageScreen::LoadMockPokemonData() {
    LOG_DEBUG("Loading mock Pokémon data...");

    // Set up boxes - creating 30 boxes for stress testing
    boxGrid->SetBoxCount(30);

    // Create first two boxes with specific Pokemon arrangements
    std::vector<pksm::ui::BoxPokemonData> box1Data;

    // Box 1: Original starters and their evolutions
    box1Data.push_back(pksm::ui::BoxPokemonData(1, 0, false));  // Bulbasaur
    box1Data.push_back(pksm::ui::BoxPokemonData(2, 0, false));  // Ivysaur
    box1Data.push_back(pksm::ui::BoxPokemonData(3, 0, false));  // Venusaur
    box1Data.push_back(pksm::ui::BoxPokemonData(3, 0, true));  // Shiny Venusaur
    box1Data.push_back(pksm::ui::BoxPokemonData(3, 1, false));  // Gigantamax Venusaur (using form 1)
    box1Data.push_back(pksm::ui::BoxPokemonData(3, 2, false));  // Mega Venusaur (using form 2)

    box1Data.push_back(pksm::ui::BoxPokemonData(4, 0, false));  // Charmander
    box1Data.push_back(pksm::ui::BoxPokemonData(5, 0, false));  // Charmeleon
    box1Data.push_back(pksm::ui::BoxPokemonData(6, 0, false));  // Charizard
    box1Data.push_back(pksm::ui::BoxPokemonData(6, 0, true));  // Shiny Charizard
    box1Data.push_back(pksm::ui::BoxPokemonData(6, 1, false));  // Gigantamax Charizard (using form 1)
    box1Data.push_back(pksm::ui::BoxPokemonData());  // Empty slot

    box1Data.push_back(pksm::ui::BoxPokemonData(7, 0, false));  // Squirtle
    box1Data.push_back(pksm::ui::BoxPokemonData(8, 0, false));  // Wartortle
    box1Data.push_back(pksm::ui::BoxPokemonData(9, 0, false));  // Blastoise
    box1Data.push_back(pksm::ui::BoxPokemonData(9, 0, true));  // Shiny Blastoise
    box1Data.push_back(pksm::ui::BoxPokemonData(9, 1, false));  // Gigantamax Blastoise (using form 1)
    box1Data.push_back(pksm::ui::BoxPokemonData(9, 2, false));  // Mega Blastoise (using form 2)

    box1Data.push_back(pksm::ui::BoxPokemonData(10, 0, false));  // Caterpie
    box1Data.push_back(pksm::ui::BoxPokemonData(10, 0, true));  // Shiny Caterpie
    box1Data.push_back(pksm::ui::BoxPokemonData(11, 0, false));  // Metapod
    box1Data.push_back(pksm::ui::BoxPokemonData(11, 0, true));  // Shiny Metapod
    box1Data.push_back(pksm::ui::BoxPokemonData(12, 0, false));  // Butterfree
    box1Data.push_back(pksm::ui::BoxPokemonData(12, 0, true));  // Shiny Butterfree

    box1Data.push_back(pksm::ui::BoxPokemonData(3, 0, true));  // Shiny Venusaur
    box1Data.push_back(pksm::ui::BoxPokemonData(6, 0, true));  // Shiny Charizard
    box1Data.push_back(pksm::ui::BoxPokemonData());  // Empty slot
    box1Data.push_back(pksm::ui::BoxPokemonData(9, 0, true));  // Shiny Blastoise
    box1Data.push_back(pksm::ui::BoxPokemonData(7, 0, true));  // Shiny Squirtle

    // Add some empty slots to fill the box
    for (size_t i = box1Data.size(); i < 30; i++) {
        box1Data.push_back(pksm::ui::BoxPokemonData());
    }

    // Box 2: Alternating patterns
    std::vector<pksm::ui::BoxPokemonData> box2Data;

    for (int i = 0; i < 6; i++) {
        box2Data.push_back(pksm::ui::BoxPokemonData(12, 0, i % 2 == 0));  // Alternating normal/shiny Butterfree
    }

    for (int i = 0; i < 6; i++) {
        box2Data.push_back(pksm::ui::BoxPokemonData(9, 0, i % 2 == 0));  // Alternating normal/shiny Blastoise
    }

    for (int i = 0; i < 6; i++) {
        box2Data.push_back(pksm::ui::BoxPokemonData(6, 0, i % 2 == 0));  // Alternating normal/shiny Charizard
    }

    for (int i = 0; i < 6; i++) {
        box2Data.push_back(pksm::ui::BoxPokemonData(3, 0, i % 2 == 0));  // Alternating normal/shiny Venusaur
    }

    for (int i = 0; i < 6; i++) {
        box2Data.push_back(pksm::ui::BoxPokemonData(10, 0, i % 2 == 0));  // Alternating normal/shiny Caterpie
    }

    // Fill remaining slots in box 2
    for (size_t i = box2Data.size(); i < 30; i++) {
        box2Data.push_back(pksm::ui::BoxPokemonData());
    }

    // Generate 28 more boxes with randomized Pokemon
    std::srand(std::time(nullptr));  // Seed the random number generator

    // Create boxes 3-30 with randomized content
    for (int boxIdx = 2; boxIdx < 30; boxIdx++) {
        std::vector<pksm::ui::BoxPokemonData> boxData;

        // Fill each box with 30 slots
        for (int slot = 0; slot < 30; slot++) {
            // 35% chance of empty slot
            if (std::rand() % 100 < 20) {
                boxData.push_back(pksm::ui::BoxPokemonData());
            } else {
                // Random species (1-151 for Gen 1 Pokémon)
                u16 species = 1 + (std::rand() % 1025);

                // Random shiny (10% chance)
                bool shiny = (std::rand() % 100 < 10);

                // Using form 0 as requested to avoid issues with missing forms
                boxData.push_back(pksm::ui::BoxPokemonData(species, 0, shiny));
            }
        }

        // Load the random box data
        boxGrid->SetBoxData(boxIdx, boxData);
    }

    // Load the first two boxes with predetermined data
    boxGrid->SetBoxData(0, box1Data);
    boxGrid->SetBoxData(1, box2Data);

    // Start at box 0
    boxGrid->SetCurrentBox(0);

    LOG_DEBUG("Mock Pokémon data loaded successfully with 30 boxes");
}

StorageScreen::~StorageScreen() = default;

void StorageScreen::OnInput(u64 down, u64 up, u64 held) {
    // First handle help-related input
    if (HandleHelpInput(down)) {
        return;  // Input was handled by help system
    }

    // Only process other input if not in help overlay
    if (down & HidNpadButton_B) {
        LOG_DEBUG("B button pressed, returning to main menu");
        if (onBack) {
            onBack();
        }
    } else if (down & HidNpadButton_A) {
        // Handle A button for BoxGrid selection
        boxGrid->HandleSelectInput(down);
    } else if (down & HidNpadButton_R) {
        // Next box
        int currentBox = boxGrid->GetCurrentBox();
        if (currentBox < 0) {
            boxGrid->SetCurrentBox(0);
        } else {
            boxGrid->SetCurrentBox(currentBox + 1);
        }
    } else if (down & HidNpadButton_L) {
        // Previous box
        int currentBox = boxGrid->GetCurrentBox();
        if (currentBox > 0) {
            boxGrid->SetCurrentBox(currentBox - 1);
        }
    } else {
        // Pass through input to BoxGrid
        pu::ui::TouchPoint touch;
        boxGrid->OnInput(down, up, held, touch);
    }
}

std::vector<pksm::ui::HelpItem> StorageScreen::GetHelpOverlayItems() const {
    return {
        {{{pksm::ui::HelpButton::A}}, "Select Pokémon"},
        {{{pksm::ui::HelpButton::B}}, "Back to Main Menu"},
        {{{pksm::ui::HelpButton::DPad}, {pksm::ui::HelpButton::AnalogStick}}, "Navigate Box"},
        {{{pksm::ui::HelpButton::L}}, "Previous Box"},
        {{{pksm::ui::HelpButton::R}}, "Next Box"},
        {{{pksm::ui::HelpButton::Minus}}, "Close Help"}
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