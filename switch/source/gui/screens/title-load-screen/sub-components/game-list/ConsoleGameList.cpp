#include "gui/screens/title-load-screen/sub-components/game-list/ConsoleGameList.hpp"

#include <cmath>
#include <sstream>

#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"

pksm::ui::ConsoleGameList::ConsoleGameList(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    const GameListLayoutConfig& config,
    input::FocusManager::Ref parentFocusManager
)
  : IGameList(),
    selectionState(SelectionState::GameCard),
    focused(false),
    onSelectionChangedCallback(nullptr),
    x(x),
    y(y),
    width(width),
    height(height),
    config(config) {
    LOG_DEBUG("Initializing ConsoleGameList component...");

    // Initialize selection managers
    consoleGameListSelectionManager = input::SelectionManager::New("ConsoleGameList Manager");
    consoleGameListSelectionManager->SetActive(true);  // This is a root selection manager
    gameGridSelectionManager = input::SelectionManager::New("GameGrid Manager");
    consoleGameListSelectionManager->RegisterChildManager(gameGridSelectionManager);

    LOG_DEBUG("ConsoleGameList selection manager set");

    // Initialize focus managers
    installedGamesManager = input::FocusManager::New("InstalledGames (GameGrid) Manager");

    // Initialize container
    container = pu::ui::Container::New(x, y, width, height);

    // Calculate key positions relative to our component's origin
    pu::i32 gameCardX = x + config.paddingLeft;
    pu::i32 dividerX = gameCardX + GAME_CARD_SIZE + SECTION_DIVIDER_PADDING_LEFT;
    pu::i32 installedStartX = dividerX + SECTION_DIVIDER_WIDTH + SECTION_DIVIDER_PADDING_LEFT;

    // Create section headers first (we need their position)
    cartridgeText = pu::ui::elm::TextBlock::New(0, y + config.paddingTop, "Game Card");
    cartridgeText->SetColor(pu::ui::Color(255, 255, 255, 255));
    cartridgeText->SetFont(pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_HEADER));
    container->Add(cartridgeText);

    // Center game card text in its section
    pu::i32 gameCardTextX = gameCardX + (GAME_CARD_SIZE - cartridgeText->GetWidth()) / 2;
    cartridgeText->SetX(gameCardTextX);

    installedText = pu::ui::elm::TextBlock::New(0, y + config.paddingTop, "Installed Games");
    installedText->SetColor(pu::ui::Color(255, 255, 255, 255));
    installedText->SetFont(pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_HEADER));
    container->Add(installedText);

    // Create section divider
    divider = pu::ui::elm::Rectangle::New(
        dividerX,
        y,  // Position relative to our origin
        SECTION_DIVIDER_WIDTH,
        height,  //
        pksm::ui::global::BACKGROUND_BLUE
    );
    container->Add(divider);

    // Create game grid for installed games
    installedGames = GameGrid::New(
        installedStartX,
        y + config.paddingTop + config.sectionTitleSpacing,
        GetHeight() - (config.paddingTop + config.sectionTitleSpacing),
        GRID_ITEMS_PER_ROW,
        installedGamesManager,
        gameGridSelectionManager
    );
    container->Add(installedGames);
    installedGames->IFocusable::SetName("GameGrid Element");
    installedGames->IFocusable::EstablishOwningRelationship();
    LOG_DEBUG("GameGrid Element established Focusable owning relationship");
    installedGames->ISelectable::SetName("GameGrid Element");
    installedGames->ISelectable::EstablishOwningRelationship();
    LOG_DEBUG("GameGrid Element established Selectable owning relationship");

    // Let container prepare elements
    container->PreRender();

    // Center installed games text in its section
    pu::i32 installedGamesWidth = installedGames->GetWidth();
    pu::i32 installedTextX = installedStartX + (installedGamesWidth - installedText->GetWidth()) / 2;
    installedText->SetX(installedTextX);

    // Store key positions for later use
    this->gameCardX = gameCardX;
    this->installedStartX = installedStartX;

    // Create the game card image component (initially without an image)
    gameCardImage = FocusableImage::New(
        gameCardX,
        cartridgeText->GetY() + config.sectionTitleSpacing,
        nullptr,  // No initial image
        94,
        config.gameOutlinePadding
    );
    gameCardImage->ISelectable::SetName("GameCard Image Element");
    gameCardImage->SetWidth(GAME_CARD_SIZE);
    gameCardImage->SetHeight(GAME_CARD_SIZE);
    gameCardImage->SetOnTouchSelect([this]() {
        if (selectionState != SelectionState::GameCard || !focused) {
            focused = true;
            selectionState = SelectionState::GameCard;
            gameCardImage->ISelectable::RequestFocus();
            HandleOnSelectionChanged();
            if (onTouchSelectCallback) {
                onTouchSelectCallback();
            }
        }
    });
    container->Add(gameCardImage);
    consoleGameListSelectionManager->RegisterSelectable(gameCardImage);

    // Set up input handler for transitions between game card and grid
    inputHandler.SetOnMoveLeft([this]() {
        if (selectionState == SelectionState::InstalledGame && installedGames->IsFirstInRow()) {
            LOG_DEBUG("Transitioning selection from installed games to game card");
            selectionState = SelectionState::GameCard;
            gameCardImage->ISelectable::RequestFocus();
            HandleOnSelectionChanged();
        }
    });
    inputHandler.SetOnMoveRight([this]() {
        if (selectionState == SelectionState::GameCard && !titles.empty()) {
            LOG_DEBUG("Transitioning selection from game card to installed games");
            selectionState = SelectionState::InstalledGame;
            installedGames->SetSelectedIndex(0);  // Always select first game when moving right
            installedGames->RequestFocus();
            HandleOnSelectionChanged();
        }
    });

    // Set up grid callbacks
    installedGames->SetOnSelectionChanged([this]() { HandleOnSelectionChanged(); });
    installedGames->SetOnTouchSelect([this]() {
        if (selectionState != SelectionState::InstalledGame || !focused) {
            selectionState = SelectionState::InstalledGame;
            focused = true;
            installedGames->RequestFocus();
            HandleOnSelectionChanged();
            if (onTouchSelectCallback) {
                onTouchSelectCallback();
            }
        }
    });

    SetFocusManager(parentFocusManager);

    LOG_DEBUG("ConsoleGameList component initialization complete");
}

