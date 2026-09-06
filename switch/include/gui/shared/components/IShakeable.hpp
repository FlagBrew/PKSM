#pragma once
#include "gui/shared/components/PulsingOutline.hpp"

namespace pksm::ui {

/**
 * Interface for UI components that can perform a shake animation
 * when the user attempts to navigate beyond boundaries.
 */
class IShakeable {
public:
    virtual ~IShakeable() = default;

    /**
     * Trigger a shake animation in the specified direction.
     *
     * @param direction The direction to shake in
     */
    virtual void shakeOutOfBounds(ShakeDirection direction) = 0;
};

}  // namespace pksm::ui