#include "gui/shared/components/BoxGrid.hpp"

#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"

pksm::ui::BoxGrid::BoxGrid(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 itemSize,
    input::FocusManager::Ref parentFocusManager,
    input::SelectionManager::Ref parentSelectionManager,
    const std::map<ShakeDirection, bool> shouldConsiderSideOutOfBounds,
    const pu::i32 numberOfRows,
    const pu::i32 itemsPerRow
)
  : ISelectable(),
    IGrid(itemsPerRow, shouldConsiderSideOutOfBounds),
    x(x),
    y(y),
    itemSize(itemSize),
    numberOfRows(numberOfRows),
    focused(false),
    selected(false) {
    // Initialize container with calculated width/height
    container = pu::ui::Container::New(x, y, GetWidth(), GetHeight());

    // Set up input handler
    inputHandler.SetOnMoveLeft([this]() { IGrid::MoveLeft(); });
    inputHandler.SetOnMoveRight([this]() { IGrid::MoveRight(); });
    inputHandler.SetOnMoveUp([this]() { IGrid::MoveUp(); });
    inputHandler.SetOnMoveDown([this]() { IGrid::MoveDown(); });

    // Set name for this component
    IFocusable::SetName("BoxGrid Element");
    ISelectable::SetName("BoxGrid Element");

    // Initialize with empty box data
    size_t slotsPerBox = static_cast<size_t>(itemsPerRow) * static_cast<size_t>(numberOfRows);
    currentBoxData.resize(slotsPerBox);

    // Set up grid with empty data
    UpdateGridFromBoxData();

    // Store the parent managers
    IFocusable::SetFocusManager(parentFocusManager);
    ISelectable::SetSelectionManager(parentSelectionManager);
    LOG_DEBUG("BoxGrid component initialization complete");
}

pu::i32 pksm::ui::BoxGrid::GetX() {
    return x;
}

pu::i32 pksm::ui::BoxGrid::GetY() {
    return y;
}

void pksm::ui::BoxGrid::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Render all items in the grid
    for (auto& item : items) {
        item->OnRender(drawer, x + item->GetX() - this->x, y + item->GetY() - this->y);
    }

    // Optionally render grid dividers/borders if needed
}

void pksm::ui::BoxGrid::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    // Handle directional input only when focused
    if (focused) {
        inputHandler.HandleInput(keys_down, keys_held);
    }

    // Pass input to all items
    for (auto& item : items) {
        item->OnInput(keys_down, keys_up, keys_held, touch_pos);
    }
}

void pksm::ui::BoxGrid::SetPokemonData(int slotIndex, const BoxPokemonData& data) {
    if (slotIndex >= 0 && static_cast<size_t>(slotIndex) < currentBoxData.size()) {
        currentBoxData[slotIndex] = data;

        // Update the displayed item if it exists
        if (static_cast<size_t>(slotIndex) < items.size()) {
            items[slotIndex]->SetImage(data.getSprite());
            items[slotIndex]->SetGender(data.gender, !data.isEmpty());
            items[slotIndex]->SetPartyNumber(data.partyNumber);
            items[slotIndex]->SetUnusable(data.unusable);
        }
    }
}

void pksm::ui::BoxGrid::SetBoxData(const BoxData& boxData) {
    // Make a copy of the input box data
    currentBoxData = boxData;

    // If pokemon vector sizes don't match, resize to our standard size
    size_t expectedSize = static_cast<size_t>(itemsPerRow) * static_cast<size_t>(numberOfRows);
    if (currentBoxData.size() != expectedSize) {
        currentBoxData.resize(expectedSize);
    }

    // The slot count never changes after construction, so the items built
    // then are updated in place; destroying and re-registering all 30 slots
    // per box switch cost enough to hitch the render loop
    if (items.size() != currentBoxData.size() || !itemsRegistered) {
        UpdateGridFromBoxData();
        return;
    }
    for (size_t i = 0; i < currentBoxData.size(); i++) {
        const BoxPokemonData& data = currentBoxData[i];
        items[i]->SetImage(data.getSprite());
        items[i]->SetGender(data.gender, !data.isEmpty());
        items[i]->SetPartyNumber(data.partyNumber);
        items[i]->SetUnusable(data.unusable);
    }
}

