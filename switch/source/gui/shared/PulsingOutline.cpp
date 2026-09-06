#include "gui/shared/components/PulsingOutline.hpp"

pksm::ui::PulsingOutlineBase::PulsingOutlineBase(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    const pu::ui::Color color,
    const u32 borderWidth
)
  : StaticOutlineBase(x, y, width, height, color, borderWidth),
    startTime(std::chrono::steady_clock::now()),
    isShaking(false),
    shakeDirection(ShakeDirection::RIGHT),
    shakeStartTime(0) {}

pu::ui::Color pksm::ui::PulsingOutlineBase::calculatePulseColor() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
    float time = (duration % 1000) / 1000.0f;  // Convert to 0-1 range

    // Calculate pulse intensity (0.0 to 1.0)
    float intensity = std::abs(time - 0.5f) * 2.0f;  // Creates a triangle wave

    // Interpolate color
    return pu::ui::Color{
        static_cast<u8>(color.r + ((255 - color.r) * intensity)),
        static_cast<u8>(color.g + ((255 - color.g) * intensity)),
        static_cast<u8>(color.b + ((255 - color.b) * intensity)),
        255
    };
}

void pksm::ui::PulsingOutlineBase::shake(ShakeDirection direction) {
    isShaking = true;
    shakeDirection = direction;
    shakeStartTime = SDL_GetTicks64();
}

pksm::ui::ShakeOffset pksm::ui::PulsingOutlineBase::calculateShakeOffset() const {
    if (!isShaking) {
        return ShakeOffset(0.0f, 0.0f);
    }

    u64 currentTime = SDL_GetTicks64();
    u64 elapsedTime = currentTime - shakeStartTime;

    // If animation is complete, stop shaking
    if (elapsedTime >= SHAKE_DURATION) {
        const_cast<PulsingOutlineBase*>(this)->isShaking = false;
        return ShakeOffset(0.0f, 0.0f);
    }

    // Calculate progress (0.0 to 1.0)
    float progress = static_cast<float>(elapsedTime) / SHAKE_DURATION;

    // Create a bounce effect:
    // 1. First move in the direction (with forward multiplier)
    // 2. Then bounce back past the origin (by bounce ratio)
    // 3. Finally settle back to the origin
    float bounceOffset;

    // Calculate the maximum forward displacement
    float maxForwardDisplacement = SHAKE_AMPLITUDE * SHAKE_FORWARD_MULTIPLIER;
    // Calculate the maximum backward displacement (in opposite direction)
    float maxBackwardDisplacement = maxForwardDisplacement * SHAKE_BOUNCE_RATIO;

    if (progress < 0.3f) {
        // Initial movement forward (0 to max)
        float phase1 = progress / 0.3f;
        bounceOffset = maxForwardDisplacement * phase1;
    } else if (progress < 0.7f) {
        // Bounce back past origin (max to -maxBackward)
        float phase2 = (progress - 0.3f) / 0.4f;
        bounceOffset = maxForwardDisplacement - (phase2 * (maxForwardDisplacement + maxBackwardDisplacement));
    } else {
        // Settle back to origin (-maxBackward to 0)
        float phase3 = (progress - 0.7f) / 0.3f;
        bounceOffset = -maxBackwardDisplacement * (1.0f - phase3);
    }

    // Apply offset based on direction
    float xOffset = 0.0f;
    float yOffset = 0.0f;

    switch (shakeDirection) {
        case ShakeDirection::UP:
            yOffset = -bounceOffset;
            break;
        case ShakeDirection::DOWN:
            yOffset = bounceOffset;
            break;
        case ShakeDirection::LEFT:
            xOffset = -bounceOffset;
            break;
        case ShakeDirection::RIGHT:
            xOffset = bounceOffset;
            break;
    }

    return ShakeOffset(xOffset, yOffset);
}

// RectangularPulsingOutline implementation
pksm::ui::RectangularPulsingOutline::RectangularPulsingOutline(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    const pu::ui::Color color,
    const pu::i32 radius,
    const u32 borderWidth
)
  : PulsingOutlineBase(x, y, width, height, color, borderWidth), radius(radius) {}

void pksm::ui::RectangularPulsingOutline::OnRender(
    pu::ui::render::Renderer::Ref& drawer,
    const pu::i32 x,
    const pu::i32 y
) {
    if (!visible)
        return;

    pu::ui::Color pulseColor = calculatePulseColor();

    // Calculate shake offset
    ShakeOffset offset = calculateShakeOffset();

    // Apply shake offset to rendering position
    pu::i32 renderX = x + static_cast<pu::i32>(offset.x);
    pu::i32 renderY = y + static_cast<pu::i32>(offset.y);

    for (u32 i = 0; i < borderWidth; i++) {
        drawer->RenderRoundedRectangle(
            pulseColor,
            renderX + i,
            renderY + i,
            width - (2 * i),
            height - (2 * i),
            radius > static_cast<pu::i32>(i) ? radius - static_cast<pu::i32>(i) : 0
        );
    }
}

// CircularPulsingOutline implementation
pksm::ui::CircularPulsingOutline::CircularPulsingOutline(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 diameter,
    const pu::ui::Color color,
    const u32 borderWidth
)
  : PulsingOutlineBase(x, y, diameter, diameter, color, borderWidth) {}

void pksm::ui::CircularPulsingOutline::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    if (!visible)
        return;

    pu::ui::Color pulseColor = calculatePulseColor();

    // Calculate shake offset
    ShakeOffset offset = calculateShakeOffset();

    // Apply shake offset to rendering position
    pu::i32 renderX = x + static_cast<pu::i32>(offset.x);
    pu::i32 renderY = y + static_cast<pu::i32>(offset.y);

    pu::i32 baseRadius = this->width / 2;  // treat 'width' as diameter for circles
    pu::i32 centerX = renderX + baseRadius;
    pu::i32 centerY = renderY + baseRadius;

    // Draw from inside out, adding to radius each time
    for (u32 i = 0; i < borderWidth; i++) {
        drawer->RenderCircle(pulseColor, centerX, centerY, baseRadius + i);
    }
}