#pragma once
#include <chrono>
#include <pu/Plutonium>

namespace pksm::input {

// Manages debouncing across all input handlers
class InputDebounceManager {
private:
    static std::chrono::time_point<std::chrono::steady_clock> lastInputTime;

public:
    // Check if enough time has passed since last input
    static bool CanProcessInput(s64 waitTimeMs);

    // Update the timestamp of the last processed input
    static void RegisterInputProcessed();
};

}  // namespace pksm::input