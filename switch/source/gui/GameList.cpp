#include "gui/GameList.hpp"

GameList::GameList(const pu::i32 x, const pu::i32 y)
    : Element(), selectionState(SelectionState::GameCard), selectedGameIndex(0), focused(false),
      backgroundColor(pu::ui::Color(75, 75, 75, 255)), onSelectionChangedCallback(nullptr),
      x(x), y(y) {
    
    // Calculate key positions relative to our component's origin
    pu::i32 gameCardX = x + GAME_CARD_LEFT_PADDING;
    pu::i32 dividerX = gameCardX + GAME_CARD_SIZE + SECTION_DIVIDER_PADDING;
    pu::i32 installedStartX = dividerX + SECTION_DIVIDER_WIDTH + SECTION_DIVIDER_PADDING;
    
    // Create section headers first (we need their position)
    cartridgeText = pu::ui::elm::TextBlock::New(0, y + MARGIN_TOP, "Game Card");
    cartridgeText->SetColor(pu::ui::Color(255, 255, 255, 255));
    
    // Center game card text in its section
    pu::i32 gameCardTextX = gameCardX + (GAME_CARD_SIZE - cartridgeText->GetWidth()) / 2;
    cartridgeText->SetX(gameCardTextX);

    installedText = pu::ui::elm::TextBlock::New(0, y + MARGIN_TOP, "Installed Games");
    installedText->SetColor(pu::ui::Color(255, 255, 255, 255));
    
    // Center installed games text in its section
    pu::i32 installedGamesWidth = GAME_SPACING * (INSTALLED_GAME_ITEMS_PER_ROW - 1) + INSTALLED_GAME_SIZE;
    pu::i32 installedTextX = installedStartX + (installedGamesWidth - installedText->GetWidth()) / 2;
    installedText->SetX(installedTextX);

    // Create section divider
    divider = pu::ui::elm::Rectangle::New(
        dividerX, y,  // Position relative to our origin
        SECTION_DIVIDER_WIDTH, GAME_CARD_SIZE + SECTION_TITLE_SPACING + MARGIN_TOP + MARGIN_BOTTOM,
        SECTION_DIVIDER_COLOR
    );

    // Store key positions for later use in SetDataSource
    this->gameCardX = gameCardX;
    this->installedStartX = installedStartX;

    // Set up input handler
    inputHandler.SetOnMoveLeft([this]() { MoveLeft(); });
    inputHandler.SetOnMoveRight([this]() { MoveRight(); });
}

pu::i32 GameList::GetX() {
    return x;
}

pu::i32 GameList::GetY() {
    return y;
}

pu::i32 GameList::GetWidth() {
    // Width from left edge to last possible installed game
    pu::i32 totalInstalledWidth = GAME_SPACING * (INSTALLED_GAME_ITEMS_PER_ROW - 1) + INSTALLED_GAME_SIZE;
    return (installedStartX + totalInstalledWidth + MARGIN_RIGHT) - GetX();
}

pu::i32 GameList::GetHeight() {
    // Include space for headers and extend to match divider height
    return divider->GetHeight();
}

void GameList::OnRender(pu::ui::render::Renderer::Ref &drawer, const pu::i32 x, const pu::i32 y) {
    // Draw background
    drawer->RenderRectangleFill(backgroundColor, x, y, GetWidth(), GetHeight());

    // Draw section headers and divider
    cartridgeText->OnRender(drawer, cartridgeText->GetX(), cartridgeText->GetY());
    installedText->OnRender(drawer, installedText->GetX(), installedText->GetY());
    divider->OnRender(drawer, divider->GetX(), divider->GetY());

    // Draw game card if present
    if (gameCardImage) {
        gameCardImage->OnRender(drawer, gameCardImage->GetX(), gameCardImage->GetY());
    }

    // Draw installed games
    for (auto& image : installedGameImages) {
        image->OnRender(drawer, image->GetX(), image->GetY());
    }
}

void GameList::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
    if (!focused) return;
    inputHandler.HandleInput(keys_down, keys_held);
}

