#include "input/HoldRepeat.hpp"

namespace {

s64 MsBetween(u64 from, u64 to) {
    return static_cast<s64>(armTicksToNs(to - from) / 1000000);
}

}  // namespace

bool pksm::input::HoldRepeat::Update(bool pressed, bool held) {
    const u64 now = armGetSystemTick();
    if (pressed) {
        heldSince = now;
        lastFire = now;
        return true;
    }
    if (!held) {
        heldSince = 0;
        return false;
    }
    if (heldSince == 0 || MsBetween(heldSince, now) < delayMs || MsBetween(lastFire, now) < periodMs) {
        return false;
    }
    lastFire = now;
    return true;
}

s64 pksm::input::HoldRepeat::HeldMs() const {
    return heldSince == 0 ? 0 : MsBetween(heldSince, armGetSystemTick());
}
