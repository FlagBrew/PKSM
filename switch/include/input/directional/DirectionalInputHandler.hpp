#pragma once
#include <chrono>
#include <functional>
#include <memory>
#include <pu/Plutonium>

#include "input/directional/AnalogStickHandler.hpp"

namespace pksm::input {
class DirectionalInputHandler {
private:
    AnalogStickHandler::Ref analogHandler;

    // Movement callbacks
    std::function<void()> onMoveLeft;
    std::function<void()> onMoveRight;
    std::function<void()> onMoveUp;
    std::function<void()> onMoveDown;

    // D-pad timing mechanism
    enum class DPadMoveStatus { None, WaitingLeft, WaitingRight, WaitingUp, WaitingDown };

    DPadMoveStatus dpadMoveStatus = DPadMoveStatus::None;
    std::chrono::time_point<std::chrono::steady_clock> dpadMoveStartTime;
    static constexpr s64 DPAD_MOVE_WAIT_TIME_MS = 80;  // Same as analog stick default

public:
    DirectionalInputHandler();

    // Set movement callbacks - these will be called for both d-pad and analog stick
    void SetOnMoveLeft(std::function<void()> callback) { onMoveLeft = callback; }
    void SetOnMoveRight(std::function<void()> callback) { onMoveRight = callback; }
    void SetOnMoveUp(std::function<void()> callback) { onMoveUp = callback; }
    void SetOnMoveDown(std::function<void()> callback) { onMoveDown = callback; }

    // Handle input - returns true if input was handled
    bool HandleInput(u64 down, u64 held);

    void ClearState();
};
}  // namespace pksm::input
