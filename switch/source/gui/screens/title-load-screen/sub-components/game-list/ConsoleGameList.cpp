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
    LOG_DEBUG("[ConsoleGameList] Initializing component...");

    // Initialize selection managers
    consoleGameListSelectionManager = input::SelectionManager::New("ConsoleGameList Manager");
    consoleGameListSelectionManager->SetActive(true);  // This is a root selection manager
    gameGridSelectionManager = input::SelectionManager::New("GameGrid Manager");
    consoleGameListSelectionManager->RegisterChildManager(gameGridSelectionManager);

    LOG_DEBUG("[ConsoleGameList] Selection manager set");

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
    cartridgeText->SetColor(global::TEXT_WHITE);
    cartridgeText->SetFont(pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_HEADER));
    container->Add(cartridgeText);

    // Center game card text in its section
    pu::i32 gameCardTextX = gameCardX + (GAME_CARD_SIZE - cartridgeText->GetWidth()) / 2;
    cartridgeText->SetX(gameCardTextX);

    installedText = pu::ui::elm::TextBlock::New(0, y + config.paddingTop, "Installed Games");
    installedText->SetColor(global::TEXT_WHITE);
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
        gameGridSelectionManager,
        std::map<ShakeDirection, bool>{{ShakeDirection::LEFT, false}, {ShakeDirection::RIGHT, true}}
    );
    container->Add(installedGames);
    installedGames->IFocusable::SetName("GameGrid Element");
    installedGames->IFocusable::EstablishOwningRelationship();
    LOG_DEBUG("[ConsoleGameList] GameGrid Element established Focusable owning relationship");
    installedGames->ISelectable::SetName("GameGrid Element");
    installedGames->ISelectable::EstablishOwningRelationship();
    LOG_DEBUG("[ConsoleGameList] GameGrid Element established Selectable owning relationship");

    // Let container prepare elements
    container->PreRender();

    // Center installed games text in its section
    pu::i32 installedGamesWidth = installedGames->GetWidth();
    pu::i32 installedTextX = installedStartX + (installedGamesWidth - installedText->GetWidth()) / 2;
    installedText->SetX(installedTextX);

    // Store key positions for later use
    this->gameCardX = gameCardX;
    this->installedStartX = installedStartX;

    gameCardSlot = GameCardSlot::New(
        gameCardX,
        cartridgeText->GetY() + config.sectionTitleSpacing,
        GAME_CARD_SIZE,
        config.gameOutlinePadding
    );
    gameCardSlot->SetTitle(nullptr);
    gameCardSlot->SetOnTouchSelect([this]() {
        LOG_TRACE("[ConsoleGameList] Touch select on game card slot");
        selectionState = SelectionState::GameCard;
        HandleOnSelectionChanged();
        if (onTouchSelectCallback) {
            onTouchSelectCallback();
        }
    });
    gameCardSlot->SetOnSelect([this]() {
        if (onSelectCallback) {
            onSelectCallback();
        }
    });
    container->Add(gameCardSlot);
    consoleGameListSelectionManager->RegisterSelectable(gameCardSlot);

    // Set up input handler for transitions between game card and grid
    directionalInputHandler.SetOnMoveLeft([this]() {
        if (selectionState == SelectionState::InstalledGame && installedGames->IsFirstInRow()) {
            if (gameCardSlot->GetTitle()) {
                LOG_TRACE("[ConsoleGameList] Transitioning selection from installed games to game card");
                selectionState = SelectionState::GameCard;
                gameCardSlot->RequestFocus();
                HandleOnSelectionChanged();
            }
            // Empty slot: stay in the grid (no shake target to the left)
        } else if (selectionState == SelectionState::GameCard) {
            gameCardSlot->shakeOutOfBounds(ShakeDirection::LEFT);
        }
    });
    directionalInputHandler.SetOnMoveRight([this]() {
        if (selectionState == SelectionState::GameCard && !titles.empty()) {
            LOG_TRACE("[ConsoleGameList] Transitioning selection from game card to installed games");
            selectionState = SelectionState::InstalledGame;
            installedGames->SetSelectedIndex(0);  // Always select first game when moving right
            installedGames->RequestFocus();
            HandleOnSelectionChanged();
        }
    });

    // Set up grid callbacks
    installedGames->SetOnSelectionChanged([this]() { HandleOnSelectionChanged(); });
    installedGames->SetOnTouchSelect([this]() {
        LOG_TRACE("[ConsoleGameList] Touch select on installed games");
        selectionState = SelectionState::InstalledGame;
        HandleOnSelectionChanged();
        if (onTouchSelectCallback) {
            onTouchSelectCallback();
        }
    });
    installedGames->SetOnSelect([this]() {
        if (onSelectCallback) {
            onSelectCallback();
        }
    });

    SetFocusManager(parentFocusManager);

    LOG_DEBUG("[ConsoleGameList] Component initialization complete");
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
    if (focused) {
        directionalInputHandler.HandleInput(keys_down, keys_held);
    }
    gameCardSlot->OnInput(keys_down, keys_up, keys_held, touch_pos);
    installedGames->OnInput(keys_down, keys_up, keys_held, touch_pos);
}

