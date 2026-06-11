#pragma once

#include <functional>
#include <pu/Plutonium>
#include <string>

namespace pksm::ui {

class BoxNavigationButton : public pu::ui::elm::Element {
private:
    // Layout constants
    static constexpr pu::i32 ELEMENT_SPACING = 10;  // Spacing between glyph elements
    static constexpr pu::ui::Color DEFAULT_COLOR = pu::ui::Color(255, 255, 255, 255);  // White
    static constexpr pu::ui::Color PRESSED_COLOR = pu::ui::Color(180, 180, 180, 255);  // Gray when pressed

    // Position
    pu::i32 x;
    pu::i32 y;

    // State
    bool isPressed;

    // Direction (left or right)
    enum class Direction { Left, Right };
    Direction direction;

    // Components
    pu::ui::elm::TextBlock::Ref chevronText;  // Chevron indicator
    pu::ui::elm::TextBlock::Ref buttonText;  // L/R button indicator

    // Layout update
    void UpdateLayout();

public:
    BoxNavigationButton(const pu::i32 x, const pu::i32 y, const Direction direction);
    ~BoxNavigationButton() = default;
    PU_SMART_CTOR(BoxNavigationButton)

    // Element implementation
    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void
    OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;

    // State management
    void SetPressed(bool pressed);
    bool IsPressed() const { return isPressed; }

    // Position
    void SetX(pu::i32 newX);
    void SetY(pu::i32 newY);
    void SetPosition(pu::i32 newX, pu::i32 newY);

    // Color
    void SetColor(const pu::ui::Color& color);

    // Accessors for the direction
    bool IsLeftButton() const { return direction == Direction::Left; }
    bool IsRightButton() const { return direction == Direction::Right; }

    // Factory methods for convenience
    static BoxNavigationButton::Ref NewLeftButton(const pu::i32 x, const pu::i32 y) {
        return BoxNavigationButton::New(x, y, Direction::Left);
    }

    static BoxNavigationButton::Ref NewRightButton(const pu::i32 x, const pu::i32 y) {
        return BoxNavigationButton::New(x, y, Direction::Right);
    }
};

}  // namespace pksm::ui