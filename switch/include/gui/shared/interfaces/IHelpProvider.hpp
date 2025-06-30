#pragma once

#include "gui/shared/components/HelpItem.hpp"

namespace pksm::ui {

class IHelpProvider {
public:
    PU_SMART_CTOR(IHelpProvider);
    virtual ~IHelpProvider() = default;
    virtual std::vector<HelpItem> GetHelpItems() const = 0;
};

}  // namespace pksm::ui