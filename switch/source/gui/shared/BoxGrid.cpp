#include "gui/shared/components/BoxGrid.hpp"

#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"

pksm::ui::BoxGrid::BoxGrid(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 itemSize,
    input::FocusManager::Ref parentFocusManager,
    input::SelectionManager::Ref parentSelectionManager,
    const pu::i32 numberOfRows,
    const pu::i32 itemsPerRow
)
  : ISelectable(),
    IGrid(itemsPerRow, {}),
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

    // Set up button handler for L/R buttons
    buttonHandler
        .RegisterButton(HidNpadButton_L, nullptr, [this]() { PreviousBox(); }, [this]() { return this->focused; });

    buttonHandler.RegisterButton(HidNpadButton_R, nullptr, [this]() { NextBox(); }, [this]() { return this->focused; });

    // Initialize focus manager
    focusManager = input::FocusManager::New("BoxGrid");
    selectionManager = input::SelectionManager::New("BoxGrid");

    // Set name for this component
    IFocusable::SetName("BoxGrid Element");
    ISelectable::SetName("BoxGrid Element");

    // Store the parent managers
    IFocusable::SetFocusManager(parentFocusManager);
    ISelectable::SetSelectionManager(parentSelectionManager);

    // Initialize with one empty box
    SetBoxCount(1);

    LOG_DEBUG("BoxGrid component initialization complete");
}

pu::i32 pksm::ui::BoxGrid::GetX() {
    return x;
}

pu::i32 pksm::ui::BoxGrid::GetY() {
    return y;
}

void pksm::ui::BoxGrid::SetDisabled(bool disabled) {
    this->disabled = disabled;
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
    // Skip input handling if disabled
    if (disabled) {
        return;
    }

    // Handle directional input only when focused
    if (focused) {
        inputHandler.HandleInput(keys_down, keys_held);

        // Handle button input (L/R)
        buttonHandler.HandleInput(keys_down, keys_up, keys_held);
    }

    // Pass input to all items
    for (auto& item : items) {
        item->OnInput(keys_down, keys_up, keys_held, touch_pos);
    }
}

bool pksm::ui::BoxGrid::HandleSelectInput(const u64 keys_down) {
    return false;  // No longer needed as button handling is done directly by BoxItem
}

// Box Data Methods
void pksm::ui::BoxGrid::SetBoxCount(size_t count) {
    // Resize the boxes vector to hold the requested number of boxes
    if (boxes.size() != count) {
        boxes.resize(count);

        // Initialize each box with empty Pokémon data
        for (auto& box : boxes) {
            // Each box has itemsPerRow * numberOfRows slots
            box.resize(static_cast<size_t>(itemsPerRow) * static_cast<size_t>(numberOfRows), BoxPokemonData());
        }
    }
}

void pksm::ui::BoxGrid::SetCurrentBox(int boxIndex) {
    if (boxIndex >= 0 && static_cast<size_t>(boxIndex) < boxes.size()) {
        currentBox = boxIndex;

        // Update the grid to display the new box
        UpdateGridFromCurrentBox();

        // Build a list of active sprites to keep in memory
        std::set<std::string> activeKeys;
        for (const auto& pokemonData : boxes[currentBox]) {
            if (!pokemonData.isEmpty()) {
                activeKeys.insert(
                    utils::PokemonSpriteManager::GenerateKey(pokemonData.species, pokemonData.form, pokemonData.shiny)
                );
            }
        }

        // Release sprites that aren't in the current box
        utils::PokemonSpriteManager::ReleaseUnusedSprites(activeKeys);
    }
}

void pksm::ui::BoxGrid::SetPokemonData(int boxIndex, int slotIndex, const BoxPokemonData& data) {
    if (boxIndex >= 0 && static_cast<size_t>(boxIndex) < boxes.size() && slotIndex >= 0 &&
        static_cast<size_t>(slotIndex) < boxes[boxIndex].size()) {
        boxes[boxIndex][slotIndex] = data;

        // If this is the current box, update the displayed item
        if (boxIndex == currentBox && static_cast<size_t>(slotIndex) < items.size()) {
            // Get the sprite for this Pokémon
            pu::sdl2::TextureHandle::Ref texture = data.getSprite();
            items[slotIndex]->SetImage(texture);
        }
    }
}

void pksm::ui::BoxGrid::SetBoxData(int boxIndex, const std::vector<BoxPokemonData>& boxData) {
    if (boxIndex >= 0 && static_cast<size_t>(boxIndex) < boxes.size()) {
        // Copy data, making sure we don't exceed the box size
        size_t copySize = std::min(boxData.size(), boxes[boxIndex].size());
        for (size_t i = 0; i < copySize; i++) {
            boxes[boxIndex][i] = boxData[i];
        }

        // Fill remaining slots with empty data if needed
        for (size_t i = copySize; i < boxes[boxIndex].size(); i++) {
            boxes[boxIndex][i] = BoxPokemonData();
        }

        // If this is the current box, update the grid
        if (boxIndex == currentBox) {
            UpdateGridFromCurrentBox();
        }
    }
}

