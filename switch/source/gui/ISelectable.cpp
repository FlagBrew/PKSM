#include "gui/ISelectable.hpp"

#include "input/SelectionManager.hpp"
#include "utils/Logger.hpp"

namespace pksm::ui {

void ISelectable::RequestSelection() {
    if (auto manager = selectionManager.lock()) {
        manager->HandleSelectionRequest(
            std::static_pointer_cast<ISelectable>(std::static_pointer_cast<IFocusable>(shared_from_this()))
        );
    }
}

void ISelectable::SetSelectionManager(std::shared_ptr<input::SelectionManager> manager) {
    selectionManager = manager;
}

void ISelectable::EstablishOwningRelationship() {
    IFocusable::EstablishOwningRelationship();  // Call base first

    if (auto manager = selectionManager.lock()) {
        LOG_DEBUG("[ISelectable] Establishing owning relationship for " + name + " with manager " + manager->name);
        manager->SetOwningSelectable(
            std::static_pointer_cast<ISelectable>(std::static_pointer_cast<IFocusable>(shared_from_this()))
        );
    }
}

void ISelectable::SetName(const std::string& name) {
    this->name = name;
}

std::string ISelectable::GetName() const {
    return name;
}

}  // namespace pksm::ui