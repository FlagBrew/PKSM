#include "gui/GameGrid.hpp"
#include "gui/UIConstants.hpp"
#include <cmath>

GameGrid::GameGrid(const pu::i32 x, const pu::i32 y, const pu::i32 startY)
    : Element(), selectedIndex(0), focused(false), selected(false),
      x(x), y(y), startY(startY), scrollOffset(0), contentHeight(0),
      isDragging(false), touchStartedOnGame(false), touchStartY(0), lastTouchY(0),
      scrollStartOffset(0), scrollVelocity(0.0f), hasMomentum(false) {
    
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
    if (gameImages.empty()) {
        return 0;
    }

    // Find the last game's bottom edge
    auto lastGame = gameImages.back();
    return (lastGame->GetY() + lastGame->GetHeight()) - GetY();
}

void GameGrid::OnRender(pu::ui::render::Renderer::Ref &drawer, const pu::i32 x, const pu::i32 y) {
    // Update momentum scrolling if active
    if (hasMomentum) {
        UpdateScrollMomentum();
    }

    // Draw all game images with scroll offset applied
    for (auto& image : gameImages) {
        // Calculate position including outline padding
        pu::i32 imageY = image->GetY() - scrollOffset;
        
        // Update the image's actual position to keep outline in sync
        image->SetY(imageY);
        image->OnRender(drawer, image->GetX(), imageY);
        
        // Restore original position for next frame
        image->SetY(image->GetY() + scrollOffset);
    }
}

bool GameGrid::ShouldStartDragging(const pu::ui::TouchPoint& touch_pos) const {
    // Only start dragging if we've moved beyond the threshold
    return std::abs(touch_pos.y - touchStartY) > DRAG_THRESHOLD;
}

void GameGrid::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
    // Handle directional input only when focused
    if (focused) {
        inputHandler.HandleInput(keys_down, keys_held);
    }
    
    // Handle touch input
    if (!touch_pos.IsEmpty()) {
        // Check if touch hits any game first
        bool hitGame = false;
        for (size_t i = 0; i < gameImages.size(); i++) {
            auto& image = gameImages[i];
            if (touch_pos.HitsRegion(image->GetX(), image->GetY() - scrollOffset, image->GetWidth(), image->GetHeight())) {
                hitGame = true;
                
                if (focused) {
                    // Handle drag and selection when focused
                    if (!isDragging) {
                        // Store initial touch state
                        if (touchStartY == 0) {  // New touch
                            touchStartY = touch_pos.y;
                            lastTouchY = touch_pos.y;
                            scrollStartOffset = scrollOffset;
                            touchStartedOnGame = true;
                        } else if (ShouldStartDragging(touch_pos)) {
                            // Convert to drag if moved enough
                            isDragging = true;
                            hasMomentum = false;
                            scrollVelocity = 0.0f;
                        } else if (touch_pos.y == touchStartY) {  // Clean tap
                            // Update selection and notify parent
                            if (selectedIndex != i || !selected) {
                                selectedIndex = i;
                                selected = true;
                                EnsureRowVisible(i / ITEMS_PER_ROW);
                                UpdateHighlights();
                                HandleOnSelectionChanged();
                            }
                        }
                    }
                } else {
                    // When not focused, just forward touches to game images
                    image->OnInput(keys_down, keys_up, keys_held, touch_pos);
                }
                break;
            }
        }
        
        if (focused) {  // Only handle grid area drags when focused
            // If we didn't hit a game but hit the grid area, prepare for potential drag
            if (!hitGame && touch_pos.HitsRegion(GetX(), GetY(), GetWidth(), GetHeight())) {
                if (!isDragging && touchStartY == 0) {  // New touch
                    touchStartY = touch_pos.y;
                    lastTouchY = touch_pos.y;
                    scrollStartOffset = scrollOffset;
                    touchStartedOnGame = false;
                } else if (!isDragging && ShouldStartDragging(touch_pos)) {
                    isDragging = true;
                    hasMomentum = false;
                    scrollVelocity = 0.0f;
                }
            }

            if (isDragging) {
                // Continue drag
                pu::i32 deltaY = lastTouchY - touch_pos.y;
                scrollOffset = scrollStartOffset + (touchStartY - touch_pos.y);
                
                // Calculate velocity for momentum
                scrollVelocity = static_cast<float>(deltaY);
                
                // Update last touch position
                lastTouchY = touch_pos.y;
                
                // Ensure we stay in bounds
                ClampScrollOffset();
            }
        }
    } else {
        // Touch released
        if (isDragging) {
            isDragging = false;
            // Start momentum if we have enough velocity
            if (std::abs(scrollVelocity) > MIN_SCROLL_VELOCITY) {
                hasMomentum = true;
            }
        }
        // Reset touch tracking
        touchStartY = 0;
        touchStartedOnGame = false;
    }
}

void GameGrid::UpdateScrollMomentum() {
    // Apply velocity to scroll position
    scrollOffset += static_cast<pu::i32>(scrollVelocity);
    
    // Apply friction
    scrollVelocity *= SCROLL_FRICTION;
    
    // Stop momentum if velocity is too low
    if (std::abs(scrollVelocity) < MIN_SCROLL_VELOCITY) {
        hasMomentum = false;
        scrollVelocity = 0.0f;
    }
    
    // Ensure we stay in bounds
    ClampScrollOffset();
}

void GameGrid::ClampScrollOffset() {
    // Only add padding at the bottom where we need it for the outline
    pu::i32 maxScroll = std::max<pu::i32>(0, contentHeight + GAME_OUTLINE_PADDING - GetHeight());
    scrollOffset = std::max<pu::i32>(0, std::min<pu::i32>(scrollOffset, maxScroll));
    
    // If we hit bounds, stop momentum
    if (scrollOffset == 0 || scrollOffset == maxScroll) {
        hasMomentum = false;
        scrollVelocity = 0.0f;
    }
}

