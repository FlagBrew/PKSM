#pragma once

#include <pu/Plutonium>

#include "gui/shared/UIConstants.hpp"
#include "gui/shared/components/PulsingOutline.hpp"
#include "gui/shared/components/ShakeableWithOutline.hpp"
#include "gui/shared/interfaces/IHelpProvider.hpp"
#include "input/ButtonInputHandler.hpp"
#include "input/TouchInputHandler.hpp"
#include "input/visual-feedback/interfaces/IFocusable.hpp"

namespace pksm::ui {

class TriggerButton : public pu::ui::elm::Element,
                      public IFocusable,
                      public IHelpProvider,
                      public ShakeableWithOutline {
public:
    enum class Side { Left, Right };

    TriggerButton(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        const pu::i32 cornerRadius,
        const Side side,
        const pu::ui::Color& backgroundColor = TRIGGER_BUTTON_COLOR,
        const pu::ui::Color& textColor = pu::ui::Color(255, 255, 255, 255),
        const std::string& navigationText = "",
        const pu::ui::Color& navigationTextColor = pu::ui::Color(255, 255, 255, 255)
    );
    PU_SMART_CTOR(TriggerButton)

    // Customization methods
    void SetBackgroundColor(const pu::ui::Color& color);
    void SetTextColor(const pu::ui::Color& color);
    void SetNavigationText(const std::string& text);
    void SetNavigationTextColor(const pu::ui::Color& color);

    // Touch selection callback
    void SetOnTouchSelect(std::function<void()> callback) { onTouchSelectCallback = callback; }
    void SetOnSelect(std::function<void()> callback) { onSelectCallback = callback; }

    bool IsFocused() const override;

    // Element implementation
    pu::i32 GetX() override { return x; }
    pu::i32 GetY() override { return y; }
    pu::i32 GetWidth() override { return width; }
    pu::i32 GetHeight() override { return height; }
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void
    OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;

    std::vector<HelpItem> GetHelpItems() const override;

private:
    void UpdateTextPosition();

    // IFocusable implementation
    void SetFocused(bool focus) override;

    void OnTriggerButtonPressed();
    void OnTriggerButtonReleased();

    pu::i32 x;
    pu::i32 y;
    pu::i32 width;
    pu::i32 height;
    pu::i32 cornerRadius;
    Side side;
    pu::ui::Color backgroundColor;
    pu::ui::Color textColor;
    pu::ui::elm::TextBlock::Ref textBlock;
    pu::ui::elm::TextBlock::Ref navigationTextBlock;
    std::string navigationText;
    pu::ui::Color navigationTextColor;
    pu::i32 outlinePadding;
    bool focused;
    std::function<void()> onTouchSelectCallback;
    std::function<void()> onSelectCallback;
    std::shared_ptr<input::FocusManager> focusManager;
    pksm::input::TouchInputHandler touchHandler;
    pksm::input::ButtonInputHandler buttonHandler;

    static constexpr u32 TRIGGER_BUTTON_GLYPH_Y_OFFSET = -7;
    static constexpr u32 TRIGGER_BUTTON_OUTLINE_PADDING = 15;
    static constexpr u32 NAVIGATION_TEXT_PADDING = 16;  // Padding between button and navigation text
    static constexpr pu::ui::Color TRIGGER_BUTTON_COLOR = pu::ui::Color(135, 135, 135, 255);
    static constexpr pu::ui::Color TRIGGER_BUTTON_COLOR_PRESSED = pu::ui::Color(70, 70, 70, 255);
};
}  // namespace pksm::ui