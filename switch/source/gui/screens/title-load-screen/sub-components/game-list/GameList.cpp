#include "gui/screens/title-load-screen/sub-components/game-list/GameList.hpp"

#include <cmath>
#include <sstream>

#include "gui/render/LineRenderer.hpp"
#include "gui/render/PatternRenderer.hpp"
#include "gui/screens/title-load-screen/sub-components/game-list/GameListCommon.hpp"
#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"

const std::vector<pksm::ui::GameListInfo> pksm::ui::GameList::NAVIGATION_ORDER = {
    {GameListType::Console, "Console"},
    {GameListType::Custom, "Custom"},
    {GameListType::Emulator, "Emulator"}
};

pksm::ui::GameList::GameList(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    input::FocusManager::Ref parentFocusManager,
    ITitleDataProvider::Ref titleProvider
)
  : Element(),
    focused(false),
    backgroundColor(pu::ui::Color(40, 51, 135, 255)),
    onSelectionChangedCallback(nullptr),
    x(x),
    y(y),
    width(width),
    height(height),
    titleProvider(titleProvider),
    currentGameListIndex(0) {
    LOG_DEBUG("Initializing GameList component...");

    // Initialize focus managers for each game list type
    consoleGameListManager = input::FocusManager::New("ConsoleGameList Manager");
    customGameListManager = input::FocusManager::New("CustomGameList Manager");
    emulatorGameListManager = input::FocusManager::New("EmulatorGameList Manager");

    // Create game lists with proper dimensions
    GameListLayoutConfig consoleListConfig = {
        .paddingLeft = CONSOLE_LIST_PADDING_LEFT,
        .paddingRight = PADDING_RIGHT,
        .paddingTop = PADDING_TOP,
        .paddingBottom = PADDING_BOTTOM,
        .sectionTitleSpacing = SECTION_TITLE_SPACING,
        .gameOutlinePadding = GAME_OUTLINE_PADDING
    };

    GameListLayoutConfig fullWidthListConfig = {
        .paddingLeft = FULL_WIDTH_LIST_PADDING_LEFT,
        .paddingRight = PADDING_RIGHT,
        .paddingTop = PADDING_TOP,
        .paddingBottom = PADDING_BOTTOM,
        .sectionTitleSpacing = SECTION_TITLE_SPACING,
        .gameOutlinePadding = GAME_OUTLINE_PADDING
    };
    // Initialize all game lists
    gameLists.resize(NAVIGATION_ORDER.size());
    for (size_t i = 0; i < NAVIGATION_ORDER.size(); i++) {
        const auto& info = NAVIGATION_ORDER[i];
        IGameList::Ref list;

        switch (info.type) {
            case GameListType::Console:
                list = ConsoleGameList::New(x, y, width, height, consoleListConfig, consoleGameListManager);
                list->SetName("Console Game List Element");
                break;
            case GameListType::Custom:
                list = CustomGameList::New(x, y, width, height, fullWidthListConfig, customGameListManager);
                list->SetName("Custom Game List Element");
                break;
            case GameListType::Emulator:
                list = EmulatorGameList::New(x, y, width, height, fullWidthListConfig, emulatorGameListManager);
                list->SetName("Emulator Game List Element");
                break;
        }

        list->EstablishOwningRelationship();
        gameLists[i] = list;
    }

    // Set active game list to first in navigation order
    activeGameList = gameLists[currentGameListIndex];
    activeGameListType = NAVIGATION_ORDER[currentGameListIndex].type;

    // Initialize container with proper dimensions
    container = pu::ui::Container::New(x, y, GetWidth(), GetHeight());

    // Create trigger buttons
    CreateTriggerButtons();
    container->Add(leftTrigger);
    container->Add(rightTrigger);

    // Create background pattern
    background = ui::render::PatternBackground::New(
        x,
        y,
        GetWidth(),
        GetHeight() - 1,  // -1 to account for bottom of the divider
        CORNER_RADIUS,
        backgroundColor
    );
    container->Add(background);

    // Add all game lists to container (only active one will be visible)
    for (auto& list : gameLists) {
        container->Add(list);
        list->SetVisible(false);
    }
    activeGameList->SetVisible(true);

    // Let container prepare elements
    container->PreRender();

    // Set up callbacks for active game list
    SetupGameListCallbacks(activeGameList);

    // Set up input handler
    inputHandler.SetOnMoveUp([this]() {
        if (activeGameList->IsFocused() && activeGameList->ShouldResignUpFocus()) {
            // Use the horizontal position to determine which trigger is closer
            float position = activeGameList->GetSelectionHorizontalPosition();
            if (position > 0.5f) {
                rightTrigger->RequestFocus();
            } else {
                leftTrigger->RequestFocus();
            }
        }
    });

    inputHandler.SetOnMoveRight([this]() {
        if (leftTrigger->IsFocused()) {
            rightTrigger->RequestFocus();
        }
    });

    inputHandler.SetOnMoveLeft([this]() {
        if (rightTrigger->IsFocused()) {
            leftTrigger->RequestFocus();
        }
    });

    inputHandler.SetOnMoveDown([this]() {
        if (rightTrigger->IsFocused() || leftTrigger->IsFocused()) {
            activeGameList->RequestFocus();
        }
    });

    SetFocusManager(parentFocusManager);

    // Set initial data for all game lists
    UpdateGameListData();

    LOG_DEBUG("GameList component initialization complete");
}

