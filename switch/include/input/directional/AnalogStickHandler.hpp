#pragma once
#include <chrono>
#include <functional>
#include <pu/Plutonium>

namespace pksm::input {
class AnalogStickHandler {
private:
    static constexpr s64 DEFAULT_MOVE_WAIT_TIME_MS = 80;

    enum class MoveStatus { None, WaitingLeft, WaitingRight, WaitingUp, WaitingDown };

    MoveStatus moveStatus = MoveStatus::None;
    std::chrono::time_point<std::chrono::steady_clock> moveStartTime;
    s64 moveWaitTimeMs;

    // Movement callbacks
    std::function<void()> onMoveLeft;
    std::function<void()> onMoveRight;
    std::function<void()> onMoveUp;
    std::function<void()> onMoveDown;

public:
    AnalogStickHandler(s64 waitTimeMs = DEFAULT_MOVE_WAIT_TIME_MS) : moveWaitTimeMs(waitTimeMs) {}
    PU_SMART_CTOR(AnalogStickHandler)

    // Set movement callbacks
    void SetOnMoveLeft(std::function<void()> callback) { onMoveLeft = callback; }
    void SetOnMoveRight(std::function<void()> callback) { onMoveRight = callback; }
    void SetOnMoveUp(std::function<void()> callback) { onMoveUp = callback; }
    void SetOnMoveDown(std::function<void()> callback) { onMoveDown = callback; }

    // Handle input - returns true if input was handled
    bool HandleInput(u64 held);

    void ClearState();
};
}  // namespace pksm::input
