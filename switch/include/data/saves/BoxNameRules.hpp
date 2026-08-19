#pragma once

#include <optional>
#include <string>

#include "sav/Sav.hpp"

namespace pksm::saves {

// Per-generation box-name policy: which saves store writable box names and how long
struct BoxNameRules {
    bool renamable = false;
    // Characters the save's name field stores; its setter truncates beyond
    size_t maxLength = 0;
};

BoxNameRules BoxNameRulesFor(const ::pksm::Sav& sav);

// First character the save's box-name encoding would lose; nullopt when the whole name stores
std::optional<std::string> FirstUnstorableBoxNameChar(const ::pksm::Sav& sav, const std::string& name);

}  // namespace pksm::saves