void pksm::ui::GameList::CreateTriggerButtons() {
    // Create left trigger button
    leftTrigger = ui::TriggerButton::New(
        x - TRIGGER_HORIZONTAL_OFFSET,  // Position slightly outside the component
        y - TRIGGER_VERTICAL_OFFSET,
        TRIGGER_BUTTON_WIDTH,
        TRIGGER_BUTTON_HEIGHT,
        CORNER_RADIUS,
        ui::TriggerButton::Side::Left,
        TRIGGER_BUTTON_COLOR
    );
    leftTrigger->SetName("LeftTrigger Button Element");
    leftTrigger->SetNavigationText("Custom");
    leftTrigger->SetOnTouchSelect([this]() {
        LOG_DEBUG("Left trigger button touched");
        leftTrigger->RequestFocus();
    });

    // Create right trigger button
    rightTrigger = ui::TriggerButton::New(
        x + GetWidth() - TRIGGER_BUTTON_WIDTH + TRIGGER_HORIZONTAL_OFFSET,  // Position slightly outside the component
        y - TRIGGER_VERTICAL_OFFSET,
        TRIGGER_BUTTON_WIDTH,
        TRIGGER_BUTTON_HEIGHT,
        CORNER_RADIUS,
        ui::TriggerButton::Side::Right,
        TRIGGER_BUTTON_COLOR
    );
    rightTrigger->SetName("RightTrigger Button Element");
    rightTrigger->SetNavigationText("Console");
    rightTrigger->SetOnTouchSelect([this]() {
        LOG_DEBUG("Right trigger button touched");
        rightTrigger->RequestFocus();
    });
}

pu::i32 pksm::ui::GameList::GetX() {
    return x;
}

pu::i32 pksm::ui::GameList::GetY() {
    return y;
}

pu::i32 pksm::ui::GameList::GetWidth() {
    return width;
}

pu::i32 pksm::ui::GameList::GetHeight() {
    return height;
}

void pksm::ui::GameList::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Render elements in specific order for correct layering
    leftTrigger->OnRender(drawer, leftTrigger->GetX(), leftTrigger->GetY());
    rightTrigger->OnRender(drawer, rightTrigger->GetX(), rightTrigger->GetY());
    background->OnRender(drawer, x, y);
    activeGameList->OnRender(drawer, activeGameList->GetX(), activeGameList->GetY());
}

