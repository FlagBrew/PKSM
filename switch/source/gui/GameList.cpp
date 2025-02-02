#include "gui/GameList.hpp"
#include "gui/UIConstants.hpp"
#include <cmath>
#include "ui/render/LineRenderer.hpp"
#include "ui/render/PatternRenderer.hpp"

using namespace pksm;  // Add namespace to avoid fully qualifying every use

GameList::GameList(const pu::i32 x, const pu::i32 y)
    : Element(), selectionState(SelectionState::GameCard), focused(false),
      backgroundColor(pu::ui::Color(40, 51, 135, 255)), onSelectionChangedCallback(nullptr),
      x(x), y(y) {
    
    // Calculate key positions relative to our component's origin
    pu::i32 gameCardX = x + GAME_CARD_LEFT_PADDING;
    pu::i32 dividerX = gameCardX + GAME_CARD_SIZE + SECTION_DIVIDER_PADDING;
    pu::i32 installedStartX = dividerX + SECTION_DIVIDER_WIDTH + SECTION_DIVIDER_PADDING;
    
    // Create section headers first (we need their position)
    cartridgeText = pu::ui::elm::TextBlock::New(0, y + MARGIN_TOP, "Game Card");
    cartridgeText->SetColor(pu::ui::Color(255, 255, 255, 255));
    cartridgeText->SetFont(UIConstants::MakeMediumFontName(UIConstants::FONT_SIZE_HEADER));
    
    // Center game card text in its section
    pu::i32 gameCardTextX = gameCardX + (GAME_CARD_SIZE - cartridgeText->GetWidth()) / 2;
    cartridgeText->SetX(gameCardTextX);

    installedText = pu::ui::elm::TextBlock::New(0, y + MARGIN_TOP, "Installed Games");
    installedText->SetColor(pu::ui::Color(255, 255, 255, 255));
    installedText->SetFont(UIConstants::MakeMediumFontName(UIConstants::FONT_SIZE_HEADER));
    
    // Create section divider
    divider = pu::ui::elm::Rectangle::New(
        dividerX, y,  // Position relative to our origin
        SECTION_DIVIDER_WIDTH, GAME_CARD_SIZE + SECTION_TITLE_SPACING + MARGIN_TOP + MARGIN_BOTTOM,
        UIConstants::BACKGROUND_BLUE
    );

    // Create game grid for installed games
    installedGames = GameGrid::New(
        installedStartX,
        y,
        y + MARGIN_TOP + SECTION_TITLE_SPACING
    );

    // Center installed games text in its section
    pu::i32 installedGamesWidth = installedGames->GetWidth();
    pu::i32 installedTextX = installedStartX + (installedGamesWidth - installedText->GetWidth()) / 2;
    installedText->SetX(installedTextX);

    // Store key positions for later use
    this->gameCardX = gameCardX;
    this->installedStartX = installedStartX;

    // Set up input handler for transitions between game card and grid
    inputHandler.SetOnMoveLeft([this]() {
        if (selectionState == SelectionState::InstalledGame && installedGames->IsFirstInRow()) {
            selectionState = SelectionState::GameCard;
            installedGames->SetFocused(false);  // Remove focus from grid
            installedGames->SetSelected(false); // Remove selection from grid
            UpdateHighlights();
            HandleOnSelectionChanged();
        }
    });
    inputHandler.SetOnMoveRight([this]() {
        if (selectionState == SelectionState::GameCard && !titles.empty()) {
            selectionState = SelectionState::InstalledGame;
            installedGames->SetSelected(true);  // Select the grid item
            installedGames->SetSelectedIndex(0);  // Always select first game when moving right
            UpdateHighlights();
            HandleOnSelectionChanged();
        }
    });

    // Set up grid callbacks
    installedGames->SetOnSelectionChanged([this]() {
        HandleOnSelectionChanged();
    });
    installedGames->SetOnTouchSelect([this]() {
        if (selectionState != SelectionState::InstalledGame || !focused) {
            selectionState = SelectionState::InstalledGame;
            focused = true;
            UpdateHighlights();
            HandleOnSelectionChanged();
            if (onTouchSelectCallback) {
                onTouchSelectCallback();
            }
        }
    });

    // Create and initialize the background texture with diagonal lines
    InitializeBackgroundTexture();
}

void GameList::InitializeBackgroundTexture() {
    // Create the diagonal line pattern texture using PatternRenderer
    backgroundTexture = ui::render::PatternRenderer::CreateDiagonalLinePattern(
        GetWidth(),
        GetHeight(),
        CORNER_RADIUS,
        14,  // Line spacing
        8,   // Line thickness
        pu::ui::Color(31, 41, 139, 255)  // Line color
    );
}

