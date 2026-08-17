#pragma once

#include "sav/Sav.hpp"

namespace pksm::saves {

// Per-generation box-name policy: which saves store real, writable box
// names and how many characters their name fields hold. Generation-agnostic
// for callers; the per-save knowledge lives here.

struct BoxNameRules {
    bool renamable = false;
    // Characters the save's name field stores; its setter truncates beyond
    size_t maxLength = 0;
};

BoxNameRules BoxNameRulesFor(const ::pksm::Sav& sav);

}  // namespace pksm::saves