void pksm::ui::GameList::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    if (focused) {
        inputHandler.HandleInput(keys_down, keys_held);
    }
    // Handle trigger button states
    if (keys_down & HidNpadButton_L) {
        OnTriggerButtonPressed(ui::TriggerButton::Side::Left);
    }

    if (keys_up & HidNpadButton_L) {
        OnTriggerButtonReleased(ui::TriggerButton::Side::Left);
    }

    if (keys_down & HidNpadButton_R) {
        OnTriggerButtonPressed(ui::TriggerButton::Side::Right);
    }

    if (keys_up & HidNpadButton_R) {
        OnTriggerButtonReleased(ui::TriggerButton::Side::Right);
    }

    // Forward input to console game list
    activeGameList->OnInput(keys_down, keys_up, keys_held, touch_pos);
    leftTrigger->OnInput(keys_down, keys_up, keys_held, touch_pos);
    rightTrigger->OnInput(keys_down, keys_up, keys_held, touch_pos);
}

void pksm::ui::GameList::SetFocused(bool focused) {
    if (this->focused != focused) {
        LOG_DEBUG(focused ? "GameList gained focus" : "GameList lost focus");
        this->focused = focused;
        if (!focused) {
            inputHandler.ClearState();
        }
        // Only request focus if the triggers are not focused
        if (focused && (!leftTrigger->IsFocused() && !rightTrigger->IsFocused())) {
            activeGameList->RequestFocus();
        }
    }
}

bool pksm::ui::GameList::IsFocused() const {
    return focused;
}

void pksm::ui::GameList::SetBackgroundColor(const pu::ui::Color& color) {
    backgroundColor = color;
    if (background) {
        background->SetBackgroundColor(color);
    }
}

pksm::titles::Title::Ref pksm::ui::GameList::GetSelectedTitle() const {
    return activeGameList->GetSelectedTitle();
}

void pksm::ui::GameList::SetOnSelectionChanged(std::function<void()> callback) {
    onSelectionChangedCallback = callback;
}

void pksm::ui::GameList::SetOnTouchSelect(std::function<void()> callback) {
    onTouchSelectCallback = callback;
}

void pksm::ui::GameList::SetOnGameListChanged(std::function<void()> callback) {
    onGameListChangedCallback = callback;
}

bool pksm::ui::GameList::ShouldResignDownFocus() const {
    if (leftTrigger->IsFocused() || rightTrigger->IsFocused()) {
        return false;
    }
    return activeGameList->ShouldResignDownFocus();
}

bool pksm::ui::GameList::ShouldResignUpFocus() const {
    return (leftTrigger->IsFocused() || rightTrigger->IsFocused());
}

void pksm::ui::GameList::SetFocusManager(std::shared_ptr<input::FocusManager> manager) {
    LOG_DEBUG("Setting focus manager on GameList");
    IFocusable::SetFocusManager(manager);

    // Register all child managers
    manager->RegisterChildManager(consoleGameListManager);
    manager->RegisterChildManager(customGameListManager);
    manager->RegisterChildManager(emulatorGameListManager);

    manager->RegisterFocusable(leftTrigger);
    manager->RegisterFocusable(rightTrigger);
}

void pksm::ui::GameList::OnTriggerButtonPressed(ui::TriggerButton::Side side) {
    switch (side) {
        case ui::TriggerButton::Side::Left:
            leftTrigger->SetBackgroundColor(TRIGGER_BUTTON_COLOR_PRESSED);
            LOG_DEBUG("Left trigger button pressed");
            break;
        case ui::TriggerButton::Side::Right:
            rightTrigger->SetBackgroundColor(TRIGGER_BUTTON_COLOR_PRESSED);
            LOG_DEBUG("Right trigger button pressed");
            break;
    }
}

void pksm::ui::GameList::OnTriggerButtonReleased(ui::TriggerButton::Side side) {
    switch (side) {
        case ui::TriggerButton::Side::Left:
            leftTrigger->SetBackgroundColor(TRIGGER_BUTTON_COLOR);
            LOG_DEBUG("Left trigger button released");
            SwitchToNextGameList(false);  // Move backwards
            break;
        case ui::TriggerButton::Side::Right:
            rightTrigger->SetBackgroundColor(TRIGGER_BUTTON_COLOR);
            LOG_DEBUG("Right trigger button released");
            SwitchToNextGameList(true);  // Move forwards
            break;
    }
}

