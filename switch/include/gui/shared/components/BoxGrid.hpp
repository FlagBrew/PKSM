#pragma once

#include <functional>
#include <map>
#include <pu/Plutonium>
#include <vector>

#include "gui/shared/components/BoxItem.hpp"
#include "gui/shared/components/BoxPokemonData.hpp"
#include "gui/shared/components/IGrid.hpp"
#include "gui/shared/components/IShakeable.hpp"
#include "gui/shared/components/SpriteImage.hpp"
#include "input/ButtonInputHandler.hpp"
#include "input/directional/DirectionalInputHandler.hpp"
#include "input/visual-feedback/FocusManager.hpp"
#include "input/visual-feedback/SelectionManager.hpp"

namespace pksm::ui {

class BoxGrid : public ISelectable, public IGrid {
private:
    // Layout constants
    static constexpr pu::i32 DEFAULT_ITEMS_PER_ROW = 6;  // Default number of items per row
    static constexpr pu::i32 DEFAULT_NUMBER_OF_ROWS = 5;  // Default number of rows
    static constexpr pu::i32 GRID_SPACING = 8;  // Spacing between grid items

    // Position and size
    pu::i32 x;
    pu::i32 y;
    pu::i32 itemSize;
    pu::i32 numberOfRows;

    // State
    bool disabled = false;
    bool focused = false;
    bool selected = false;

    // Data source
    std::vector<std::vector<BoxPokemonData>> boxes;
    int currentBox = 0;

    // Components
    pu::ui::Container::Ref container;
    std::vector<BoxItem::Ref> items;
    input::FocusManager::Ref focusManager;
    input::SelectionManager::Ref selectionManager;
    pksm::input::DirectionalInputHandler inputHandler;
    input::ButtonInputHandler buttonHandler;

    // Event callbacks
    std::function<void(int, int)> onSelectionChangedCallback;
    std::function<void(int, int)> onPokemonMovedCallback;

    // Navigation methods
    void NextBox();
    void PreviousBox();

    // IGrid layout method implementations
    pu::i32 GetItemWidth() const override { return itemSize; }
    pu::i32 GetItemHeight() const override { return itemSize; }
    pu::i32 GetHorizontalSpacing() const override { return GRID_SPACING; }
    pu::i32 GetVerticalSpacing() const override { return GRID_SPACING; }

public:
    BoxGrid(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 itemSize,
        input::FocusManager::Ref parentFocusManager,
        input::SelectionManager::Ref parentSelectionManager,
        const pu::i32 numberOfRows = DEFAULT_NUMBER_OF_ROWS,
        const pu::i32 itemsPerRow = DEFAULT_ITEMS_PER_ROW
    );
    PU_SMART_CTOR(BoxGrid)

    // Element implementation
    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override { return CalculateDefaultGridWidth(); }
    pu::i32 GetHeight() override { return CalculateDefaultGridHeight(); }
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void
    OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;

    // IFocusable implementation
    void SetFocused(bool focused) override;
    bool IsFocused() const override;

    // ISelectable implementation
    void SetSelected(bool selected) override;
    bool IsSelected() const override;

    // IGrid implementations (required abstract methods only)
    size_t GetItemCount() const override { return items.size(); }
    ShakeableWithOutline::Ref GetItemAtIndex(size_t index) override {
        if (index < items.size()) {
            return items[index];
        }
        return nullptr;
    }
    void SetSelectedIndex(size_t index) override;

    // Additional focus/selection methods
    void RequestFocus();
    void EstablishOwningRelationship();

    // Disable/enable functionality
    void SetDisabled(bool disabled);

    // Box data methods
    void SetBoxCount(size_t count);
    void SetCurrentBox(int boxIndex);
    int GetCurrentBox() const { return currentBox; }

    // Set the data for a specific Pokémon slot
    void SetPokemonData(int boxIndex, int slotIndex, const BoxPokemonData& data);
    // Set an entire box of data
    void SetBoxData(int boxIndex, const std::vector<BoxPokemonData>& boxData);
    // Get the Pokémon data at the specified box and slot
    BoxPokemonData GetPokemonData(int boxIndex, int slotIndex) const;

    // Update the grid view with current box data
    void UpdateGridFromCurrentBox();

    // Event handlers
    void SetOnSelectionChanged(std::function<void(int, int)> callback) { onSelectionChangedCallback = callback; }
    void SetOnPokemonMoved(std::function<void(int, int)> callback) { onPokemonMovedCallback = callback; }

    // Handle A button press on selected item
    bool HandleSelectInput(const u64 keys_down);
};

}  // namespace pksm::ui