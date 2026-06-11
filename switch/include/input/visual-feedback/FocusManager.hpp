#pragma once

#include "input/visual-feedback/base/InputManager.hpp"
#include "input/visual-feedback/interfaces/IFocusable.hpp"

namespace pksm::input {

class FocusManager : public InputManager<pksm::ui::IFocusable> {
public:
    FocusManager(const std::string& name) : InputManager(name) {}
    PU_SMART_CTOR(FocusManager)

    // Convenience methods that match the old interface
    void RegisterFocusable(pksm::ui::IFocusable::Ref focusable) { RegisterElement(focusable); }
    void UnregisterFocusable(pksm::ui::IFocusable::Ref focusable) { UnregisterElement(focusable); }
    void SetOwningFocusable(pksm::ui::IFocusable::Ref owner) { SetOwningElement(owner); }
    void HandleFocusRequest(pksm::ui::IFocusable::Ref requestingFocusable) {
        HandleElementRequest(requestingFocusable);
    }
    bool HasFocusedElement() const { return HasActiveElement(); }
    pksm::ui::IFocusable::Ref GetFocusedElement() const { return GetActiveElement(); }

protected:
    void SetElementActive(pksm::ui::IFocusable::Ref element, bool active) override { element->SetFocused(active); }
    bool IsElementActive(pksm::ui::IFocusable::Ref element) const override { return element->IsFocused(); }
    void SetInputManager(pksm::ui::IFocusable::Ref element, std::shared_ptr<InputManager<pksm::ui::IFocusable>> manager)
        override {
        element->SetFocusManager(std::static_pointer_cast<FocusManager>(manager));
    }
};

}  // namespace pksm::input