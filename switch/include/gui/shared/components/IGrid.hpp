#pragma once

#include <map>
#include <memory>
#include <pu/Plutonium>

#include "gui/shared/components/ShakeDirection.hpp"
#include "gui/shared/components/ShakeableWithOutline.hpp"

namespace pksm::ui {
class IGrid : public virtual pu::ui::elm::Element {
protected:
    // Common grid properties
    size_t itemsPerRow;
    size_t selectedIndex = 0;

    // Edge behaviors for shaking
    std::map<ShakeDirection, bool> shouldShakeOnEdge = {
        {ShakeDirection::LEFT, true},
        {ShakeDirection::RIGHT, true},
        {ShakeDirection::UP, true},
        {ShakeDirection::DOWN, true}
    };

    // Protected methods that implementations must define
    // These express the grid layout specifics

    /**
     * Get the width of a single grid item
     */
    virtual pu::i32 GetItemWidth() const = 0;

    /**
     * Get the height of a single grid item
     */
    virtual pu::i32 GetItemHeight() const = 0;

    /**
     * Get the horizontal spacing between grid items
     */
    virtual pu::i32 GetHorizontalSpacing() const = 0;

    /**
     * Get the vertical spacing between grid items
     */
    virtual pu::i32 GetVerticalSpacing() const = 0;

public:
    IGrid(const size_t itemsPerRow, const std::map<ShakeDirection, bool>& edgeShakeBehavior = {})
      : itemsPerRow(itemsPerRow) {
        // Override any edge behaviors specified
        for (const auto& [direction, shouldShake] : edgeShakeBehavior) {
            shouldShakeOnEdge[direction] = shouldShake;
        }
    }

    virtual ~IGrid() = default;

    // Grid state queries with default implementations
    virtual bool IsFirstInRow(size_t index) const { return index % itemsPerRow == 0; }

    virtual bool IsLastInRow(size_t index) const {
        return (index + 1) % itemsPerRow == 0 || index == GetItemCount() - 1;
    }

    virtual bool IsInFirstRow(size_t index) const { return index < itemsPerRow; }

    virtual bool IsInLastRow(size_t index) const {
        size_t totalRows = CalculateNumberOfRows(GetItemCount());
        size_t rowIndex = index / itemsPerRow;
        return rowIndex == totalRows - 1;
    }

    // Abstract methods that must be implemented
    virtual size_t GetItemCount() const = 0;
    virtual ShakeableWithOutline::Ref GetItemAtIndex(size_t index) = 0;
    virtual void SetSelectedIndex(size_t index) = 0;

    // Default navigation implementations that use ShakeableWithOutline
    virtual void MoveLeft() {
        if (IsFirstInRow(selectedIndex)) {
            if (shouldShakeOnEdge[ShakeDirection::LEFT]) {
                auto item = GetItemAtIndex(selectedIndex);
                if (item) {
                    item->shakeOutOfBounds(ShakeDirection::LEFT);
                }
            }
        } else {
            SetSelectedIndex(selectedIndex - 1);
        }
    }

    virtual void MoveRight() {
        if (IsLastInRow(selectedIndex)) {
            if (shouldShakeOnEdge[ShakeDirection::RIGHT]) {
                auto item = GetItemAtIndex(selectedIndex);
                if (item) {
                    item->shakeOutOfBounds(ShakeDirection::RIGHT);
                }
            }
        } else if (selectedIndex < GetItemCount() - 1) {
            SetSelectedIndex(selectedIndex + 1);
        }
    }

    virtual void MoveUp() {
        if (IsInFirstRow(selectedIndex)) {
            if (shouldShakeOnEdge[ShakeDirection::UP]) {
                auto item = GetItemAtIndex(selectedIndex);
                if (item) {
                    item->shakeOutOfBounds(ShakeDirection::UP);
                }
            }
        } else {
            SetSelectedIndex(selectedIndex - itemsPerRow);
        }
    }

    virtual void MoveDown() {
        if (IsInLastRow(selectedIndex)) {
            if (shouldShakeOnEdge[ShakeDirection::DOWN]) {
                auto item = GetItemAtIndex(selectedIndex);
                if (item) {
                    item->shakeOutOfBounds(ShakeDirection::DOWN);
                }
            }
        } else if (selectedIndex + itemsPerRow < GetItemCount()) {
            SetSelectedIndex(selectedIndex + itemsPerRow);
        }
    }

    // Getters for common properties
    size_t GetGridSelectedIndex() const { return selectedIndex; }

    // Configure edge behavior
    void SetEdgeShakeBehavior(ShakeDirection direction, bool shouldShake) {
        shouldShakeOnEdge[direction] = shouldShake;
    }

    // Layout helper methods

    /**
     * Calculate the row and column for an item at the given index
     *
     * @param index The item index
     * @return A pair of (row, column)
     */
    std::pair<size_t, size_t> CalculateRowAndColumn(size_t index) const {
        size_t row = index / itemsPerRow;
        size_t col = index % itemsPerRow;
        return std::make_pair(row, col);
    }

    /**
     * Calculate the position of a grid item using the grid's properties
     * Note: This method must be called from a non-const context since Element's GetX/GetY aren't const
     *
     * @param index The index of the item in the grid
     * @return A pair containing the calculated (x,y) coordinates
     */
    std::pair<pu::i32, pu::i32> CalculateItemPosition(size_t index) {
        auto rowAndCol = CalculateRowAndColumn(index);
        size_t row = rowAndCol.first;
        size_t col = rowAndCol.second;

        // Use Element's GetX/GetY directly
        pu::i32 x = GetX() + static_cast<pu::i32>(col) * (GetItemWidth() + GetHorizontalSpacing());
        pu::i32 y = GetY() + static_cast<pu::i32>(row) * (GetItemHeight() + GetVerticalSpacing());

        return std::make_pair(x, y);
    }

    /**
     * Calculate the default width needed for the grid based on the grid's properties
     *
     * @return The total width of the grid
     */
    pu::i32 CalculateDefaultGridWidth() const {
        // Width = (items per row * item width) + ((items per row - 1) * spacing)
        return (GetItemWidth() * static_cast<pu::i32>(itemsPerRow)) +
            (GetHorizontalSpacing() * static_cast<pu::i32>(itemsPerRow - 1));
    }

    /**
     * Calculate the default height needed for the grid based on the grid's properties and item count
     *
     * @param itemCount Total number of items in the grid (defaults to GetItemCount())
     * @return The total height of the grid
     */
    pu::i32 CalculateDefaultGridHeight(size_t itemCount = 0) const {
        // Use provided item count or get from the grid
        if (itemCount == 0) {
            itemCount = GetItemCount();
        }

        // Calculate number of rows needed
        size_t numberOfRows = CalculateNumberOfRows(itemCount);

        // Height = (number of rows * item height) + ((number of rows - 1) * spacing)
        return (GetItemHeight() * static_cast<pu::i32>(numberOfRows)) +
            (GetVerticalSpacing() * static_cast<pu::i32>(numberOfRows - 1));
    }

    /**
     * Calculate the number of rows needed for a given number of items
     *
     * @param totalItems Total number of items to display
     * @return The number of rows needed
     */
    size_t CalculateNumberOfRows(size_t totalItems) const {
        // Calculate rows needed, rounding up for partial rows
        return (totalItems + itemsPerRow - 1) / itemsPerRow;
    }
};
}  // namespace pksm::ui