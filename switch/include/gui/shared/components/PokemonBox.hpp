#pragma once

#include <pu/Plutonium>
#include <set>

#include "gui/shared/components/BoxGrid.hpp"
#include "gui/shared/components/BoxNavigationButton.hpp"
#include "gui/shared/components/BoxSpacesButton.hpp"
#include "gui/shared/components/PillTextContainer.hpp"
#include "gui/shared/components/StaticOutline.hpp"
#include "input/ButtonInputHandler.hpp"
#include "input/visual-feedback/FocusManager.hpp"
#include "input/visual-feedback/SelectionManager.hpp"
#include "utils/PokemonSpriteManager.hpp"

namespace pksm::ui {

class PokemonBox : public pu::ui::elm::Element, public ISelectable {
private:
    // Layout constants
    static constexpr pu::i32 FRAME_PADDING = 10;  // Padding inside the frame
    static constexpr pu::i32 CORNER_RADIUS = 20;  // Corner radius for the rounded rectangle frame
    static constexpr pu::i32 BORDER_THICKNESS = 2;  // Border thickness
    static constexpr pu::i32 TOP_FRAME_HEIGHT = 120;  // Extra height for the top part of the frame
    static constexpr pu::i32 BOTTOM_FRAME_HEIGHT = 80;  // Extra height for the bottom part of the frame
    static constexpr pu::i32 NAVIGATION_SIDE_MARGIN = 25;  // Margin from frame edge to navigation elements
    static constexpr pu::i32 BOX_SPACES_BOTTOM_MARGIN = 15;  // Margin from bottom of frame to BoxSpaces button
    static constexpr pu::i32 BOX_COUNTER_RIGHT_MARGIN = 25;  // Right margin for box counter

    static constexpr pu::ui::Color DEFAULT_FRAME_COLOR = pu::ui::Color(0, 136, 170, 255);  // Bluish frame
    static constexpr pu::ui::Color DEFAULT_BORDER_COLOR = pu::ui::Color(26, 56, 66, 255);  // Light gray border
    static constexpr pu::ui::Color BUTTON_PRESSED_COLOR = pu::ui::Color(180, 180, 180, 255);
    // Defaults
    static constexpr pu::i32 DEFAULT_ITEMS_PER_ROW = 6;  // Default number of items per row
    static constexpr pu::i32 DEFAULT_NUMBER_OF_ROWS = 5;  // Default number of rows

    // Position and size
    pu::i32 x;
    pu::i32 y;

    // Colors
    pu::ui::Color frameColor;  // Color of the frame
    pu::ui::Color borderColor;  // Border color

    // State
    bool focused = false;
    bool selected = false;
    bool disabled = false;

    // Data storage
    std::vector<BoxData> boxes;  // All box data
    int currentBox = 0;  // Current displayed box

    // Components
    pu::ui::Container::Ref container;  // Container for child elements
    BoxGrid::Ref boxGrid;  // The internal BoxGrid component
    PillTextContainer::Ref boxNamePill;  // Pill container for box name
    BoxNavigationButton::Ref leftNavigationButton;  // Left (L) navigation button
    BoxNavigationButton::Ref rightNavigationButton;  // Right (R) navigation button
    BoxSpacesButton::Ref boxSpacesButton;  // Box Spaces button at the bottom
    pu::ui::elm::TextBlock::Ref boxCounterText;  // Box counter (current/total) text
    RectangularOutline::Ref borderOutline;  // Outline for the border

    // Focus and selection managers
    input::ButtonInputHandler buttonHandler;  // Button handler for L/R navigation
    input::FocusManager::Ref buttonGridFocusManager;
    input::SelectionManager::Ref buttonGridSelectionManager;
    pksm::input::DirectionalInputHandler boxGridDirectionalHandler;
    pksm::input::DirectionalInputHandler boxHeaderDirectionalHandler;
    pksm::input::DirectionalInputHandler boxFooterDirectionalHandler;

    // Event callbacks
    std::function<void(int, int)> onSelectionChangedCallback;  // (boxIndex, slotIndex)

    // Cached frame texture with cutout
    SDL_Texture* maskTexture = nullptr;

    // Create or update the mask texture when properties change
    void UpdateMaskTexture();

    // Navigate to next/previous box
    void NextBox();
    void PreviousBox();

    // Manage sprite cache when changing boxes
    void UpdateSpriteCache();
    void UpdateBoxGrid();

    // Update box name display
    void UpdateBoxNameText();

    // Initialize the Box Spaces button
    void InitializeBoxSpacesButton();

    // Update box counter text
    void UpdateBoxCounterText();

    void SetFocusManager(input::FocusManager::Ref focusManager);
    void SetSelectionManager(input::SelectionManager::Ref selectionManager);

public:
    PokemonBox(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 itemSize,
        input::FocusManager::Ref parentFocusManager,
        input::SelectionManager::Ref parentSelectionManager,
        const std::map<ShakeDirection, bool> shouldConsiderSideOutOfBounds = {},
        const pu::i32 numberOfRows = DEFAULT_NUMBER_OF_ROWS,
        const pu::i32 itemsPerRow = DEFAULT_ITEMS_PER_ROW
    );
    ~PokemonBox();
    PU_SMART_CTOR(PokemonBox)

    // Element implementation
    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void
    OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;

    // IFocusable implementation
    void SetFocused(bool focused) override;
    bool IsFocused() const override;

    // ISelectable implementation
    void SetSelected(bool selected) override;
    bool IsSelected() const override;

    // Focus/selection handling
    void EstablishOwningRelationship();

    // Box management methods
    void SetBoxCount(size_t count);
    void SetCurrentBox(int boxIndex);
    int GetCurrentBox() const { return currentBox; }

    // Pokemon data management
    void SetPokemonData(int boxIndex, int slotIndex, const BoxPokemonData& data);
    void SetBoxData(int boxIndex, const BoxData& boxData);
    BoxPokemonData GetPokemonData(int boxIndex, int slotIndex) const;

    // Current selection
    int GetSelectedSlot() const;

    // Enable/disable
    void SetDisabled(bool disabled);

    // Event handlers
    void SetOnSelectionChanged(std::function<void(int, int)> callback) { onSelectionChangedCallback = callback; }

    // Appearance configuration
    void SetColors(const pu::ui::Color& frameColor, const pu::ui::Color& borderColor);
};

}  // namespace pksm::ui