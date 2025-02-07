#include "gui/GameGrid.hpp"
#include "gui/UIConstants.hpp"
#include <cmath>

GameGrid::GameGrid(const pu::i32 x, const pu::i32 y)
    : Element(), selectedIndex(0), focused(false), selected(false),
      x(x), y(y) {
    
    // Initialize ScrollView
    scrollView = std::make_unique<ScrollView>(
        x - GAME_OUTLINE_PADDING, 
        y - GAME_OUTLINE_PADDING, 
        GAME_SPACING * (ITEMS_PER_ROW - 1) + INSTALLED_GAME_SIZE + (GAME_OUTLINE_PADDING * 2), 
        0
    );
    
    // Set up input handler
    inputHandler.SetOnMoveLeft([this]() { MoveLeft(); });
    inputHandler.SetOnMoveRight([this]() { MoveRight(); });
    inputHandler.SetOnMoveUp([this]() { MoveUp(); });
    inputHandler.SetOnMoveDown([this]() { MoveDown(); });
}

pu::i32 GameGrid::GetX() {
    return x;
}

pu::i32 GameGrid::GetY() {
    return y;
}

pu::i32 GameGrid::GetWidth() {
    // Width is the space needed for all items in a row
    return GAME_SPACING * (ITEMS_PER_ROW - 1) + INSTALLED_GAME_SIZE;
}

pu::i32 GameGrid::GetHeight() {
    // Use ScrollView's height if set
    return scrollView->GetHeight();
}

void GameGrid::OnRender(pu::ui::render::Renderer::Ref &drawer, const pu::i32 x, const pu::i32 y) {
    scrollView->OnRender(drawer, x, y);
}

void GameGrid::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
    // Handle directional input only when focused
    if (focused) {
        inputHandler.HandleInput(keys_down, keys_held);
    }
    
    // Let ScrollView handle all touch input
    scrollView->OnInput(keys_down, keys_up, keys_held, touch_pos);
}

void GameGrid::SetFocused(bool focused) {
    if (this->focused != focused) {
        this->focused = focused;
        this->scrollView->SetFocused(focused);
        UpdateHighlights();
    }
}

bool GameGrid::IsFocused() const {
    return focused;
}

void GameGrid::SetDataSource(const std::vector<titles::TitleRef>& titles) {
    // Store titles
    this->titles = titles;

    // Clear existing images
    gameImages.clear();

    // Create game images for all titles
    for (size_t i = 0; i < titles.size(); i++) {
        // Calculate grid position
        size_t row = i / ITEMS_PER_ROW;  // Integer division for row number
        size_t col = i % ITEMS_PER_ROW;  // Modulo for column number

        auto gameImage = FocusableImage::New(
            GetX() + (col * GAME_SPACING),
            y + (row * (INSTALLED_GAME_SIZE + ROW_SPACING)),
            titles[i]->getIcon(),
            94,
            GAME_OUTLINE_PADDING
        );
        gameImage->SetWidth(INSTALLED_GAME_SIZE);
        gameImage->SetHeight(INSTALLED_GAME_SIZE);
        
        // Set up touch handling for this game
        const size_t index = i;  // Store index for lambda capture
        gameImage->SetOnTouchSelect([this, index]() {
            SetSelectedIndex(index);
            if (!focused && onTouchSelectCallback) {
                this->SetFocused(true);
                this->SetSelected(true);  // Ensure selected state is set
                UpdateHighlights();  // Update highlights after changing states
                onTouchSelectCallback();
            }
        });
        
        gameImages.push_back(gameImage);
        scrollView->Add(gameImage);
    }

    // Calculate total content height including padding for outline and row spacing
    if (!gameImages.empty()) {
        auto lastGame = gameImages.back();
        auto firstGame = gameImages.front();
        // Include outline padding for both top and bottom of content area
        pu::i32 contentHeight = (lastGame->GetY() + lastGame->GetHeight() + GAME_OUTLINE_PADDING) - 
                               (firstGame->GetY() - GAME_OUTLINE_PADDING);
        scrollView->SetContentHeight(contentHeight);
    } else {
        scrollView->SetContentHeight(0);
    }

    UpdateHighlights();
}

titles::TitleRef GameGrid::GetSelectedTitle() const {
    if (selectedIndex < titles.size()) {
        return titles[selectedIndex];
    }
    return nullptr;
}

size_t GameGrid::GetSelectedIndex() const {
    return selectedIndex;
}

void GameGrid::MoveLeft() {
    SetSelectedIndex(selectedIndex - 1);
}

void GameGrid::MoveRight() {
    SetSelectedIndex(selectedIndex + 1);
}

void GameGrid::MoveUp() {
    if (IsInTopRow()) {
        if (onTopRowCallback) {
            onTopRowCallback();
        }
    } else {
        SetSelectedIndex(selectedIndex - ITEMS_PER_ROW);
    }
}

void GameGrid::MoveDown() {
    if (IsInBottomRow()) {
        if (onBottomRowCallback) {
            onBottomRowCallback();
        }
    } else {
        SetSelectedIndex(selectedIndex + ITEMS_PER_ROW);
    }
}

bool GameGrid::IsInTopRow() const {
    return selectedIndex < ITEMS_PER_ROW;
}

bool GameGrid::IsInBottomRow() const {
    size_t nextIndex = selectedIndex + ITEMS_PER_ROW;
    return nextIndex >= gameImages.size();
}

void GameGrid::UpdateHighlights() {
    for (size_t i = 0; i < gameImages.size(); i++) {
        bool isSelected = selected && selectedIndex == i;
        gameImages[i]->SetSelected(isSelected);
        gameImages[i]->SetFocused(focused && isSelected);
    }
}

void GameGrid::SetSelected(bool selected) {
    if (this->selected != selected) {
        this->selected = selected;
        UpdateHighlights();
    }
}

bool GameGrid::IsSelected() const {
    return selected;
}

void GameGrid::SetOnSelectionChanged(std::function<void()> callback) {
    onSelectionChangedCallback = callback;
}

void GameGrid::SetOnTouchSelect(std::function<void()> callback) {
    onTouchSelectCallback = callback;
}

void GameGrid::SetOnTopRow(std::function<void()> callback) {
    onTopRowCallback = callback;
}

void GameGrid::SetOnBottomRow(std::function<void()> callback) {
    onBottomRowCallback = callback;
}

void GameGrid::HandleOnSelectionChanged() {
    if (onSelectionChangedCallback) {
        onSelectionChangedCallback();
    }
}

void GameGrid::SetSelectedIndex(size_t index) {
    if (index >= 0 && index < gameImages.size() && selectedIndex != index) {
        selectedIndex = index;
        EnsureRowVisible(index / ITEMS_PER_ROW);
        UpdateHighlights();
        HandleOnSelectionChanged();
    }
}

void GameGrid::EnsureRowVisible(size_t row) {
    if (gameImages.empty()) return;

    // Calculate row boundaries including outline padding
    pu::i32 rowTop = 0 + (row * (INSTALLED_GAME_SIZE + ROW_SPACING)) - GAME_OUTLINE_PADDING;
    
    // Scroll to make the row visible
    scrollView->ScrollToOffset(rowTop, true);
}

void GameGrid::SetAvailableHeight(pu::i32 height) {
    scrollView->SetHeight(height);
} 