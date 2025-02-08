#include "gui/IFocusable.hpp"

#include "input/FocusManager.hpp"
#include "utils/Logger.hpp"

namespace pksm::ui {

void IFocusable::RequestFocus() {
    if (auto manager = focusManager.lock()) {
        manager->HandleFocusRequest(shared_from_this());
    }
}

void IFocusable::SetFocusManager(std::shared_ptr<input::FocusManager> manager) {
    focusManager = manager;
}

void IFocusable::EstablishOwningRelationship() {
    if (auto manager = focusManager.lock()) {
        LOG_DEBUG("[IFocusable] Establishing owning relationship for " + name + " with manager " + manager->name);
        manager->SetOwningFocusable(IFocusable::shared_from_this());
    }
}

void IFocusable::SetName(const std::string& name) {
    this->name = name;
}

std::string IFocusable::GetName() const {
    return name;
}

}  // namespace pksm::ui