#pragma once

#include <algorithm>
#include <memory>
#include <pu/Plutonium>
#include <vector>

#include "utils/Logger.hpp"

namespace pksm::input {

template <typename T>
class InputManager : public std::enable_shared_from_this<InputManager<T>> {
public:
    using ElementRef = typename T::Ref;
    PU_SMART_CTOR(InputManager)

    InputManager(const std::string& name) : name(name), isActive(false) {}

    // Registration methods
    void RegisterElement(ElementRef element);
    void UnregisterElement(ElementRef element);
    void RegisterChildManager(Ref childManager);
    void UnregisterChildManager(Ref childManager);
    void SetOwningElement(ElementRef owner);

    // Element management
    void HandleElementRequest(ElementRef requestingElement);
    bool HasActiveElement() const;
    ElementRef GetActiveElement() const;

    // State
    bool IsActive() const;
    void SetActive(bool active);

    std::string name;

protected:
    // Virtual methods to be implemented by derived classes
    virtual void SetElementActive(ElementRef element, bool active) = 0;
    virtual bool IsElementActive(ElementRef element) const = 0;
    virtual void SetInputManager(ElementRef element, std::shared_ptr<InputManager<T>> manager) = 0;

private:
    std::weak_ptr<InputManager<T>> parentManager;
    std::vector<Ref> childManagers;
    std::vector<ElementRef> elements;
    std::weak_ptr<T> currentlyActive;
    std::weak_ptr<T> owningElement;
    bool isActive;

