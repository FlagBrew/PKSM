#pragma once

#include <memory>
#include <pu/Plutonium>

#include "gui/IFocusable.hpp"

namespace pksm::input {
class SelectionManager;
}  // namespace pksm::input

namespace pksm::ui {

class ISelectable : public IFocusable {
protected:
    std::weak_ptr<input::SelectionManager> selectionManager;
    std::string name = "";

public:
    ISelectable() = default;
    PU_SMART_CTOR(ISelectable)
    virtual ~ISelectable() = default;

    // Controls whether this component is currently selected
    virtual void SetSelected(bool selected) = 0;

    // Returns whether this component is currently selected
    virtual bool IsSelected() const = 0;

    // Sets the selection manager for this selectable
    void SetSelectionManager(std::shared_ptr<input::SelectionManager> manager);

    // Called when this component wants to request selection
    void RequestSelection();

    // Establishes the owning relationship with the selection manager
    // Should only be called after the object is fully constructed and owned by a shared_ptr
    void EstablishOwningRelationship();

    // Sets the name of the selectable
    void SetName(const std::string& name);

    // Returns the name of the selectable
    std::string GetName() const;
};

}  // namespace pksm::ui