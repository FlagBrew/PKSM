#pragma once
#include <pu/Plutonium>
#include "gui/AnalogStickHandler.hpp"
#include <functional>
#include <memory>

class DirectionalInputHandler {
private:
    std::unique_ptr<AnalogStickHandler> analogHandler;
    
    // Movement callbacks
    std::function<void()> onMoveLeft;
    std::function<void()> onMoveRight;
    std::function<void()> onMoveUp;
    std::function<void()> onMoveDown;

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
        // First check for d-pad input (immediate response)
        if (down & HidNpadButton_Left) {
            if (onMoveLeft) onMoveLeft();
            return true;
        }
        else if (down & HidNpadButton_Right) {
            if (onMoveRight) onMoveRight();
            return true;
        }
        else if (down & HidNpadButton_Up) {
            if (onMoveUp) onMoveUp();
            return true;
        }
        else if (down & HidNpadButton_Down) {
            if (onMoveDown) onMoveDown();
            return true;
        }

        // Then check for analog stick input (with timing)
        return analogHandler->HandleInput(held);
    }
}; 