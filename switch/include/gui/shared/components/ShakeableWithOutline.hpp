#pragma once
#include "gui/shared/components/IShakeable.hpp"
#include "gui/shared/components/PulsingOutline.hpp"

namespace pksm::ui {

/**
 * Base class for UI components that have a pulsing outline and can perform shake animations.
 * This class implements the IShakeable interface with a default implementation that
 * delegates to the component's pulsing outline.
 */
class ShakeableWithOutline : public IShakeable {
protected:
    // The pulsing outline that will be animated
    pksm::ui::PulsingOutlineBase::Ref pulsingOutline;

    /**
     * Constructor that takes a reference to a pulsing outline.
     *
     * @param outline The pulsing outline to animate
     */
    explicit ShakeableWithOutline(pksm::ui::PulsingOutlineBase::Ref outline) : pulsingOutline(outline) {
        outline->SetVisible(false);
    }

public:
    virtual ~ShakeableWithOutline() = default;

    /**
     * Implementation of IShakeable interface that delegates to the pulsing outline.
     *
     * @param direction The direction to shake in
     */
    void shakeOutOfBounds(ShakeDirection direction) override {
        if (pulsingOutline && pulsingOutline->IsVisible()) {
            pulsingOutline->shake(direction);
        }
    }
};

}  // namespace pksm::ui