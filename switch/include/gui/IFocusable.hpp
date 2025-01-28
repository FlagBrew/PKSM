#pragma once

class IFocusable {
public:
    virtual ~IFocusable() = default;
    
    // Controls whether this component is currently focused for input
    virtual void SetFocused(bool focused) = 0;
    
    // Returns whether this component is currently focused
    virtual bool IsFocused() const = 0;
}; 