pu::i32 pksm::ui::ConsoleGameList::GetX() {
    return x;
}

pu::i32 pksm::ui::ConsoleGameList::GetY() {
    return y;
}

pu::i32 pksm::ui::ConsoleGameList::GetWidth() {
    return width;
}

pu::i32 pksm::ui::ConsoleGameList::GetHeight() {
    return height;
}

void pksm::ui::ConsoleGameList::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Render all elements in the container
    for (auto& element : container->GetElements()) {
        element->OnRender(drawer, element->GetX(), element->GetY());
    }
}

void pksm::ui::ConsoleGameList::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
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

void pksm::ui::ConsoleGameList::SetFocused(bool focused) {
    if (this->focused != focused) {
        LOG_DEBUG(focused ? "ConsoleGameList gained focus" : "ConsoleGameList lost focus");
        this->focused = focused;
        // Update visual state only, don't request focus
        if (focused) {
            // Just update the visual state of the appropriate section
            if (selectionState == SelectionState::GameCard) {
                LOG_DEBUG("Requesting focus on game card image");
                gameCardImage->ISelectable::RequestFocus();
            } else {
                LOG_DEBUG("Requesting focus on installed games");
                installedGames->RequestFocus();
            }
        }
    }
}

bool pksm::ui::ConsoleGameList::IsFocused() const {
    return focused;
}

void pksm::ui::ConsoleGameList::SetFocusManager(std::shared_ptr<input::FocusManager> manager) {
    LOG_DEBUG("Setting focus manager on ConsoleGameList");
    IFocusable::SetFocusManager(manager);

    // When we get a focus manager, register our child managers
    if (manager) {
        manager->RegisterChildManager(installedGamesManager);
        manager->RegisterFocusable(gameCardImage);
    }
}

void pksm::ui::ConsoleGameList::SetDataSource(const std::vector<titles::Title::Ref>& titles) {
    LOG_DEBUG("Setting ConsoleGameList data source with " + std::to_string(titles.size()) + " titles");
    LOG_MEMORY();  // Memory check when loading new titles

    // Store titles
    this->titles = titles;

    // Update game card image if available
    if (!titles.empty()) {
        LOG_DEBUG("Setting game card image for first title");
        gameCardImage->ISelectable::SetName(gameCardImage->ISelectable::GetName() + " " + titles[0]->getName());
        gameCardImage->SetImage(titles[0]->getIcon());
        gameCardImage->SetWidth(GAME_CARD_SIZE);
        gameCardImage->SetHeight(GAME_CARD_SIZE);
    } else {
        LOG_DEBUG("No titles available, clearing game card image");
        gameCardImage->SetImage(nullptr);
    }

    // Set up installed games grid with remaining titles
    std::vector<titles::Title::Ref> installedTitles;
    if (titles.size() > 1) {
        LOG_DEBUG("Setting up installed games grid with " + std::to_string(titles.size() - 1) + " titles");
        installedTitles.assign(titles.begin() + 1, titles.end());
    }
    installedGames->SetDataSource(installedTitles);
}

pksm::titles::Title::Ref pksm::ui::ConsoleGameList::GetSelectedTitle() const {
    if (selectionState == SelectionState::GameCard) {
        // Return game card title if it exists
        return !titles.empty() ? titles[0] : nullptr;
    } else {
        // Return selected installed title
        return installedGames->GetSelectedTitle();
    }
}

void pksm::ui::ConsoleGameList::SetOnSelectionChanged(std::function<void()> callback) {
    onSelectionChangedCallback = callback;
}

void pksm::ui::ConsoleGameList::SetOnTouchSelect(std::function<void()> callback) {
    onTouchSelectCallback = callback;
}

void pksm::ui::ConsoleGameList::HandleOnSelectionChanged() {
    if (onSelectionChangedCallback) {
        auto selected = GetSelectedTitle();
        if (selected) {
            LOG_DEBUG("Selection changed to title: " + selected->getName());
        }
        onSelectionChangedCallback();
    }
}