void GameList::SetFocused(bool focused) {
    if (this->focused != focused) {
        this->focused = focused;
        UpdateHighlights();
    }
}

bool GameList::IsFocused() const {
    return focused;
}

void GameList::SetBackgroundColor(const pu::ui::Color& color) {
    backgroundColor = color;
}

void GameList::SetDataSource(const std::vector<titles::TitleRef>& titles) {
    // Store titles
    this->titles = titles;

    // Clear existing images
    installedGameImages.clear();
    gameCardImage = nullptr;

    // Create game card image if first title exists
    if (!titles.empty()) {
        gameCardImage = FocusableImage::New(
            gameCardX,
            cartridgeText->GetY() + SECTION_TITLE_SPACING,
            titles[0]->getIcon(),
            94,
            GAME_OUTLINE_PADDING
        );
        gameCardImage->SetWidth(GAME_CARD_SIZE);
        gameCardImage->SetHeight(GAME_CARD_SIZE);
    }

    // Create installed game images for remaining titles
    for (size_t i = 1; i < titles.size(); i++) {
        auto gameImage = FocusableImage::New(
            installedStartX + ((i-1) * GAME_SPACING),
            cartridgeText->GetY() + SECTION_TITLE_SPACING,
            titles[i]->getIcon(),
            94,
            GAME_OUTLINE_PADDING
        );
        gameImage->SetWidth(INSTALLED_GAME_SIZE);
        gameImage->SetHeight(INSTALLED_GAME_SIZE);
        installedGameImages.push_back(gameImage);
    }

    UpdateHighlights();
}

titles::TitleRef GameList::GetSelectedTitle() const {
    if (selectionState == SelectionState::GameCard) {
        // Return game card title if it exists
        return !titles.empty() ? titles[0] : nullptr;
    } else if (selectionState == SelectionState::InstalledGame && selectedGameIndex < installedGameImages.size()) {
        // Return selected installed title
        return titles[selectedGameIndex + 1];  // +1 because first title is game card
    }
    return nullptr;
}

void GameList::MoveLeft() {
    auto prevState = selectionState;
    auto prevIndex = selectedGameIndex;

    if (selectionState == SelectionState::InstalledGame && selectedGameIndex > 0) {
        selectedGameIndex--;
    } else if (selectionState == SelectionState::InstalledGame && selectedGameIndex == 0) {
        selectionState = SelectionState::GameCard;
    }

    if (prevState != selectionState || prevIndex != selectedGameIndex) {
        UpdateHighlights();
        HandleOnSelectionChanged();
    }
}

void GameList::MoveRight() {
    auto prevState = selectionState;
    auto prevIndex = selectedGameIndex;

    if (selectionState == SelectionState::GameCard && !installedGameImages.empty()) {
        selectionState = SelectionState::InstalledGame;
        selectedGameIndex = 0;
    } else if (selectionState == SelectionState::InstalledGame && selectedGameIndex < installedGameImages.size() - 1) {
        selectedGameIndex++;
    }

    if (prevState != selectionState || prevIndex != selectedGameIndex) {
        UpdateHighlights();
        HandleOnSelectionChanged();
    }
}

void GameList::UpdateHighlights() {
    // Update game card highlight
    if (gameCardImage) {
        bool isSelected = selectionState == SelectionState::GameCard;
        gameCardImage->SetSelected(isSelected);
        gameCardImage->SetFocused(focused && isSelected);
    }

    // Update installed games highlight
    for (size_t i = 0; i < installedGameImages.size(); i++) {
        bool isSelected = selectionState == SelectionState::InstalledGame && selectedGameIndex == i;
        installedGameImages[i]->SetSelected(isSelected);
        installedGameImages[i]->SetFocused(focused && isSelected);
    }
}

void GameList::SetOnSelectionChanged(std::function<void()> callback) {
    onSelectionChangedCallback = callback;
}

void GameList::HandleOnSelectionChanged() {
    if (onSelectionChangedCallback) {
        onSelectionChangedCallback();
    }
} 