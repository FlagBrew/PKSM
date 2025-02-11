#include "gui/EmulatorGameList.hpp"

#include <cmath>
#include <sstream>

#include "gui/UIConstants.hpp"
#include "utils/Logger.hpp"

pksm::ui::EmulatorGameList::EmulatorGameList(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    const GameListLayoutConfig& config,
    input::FocusManager::Ref parentFocusManager
)
  : Element(),
    focused(false),
    onSelectionChangedCallback(nullptr),
    x(x),
    y(y),
    width(width),
    height(height),
    config(config) {
    LOG_DEBUG("Initializing EmulatorGameList component...");

    // Initialize selection managers
    gameGridSelectionManager = input::SelectionManager::New("GameGrid Manager");
    gameGridSelectionManager->SetActive(true);  // This is a root selection manager

    LOG_DEBUG("EmulatorGameList selection manager set");

    // Initialize focus managers
    gameGridManager = input::FocusManager::New("GameGrid Manager");

    // Initialize container
    container = pu::ui::Container::New(x, y, width, height);

    // Create section header
    titleText = pu::ui::elm::TextBlock::New(0, y + config.paddingTop, "Emulator Games");
    titleText->SetColor(pu::ui::Color(255, 255, 255, 255));
    titleText->SetFont(pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_HEADER));
    container->Add(titleText);

    // Create game grid
    gameGrid = GameGrid::New(
        x + config.paddingLeft,
        y + config.paddingTop + config.sectionTitleSpacing,
        GetHeight() - (config.paddingTop + config.sectionTitleSpacing),
        GRID_ITEMS_PER_ROW,
        gameGridManager,
        gameGridSelectionManager
    );
    container->Add(gameGrid);
    gameGrid->IFocusable::SetName("GameGrid Element");
    gameGrid->IFocusable::EstablishOwningRelationship();
    LOG_DEBUG("GameGrid Element established Focusable owning relationship");
    gameGrid->ISelectable::SetName("GameGrid Element");
    gameGrid->ISelectable::EstablishOwningRelationship();
    LOG_DEBUG("GameGrid Element established Selectable owning relationship");

    // Let container prepare elements
    container->PreRender();

    // Center title text
    pu::i32 gridWidth = gameGrid->GetWidth();
    pu::i32 titleX = x + config.paddingLeft + (gridWidth - titleText->GetWidth()) / 2;
    titleText->SetX(titleX);

    // Set up grid callbacks
    gameGrid->SetOnSelectionChanged([this]() { HandleOnSelectionChanged(); });
    gameGrid->SetOnTouchSelect([this]() {
        if (!focused) {
            focused = true;
            gameGrid->RequestFocus();
            HandleOnSelectionChanged();
            if (onTouchSelectCallback) {
                onTouchSelectCallback();
            }
        }
    });

    SetFocusManager(parentFocusManager);

    LOG_DEBUG("EmulatorGameList component initialization complete");
}

pu::i32 pksm::ui::EmulatorGameList::GetX() {
    return x;
}

pu::i32 pksm::ui::EmulatorGameList::GetY() {
    return y;
}

pu::i32 pksm::ui::EmulatorGameList::GetWidth() {
    return width;
}

pu::i32 pksm::ui::EmulatorGameList::GetHeight() {
    return height;
}

void pksm::ui::EmulatorGameList::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Render all elements in the container
    for (auto& element : container->GetElements()) {
        element->OnRender(drawer, element->GetX(), element->GetY());
    }
}

void pksm::ui::EmulatorGameList::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    // Handle directional input only when focused
    if (focused) {
        gameGrid->OnInput(keys_down, keys_up, keys_held, touch_pos);
    }

    // Always handle touch input regardless of focus state
    if (!touch_pos.IsEmpty()) {
        gameGrid->OnInput(keys_down, keys_up, keys_held, touch_pos);
    }
}

void pksm::ui::EmulatorGameList::SetFocused(bool focused) {
    if (this->focused != focused) {
        LOG_DEBUG(focused ? "EmulatorGameList gained focus" : "EmulatorGameList lost focus");
        this->focused = focused;
        if (focused) {
            gameGrid->RequestFocus();
        }
    }
}

bool pksm::ui::EmulatorGameList::IsFocused() const {
    return focused;
}

void pksm::ui::EmulatorGameList::SetFocusManager(std::shared_ptr<input::FocusManager> manager) {
    LOG_DEBUG("Setting focus manager on EmulatorGameList");
    IFocusable::SetFocusManager(manager);

    // When we get a focus manager, register our child managers
    if (manager) {
        manager->RegisterChildManager(gameGridManager);
    }
}

void pksm::ui::EmulatorGameList::SetDataSource(const std::vector<titles::Title::Ref>& titles) {
    LOG_DEBUG("Setting EmulatorGameList data source with " + std::to_string(titles.size()) + " titles");
    LOG_MEMORY();  // Memory check when loading new titles

    // Store titles
    this->titles = titles;
    gameGrid->SetDataSource(titles);
}

pksm::titles::Title::Ref pksm::ui::EmulatorGameList::GetSelectedTitle() const {
    return gameGrid->GetSelectedTitle();
}

void pksm::ui::EmulatorGameList::SetOnSelectionChanged(std::function<void()> callback) {
    onSelectionChangedCallback = callback;
}

void pksm::ui::EmulatorGameList::SetOnTouchSelect(std::function<void()> callback) {
    onTouchSelectCallback = callback;
}

void pksm::ui::EmulatorGameList::HandleOnSelectionChanged() {
    if (onSelectionChangedCallback) {
        auto selected = GetSelectedTitle();
        if (selected) {
            LOG_DEBUG("Selection changed to title: " + selected->getName());
        }
        onSelectionChangedCallback();
    }
}