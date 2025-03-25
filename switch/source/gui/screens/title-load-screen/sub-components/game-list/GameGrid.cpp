#include "gui/screens/title-load-screen/sub-components/game-list/GameGrid.hpp"

#include <cmath>

#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"

pksm::ui::GameGrid::GameGrid(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 height,
    const size_t itemsPerRow,
    input::FocusManager::Ref parentFocusManager,
    input::SelectionManager::Ref parentSelectionManager,
    const std::map<ShakeDirection, bool> shouldConsiderSideOutOfBounds
)
  : pu::ui::elm::Element(),
    ISelectable(),
    IGrid(itemsPerRow, shouldConsiderSideOutOfBounds),
    focused(false),
    selected(false),
    x(x),
    y(y),
    height(height) {
    // Initialize container
    container = pu::ui::Container::New(x, y, GetWidth(), GetHeight());

    // Initialize ScrollView
    scrollView = ScrollView::New(
        x - (GAME_OUTLINE_PADDING + 20),
        y - GAME_OUTLINE_PADDING,
        GetWidth() + (GAME_OUTLINE_PADDING * 2) +
            (20 * 2),  // extra 20 to account for out of bounds shake animation to the
                       // side (extra viewport to the side does not really matter)
        height
    );
    container->Add(scrollView);
    container->PreRender();

    // Set up input handler
    inputHandler.SetOnMoveLeft([this]() { IGrid::MoveLeft(); });
    inputHandler.SetOnMoveRight([this]() { IGrid::MoveRight(); });
    inputHandler.SetOnMoveUp([this]() { IGrid::MoveUp(); });
    inputHandler.SetOnMoveDown([this]() { IGrid::MoveDown(); });

    SetFocusManager(parentFocusManager);
    SetSelectionManager(parentSelectionManager);

    LOG_DEBUG("GameGrid component initialization complete");
}

void pksm::ui::GameGrid::SetSelectedIndex(size_t index) {
    if (index >= 0 && index < gameImages.size() && selectedIndex != index) {
        selectedIndex = index;
        EnsureRowVisible(index / itemsPerRow);

        // Request focus on the newly selected game
        if (focused) {
            gameImages[selectedIndex]->RequestFocus();
        }

        HandleOnSelectionChanged();
    }
}

// Element implementation methods
pu::i32 pksm::ui::GameGrid::GetX() {
    return x;
}

pu::i32 pksm::ui::GameGrid::GetY() {
    return y;
}

void pksm::ui::GameGrid::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    scrollView->OnRender(drawer, x, y);
}

void pksm::ui::GameGrid::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    // Handle directional input only when focused
    if (focused) {
        inputHandler.HandleInput(keys_down, keys_held);
    }

    // Let ScrollView handle all touch input
    scrollView->OnInput(keys_down, keys_up, keys_held, touch_pos);

    // Let the selected game handle its own non-touch input (since scrollview handles touch of its children)
    if (touch_pos.IsEmpty()) {
        gameImages[selectedIndex]->OnInput(keys_down, keys_up, keys_held, touch_pos);
    }
}

void pksm::ui::GameGrid::SetFocused(bool focused) {
    LOG_DEBUG("[GameGrid] Setting focused: " + std::to_string(focused));
    if (this->focused != focused) {
        this->focused = focused;
        this->scrollView->SetFocused(focused);
        if (!focused) {
            inputHandler.ClearState();
        }
        // When gaining focus, ensure the selected game is focused
        if (focused && selectedIndex < gameImages.size()) {
            gameImages[selectedIndex]->RequestFocus();
        }
    }
}

bool pksm::ui::GameGrid::IsFocused() const {
    return focused;
}

void pksm::ui::GameGrid::SetDataSource(const std::vector<titles::Title::Ref>& titles) {
    // Store titles
    this->titles = titles;

    // Clear existing images and unregister them from focus manager
    for (auto& image : gameImages) {
        if (auto fm = this->focusManager.lock()) {
            fm->UnregisterFocusable(image);
        }
        if (auto sm = this->selectionManager.lock()) {
            sm->UnregisterSelectable(image);
        }
    }
    gameImages.clear();
    scrollView->Clear();  // Clear all elements from the scrollView

    // Create game images for all titles
    for (size_t i = 0; i < titles.size(); i++) {
        // Calculate position using IGrid's helper method
        auto position = CalculateItemPosition(i);

        auto gameImage =
            FocusableImage::New(position.first, position.second, titles[i]->getIcon(), 94, GAME_OUTLINE_PADDING);
        gameImage->IFocusable::SetName("GameImage Element with title: " + titles[i]->getName());
        gameImage->ISelectable::SetName("GameImage Element with title: " + titles[i]->getName());
        gameImage->SetWidth(GetItemWidth());
        gameImage->SetHeight(GetItemHeight());

        // Set up touch handling for this game
        const size_t index = i;  // Store index for lambda capture
        gameImage->SetOnTouchSelect([this, index]() {
            SetSelectedIndex(index);
            if (onTouchSelectCallback) {
                onTouchSelectCallback();
            }
        });
        gameImage->SetOnSelect([this, index]() {
            if (onSelectCallback) {
                onSelectCallback();
            }
        });

        if (auto fm = this->focusManager.lock()) {
            fm->RegisterFocusable(gameImage);
        }
        if (auto sm = this->selectionManager.lock()) {
            sm->RegisterSelectable(gameImage);
        }
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

    if (gameImages.size() > selectedIndex && IsFocused()) {
        gameImages[selectedIndex]->RequestFocus();
    }
}

pksm::titles::Title::Ref pksm::ui::GameGrid::GetSelectedTitle() const {
    if (selectedIndex < titles.size()) {
        return titles[selectedIndex];
    }
    return nullptr;
}

size_t pksm::ui::GameGrid::GetSelectedIndex() const {
    return selectedIndex;
}

bool pksm::ui::GameGrid::IsSelected() const {
    return selected;
}

void pksm::ui::GameGrid::SetSelected(bool selected) {
    this->selected = selected;
}

void pksm::ui::GameGrid::HandleOnSelectionChanged() {
    if (onSelectionChangedCallback) {
        onSelectionChangedCallback();
    }
}

void pksm::ui::GameGrid::EnsureRowVisible(size_t row) {
    if (gameImages.empty())
        return;

    // Calculate row boundaries including outline padding
    pu::i32 rowTop = 0 + (row * (GetItemHeight() + GetVerticalSpacing())) - GAME_OUTLINE_PADDING;

    // Scroll to make the row visible
    scrollView->ScrollToOffset(rowTop, true);
}