void pksm::ui::GameList::SwitchToNextGameList(bool forward) {
    // Hide current game list
    activeGameList->SetVisible(false);

    // Calculate next index
    size_t nextIndex;
    if (forward) {
        nextIndex = (currentGameListIndex + 1) % NAVIGATION_ORDER.size();
    } else {
        nextIndex = (currentGameListIndex == 0) ? NAVIGATION_ORDER.size() - 1 : currentGameListIndex - 1;
    }

    // Update current index and active game list
    currentGameListIndex = nextIndex;
    activeGameList = gameLists[currentGameListIndex];
    activeGameListType = NAVIGATION_ORDER[currentGameListIndex].type;
    // Show new game list and request focus
    activeGameList->SetVisible(true);
    activeGameList->RequestFocus();

    // Update trigger button text
    size_t prevIndex = (currentGameListIndex == 0) ? NAVIGATION_ORDER.size() - 1 : currentGameListIndex - 1;
    nextIndex = (currentGameListIndex + 1) % NAVIGATION_ORDER.size();

    leftTrigger->SetNavigationText(NAVIGATION_ORDER[prevIndex].navigationTitle);
    rightTrigger->SetNavigationText(NAVIGATION_ORDER[nextIndex].navigationTitle);

    if (onGameListChangedCallback) {
        onGameListChangedCallback();
    }

    // Set up callbacks for new active game list
    SetupGameListCallbacks(activeGameList);
    onSelectionChangedCallback();
}

void pksm::ui::GameList::SetupGameListCallbacks(IGameList::Ref gameList) {
    gameList->SetOnSelectionChanged([this]() {
        if (onSelectionChangedCallback) {
            onSelectionChangedCallback();
        }
    });

    gameList->SetOnTouchSelect([this]() {
        if (onTouchSelectCallback) {
            onTouchSelectCallback();
        }
    });
}

void pksm::ui::GameList::UpdateGameListData() {
    // Update data for each game list type
    for (size_t i = 0; i < NAVIGATION_ORDER.size(); i++) {
        const auto& info = NAVIGATION_ORDER[i];
        auto& list = gameLists[i];

        switch (info.type) {
            case GameListType::Console: {
                std::vector<titles::Title::Ref> consoleTitles;
                // Add game card title if present
                auto gameCardTitle = titleProvider->GetGameCardTitle();
                if (gameCardTitle) {
                    consoleTitles.push_back(gameCardTitle);
                }
                // Add installed titles
                auto installedTitles = titleProvider->GetInstalledTitles();
                consoleTitles.insert(consoleTitles.end(), installedTitles.begin(), installedTitles.end());
                list->SetDataSource(consoleTitles);
                break;
            }
            case GameListType::Custom:
                list->SetDataSource(titleProvider->GetCustomTitles());
                break;
            case GameListType::Emulator:
                list->SetDataSource(titleProvider->GetEmulatorTitles());
                break;
        }
    }
}

bool pksm::ui::GameList::HandleNonDirectionalInput(const u64 keys_down, const u64 keys_up, const u64 keys_held) {
    if (leftTrigger->IsFocused()) {
        if (keys_down & HidNpadButton_A) {
            LOG_DEBUG("`A` button pressed on left trigger");
            OnTriggerButtonPressed(ui::TriggerButton::Side::Left);
            return true;
        }
        if (keys_up & HidNpadButton_A) {
            LOG_DEBUG("`A` button released on left trigger");
            OnTriggerButtonReleased(ui::TriggerButton::Side::Left);
            return true;
        }
    }
    if (rightTrigger->IsFocused()) {
        if (keys_down & HidNpadButton_A) {
            LOG_DEBUG("`A` button pressed on right trigger");
            OnTriggerButtonPressed(ui::TriggerButton::Side::Right);
            return true;
        }
        if (keys_up & HidNpadButton_A) {
            LOG_DEBUG("`A` button released on right trigger");
            OnTriggerButtonReleased(ui::TriggerButton::Side::Right);
            return true;
        }
    }
    if (keys_down & HidNpadButton_B) {
        if (leftTrigger->IsFocused() || rightTrigger->IsFocused()) {
            activeGameList->RequestFocus();
            return true;
        }
    }

    return false;
}

bool pksm::ui::GameList::IsGameListDependentOnUser() const {
    return activeGameListType == GameListType::Console;
}