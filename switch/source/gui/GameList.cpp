#include "gui/GameList.hpp"
#include "gui/UIConstants.hpp"
#include <cmath>
#include <sstream>
#include "ui/render/LineRenderer.hpp"
#include "ui/render/PatternRenderer.hpp"
#include "utils/Logger.hpp"

pksm::GameList::GameList(const pu::i32 x, const pu::i32 y)
    : Element(), focused(false),
      backgroundColor(pu::ui::Color(40, 51, 135, 255)), onSelectionChangedCallback(nullptr),
      x(x), y(y) {
    
    LOG_DEBUG("Initializing GameList component...");

    // Create console game list
    ConsoleGameList::LayoutConfig config = {
        .marginLeft = MARGIN_LEFT,
        .marginRight = MARGIN_RIGHT,
        .marginTop = MARGIN_TOP,
        .marginBottom = MARGIN_BOTTOM,
        .sectionTitleSpacing = SECTION_TITLE_SPACING,
        .gameOutlinePadding = GAME_OUTLINE_PADDING
    };
    consoleGameList = ConsoleGameList::New(x, y, config);

    // Create background pattern
    background = ui::render::PatternBackground::New(
        x, 
        y,
        GetWidth(), 
        GetHeight() - 1, // -1 to account for bottom of the divider
        CORNER_RADIUS,
        backgroundColor
    );

    // Create trigger buttons
    CreateTriggerButtons();

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

    LOG_DEBUG("GameList component initialization complete");
}

void pksm::GameList::CreateTriggerButtons() {
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
}

pu::i32 pksm::GameList::GetX() {
    return x;
}

pu::i32 pksm::GameList::GetY() {
    return y;
}

pu::i32 pksm::GameList::GetWidth() {
    return consoleGameList->GetWidth();
}

pu::i32 pksm::GameList::GetHeight() {
    return consoleGameList->GetHeight();
}

void pksm::GameList::OnRender(pu::ui::render::Renderer::Ref &drawer, const pu::i32 x, const pu::i32 y) {
    // Draw trigger buttons
    leftTrigger->OnRender(drawer, leftTrigger->GetX(), leftTrigger->GetY());
    rightTrigger->OnRender(drawer, rightTrigger->GetX(), rightTrigger->GetY());

    // Draw background pattern
    if (background) {
        background->OnRender(drawer, x, y);
    }

    // Draw console game list
    consoleGameList->OnRender(drawer, consoleGameList->GetX(), consoleGameList->GetY());
}

void pksm::GameList::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
    // Forward input to console game list
    consoleGameList->OnInput(keys_down, keys_up, keys_held, touch_pos);

    // Handle trigger button states
    if (keys_down & HidNpadButton_L) {
        LOG_DEBUG("Left trigger button pressed");
        leftTrigger->SetBackgroundColor(TRIGGER_BUTTON_COLOR_PRESSED);
    }

    if (keys_up & HidNpadButton_L) {
        LOG_DEBUG("Left trigger button released");
        leftTrigger->SetBackgroundColor(TRIGGER_BUTTON_COLOR);
    }

    if (keys_down & HidNpadButton_R) {
        LOG_DEBUG("Right trigger button pressed");
        rightTrigger->SetBackgroundColor(TRIGGER_BUTTON_COLOR_PRESSED);
    }

    if (keys_up & HidNpadButton_R) {
        LOG_DEBUG("Right trigger button released");
        rightTrigger->SetBackgroundColor(TRIGGER_BUTTON_COLOR);
    }
}

void pksm::GameList::SetFocused(bool focused) {
    if (this->focused != focused) {
        LOG_DEBUG(focused ? "GameList gained focus" : "GameList lost focus");
        this->focused = focused;
        consoleGameList->SetFocused(focused);
    }
}

bool pksm::GameList::IsFocused() const {
    return focused;
}

void pksm::GameList::SetBackgroundColor(const pu::ui::Color& color) {
    backgroundColor = color;
    if (background) {
        background->SetBackgroundColor(color);
    }
}

void pksm::GameList::SetDataSource(const std::vector<titles::TitleRef>& titles) {
    LOG_DEBUG("Setting GameList data source with " + std::to_string(titles.size()) + " titles");
    this->titles = titles;
    consoleGameList->SetDataSource(titles);
}

titles::TitleRef pksm::GameList::GetSelectedTitle() const {
    return consoleGameList->GetSelectedTitle();
}

void pksm::GameList::SetOnSelectionChanged(std::function<void()> callback) {
    onSelectionChangedCallback = callback;
}

void pksm::GameList::SetOnTouchSelect(std::function<void()> callback) {
    onTouchSelectCallback = callback;
}

pksm::GameList::~GameList() = default; 