#pragma once
#include <functional>
#include <pu/Plutonium>

#include "gui/shared/components/PulsingOutline.hpp"
#include "input/directional/DirectionalInputHandler.hpp"
#include "input/visual-feedback/interfaces/IFocusable.hpp"

namespace pksm::ui {
class FocusableMenu : public pu::ui::elm::Menu, public IFocusable {
private:
    bool focused;
    pksm::ui::PulsingOutline::Ref outline;
    int lastPosition;  // Stores position when unfocusing
    std::vector<std::string> currentDataSource;  // Owns the current data source
    pksm::input::DirectionalInputHandler inputHandler;  // Handles directional input

protected:
    std::function<void()> onTouchSelectCallback;

public:
    FocusableMenu(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::ui::Color items_clr,
        const pu::ui::Color items_focus_clr,
        const pu::i32 items_height,
        const u32 items_to_show
    );
    PU_SMART_CTOR(FocusableMenu)

    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos)
        override;

    // IFocusable implementation
    void SetFocused(bool focused) override;
    bool IsFocused() const override;

    // Data source management
    void SetDataSource(const std::vector<std::string>& items);
    const std::vector<std::string>& GetDataSource() const;

    // Touch selection callback
    void SetOnTouchSelect(std::function<void()> callback);

    std::string GetSelectedItemText() const;

    // Const-correct version of GetSelectedIndex
    pu::i32 GetSelectedIndex() const { return Menu::selected_item_idx; }

private:
    // Position management methods
    void StorePosition();
    void RestorePosition();
    void ResetPosition();  // Called when data source changes

    // Movement methods
    void MoveUp();
    void MoveDown();
};
}  // namespace pksm::ui
