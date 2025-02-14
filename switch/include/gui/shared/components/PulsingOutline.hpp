#pragma once
#include <chrono>
#include <pu/Plutonium>

#include "gui/shared/components/StaticOutline.hpp"

namespace pksm::ui {

class PulsingOutlineBase : public StaticOutlineBase {
protected:
    std::chrono::steady_clock::time_point startTime;

    PulsingOutlineBase(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        const pu::ui::Color color,
        const u32 borderWidth = DEFAULT_BORDER_WIDTH
    );

public:
    using StaticOutlineBase::StaticOutlineBase;

    pu::ui::Color calculatePulseColor() const;
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
        const pu::ui::Color color,
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
        const pu::ui::Color color,
        const u32 borderWidth = DEFAULT_BORDER_WIDTH
    );
    PU_SMART_CTOR(CircularPulsingOutline)

    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
};

// For backward compatibility, typedef the rectangular version as the default
using PulsingOutline = RectangularPulsingOutline;

}  // namespace pksm::ui