pu::i32 GameList::GetX() {
    return x;
}

pu::i32 GameList::GetY() {
    return y;
}

pu::i32 GameList::GetWidth() {
    // Width from left edge to end of installed games section
    return (installedStartX + installedGames->GetWidth() + MARGIN_RIGHT) - GetX();
}

pu::i32 GameList::GetHeight() {
    // Include space for headers and extend to match divider height
    return divider->GetHeight();
}

void GameList::OnRender(pu::ui::render::Renderer::Ref &drawer, const pu::i32 x, const pu::i32 y) {
    // Draw background with rounded corners
    drawer->RenderRoundedRectangleFill(backgroundColor, x, y, GetWidth(), GetHeight() - 1, CORNER_RADIUS);

    // Draw the pre-rendered background texture
    SDL_Rect destRect = { x, y, GetWidth(), GetHeight() };
    SDL_RenderCopy(pu::ui::render::GetMainRenderer(), backgroundTexture, nullptr, &destRect);

    // Draw section headers and divider
    cartridgeText->OnRender(drawer, cartridgeText->GetX(), cartridgeText->GetY());
    installedText->OnRender(drawer, installedText->GetX(), installedText->GetY());
    divider->OnRender(drawer, divider->GetX(), divider->GetY());

    // Draw game card if present
    if (gameCardImage) {
        gameCardImage->OnRender(drawer, gameCardImage->GetX(), gameCardImage->GetY());
    }

    // Enable clipping for installed games section
    SDL_Rect clipRect = {
        static_cast<pu::i32>(installedStartX - GAME_OUTLINE_PADDING),  // Account for outline on left
        static_cast<pu::i32>(y + MARGIN_TOP + SECTION_TITLE_SPACING - GAME_OUTLINE_PADDING),  // Start at first row of games
        static_cast<pu::i32>(GetWidth() - (installedStartX - x) + (GAME_OUTLINE_PADDING * 2)),  // Width plus outline padding
        static_cast<pu::i32>(GetHeight() - (MARGIN_TOP + SECTION_TITLE_SPACING))  // Height of visible area
    };
    SDL_RenderSetClipRect(pu::ui::render::GetMainRenderer(), &clipRect);

    // Draw installed games (they will be clipped)
    installedGames->OnRender(drawer, installedGames->GetX(), installedGames->GetY());

    // Disable clipping
    SDL_RenderSetClipRect(pu::ui::render::GetMainRenderer(), nullptr);
}

void GameList::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
    // Handle directional input only when focused
    if (focused) {
        inputHandler.HandleInput(keys_down, keys_held);

        // Let the grid handle its own directional input when it's focused
        if (selectionState == SelectionState::InstalledGame) {
            installedGames->OnInput(keys_down, keys_up, keys_held, touch_pos);
        }
    }

    // Always handle touch input regardless of focus state
    if (!touch_pos.IsEmpty()) {
        // Let game card handle its own touch input
        if (gameCardImage) {
            gameCardImage->OnInput(keys_down, keys_up, keys_held, touch_pos);
        }

        // Let grid handle touch input
        installedGames->OnInput(keys_down, keys_up, keys_held, touch_pos);
    }
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

    // Clear existing game card
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
        gameCardImage->SetOnTouchSelect([this]() {
            if (selectionState != SelectionState::GameCard || !focused) {
                focused = true;
                selectionState = SelectionState::GameCard;
                UpdateHighlights();
                HandleOnSelectionChanged();
                if (onTouchSelectCallback) {
                    onTouchSelectCallback();
                }
            }
        });
    }

    // Set up installed games grid with remaining titles
    std::vector<titles::TitleRef> installedTitles;
    if (titles.size() > 1) {
        installedTitles.assign(titles.begin() + 1, titles.end());
    }
    installedGames->SetDataSource(installedTitles);

    UpdateHighlights();
}

titles::TitleRef GameList::GetSelectedTitle() const {
    if (selectionState == SelectionState::GameCard) {
        // Return game card title if it exists
        return !titles.empty() ? titles[0] : nullptr;
    } else {
        // Return selected installed title
        return installedGames->GetSelectedTitle();
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
    installedGames->SetFocused(focused && selectionState == SelectionState::InstalledGame);
    installedGames->SetSelected(selectionState == SelectionState::InstalledGame);
}

void GameList::SetOnSelectionChanged(std::function<void()> callback) {
    onSelectionChangedCallback = callback;
}

void GameList::SetOnTouchSelect(std::function<void()> callback) {
    onTouchSelectCallback = callback;
}

void GameList::HandleOnSelectionChanged() {
    if (onSelectionChangedCallback) {
        onSelectionChangedCallback();
    }
}

GameList::~GameList() {
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
    }
} 