    void ResignElement();
    void SetParentManager(Ref parent);
};

// Template implementation
template <typename T>
void InputManager<T>::RegisterElement(ElementRef element) {
    if (element) {
        // Only add if not already registered
        if (std::find(elements.begin(), elements.end(), element) == elements.end()) {
            LOG_DEBUG("[InputManager] Registering element " + element->GetName() + " with manager: " + name);
            elements.push_back(element);
            SetInputManager(element, this->shared_from_this());
        }
    }
}

template <typename T>
void InputManager<T>::UnregisterElement(ElementRef element) {
    if (element) {
        LOG_DEBUG("[InputManager] Unregistering element " + element->GetName() + " from manager: " + name);
        auto it = std::find(elements.begin(), elements.end(), element);
        if (it != elements.end()) {
            if (auto active = currentlyActive.lock()) {
                if (active == element) {
                    currentlyActive.reset();
                }
            }
            elements.erase(it);
            SetInputManager(element, nullptr);
        }
    }
}

template <typename T>
void InputManager<T>::RegisterChildManager(Ref childManager) {
    if (childManager) {
        // Only add if not already registered
        if (std::find(childManagers.begin(), childManagers.end(), childManager) == childManagers.end()) {
            LOG_DEBUG("[InputManager] Registering child manager: " + childManager->name + " to " + name);
            childManagers.push_back(childManager);
            childManager->SetParentManager(this->shared_from_this());
        }
    }
}

template <typename T>
void InputManager<T>::UnregisterChildManager(Ref childManager) {
    if (childManager) {
        LOG_DEBUG("[InputManager] Unregistering child manager: " + childManager->name + " from " + name);
        auto it = std::find(childManagers.begin(), childManagers.end(), childManager);
        if (it != childManagers.end()) {
            childManager->SetParentManager(nullptr);
            childManagers.erase(it);
        }
    }
}

template <typename T>
void InputManager<T>::SetParentManager(Ref parent) {
    parentManager = parent;
    LOG_DEBUG("[InputManager] Setting parent manager for " + name + " to " + parent->name);
}

template <typename T>
void InputManager<T>::SetOwningElement(ElementRef owner) {
    owningElement = owner;
    LOG_DEBUG("[InputManager] Setting owning element for " + name + " to " + owner->GetName());
}

template <typename T>
void InputManager<T>::ResignElement() {
    if (!isActive)
        return;
    LOG_DEBUG("[InputManager] Resigning from manager: " + name);

    isActive = false;

    if (auto owner = owningElement.lock()) {
        LOG_DEBUG("[InputManager] Resigning from owning element: " + owner->GetName());
        SetElementActive(owner, false);
    }

    // Resign all child managers first
    for (auto& child : childManagers) {
        LOG_DEBUG("[InputManager] Resigning child manager: " + child->name);
        child->ResignElement();
    }

    // Resign any active element
    if (auto active = currentlyActive.lock()) {
        LOG_DEBUG("[InputManager] Resigning from: " + active->GetName());
        SetElementActive(active, false);
        currentlyActive.reset();
    }
}

template <typename T>
void InputManager<T>::HandleElementRequest(ElementRef requestingElement) {
    LOG_DEBUG("[InputManager] " + name + " handling request from " + requestingElement->GetName());

    if (!isActive) {
        // Activate this branch of the tree
        isActive = true;
        LOG_DEBUG(
            "[InputManager] Activating manager: " + name + " with owning element: " + owningElement.lock()->GetName()
        );

        // Set new active element
        if (requestingElement != owningElement.lock()) {
            currentlyActive = requestingElement;
        }
        SetElementActive(requestingElement, true);
        SetElementActive(owningElement.lock(), true);
        LOG_DEBUG("[InputManager] Element set to: " + requestingElement->GetName());

        // Resign all siblings (both elements and managers)
        for (auto& child : parentManager.lock()->childManagers) {
            if (child->owningElement.lock() != owningElement.lock() && child->IsActive()) {
                LOG_DEBUG("[InputManager] Resigning unrelated sibling manager: " + child->name + " in manager: " + name);
                child->ResignElement();
            }
        }

        for (auto& element : parentManager.lock()->elements) {
            if (IsElementActive(element) && element != requestingElement) {
                LOG_DEBUG(
                    "[InputManager] Resigning unrelated sibling element: " + element->GetName() + " in manager: " + name
                );
                SetElementActive(element, false);
                parentManager.lock()->currentlyActive.reset();
            }
        }

        // Propagate up to parent hierarchy
        if (auto parent = parentManager.lock()) {
            if (!parent->IsActive()) {
                if (auto owner = owningElement.lock()) {
                    LOG_DEBUG("[InputManager] Propagating to parent via: " + owner->GetName());
                    parent->HandleElementRequest(owner);
                }
            }
        }
    } else {
        // Manager is already active - update active element within
        auto current = currentlyActive.lock();
        LOG_DEBUG(
            "[InputManager] Manager: " + name + " Current: " + (current ? current->GetName() : "nothing") +
            " requesting: " + requestingElement->GetName() +
            " owning: " + (owningElement.lock() ? owningElement.lock()->GetName() : "nothing")
        );
        if (current != requestingElement && requestingElement != owningElement.lock()) {
            LOG_DEBUG(
                "[InputManager] Switching within " + name + " from " + (current ? current->GetName() : "nothing") +
                " to " + requestingElement->GetName() + " in manager: " + name
            );

            if (current) {
                LOG_DEBUG("[InputManager] Resigning previous: " + current->GetName() + " in manager: " + name);
                SetElementActive(current, false);
                currentlyActive.reset();
            }

            // Resign any child managers not related to new element
            for (auto& child : childManagers) {
                if (child->IsActive()) {
                    LOG_DEBUG(
                        "[InputManager] Resigning unrelated child manager: " + child->name + " in manager: " + name
                    );
                    child->ResignElement();
                }
            }

            currentlyActive = requestingElement;
            SetElementActive(requestingElement, true);
            LOG_DEBUG("[InputManager] New element set to: " + requestingElement->GetName() + " in manager: " + name);
        }
    }
}

template <typename T>
bool InputManager<T>::HasActiveElement() const {
    return !currentlyActive.expired();
}

template <typename T>
typename InputManager<T>::ElementRef InputManager<T>::GetActiveElement() const {
    return currentlyActive.lock();
}

template <typename T>
bool InputManager<T>::IsActive() const {
    return isActive;
}

template <typename T>
void InputManager<T>::SetActive(bool active) {
    isActive = active;
}

}  // namespace pksm::input