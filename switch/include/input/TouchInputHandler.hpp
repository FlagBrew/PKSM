#pragma once

#include <functional>
#include <pu/Plutonium>

namespace pksm::input {

class TouchInputHandler {
private:
    bool isPressed;
    std::function<void()> onTouchUpInsideCallback;
    std::function<void()> onTouchDownCallback;

    // Static variable to track if any touch is currently active
    static bool anyTouchActive;

public:
    TouchInputHandler();

    void SetOnTouchUpInside(std::function<void()> callback) { onTouchUpInsideCallback = callback; }
    void SetOnTouchDown(std::function<void()> callback) { onTouchDownCallback = callback; }

    bool HandleInput(
        const pu::ui::TouchPoint& touch_pos,
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height
    );
    bool IsPressed() const;

    // Static methods to check and reset touch state
    static bool IsAnyTouchActive();
    static void ResetTouchState();
};

}  // namespace pksm::input