void GameGrid::SetFocused(bool focused) {
    if (this->focused != focused) {
        this->focused = focused;
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
    scrollOffset = 0;

    // Create game images for all titles
    for (size_t i = 0; i < titles.size(); i++) {
        // Calculate grid position
        size_t row = i / ITEMS_PER_ROW;  // Integer division for row number
        size_t col = i % ITEMS_PER_ROW;  // Modulo for column number

        auto gameImage = FocusableImage::New(
            GetX() + (col * GAME_SPACING),
            startY + (row * (INSTALLED_GAME_SIZE + ROW_SPACING)),
            titles[i]->getIcon(),
            94,
            GAME_OUTLINE_PADDING
        );
        gameImage->SetWidth(INSTALLED_GAME_SIZE);
        gameImage->SetHeight(INSTALLED_GAME_SIZE);
        
        // Set up touch handling for this game
        const size_t index = i;  // Store index for lambda capture
        gameImage->SetOnTouchSelect([this, index]() {
            if (selectedIndex != index) {
                selectedIndex = index;
                UpdateHighlights();
                EnsureRowVisible(index / ITEMS_PER_ROW);
                HandleOnSelectionChanged();
            }
            // Only notify parent if we weren't already focused
            if (!focused && onTouchSelectCallback) {
                focused = true;
                selected = true;  // Ensure selected state is set
                UpdateHighlights();  // Update highlights after changing states
                onTouchSelectCallback();
            }
        });
        
        gameImages.push_back(gameImage);
    }

    // Calculate total content height including padding for outline and row spacing
    if (!gameImages.empty()) {
        auto lastGame = gameImages.back();
        contentHeight = (lastGame->GetY() + lastGame->GetHeight() + GAME_OUTLINE_PADDING + ROW_SPACING) - GetY();
    } else {
        contentHeight = 0;
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
    if (selectedIndex > 0) {
        selectedIndex--;
        UpdateHighlights();
        HandleOnSelectionChanged();
    }
}

void GameGrid::MoveRight() {
    if (selectedIndex < gameImages.size() - 1) {
        selectedIndex++;
        UpdateHighlights();
        HandleOnSelectionChanged();
    }
}

void GameGrid::MoveUp() {
    if (IsInTopRow()) {
        if (onTopRowCallback) {
            onTopRowCallback();
        }
    } else if (selectedIndex >= ITEMS_PER_ROW) {
        selectedIndex -= ITEMS_PER_ROW;
        EnsureRowVisible(selectedIndex / ITEMS_PER_ROW);
        UpdateHighlights();
        HandleOnSelectionChanged();
    }
}

void GameGrid::MoveDown() {
    size_t nextIndex = selectedIndex + ITEMS_PER_ROW;
    if (IsInBottomRow()) {
        if (onBottomRowCallback) {
            onBottomRowCallback();
        }
    } else if (nextIndex < gameImages.size()) {
        selectedIndex = nextIndex;
        EnsureRowVisible(selectedIndex / ITEMS_PER_ROW);
        UpdateHighlights();
        HandleOnSelectionChanged();
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
    if (index < gameImages.size() && selectedIndex != index) {
        selectedIndex = index;
        EnsureRowVisible(index / ITEMS_PER_ROW);
        UpdateHighlights();
        HandleOnSelectionChanged();
    }
}

void GameGrid::EnsureRowVisible(size_t row) {
    if (gameImages.empty()) return;

    // Calculate row boundaries including outline padding
    pu::i32 rowTop = startY + (row * (INSTALLED_GAME_SIZE + ROW_SPACING)) - GAME_OUTLINE_PADDING;
    pu::i32 rowBottom = rowTop + INSTALLED_GAME_SIZE + (2 * GAME_OUTLINE_PADDING) + ROW_SPACING;

    // Calculate visible boundaries
    pu::i32 visibleTop = GetY();
    pu::i32 visibleBottom = visibleTop + GetHeight();

    // Adjust scroll offset to make row fully visible
    if (rowTop - scrollOffset < visibleTop) {
        // Scroll up to show row top with padding
        scrollOffset = rowTop - visibleTop;
    } else if (rowBottom - scrollOffset > visibleBottom) {
        // Scroll down to show row bottom with padding
        scrollOffset = rowBottom - visibleBottom;
    }

    // Special case: if we're on the first row, always scroll to top
    if (row == 0) {
        scrollOffset = 0;
    }

    // Special case: if we're on the last row, ensure full visibility with padding
    size_t lastRow = (gameImages.size() - 1) / ITEMS_PER_ROW;
    if (row == lastRow) {
        // Calculate the minimum scroll needed to show the last row with full padding
        pu::i32 lastRowBottom = startY + (lastRow * (INSTALLED_GAME_SIZE + ROW_SPACING)) + 
                               INSTALLED_GAME_SIZE + (2 * GAME_OUTLINE_PADDING) + ROW_SPACING;
        pu::i32 minScroll = std::max<pu::i32>(0, lastRowBottom - visibleBottom);
        scrollOffset = std::max<pu::i32>(scrollOffset, minScroll);
    }

    // Clamp scroll offset
    pu::i32 maxScroll = std::max<pu::i32>(0, contentHeight + (2 * GAME_OUTLINE_PADDING) + ROW_SPACING - GetHeight());
    scrollOffset = std::max<pu::i32>(0, std::min<pu::i32>(scrollOffset, maxScroll));
} 