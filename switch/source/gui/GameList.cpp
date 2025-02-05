#include "gui/GameList.hpp"
#include "gui/UIConstants.hpp"
#include <cmath>
#include <sstream>
#include "ui/render/LineRenderer.hpp"
#include "ui/render/PatternRenderer.hpp"
#include "utils/Logger.hpp"

pksm::GameList::GameList(const pu::i32 x, const pu::i32 y)
    : Element(), selectionState(SelectionState::GameCard), focused(false),
      backgroundColor(pu::ui::Color(40, 51, 135, 255)), onSelectionChangedCallback(nullptr),
      x(x), y(y) {
    
    LOG_DEBUG("Initializing GameList component...");
    
    // Calculate key positions relative to our component's origin
    pu::i32 gameCardX = x + MARGIN_LEFT;
    pu::i32 dividerX = gameCardX + GAME_CARD_SIZE + SECTION_DIVIDER_PADDING;
    pu::i32 installedStartX = dividerX + SECTION_DIVIDER_WIDTH + SECTION_DIVIDER_PADDING;
    
    // Create section headers first (we need their position)
    cartridgeText = pu::ui::elm::TextBlock::New(0, y + MARGIN_TOP, "Game Card");
    cartridgeText->SetColor(pu::ui::Color(255, 255, 255, 255));
    cartridgeText->SetFont(UIConstants::MakeMediumFontName(UIConstants::FONT_SIZE_HEADER));

    // Create L button text
    LButtonText = pu::ui::elm::TextBlock::New(x, y, "\ue0a4");
    LButtonText->SetColor(pu::ui::Color(255, 255, 255, 255));
    LButtonText->SetFont(UIConstants::MakeSwitchButtonFontName(UIConstants::FONT_SIZE_TITLE));
    
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

    // Create background pattern
    background = ui::render::PatternBackground::New(
        x, 
        y,
        GetWidth(), 
        GetHeight() - 1, // -1 to account for bottom of the divider
        CORNER_RADIUS,
        backgroundColor
    );

    // Set up input handler for transitions between game card and grid
    inputHandler.SetOnMoveLeft([this]() {
        if (selectionState == SelectionState::InstalledGame && installedGames->IsFirstInRow()) {
            LOG_DEBUG("Transitioning selection from installed games to game card");
            selectionState = SelectionState::GameCard;
            installedGames->SetFocused(false);  // Remove focus from grid
            installedGames->SetSelected(false); // Remove selection from grid
            UpdateHighlights();
            HandleOnSelectionChanged();
        }
    });
    inputHandler.SetOnMoveRight([this]() {
        if (selectionState == SelectionState::GameCard && !titles.empty()) {
            LOG_DEBUG("Transitioning selection from game card to installed games");
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

    LOG_DEBUG("GameList component initialization complete");
}

pu::i32 pksm::GameList::GetX() {
    return x;
}

pu::i32 pksm::GameList::GetY() {
    return y;
}

pu::i32 pksm::GameList::GetWidth() {
    // Width from left edge to end of installed games section
    return (installedStartX + installedGames->GetWidth() + MARGIN_RIGHT) - GetX();
}

pu::i32 pksm::GameList::GetHeight() {
    // Include space for headers and extend to match divider height
    return divider->GetHeight();
}

void pksm::GameList::OnRender(pu::ui::render::Renderer::Ref &drawer, const pu::i32 x, const pu::i32 y) {
    // Draw background pattern
    if (background) {
        background->OnRender(drawer, x, y);
    }

    // Draw section headers and divider
    cartridgeText->OnRender(drawer, cartridgeText->GetX(), cartridgeText->GetY());
    installedText->OnRender(drawer, installedText->GetX(), installedText->GetY());
    LButtonText->OnRender(drawer, LButtonText->GetX(), LButtonText->GetY());
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

void pksm::GameList::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
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

void pksm::GameList::SetFocused(bool focused) {
    if (this->focused != focused) {
        LOG_DEBUG(focused ? "GameList gained focus" : "GameList lost focus");
        this->focused = focused;
        UpdateHighlights();
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
    LOG_MEMORY();  // Memory check when loading new titles
    
    // Store titles
    this->titles = titles;

    // Clear existing game card
    gameCardImage = nullptr;

    // Create game card image if first title exists
    if (!titles.empty()) {
        LOG_DEBUG("Creating game card image for first title");
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
        LOG_DEBUG("Setting up installed games grid with " + std::to_string(titles.size() - 1) + " titles");
        installedTitles.assign(titles.begin() + 1, titles.end());
    }
    installedGames->SetDataSource(installedTitles);

    UpdateHighlights();
}

titles::TitleRef pksm::GameList::GetSelectedTitle() const {
    if (selectionState == SelectionState::GameCard) {
        // Return game card title if it exists
        return !titles.empty() ? titles[0] : nullptr;
    } else {
        // Return selected installed title
        return installedGames->GetSelectedTitle();
    }
}

void pksm::GameList::UpdateHighlights() {
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

void pksm::GameList::SetOnSelectionChanged(std::function<void()> callback) {
    onSelectionChangedCallback = callback;
}

void pksm::GameList::SetOnTouchSelect(std::function<void()> callback) {
    onTouchSelectCallback = callback;
}

void pksm::GameList::HandleOnSelectionChanged() {
    if (onSelectionChangedCallback) {
        auto selected = GetSelectedTitle();
        if (selected) {
            LOG_DEBUG("Selection changed to title: " + selected->getName());
        }
        onSelectionChangedCallback();
    }
}

pksm::GameList::~GameList() {
    // PatternBackground will clean up its own texture
} 