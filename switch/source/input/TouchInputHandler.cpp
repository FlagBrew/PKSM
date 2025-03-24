#include "input/TouchInputHandler.hpp"

#include "utils/Logger.hpp"

namespace pksm::input {

// Initialize static member
bool TouchInputHandler::anyTouchActive = false;

TouchInputHandler::TouchInputHandler() : isPressed(false) {}

bool TouchInputHandler::HandleInput(
    const pu::ui::TouchPoint& touch_pos,
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height
) {
    if (!touch_pos.IsEmpty()) {
        // Touch is active
        if (!isPressed && touch_pos.HitsRegion(x, y, width, height)) {
            // Touch down inside the region
            // Only register if no other touch is active or this is already pressed
            if (!anyTouchActive || isPressed) {
                isPressed = true;
                anyTouchActive = true;
                LOG_DEBUG("[TouchInputHandler] Touch down inside region");

                if (onTouchDownCallback) {
                    onTouchDownCallback();
                }
                return true;
            }
        } else if (isPressed && !touch_pos.HitsRegion(x, y, width, height)) {
            // Touch moved outside while pressed
            LOG_DEBUG("[TouchInputHandler] Touch moved outside while pressed");
            isPressed = false;
            return false;
        }
    } else if (isPressed) {
        // Touch released
        LOG_DEBUG("[TouchInputHandler] Touch released");
        isPressed = false;
        anyTouchActive = false;  // Reset global touch state when touch is released

        // Only trigger if touch ends inside the region (last known state was inside)
        if (onTouchUpInsideCallback) {
            LOG_DEBUG("[TouchInputHandler] Executing touch up inside callback");
            onTouchUpInsideCallback();
            return true;
        }
    } else if (touch_pos.IsEmpty() && anyTouchActive) {
        // Touch released somewhere else, reset global state
        anyTouchActive = false;
    }

    return false;
}

bool TouchInputHandler::IsPressed() const {
    return isPressed;
}

bool TouchInputHandler::IsAnyTouchActive() {
    return anyTouchActive;
}

void TouchInputHandler::ResetTouchState() {
    anyTouchActive = false;
}

}  // namespace pksm::input