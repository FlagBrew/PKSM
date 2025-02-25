#pragma once

#include <functional>
#include <memory>
#include <pu/Plutonium>
#include <vector>

#include "gui/shared/components/HelpFooter.hpp"
#include "gui/shared/components/HelpOverlay.hpp"
#include "gui/shared/interfaces/IHelpProvider.hpp"
#include "utils/Logger.hpp"

namespace pksm::layout {

/**
 * @brief Base layout class that handles common help footer and overlay functionality
 *
 * This class provides common functionality for layouts that use a help footer
 * and need to show a help overlay when the minus button is pressed.
 */
class BaseLayout : public pu::ui::Layout {
public:
    BaseLayout(std::function<void(pu::ui::Overlay::Ref)> onShowOverlay, std::function<void()> onHideOverlay);
    PU_SMART_CTOR(BaseLayout)
    virtual ~BaseLayout() = default;

protected:
    // Overlay callbacks
    std::function<void(pu::ui::Overlay::Ref)> onShowOverlay;
    std::function<void()> onHideOverlay;

    // Help state
    bool isHelpOverlayVisible;

    // Help components
    pksm::ui::HelpFooter::Ref helpFooter;

    // Help methods
    void UpdateHelpItems(pksm::ui::IHelpProvider::Ref helpItemProvider);
    void ShowHelpOverlay(const std::vector<pksm::ui::HelpItem>& helpItems);
    void HideHelpOverlay();

    // Input handling
    bool HandleHelpInput(u64 down);

    // Override this to provide custom help items for the overlay
    virtual std::vector<pksm::ui::HelpItem> GetHelpOverlayItems() const;

    // Call this to create and add the help footer to the layout
    void InitializeHelpFooter();

    // Override these to handle UI element disabling/enabling when help overlay is shown/hidden
    virtual void OnHelpOverlayShown() {}
    virtual void OnHelpOverlayHidden() {}
};

}  // namespace pksm::layout