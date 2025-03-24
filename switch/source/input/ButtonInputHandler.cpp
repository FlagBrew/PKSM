#include "input/ButtonInputHandler.hpp"

#include "utils/Logger.hpp"

namespace pksm::input {

ButtonInputHandler::ButtonInputHandler() {}

bool ButtonInputHandler::HandleInput(const u64 keys_down, const u64 keys_up, const u64 keys_held) {
    bool handled = false;

    // Check for button presses
    for (auto& [buttonMask, isPressed] : buttonStates) {
        // Check if we should process this button
        auto conditionIt = buttonConditions.find(buttonMask);
        bool shouldProcess = true;

        // If there's a condition function, evaluate it
        if (conditionIt != buttonConditions.end() && conditionIt->second) {
            shouldProcess = conditionIt->second();
        }

        // Skip processing if the condition is not met
        if (!shouldProcess) {
            continue;
        }

        // Check for button down
        if (keys_down & buttonMask) {
            LOG_DEBUG("[ButtonInputHandler] Button down detected: " + std::to_string(buttonMask));
            isPressed = true;

            // Call the button down callback if it exists
            auto downIt = buttonDownCallbacks.find(buttonMask);
            if (downIt != buttonDownCallbacks.end() && downIt->second) {
                downIt->second();
            }

            handled = true;
        }

        // Check for button up, but only if the button was previously pressed
        if (keys_up & buttonMask && isPressed) {
            LOG_DEBUG("[ButtonInputHandler] Button up detected: " + std::to_string(buttonMask));
            isPressed = false;

            // Call the button up callback if it exists
            auto upIt = buttonUpCallbacks.find(buttonMask);
            if (upIt != buttonUpCallbacks.end() && upIt->second) {
                upIt->second();
            }

            handled = true;
        }
    }

    return handled;
}

void ButtonInputHandler::RegisterButton(
    const u64 button_mask,
    const std::function<void()>& down_callback,
    const std::function<void()>& up_callback,
    const std::function<bool()>& condition
) {
    // Register the button in our state map
    buttonStates[button_mask] = false;

    // Set callbacks if provided
    if (down_callback) {
        buttonDownCallbacks[button_mask] = down_callback;
    }

    if (up_callback) {
        buttonUpCallbacks[button_mask] = up_callback;
    }

    // Set condition if provided
    if (condition) {
        buttonConditions[button_mask] = condition;
    }
}

bool ButtonInputHandler::IsButtonPressed(const u64 button_mask) const {
    auto it = buttonStates.find(button_mask);
    if (it != buttonStates.end()) {
        return it->second;
    }
    return false;
}

}  // namespace pksm::input