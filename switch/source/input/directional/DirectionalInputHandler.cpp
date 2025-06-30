#include "input/directional/DirectionalInputHandler.hpp"

#include "input/directional/InputDebounceManager.hpp"

namespace pksm::input {

DirectionalInputHandler::DirectionalInputHandler() : analogHandler(AnalogStickHandler::New()) {
    // Set up analog handler callbacks
    analogHandler->SetOnMoveLeft([this]() {
        if (onMoveLeft)
            onMoveLeft();
    });
    analogHandler->SetOnMoveRight([this]() {
        if (onMoveRight)
            onMoveRight();
    });
    analogHandler->SetOnMoveUp([this]() {
        if (onMoveUp)
            onMoveUp();
    });
    analogHandler->SetOnMoveDown([this]() {
        if (onMoveDown)
            onMoveDown();
    });
}

bool DirectionalInputHandler::HandleInput(u64 down, u64 held) {
    // First check for d-pad input with timing
    if (dpadMoveStatus != DPadMoveStatus::None) {
        const auto curTime = std::chrono::steady_clock::now();
        const auto timeDiffMs =
            std::chrono::duration_cast<std::chrono::milliseconds>(curTime - dpadMoveStartTime).count();

        if (timeDiffMs >= DPAD_MOVE_WAIT_TIME_MS) {
            // Execute the movement based on direction
            switch (dpadMoveStatus) {
                case DPadMoveStatus::WaitingLeft:
                    if (onMoveLeft)
                        onMoveLeft();
                    break;

                case DPadMoveStatus::WaitingRight:
                    if (onMoveRight)
                        onMoveRight();
                    break;

                case DPadMoveStatus::WaitingUp:
                    if (onMoveUp)
                        onMoveUp();
                    break;

                case DPadMoveStatus::WaitingDown:
                    if (onMoveDown)
                        onMoveDown();
                    break;

                default:  // This handles DPadMoveStatus::None
                    break;
            }

            // Update global input timestamp for debouncing across all handlers
            InputDebounceManager::RegisterInputProcessed();

            // Reset state to allow next movement
            dpadMoveStatus = DPadMoveStatus::None;
            return true;
        }
        return true;  // Input is being handled, even if we're still waiting
    }
    // Check if enough time has passed since last input from any handler
    else if (InputDebounceManager::CanProcessInput(DPAD_MOVE_WAIT_TIME_MS)) {
        // Start new d-pad movement if not already waiting
        if (down & HidNpadButton_Left) {
            dpadMoveStatus = DPadMoveStatus::WaitingLeft;
            dpadMoveStartTime = std::chrono::steady_clock::now();
            return true;
        } else if (down & HidNpadButton_Right) {
            dpadMoveStatus = DPadMoveStatus::WaitingRight;
            dpadMoveStartTime = std::chrono::steady_clock::now();
            return true;
        } else if (down & HidNpadButton_Up) {
            dpadMoveStatus = DPadMoveStatus::WaitingUp;
            dpadMoveStartTime = std::chrono::steady_clock::now();
            return true;
        } else if (down & HidNpadButton_Down) {
            dpadMoveStatus = DPadMoveStatus::WaitingDown;
            dpadMoveStartTime = std::chrono::steady_clock::now();
            return true;
        }
    } else {
        // Clear state if no relevant input is detected
        dpadMoveStatus = DPadMoveStatus::None;
    }

    // Then check for analog stick input (with its own timing)
    return analogHandler->HandleInput(held);
}

void DirectionalInputHandler::ClearState() {
    dpadMoveStatus = DPadMoveStatus::None;
    analogHandler->ClearState();
}

}  // namespace pksm::input
