#include "gui/screens/title-load-screen/sub-components/game-list/GameList.hpp"

#include <cmath>
#include <sstream>

#include "gui/render/LineRenderer.hpp"
#include "gui/render/PatternRenderer.hpp"
#include "gui/screens/title-load-screen/sub-components/game-list/GameListCommon.hpp"
#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"

pksm::ui::GameList::GameList(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    input::FocusManager::Ref parentFocusManager
)
  : Element(),
    focused(false),
    backgroundColor(pu::ui::Color(40, 51, 135, 255)),
    onSelectionChangedCallback(nullptr),
    x(x),
    y(y),
    width(width),
    height(height) {
    LOG_DEBUG("Initializing GameList component...");

    // Initialize focus manager for console game list
    consoleGameListManager = input::FocusManager::New("ConsoleGameList Manager");

    // Create console game list first since other components depend on its dimensions
    GameListLayoutConfig config = {
        .paddingLeft = PADDING_LEFT,
        .paddingRight = PADDING_RIGHT,
        .paddingTop = PADDING_TOP,
        .paddingBottom = PADDING_BOTTOM,
        .sectionTitleSpacing = SECTION_TITLE_SPACING,
        .gameOutlinePadding = GAME_OUTLINE_PADDING
    };
    consoleGameList = ConsoleGameList::New(x, y, width, height, config, consoleGameListManager);
    consoleGameList->SetName("Console Game List Element");
    consoleGameList->EstablishOwningRelationship();
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

    // Add console game list last (it should render on top)
    container->Add(consoleGameList);

    // Let container prepare elements
    container->PreRender();

    // Set up callbacks
    consoleGameList->SetOnSelectionChanged([this]() {
        if (onSelectionChangedCallback) {
            onSelectionChangedCallback();
        }
    });
    consoleGameList->SetOnTouchSelect([this]() {
        if (onTouchSelectCallback) {
            onTouchSelectCallback();
        }
    });

    // Set up input handler
    inputHandler.SetOnMoveUp([this]() {
        if (consoleGameList->IsFocused() && consoleGameList->ShouldResignUpFocus()) {
            // Use the horizontal position to determine which trigger is closer
            float position = consoleGameList->GetSelectionHorizontalPosition();
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
            consoleGameList->RequestFocus();
        }
    });

    SetFocusManager(parentFocusManager);

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
    leftTrigger->SetNavigationText("Emulator");
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
    rightTrigger->SetNavigationText("Custom");
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
    consoleGameList->OnRender(drawer, consoleGameList->GetX(), consoleGameList->GetY());
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
    consoleGameList->OnInput(keys_down, keys_up, keys_held, touch_pos);
    leftTrigger->OnInput(keys_down, keys_up, keys_held, touch_pos);
    rightTrigger->OnInput(keys_down, keys_up, keys_held, touch_pos);
}

void pksm::ui::GameList::SetFocused(bool focused) {
    if (this->focused != focused) {
        LOG_DEBUG(focused ? "GameList gained focus" : "GameList lost focus");
        this->focused = focused;
        // Only request focus if the triggers are not focused
        if (focused && (!leftTrigger->IsFocused() && !rightTrigger->IsFocused())) {
            consoleGameList->RequestFocus();
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

void pksm::ui::GameList::SetDataSource(const std::vector<titles::Title::Ref>& titles) {
    LOG_DEBUG("Setting GameList data source with " + std::to_string(titles.size()) + " titles");
    this->titles = titles;
    consoleGameList->SetDataSource(titles);
}

pksm::titles::Title::Ref pksm::ui::GameList::GetSelectedTitle() const {
    return consoleGameList->GetSelectedTitle();
}

void pksm::ui::GameList::SetOnSelectionChanged(std::function<void()> callback) {
    onSelectionChangedCallback = callback;
}

void pksm::ui::GameList::SetOnTouchSelect(std::function<void()> callback) {
    onTouchSelectCallback = callback;
}

bool pksm::ui::GameList::ShouldResignDownFocus() const {
    if (leftTrigger->IsFocused() || rightTrigger->IsFocused()) {
        return false;
    }
    return consoleGameList->ShouldResignDownFocus();
}

void pksm::ui::GameList::SetFocusManager(std::shared_ptr<input::FocusManager> manager) {
    LOG_DEBUG("Setting focus manager on GameList");
    IFocusable::SetFocusManager(manager);

    manager->RegisterChildManager(consoleGameListManager);
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
            break;
        case ui::TriggerButton::Side::Right:
            rightTrigger->SetBackgroundColor(TRIGGER_BUTTON_COLOR);
            LOG_DEBUG("Right trigger button released");
            break;
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
            consoleGameList->RequestFocus();
            return true;
        }
    }

    return false;
}