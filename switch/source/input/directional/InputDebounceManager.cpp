#include "input/directional/InputDebounceManager.hpp"

namespace pksm::input {

// Initialize static members
std::chrono::time_point<std::chrono::steady_clock> InputDebounceManager::lastInputTime = std::chrono::steady_clock::now(
);

bool InputDebounceManager::CanProcessInput(s64 waitTimeMs) {
    const auto curTime = std::chrono::steady_clock::now();
    const auto timeDiffMs = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - lastInputTime).count();
    return timeDiffMs >= waitTimeMs;
}

void InputDebounceManager::RegisterInputProcessed() {
    lastInputTime = std::chrono::steady_clock::now();
}

}  // namespace pksm::input