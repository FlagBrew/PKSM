#pragma once

#include "gui/ISelectable.hpp"
#include "input/base/InputManager.hpp"

namespace pksm::input {

class SelectionManager : public InputManager<pksm::ui::ISelectable> {
public:
    SelectionManager(const std::string& name) : InputManager(name) {}
    PU_SMART_CTOR(SelectionManager)

    // Convenience methods that match the old interface
    void RegisterSelectable(pksm::ui::ISelectable::Ref selectable) { RegisterElement(selectable); }
    void UnregisterSelectable(pksm::ui::ISelectable::Ref selectable) { UnregisterElement(selectable); }
    void SetOwningSelectable(pksm::ui::ISelectable::Ref owner) { SetOwningElement(owner); }
    void HandleSelectionRequest(pksm::ui::ISelectable::Ref requestingSelectable) {
        HandleElementRequest(requestingSelectable);
    }
    bool HasSelectedElement() const { return HasActiveElement(); }
    pksm::ui::ISelectable::Ref GetSelectedElement() const { return GetActiveElement(); }

protected:
    void SetElementActive(pksm::ui::ISelectable::Ref element, bool active) override { element->SetSelected(active); }
    bool IsElementActive(pksm::ui::ISelectable::Ref element) const override { return element->IsSelected(); }
    void SetInputManager(
        pksm::ui::ISelectable::Ref element,
        std::shared_ptr<InputManager<pksm::ui::ISelectable>> manager
    ) override {
        element->SetSelectionManager(std::static_pointer_cast<SelectionManager>(manager));
    }
};

}  // namespace pksm::input