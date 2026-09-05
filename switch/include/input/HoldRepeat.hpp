#pragma once

#include <switch.h>

namespace pksm::input {

// Turns a held button into repeated actions: once on the press, then after a pause at a steady
// period. HeldMs lets a caller step larger the longer the hold lasts.
class HoldRepeat {
private:
    s64 delayMs;
    s64 periodMs;
    u64 heldSince = 0;  // 0 while released
    u64 lastFire = 0;

public:
    HoldRepeat(s64 delayMs, s64 periodMs) : delayMs(delayMs), periodMs(periodMs) {}

    // pressed: fresh press this frame; held: still down. True when the caller should act now
    bool Update(bool pressed, bool held);

    // Milliseconds the button has been down; 0 while released
    s64 HeldMs() const;

    // Forget the hold, for when something else swallows the release (a dialog, a focus change)
    void Reset() { heldSince = 0; }
};

}  // namespace pksm::input