pksm::ui::BoxPokemonData pksm::ui::BoxGrid::GetPokemonData(int slotIndex) const {
    if (slotIndex >= 0 && static_cast<size_t>(slotIndex) < currentBoxData.size()) {
        return currentBoxData[slotIndex];
    }
    return BoxPokemonData();  // Return empty data for invalid indices
}

void pksm::ui::BoxGrid::UpdateGridFromBoxData() {
    // Clear existing items and unregister them from focus/selection managers
    for (auto& item : items) {
        if (auto fm = this->focusManager.lock()) {
            fm->UnregisterFocusable(item);
        }
        if (auto sm = this->selectionManager.lock()) {
            sm->UnregisterSelectable(item);
        }
    }
    items.clear();
    container->Clear();

    // Create box items for the current box
    size_t numSlots = currentBoxData.size();

    for (size_t i = 0; i < numSlots; i++) {
        // Calculate position using IGrid's helper method
        auto position = CalculateItemPosition(i);

        // Get the Pokémon data
        BoxPokemonData& pokemonData = currentBoxData[i];

        // Create a BoxItem for this slot
        auto boxItem = BoxItem::New(position.first, position.second, itemSize, itemSize, pokemonData.getSprite());
        boxItem->SetGender(pokemonData.gender, !pokemonData.isEmpty());
        boxItem->SetPartyNumber(pokemonData.partyNumber);
        boxItem->SetUnusable(pokemonData.unusable);
        boxItem->IFocusable::SetName("BoxItem Element: Slot " + std::to_string(i));
        boxItem->ISelectable::SetName("BoxItem Element: Slot " + std::to_string(i));

        // Set up touch selection for this item
        const size_t index = i;
        boxItem->SetOnTouchSelect([this, index]() {
            SetSelectedIndex(index);
            if (onSelectionChangedCallback) {
                onSelectionChangedCallback(static_cast<int>(index));
            }
        });

        // Set up selection callback
        boxItem->SetOnSelect([this, index]() {
            if (onSelectionChangedCallback) {
                onSelectionChangedCallback(static_cast<int>(index));
            }
            if (onSlotActivatedCallback) {
                onSlotActivatedCallback(static_cast<int>(index));
            }
        });

        // Register with focus and selection managers
        if (auto fm = this->focusManager.lock()) {
            fm->RegisterFocusable(boxItem);
        }
        if (auto sm = this->selectionManager.lock()) {
            sm->RegisterSelectable(boxItem);
        }

        // Add to our containers
        items.push_back(boxItem);
        container->Add(boxItem);
    }

    itemsRegistered = focusManager.lock() != nullptr && selectionManager.lock() != nullptr;

    // Select the first item by default if we have any items
    if (!items.empty()) {
        SetSelectedIndex(0);

        // If this BoxGrid is focused, also focus the first item
        if (focused && items.size() > 0) {
            items[0]->RequestFocus();
            LOG_TRACE("Initial box item focused");
        }
    }
}

void pksm::ui::BoxGrid::SetSelectedIndex(size_t index) {
    LOG_TRACE("[BoxGrid] Setting selected index: " + std::to_string(index));
    if (index < items.size() && selectedIndex != index) {
        selectedIndex = index;

        // If this grid is focused, also request focus for the item
        if (focused && items.size() > index) {
            items[index]->RequestFocus();
        }

        // Notify about selection change
        if (onSelectionChangedCallback) {
            onSelectionChangedCallback(static_cast<int>(selectedIndex));
        }
    }
}

// IFocusable implementation
void pksm::ui::BoxGrid::SetFocused(bool focused) {
    LOG_TRACE("[BoxGrid] SetFocused: " + std::to_string(focused));
    if (this->focused != focused) {
        this->focused = focused;

        // When gaining focus, focus the selected box item
        if (focused && !items.empty() && selectedIndex < items.size()) {
            LOG_TRACE("[BoxGrid] Focusing item: " + std::to_string(selectedIndex));
            items[selectedIndex]->RequestFocus();
        }
    }
}

bool pksm::ui::BoxGrid::IsFocused() const {
    return focused;
}

// ISelectable implementation
void pksm::ui::BoxGrid::SetSelected(bool selected) {
    this->selected = selected;
}

bool pksm::ui::BoxGrid::IsSelected() const {
    return selected;
}

void pksm::ui::BoxGrid::EstablishOwningRelationship() {
    LOG_TRACE("[BoxGrid] Establishing owning relationship");

    IFocusable::EstablishOwningRelationship();
    ISelectable::EstablishOwningRelationship();
}