pksm::ui::BoxPokemonData pksm::ui::BoxGrid::GetPokemonData(int boxIndex, int slotIndex) const {
    if (boxIndex >= 0 && static_cast<size_t>(boxIndex) < boxes.size() && slotIndex >= 0 &&
        static_cast<size_t>(slotIndex) < boxes[boxIndex].size()) {
        return boxes[boxIndex][slotIndex];
    }
    return BoxPokemonData();  // Return empty data for invalid indices
}

void pksm::ui::BoxGrid::UpdateGridFromCurrentBox() {
    // Clear existing items and unregister them from focus/selection managers
    for (auto& item : items) {
        focusManager->UnregisterFocusable(item);
        selectionManager->UnregisterSelectable(item);
    }
    items.clear();
    container->Clear();

    // Make sure the current box is valid
    if (currentBox < 0 || static_cast<size_t>(currentBox) >= boxes.size()) {
        return;
    }

    // Create box items for the current box
    size_t numSlots = boxes[currentBox].size();

    for (size_t i = 0; i < numSlots; i++) {
        // Calculate position using IGrid's helper method
        auto position = CalculateItemPosition(i);

        // Get the Pokémon data
        BoxPokemonData& pokemonData = boxes[currentBox][i];

        // Get the texture handle for this Pokémon
        pu::sdl2::TextureHandle::Ref textureHandle = pokemonData.getSprite();

        // Create a BoxItem for this slot
        auto boxItem = BoxItem::New(position.first, position.second, itemSize, itemSize, textureHandle);
        boxItem->IFocusable::SetName("BoxItem Element: Slot " + std::to_string(i));
        boxItem->ISelectable::SetName("BoxItem Element: Slot " + std::to_string(i));

        // Set up touch selection for this item
        const size_t index = i;
        boxItem->SetOnTouchSelect([this, index]() {
            SetSelectedIndex(index);
            if (onSelectionChangedCallback) {
                onSelectionChangedCallback(currentBox, static_cast<int>(index));
            }
        });

        // Set up selection callback
        boxItem->SetOnSelect([this, index]() {
            if (onSelectionChangedCallback) {
                onSelectionChangedCallback(currentBox, static_cast<int>(index));
            }
        });

        // Register with focus and selection managers
        focusManager->RegisterFocusable(boxItem);
        selectionManager->RegisterSelectable(boxItem);

        // Add to our containers
        items.push_back(boxItem);
        container->Add(boxItem);
    }

    // Select the first item by default if we have any items
    if (!items.empty()) {
        SetSelectedIndex(0);

        // If this BoxGrid is focused, also focus the first item
        if (focused && items.size() > 0) {
            items[0]->RequestFocus();
            LOG_DEBUG("Initial box item focused");
        }
    }
}

void pksm::ui::BoxGrid::SetSelectedIndex(size_t index) {
    if (index < items.size() && selectedIndex != index) {
        selectedIndex = index;

        // If this grid is focused, also request focus for the item
        if (focused && items.size() > index) {
            items[index]->RequestFocus();
        }

        // Notify about selection change
        if (onSelectionChangedCallback) {
            onSelectionChangedCallback(currentBox, static_cast<int>(selectedIndex));
        }
    }
}

// IFocusable implementation
void pksm::ui::BoxGrid::SetFocused(bool focused) {
    LOG_DEBUG("[BoxGrid] SetFocused: " + std::to_string(focused));
    if (this->focused != focused) {
        this->focused = focused;

        // When gaining focus, focus the selected box item
        if (focused && !items.empty() && selectedIndex < items.size()) {
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

// Additional focus/selection methods
void pksm::ui::BoxGrid::RequestFocus() {
    LOG_DEBUG("[BoxGrid] Requesting focus");
    if (auto manager = IFocusable::focusManager.lock()) {
        manager->HandleFocusRequest(shared_from_this());
    }
}

void pksm::ui::BoxGrid::EstablishOwningRelationship() {
    LOG_DEBUG("[BoxGrid] Establishing owning relationship");

    // First establish selectable relationship
    ISelectable::EstablishOwningRelationship();

    // Now safely set up the focus manager child relationship
    if (auto parentFocusManager = IFocusable::focusManager.lock()) {
        parentFocusManager->RegisterChildManager(focusManager);
        focusManager->SetOwningFocusable(std::static_pointer_cast<IFocusable>(shared_from_this()));
        // Register self with parent focus manager
        parentFocusManager->RegisterFocusable(std::static_pointer_cast<IFocusable>(shared_from_this()));
    }

    // Set up the selection manager child relationship
    if (auto parentSelectionManager = ISelectable::selectionManager.lock()) {
        parentSelectionManager->RegisterChildManager(selectionManager);
        selectionManager->SetOwningSelectable(std::static_pointer_cast<ISelectable>(shared_from_this()));
        // Register self with parent selection manager
        parentSelectionManager->RegisterSelectable(std::static_pointer_cast<ISelectable>(shared_from_this()));
    }
}

// Add new box navigation methods
void pksm::ui::BoxGrid::NextBox() {
    int nextBox = currentBox + 1;
    if (static_cast<size_t>(nextBox) < boxes.size()) {
        SetCurrentBox(nextBox);
    }
}

void pksm::ui::BoxGrid::PreviousBox() {
    if (currentBox > 0) {
        SetCurrentBox(currentBox - 1);
    }
}