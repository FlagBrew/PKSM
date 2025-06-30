#pragma once

#include <functional>
#include <memory>
#include <pu/Plutonium>
#include <vector>

#include "data/providers/interfaces/ITitleDataProvider.hpp"
#include "gui/render/PatternRenderer.hpp"
#include "gui/screens/title-load-screen/sub-components/game-list/ConsoleGameList.hpp"
#include "gui/screens/title-load-screen/sub-components/game-list/CustomGameList.hpp"
#include "gui/screens/title-load-screen/sub-components/game-list/EmulatorGameList.hpp"
#include "gui/screens/title-load-screen/sub-components/game-list/GameListCommon.hpp"
#include "gui/screens/title-load-screen/sub-components/game-list/IGameList.hpp"
#include "gui/screens/title-load-screen/sub-components/game-list/TriggerButton.hpp"
#include "gui/shared/UIConstants.hpp"
#include "gui/shared/components/FocusableImage.hpp"
#include "gui/shared/interfaces/IHelpProvider.hpp"
#include "input/directional/DirectionalInputHandler.hpp"
#include "input/visual-feedback/FocusManager.hpp"
#include "input/visual-feedback/interfaces/IFocusable.hpp"
#include "titles/Title.hpp"

namespace pksm::ui {

class GameList : public pu::ui::elm::Element, public IFocusable, public IHelpProvider {
private:
    // Layout constants
    static constexpr u32 CORNER_RADIUS = 30;  // Radius for rounded corners

    // Navigation constants
    static const std::vector<GameListInfo> NAVIGATION_ORDER;

    // Background margin constants
    static constexpr u32 FULL_WIDTH_LIST_PADDING_LEFT = 72;  // Left padding of the component
    static constexpr u32 CONSOLE_LIST_PADDING_LEFT = 80;  // Left padding of the component
    static constexpr u32 PADDING_RIGHT = 80;  // Right padding of the component
    static constexpr u32 PADDING_TOP = 20;  // Space between background and section titles
    static constexpr u32 PADDING_BOTTOM = 120;  // Space below game icons

    // Section spacing constants
    static constexpr u32 SECTION_TITLE_SPACING = 70;  // Space between section titles and game icons
    static constexpr u32 GAME_OUTLINE_PADDING = 15;  // Padding for the selection outline

    // Trigger button constants
    static constexpr u32 TRIGGER_BUTTON_WIDTH = 180;  // Width of L/R trigger buttons
    static constexpr u32 TRIGGER_BUTTON_HEIGHT = 100;  // Height of L/R trigger buttons
    static constexpr u32 TRIGGER_HORIZONTAL_OFFSET = 16;  // Offset from component edges
    static constexpr u32 TRIGGER_VERTICAL_OFFSET = 28;  // Offset from component edges

    // Game list colors
    static constexpr pu::ui::Color GAME_LIST_BACKGROUND_COLOR = pu::ui::Color(40, 51, 135, 255);

    // State
    bool focused = false;
    bool disabled = false;
    pu::ui::Color backgroundColor;
    std::function<void()> onSelectionChangedCallback;
    std::function<void()> onTouchSelectCallback;
    std::function<void()> onSelectCallback;
    std::function<void()> onGameListChangedCallback;
    std::function<void()> onShouldUpdateHelpTextCallback;

    // Focus consolidation
    void FocusLeftTrigger();
    void FocusRightTrigger();
    void FocusActiveGameList();

    // Position tracking
    pu::i32 x;  // Component's x position
    pu::i32 y;  // Component's y position
    pu::i32 width;  // Component's width
    pu::i32 height;  // Component's height
    ITitleDataProvider::Ref titleProvider;
    size_t currentGameListIndex;

    // Focus management
    input::FocusManager::Ref consoleGameListManager;
    input::FocusManager::Ref customGameListManager;
    input::FocusManager::Ref emulatorGameListManager;

    // Core components
    pu::ui::Container::Ref container;
    pksm::input::DirectionalInputHandler directionalInputHandler;

    // UI Elements
    ui::TriggerButton::Ref leftTrigger;
    ui::TriggerButton::Ref rightTrigger;
    ui::render::PatternBackground::Ref background;
    std::vector<IGameList::Ref> gameLists;  // All available game lists
    IGameList::Ref activeGameList;  // Currently active game list
    GameListType activeGameListType;

    AccountUid currentUserId;  // Add current user ID field

    // Helper methods
    void SwitchToNextGameList(bool forward);
    void SetupGameListCallbacks(IGameList::Ref gameList);
    void UpdateGameListData();
    void CreateTriggerButtons();
    void UpdateConsoleGameListData();  // New helper method

public:
    GameList(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        input::FocusManager::Ref parentFocusManager,
        ITitleDataProvider::Ref titleProvider,
        const AccountUid& initialUserId  // Add initial user ID parameter
    );
    PU_SMART_CTOR(GameList)

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
    void SetFocusManager(std::shared_ptr<input::FocusManager> manager);

    // Public interface
    void SetBackgroundColor(const pu::ui::Color& color);
    titles::Title::Ref GetSelectedTitle() const;
    bool IsGameListDependentOnUser() const;
    void SetOnSelectionChanged(std::function<void()> callback) { onSelectionChangedCallback = callback; }
    void SetOnTouchSelect(std::function<void()> callback) { onTouchSelectCallback = callback; }
    void SetOnSelect(std::function<void()> callback) { onSelectCallback = callback; }
    void SetOnGameListChanged(std::function<void()> callback) { onGameListChangedCallback = callback; }
    void SetOnShouldUpdateHelpText(std::function<void()> callback) { onShouldUpdateHelpTextCallback = callback; }
    void SetDisabled(bool disabled);

    // Focus management
    bool ShouldResignDownFocus() const;
    bool ShouldResignUpFocus() const;

    // Add method to update current user
    void OnAccountChanged(const AccountUid& newUserId);

    // IHelpProvider implementation
    std::vector<HelpItem> GetHelpItems() const override;
};
}  // namespace pksm::ui