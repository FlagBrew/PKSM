#include "gui/ConsoleGameList.hpp"
#include "gui/UIConstants.hpp"
#include <cmath>
#include <sstream>
#include "utils/Logger.hpp"

pksm::ConsoleGameList::ConsoleGameList(const pu::i32 x, const pu::i32 y, const LayoutConfig& config)
    : Element(), selectionState(SelectionState::GameCard), focused(false),
      onSelectionChangedCallback(nullptr), x(x), y(y), config(config) {
    
    LOG_DEBUG("Initializing ConsoleGameList component...");

    // Initialize container
    container = std::make_unique<pu::ui::Container>(x, y, 0, 0);  // Width/height will be set after we know our dimensions
    
    // Calculate key positions relative to our component's origin
    pu::i32 gameCardX = x + config.marginLeft;
    pu::i32 dividerX = gameCardX + GAME_CARD_SIZE + SECTION_DIVIDER_PADDING;
    pu::i32 installedStartX = dividerX + SECTION_DIVIDER_WIDTH + SECTION_DIVIDER_PADDING;
    
    // Create section headers first (we need their position)
    cartridgeText = pu::ui::elm::TextBlock::New(0, y + config.marginTop, "Game Card");
    cartridgeText->SetColor(pu::ui::Color(255, 255, 255, 255));
    cartridgeText->SetFont(UIConstants::MakeMediumFontName(UIConstants::FONT_SIZE_HEADER));
    container->Add(cartridgeText);
    
    // Center game card text in its section
    pu::i32 gameCardTextX = gameCardX + (GAME_CARD_SIZE - cartridgeText->GetWidth()) / 2;
    cartridgeText->SetX(gameCardTextX);

    installedText = pu::ui::elm::TextBlock::New(0, y + config.marginTop, "Installed Games");
    installedText->SetColor(pu::ui::Color(255, 255, 255, 255));
    installedText->SetFont(UIConstants::MakeMediumFontName(UIConstants::FONT_SIZE_HEADER));
    container->Add(installedText);
    
    // Create section divider
    divider = pu::ui::elm::Rectangle::New(
        dividerX, y,  // Position relative to our origin
        SECTION_DIVIDER_WIDTH, 0,  // Height will be set after we know our height
        UIConstants::BACKGROUND_BLUE
    );
    container->Add(divider);

    // Create game grid for installed games
    installedGames = GameGrid::New(
        installedStartX,
        y + config.marginTop + config.sectionTitleSpacing
    );
    container->Add(installedGames);

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

    LOG_DEBUG("ConsoleGameList component initialization complete");
}

pu::i32 pksm::ConsoleGameList::GetX() {
    return x;
}

pu::i32 pksm::ConsoleGameList::GetY() {
    return y;
}

pu::i32 pksm::ConsoleGameList::GetWidth() {
    // Width from left edge to end of installed games section
    return (installedStartX + installedGames->GetWidth() + config.marginRight) - GetX();
}

pu::i32 pksm::ConsoleGameList::GetHeight() {
    // Calculate height based on our layout requirements
    pu::i32 height = config.marginTop + config.sectionTitleSpacing + GAME_CARD_SIZE + config.marginBottom;
    return height;
}

void pksm::ConsoleGameList::OnRender(pu::ui::render::Renderer::Ref &drawer, const pu::i32 x, const pu::i32 y) {
    // Update container dimensions
    container->SetWidth(GetWidth());
    container->SetHeight(GetHeight());

    // Update divider height
    divider->SetHeight(GetHeight());

    // Let container prepare elements
    container->PreRender();

    // Render all elements in the container
    for (auto& element : container->GetElements()) {
        element->OnRender(drawer, element->GetX(), element->GetY());
    }
}

void pksm::ConsoleGameList::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
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

void pksm::ConsoleGameList::SetFocused(bool focused) {
    if (this->focused != focused) {
        LOG_DEBUG(focused ? "ConsoleGameList gained focus" : "ConsoleGameList lost focus");
        this->focused = focused;
        UpdateHighlights();
    }
}

bool pksm::ConsoleGameList::IsFocused() const {
    return focused;
}

void pksm::ConsoleGameList::SetDataSource(const std::vector<titles::TitleRef>& titles) {
    LOG_DEBUG("Setting ConsoleGameList data source with " + std::to_string(titles.size()) + " titles");
    LOG_MEMORY();  // Memory check when loading new titles
    
    // Store titles
    this->titles = titles;

    // Clear existing game card
    if (gameCardImage) {
        container->GetElements().erase(
            std::remove(container->GetElements().begin(), container->GetElements().end(), gameCardImage),
            container->GetElements().end()
        );
        gameCardImage = nullptr;
    }

    // Create game card image if first title exists
    if (!titles.empty()) {
        LOG_DEBUG("Creating game card image for first title");
        gameCardImage = FocusableImage::New(
            gameCardX,
            cartridgeText->GetY() + config.sectionTitleSpacing,
            titles[0]->getIcon(),
            94,
            config.gameOutlinePadding
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
        container->Add(gameCardImage);
    }

    // Set up installed games grid with remaining titles
    std::vector<titles::TitleRef> installedTitles;
    if (titles.size() > 1) {
        LOG_DEBUG("Setting up installed games grid with " + std::to_string(titles.size() - 1) + " titles");
        installedTitles.assign(titles.begin() + 1, titles.end());
    }
    installedGames->SetDataSource(installedTitles);

    // Update grid's available height based on our height, accounting for the header area
    pu::i32 gridHeight = GetHeight() - (config.marginTop + config.sectionTitleSpacing);
    installedGames->SetAvailableHeight(gridHeight);

    UpdateHighlights();
}

titles::TitleRef pksm::ConsoleGameList::GetSelectedTitle() const {
    if (selectionState == SelectionState::GameCard) {
        // Return game card title if it exists
        return !titles.empty() ? titles[0] : nullptr;
    } else {
        // Return selected installed title
        return installedGames->GetSelectedTitle();
    }
}

void pksm::ConsoleGameList::UpdateHighlights() {
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

void pksm::ConsoleGameList::SetOnSelectionChanged(std::function<void()> callback) {
    onSelectionChangedCallback = callback;
}

void pksm::ConsoleGameList::SetOnTouchSelect(std::function<void()> callback) {
    onTouchSelectCallback = callback;
}

void pksm::ConsoleGameList::HandleOnSelectionChanged() {
    if (onSelectionChangedCallback) {
        auto selected = GetSelectedTitle();
        if (selected) {
            LOG_DEBUG("Selection changed to title: " + selected->getName());
        }
        onSelectionChangedCallback();
    }
}

pksm::ConsoleGameList::~ConsoleGameList() = default; 