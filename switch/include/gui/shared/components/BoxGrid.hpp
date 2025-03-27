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
#include "input/directional/DirectionalInputHandler.hpp"
#include "input/visual-feedback/FocusManager.hpp"
#include "input/visual-feedback/SelectionManager.hpp"

namespace pksm::ui {

class BoxGrid : public ISelectable, public IGrid {
private:
    static constexpr pu::i32 GRID_SPACING = 8;  // Spacing between grid items

    // Position and size
    pu::i32 x;
    pu::i32 y;
    pu::i32 itemSize;
    pu::i32 numberOfRows;

    // State
    bool focused = false;
    bool selected = false;

    // Current box data
    BoxData currentBoxData;

    // Components
    pu::ui::Container::Ref container;
    std::vector<BoxItem::Ref> items;
    pksm::input::DirectionalInputHandler inputHandler;

    // Event callbacks
    std::function<void(int)> onSelectionChangedCallback;
    std::function<void(int)> onPokemonMovedCallback;

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
        const std::map<ShakeDirection, bool> shouldConsiderSideOutOfBounds,
        const pu::i32 numberOfRows,
        const pu::i32 itemsPerRow
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
    void EstablishOwningRelationship();

    // Set the data for a specific Pokémon slot
    void SetPokemonData(int slotIndex, const BoxPokemonData& data);

    // Set the entire box data
    void SetBoxData(const BoxData& boxData);

    // Get the Pokémon data at the specified slot
    BoxPokemonData GetPokemonData(int slotIndex) const;

    // Update the grid view with current box data
    void UpdateGridFromBoxData();

    // Event handlers
    void SetOnSelectionChanged(std::function<void(int)> callback) { onSelectionChangedCallback = callback; }
    void SetOnPokemonMoved(std::function<void(int)> callback) { onPokemonMovedCallback = callback; }

    // Get current selection
    size_t GetSelectedIndex() const { return selectedIndex; }

    bool ShouldResignUpFocus() const { return IsInFirstRow(selectedIndex); }
    bool ShouldResignDownFocus() const { return IsInLastRow(selectedIndex); }
};

}  // namespace pksm::ui