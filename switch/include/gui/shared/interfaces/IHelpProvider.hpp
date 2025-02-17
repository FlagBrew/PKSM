#pragma once

#include "gui/shared/components/HelpFooter.hpp"

namespace pksm::ui {

class IHelpProvider {
public:
    PU_SMART_CTOR(IHelpProvider);
    virtual ~IHelpProvider() = default;
    virtual std::vector<HelpItem> GetHelpItems() const = 0;
};

}  // namespace pksm::ui