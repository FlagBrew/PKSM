#pragma once

#include <memory>
#include <pu/Plutonium>

namespace pksm::input {
class FocusManager;
}  // namespace pksm::input

namespace pksm::ui {

class IFocusable : public std::enable_shared_from_this<IFocusable> {
protected:
    std::weak_ptr<input::FocusManager> focusManager;
    std::string name = "";

public:
    IFocusable() = default;
    PU_SMART_CTOR(IFocusable)
    virtual ~IFocusable() = default;

    // Controls whether this component is currently focused for input
    virtual void SetFocused(bool focused) = 0;

    // Returns whether this component is currently focused
    virtual bool IsFocused() const = 0;

    // Sets the focus manager for this focusable
    void SetFocusManager(std::shared_ptr<input::FocusManager> manager);

    // Called when this component wants to request focus
    void RequestFocus();

    // Establishes the owning relationship with the focus manager
    // Should only be called after the object is fully constructed and owned by a shared_ptr
    void EstablishOwningRelationship();

    // Sets the name of the focusable
    void SetName(const std::string& name);

    // Returns the name of the focusable
    std::string GetName() const;
};

}  // namespace pksm::ui