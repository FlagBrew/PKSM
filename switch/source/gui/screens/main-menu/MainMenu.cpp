#include "gui/screens/main-menu/MainMenu.hpp"

#include "gui/screens/main-menu/sub-components/menu-grid/MenuButtonGrid.hpp"

namespace pksm::layout {

MainMenu::MainMenu(std::function<void()> onBack) : Layout(), onBack(onBack) {
    // Initialize focus managers
    mainMenuFocusManager = pksm::input::FocusManager::New("MainMenu Manager");
    menuGridFocusManager = pksm::input::FocusManager::New("MenuGrid Manager");
    mainMenuFocusManager->RegisterChildManager(menuGridFocusManager);
    mainMenuFocusManager->SetActive(true);  // Root manager should be active

    this->SetBackgroundColor(bgColor);
    background = ui::AnimatedBackground::New();
    this->Add(background);

    // Create trainer info component with dynamic height
    trainerInfo = pksm::ui::TrainerInfo::New(
        TRAINER_INFO_SIDE_MARGIN,
        TRAINER_INFO_TOP_MARGIN,
        TRAINER_INFO_WIDTH,
        "Wow",  // OT Name
        3,  // Badges
        0,  // Wonder Cards
        54,  // National Dex Seen
        26,  // National Dex Caught
        0.48f,  // Completion
        "26h 15m"  // Time played
    );
    this->Add(trainerInfo);

    // Calculate MenuButtonGrid width and position
    const pu::i32 MENU_GRID_MARGIN = 32;  // Margin between TrainerInfo and MenuButtonGrid
    const pu::i32 menuGridWidth = GetWidth() - (TRAINER_INFO_SIDE_MARGIN + TRAINER_INFO_WIDTH + (MENU_GRID_MARGIN * 2));
    const pu::i32 menuGridX = TRAINER_INFO_SIDE_MARGIN + TRAINER_INFO_WIDTH + MENU_GRID_MARGIN;

    // Create menu button grid
    menuGrid = pksm::ui::MenuButtonGrid::New(menuGridX, MENU_GRID_TOP_MARGIN, menuGridWidth);
    this->Add(menuGrid);

    // Set up input handling
    this->SetOnInput(
        std::bind(&MainMenu::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    );
}

MainMenu::~MainMenu() = default;

void MainMenu::OnInput(u64 down, u64 up, u64 held) {
    if (down & HidNpadButton_B) {
        if (onBack) {
            onBack();
        }
    }
}

}  // namespace pksm::layout