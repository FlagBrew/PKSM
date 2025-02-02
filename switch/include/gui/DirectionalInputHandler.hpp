#pragma once
#include <pu/Plutonium>
#include "gui/AnalogStickHandler.hpp"
#include <functional>
#include <memory>
#include <chrono>

class DirectionalInputHandler {
private:
    std::unique_ptr<AnalogStickHandler> analogHandler;
    
    // Movement callbacks
    std::function<void()> onMoveLeft;
    std::function<void()> onMoveRight;
    std::function<void()> onMoveUp;
    std::function<void()> onMoveDown;

    // D-pad timing mechanism
    enum class DPadMoveStatus {
        None,
        WaitingLeft,
        WaitingRight,
        WaitingUp,
        WaitingDown
    };

    DPadMoveStatus dpadMoveStatus = DPadMoveStatus::None;
    std::chrono::time_point<std::chrono::steady_clock> dpadMoveStartTime;
    static constexpr s64 DPAD_MOVE_WAIT_TIME_MS = 150;  // Same as analog stick default

public:
    DirectionalInputHandler() : analogHandler(std::make_unique<AnalogStickHandler>()) {
        // Set up analog handler callbacks
        analogHandler->SetOnMoveLeft([this]() { if (onMoveLeft) onMoveLeft(); });
        analogHandler->SetOnMoveRight([this]() { if (onMoveRight) onMoveRight(); });
        analogHandler->SetOnMoveUp([this]() { if (onMoveUp) onMoveUp(); });
        analogHandler->SetOnMoveDown([this]() { if (onMoveDown) onMoveDown(); });
    }

    // Set movement callbacks - these will be called for both d-pad and analog stick
    void SetOnMoveLeft(std::function<void()> callback) { onMoveLeft = callback; }
    void SetOnMoveRight(std::function<void()> callback) { onMoveRight = callback; }
    void SetOnMoveUp(std::function<void()> callback) { onMoveUp = callback; }
    void SetOnMoveDown(std::function<void()> callback) { onMoveDown = callback; }

    // Handle input - returns true if input was handled
    bool HandleInput(u64 down, u64 held) {
        // First check for d-pad input with timing
        if (dpadMoveStatus != DPadMoveStatus::None) {
            const auto curTime = std::chrono::steady_clock::now();
            const auto timeDiffMs = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - dpadMoveStartTime).count();
            
            if (timeDiffMs >= DPAD_MOVE_WAIT_TIME_MS) {
                // Execute the movement based on direction
                switch (dpadMoveStatus) {
                    case DPadMoveStatus::WaitingLeft:
                        if (onMoveLeft) onMoveLeft();
                        break;
                        
                    case DPadMoveStatus::WaitingRight:
                        if (onMoveRight) onMoveRight();
                        break;
                        
                    case DPadMoveStatus::WaitingUp:
                        if (onMoveUp) onMoveUp();
                        break;
                        
                    case DPadMoveStatus::WaitingDown:
                        if (onMoveDown) onMoveDown();
                        break;

                    default:  // This handles DPadMoveStatus::None
                        break;
                }
                
                // Reset state to allow next movement
                dpadMoveStatus = DPadMoveStatus::None;
                return true;
            }
            return true;  // Input is being handled, even if we're still waiting
        }
        // Start new d-pad movement if not already waiting
        else if (down & HidNpadButton_Left) {
            dpadMoveStatus = DPadMoveStatus::WaitingLeft;
            dpadMoveStartTime = std::chrono::steady_clock::now();
            return true;
        }
        else if (down & HidNpadButton_Right) {
            dpadMoveStatus = DPadMoveStatus::WaitingRight;
            dpadMoveStartTime = std::chrono::steady_clock::now();
            return true;
        }
        else if (down & HidNpadButton_Up) {
            dpadMoveStatus = DPadMoveStatus::WaitingUp;
            dpadMoveStartTime = std::chrono::steady_clock::now();
            return true;
        }
        else if (down & HidNpadButton_Down) {
            dpadMoveStatus = DPadMoveStatus::WaitingDown;
            dpadMoveStartTime = std::chrono::steady_clock::now();
            return true;
        }

        // Then check for analog stick input (with its own timing)
        return analogHandler->HandleInput(held);
    }
}; 