void pksm::ui::ConsoleGameList::SetFocused(bool focused) {
    if (this->focused != focused) {
        LOG_TRACE(focused ? "[ConsoleGameList] Gained focus" : "[ConsoleGameList] Lost focus");
        this->focused = focused;
        if (!focused) {
            directionalInputHandler.ClearState();
        }
        // Update visual state only, don't request focus
        if (focused) {
            // Just update the visual state of the appropriate section
            if (selectionState == SelectionState::GameCard && gameCardSlot->GetTitle()) {
                LOG_TRACE("[ConsoleGameList] Requesting focus on game card image");
                gameCardSlot->ISelectable::RequestFocus();
            } else {
                LOG_TRACE("[ConsoleGameList] Requesting focus on installed games");
                installedGames->RequestFocus();
            }
        }
    }
}

bool pksm::ui::ConsoleGameList::IsFocused() const {
    return focused;
}

void pksm::ui::ConsoleGameList::SetFocusManager(std::shared_ptr<input::FocusManager> manager) {
    LOG_DEBUG("[ConsoleGameList] Setting focus manager");
    IFocusable::SetFocusManager(manager);

    // When we get a focus manager, register our child managers
    if (manager) {
        manager->RegisterChildManager(installedGamesManager);
        manager->RegisterFocusable(gameCardSlot);
    }
}

void pksm::ui::ConsoleGameList::SetGameCardTitle(titles::Title::Ref title) {
    LOG_DEBUG(
        title ? "[ConsoleGameList] Game card set: " + title->getName() : "[ConsoleGameList] Game card slot empty"
    );
    gameCardSlot->SetTitle(title);

    // Removing the selected cart moves selection to the installed grid;
    // inserting one never steals an existing selection
    if (!title && selectionState == SelectionState::GameCard) {
        selectionState = SelectionState::InstalledGame;
        if (focused) {
            installedGames->RequestFocus();
        }
        HandleOnSelectionChanged();
    }
}

void pksm::ui::ConsoleGameList::SetDataSource(const std::vector<titles::Title::Ref>& titles) {
    LOG_DEBUG("[ConsoleGameList] Setting data source with " + std::to_string(titles.size()) + " installed titles");
    LOG_MEMORY();  // Memory check when loading new titles

    // Preserve the grid selection by title identity across refreshes
    titles::Title::Ref prevSelected =
        (selectionState == SelectionState::InstalledGame) ? installedGames->GetSelectedTitle() : nullptr;

    // Installed titles only; the game card arrives via SetGameCardTitle
    this->titles = titles;
    installedGames->SetDataSource(titles);

    size_t restoredIndex = 0;
    if (prevSelected) {
        for (size_t i = 0; i < titles.size(); i++) {
            if (titles[i]->getTitleId() == prevSelected->getTitleId()) {
                restoredIndex = i;
                break;
            }
        }
    }
    installedGames->SetSelectedIndex(restoredIndex);

    // Only correct the selection state if it became invalid
    if (selectionState == SelectionState::GameCard && !gameCardSlot->GetTitle()) {
        selectionState = SelectionState::InstalledGame;
    }
}

pksm::titles::Title::Ref pksm::ui::ConsoleGameList::GetSelectedTitle() const {
    LOG_TRACE("[ConsoleGameList] Getting selected title");
    if (selectionState == SelectionState::GameCard) {
        LOG_TRACE("[ConsoleGameList] Returning game card title");
        return gameCardSlot->GetTitle();
    } else {
        LOG_TRACE("[ConsoleGameList] Returning installed title");
        return installedGames->GetSelectedTitle();
    }
}

void pksm::ui::ConsoleGameList::HandleOnSelectionChanged() {
    LOG_TRACE("[ConsoleGameList] Handling selection changed");
    if (onSelectionChangedCallback) {
        auto selected = GetSelectedTitle();
        if (selected) {
            LOG_TRACE("[ConsoleGameList] Selection changed to title: " + selected->getName());
        }
        onSelectionChangedCallback();
    }
}