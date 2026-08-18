#pragma once

#include <optional>
#include <string>

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

// First character of `name` the save's box-name encoding would lose (its
// character set lacks it, so the setter truncates there or stores a
// replacement mark), or nullopt when the whole name stores. Answered by
// round-tripping through the same core string routines the save's boxName
// setter and getter use, so no character-table knowledge lives app-side.
std::optional<std::string> FirstUnstorableBoxNameChar(const ::pksm::Sav& sav, const std::string& name);

}  // namespace pksm::saves
