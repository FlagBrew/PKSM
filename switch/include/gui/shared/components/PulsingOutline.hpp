#pragma once
#include <chrono>
#include <pu/Plutonium>

#include "gui/shared/UIConstants.hpp"
#include "gui/shared/components/ShakeDirection.hpp"
#include "gui/shared/components/StaticOutline.hpp"

namespace pksm::ui {

// Simple struct to represent an offset in 2D space
struct ShakeOffset {
    float x;
    float y;

    ShakeOffset(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

class PulsingOutlineBase : public StaticOutlineBase {
protected:
    std::chrono::steady_clock::time_point startTime;

    // Shake animation state
    bool isShaking;
    ShakeDirection shakeDirection;
    u64 shakeStartTime;

    // Shake animation constants
    static constexpr u64 SHAKE_DURATION = 100;  // milliseconds
    static constexpr float SHAKE_AMPLITUDE = 8.0f;  // pixels
    static constexpr float SHAKE_FORWARD_MULTIPLIER = 1.8f;  // multiplier for initial movement
    static constexpr float SHAKE_BOUNCE_RATIO = 0.3f;  // how much to bounce back (relative to forward movement)

    PulsingOutlineBase(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        const pu::ui::Color color = pksm::ui::global::OUTLINE_COLOR,
        const u32 borderWidth = DEFAULT_BORDER_WIDTH
    );

    // Calculate shake offset based on current animation state
    ShakeOffset calculateShakeOffset() const;

public:
    PU_SMART_CTOR(PulsingOutlineBase)
    pu::ui::Color calculatePulseColor() const;

    // Trigger shake animation in the specified direction
    void shake(ShakeDirection direction);
};

class RectangularPulsingOutline : public PulsingOutlineBase {
private:
    pu::i32 radius;

public:
    RectangularPulsingOutline(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        const pu::ui::Color color = pksm::ui::global::OUTLINE_COLOR,
        const pu::i32 radius = 0,
        const u32 borderWidth = DEFAULT_BORDER_WIDTH
    );
    PU_SMART_CTOR(RectangularPulsingOutline)

    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
};

class CircularPulsingOutline : public PulsingOutlineBase {
public:
    CircularPulsingOutline(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 diameter,
        const pu::ui::Color color = pksm::ui::global::OUTLINE_COLOR,
        const u32 borderWidth = DEFAULT_BORDER_WIDTH
    );
    PU_SMART_CTOR(CircularPulsingOutline)

    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
};

// For backward compatibility, typedef the rectangular version as the default
using PulsingOutline = RectangularPulsingOutline;

}  // namespace pksm::ui