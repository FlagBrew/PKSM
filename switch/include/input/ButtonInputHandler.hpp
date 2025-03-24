#pragma once

#include <functional>
#include <switch.h>
#include <unordered_map>

namespace pksm::input {

class ButtonInputHandler {
public:
    ButtonInputHandler();

    // Main input handling method - processes input against all registered buttons
    // Returns true if any button was handled
    bool HandleInput(const u64 keys_down, const u64 keys_up, const u64 keys_held);

    // Register a button with its callbacks and condition for when it should be processed
    void RegisterButton(
        const u64 button_mask,
        const std::function<void()>& down_callback = nullptr,
        const std::function<void()>& up_callback = nullptr,
        const std::function<bool()>& condition = nullptr
    );

    // Check if a specific button is currently pressed
    bool IsButtonPressed(const u64 button_mask) const;

private:
    // Track press state for each button
    std::unordered_map<u64, bool> buttonStates;

    // Callbacks for each button
    std::unordered_map<u64, std::function<void()>> buttonDownCallbacks;
    std::unordered_map<u64, std::function<void()>> buttonUpCallbacks;

    // Conditions for when each button should be processed
    std::unordered_map<u64, std::function<bool()>> buttonConditions;
};

}  // namespace pksm::input