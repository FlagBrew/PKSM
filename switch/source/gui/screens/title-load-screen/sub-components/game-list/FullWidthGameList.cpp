#include "gui/screens/title-load-screen/sub-components/game-list/FullWidthGameList.hpp"

#include <cmath>
#include <sstream>

#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"

namespace pksm::ui {

FullWidthGameList::FullWidthGameList(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    const GameListLayoutConfig& config,
    input::FocusManager::Ref parentFocusManager,
    const std::string& titleStr
)
  : Element(),
    focused(false),
    onSelectionChangedCallback(nullptr),
    x(x),
    y(y),
    width(width),
    height(height),
    config(config) {
    LOG_DEBUG("Initializing FullWidthGameList component...");

    // Initialize selection managers
    gameGridSelectionManager = input::SelectionManager::New("GameGrid Manager");
    gameGridSelectionManager->SetActive(true);  // This is a root selection manager

    LOG_DEBUG("FullWidthGameList selection manager set");

    // Initialize focus managers
    gameGridManager = input::FocusManager::New("GameGrid Manager");

    // Initialize container
    container = pu::ui::Container::New(x, y, width, height);

    // Create section header
    titleText = pu::ui::elm::TextBlock::New(0, y + config.paddingTop, titleStr);
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

    LOG_DEBUG("FullWidthGameList component initialization complete");
}

pu::i32 FullWidthGameList::GetX() {
    return x;
}

pu::i32 FullWidthGameList::GetY() {
    return y;
}

pu::i32 FullWidthGameList::GetWidth() {
    return width;
}

pu::i32 FullWidthGameList::GetHeight() {
    return height;
}

void FullWidthGameList::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Render all elements in the container
    for (auto& element : container->GetElements()) {
        element->OnRender(drawer, element->GetX(), element->GetY());
    }
}

void FullWidthGameList::OnInput(
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

void FullWidthGameList::SetFocused(bool focused) {
    if (this->focused != focused) {
        LOG_DEBUG(focused ? "FullWidthGameList gained focus" : "FullWidthGameList lost focus");
        this->focused = focused;
        if (focused) {
            gameGrid->RequestFocus();
        }
    }
}

bool FullWidthGameList::IsFocused() const {
    return focused;
}

void FullWidthGameList::SetFocusManager(std::shared_ptr<input::FocusManager> manager) {
    LOG_DEBUG("Setting focus manager on FullWidthGameList");
    IFocusable::SetFocusManager(manager);

    // When we get a focus manager, register our child managers
    if (manager) {
        manager->RegisterChildManager(gameGridManager);
    }
}

void FullWidthGameList::SetDataSource(const std::vector<titles::Title::Ref>& titles) {
    LOG_DEBUG("Setting FullWidthGameList data source with " + std::to_string(titles.size()) + " titles");
    LOG_MEMORY();

    // Store titles
    this->titles = titles;
    gameGrid->SetDataSource(titles);
}

pksm::titles::Title::Ref FullWidthGameList::GetSelectedTitle() const {
    return gameGrid->GetSelectedTitle();
}

void FullWidthGameList::SetOnSelectionChanged(std::function<void()> callback) {
    onSelectionChangedCallback = callback;
}

void FullWidthGameList::SetOnTouchSelect(std::function<void()> callback) {
    onTouchSelectCallback = callback;
}

void FullWidthGameList::HandleOnSelectionChanged() {
    if (onSelectionChangedCallback) {
        auto selected = GetSelectedTitle();
        if (selected) {
            LOG_DEBUG("Selection changed to title: " + selected->getName());
        }
        onSelectionChangedCallback();
    }
}

}  // namespace pksm::ui