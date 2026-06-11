#include "input/directional/AnalogStickHandler.hpp"

#include "input/directional/InputDebounceManager.hpp"

bool pksm::input::AnalogStickHandler::HandleInput(u64 held) {
    // Check if we're waiting for movement
    if (moveStatus != MoveStatus::None) {
        const auto curTime = std::chrono::steady_clock::now();
        const auto timeDiffMs = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - moveStartTime).count();

        if (timeDiffMs >= moveWaitTimeMs) {
            // Execute the movement based on direction
            switch (moveStatus) {
                case MoveStatus::WaitingLeft:
                    if (onMoveLeft)
                        onMoveLeft();
                    break;

                case MoveStatus::WaitingRight:
                    if (onMoveRight)
                        onMoveRight();
                    break;

                case MoveStatus::WaitingUp:
                    if (onMoveUp)
                        onMoveUp();
                    break;

                case MoveStatus::WaitingDown:
                    if (onMoveDown)
                        onMoveDown();
                    break;

                default:
                    break;
            }

            // Update global input timestamp for debouncing across all handlers
            InputDebounceManager::RegisterInputProcessed();

            // Reset state to allow next movement
            moveStatus = MoveStatus::None;
            return true;
        }
        return true;  // Input is being handled, even if we're still waiting
    }
    // Check if enough time has passed since last input from any handler
    else if (InputDebounceManager::CanProcessInput(moveWaitTimeMs)) {
        // Start new movement if not already waiting
        if (held & (HidNpadButton_StickLLeft | HidNpadButton_StickRLeft)) {
            moveStatus = MoveStatus::WaitingLeft;
            moveStartTime = std::chrono::steady_clock::now();
            return true;
        } else if (held & (HidNpadButton_StickLRight | HidNpadButton_StickRRight)) {
            moveStatus = MoveStatus::WaitingRight;
            moveStartTime = std::chrono::steady_clock::now();
            return true;
        } else if (held & (HidNpadButton_StickLUp | HidNpadButton_StickRUp)) {
            moveStatus = MoveStatus::WaitingUp;
            moveStartTime = std::chrono::steady_clock::now();
            return true;
        } else if (held & (HidNpadButton_StickLDown | HidNpadButton_StickRDown)) {
            moveStatus = MoveStatus::WaitingDown;
            moveStartTime = std::chrono::steady_clock::now();
            return true;
        }
    }

    // Clear state if no relevant input is detected
    moveStatus = MoveStatus::None;
    return false;
}

void pksm::input::AnalogStickHandler::ClearState() {
    moveStatus = MoveStatus::None;
}