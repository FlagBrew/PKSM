#pragma once

#include <memory>
#include <string>
#include <vector>

#include "sav/Sav.hpp"

namespace pksm::saves {

// A save parsed by PKSM-Core together with where its bytes came from.
// Produced by the save provider, then owned by the save data accessor -
// exactly one live Sav exists at a time, and all edits go through it.
struct LoadedSave {
    std::unique_ptr<::pksm::Sav> sav;
    std::string path;
};

// A resolved-but-unread save load. Resolution and mounts happen on the
// UI thread (mount registration must not race the logger thread's path
// lookups); a worker then tries candidates in order, first parse wins.
struct PendingLoad {
    std::vector<std::string> candidates;
    // Overrides the winning path in LoadedSave (nsosave: carries mount
    // identity for write-back)
    std::string recordedPath;
    std::string description;  // log context, e.g. "JKSV save"
    bool mounted = false;     // holds "save:"; released by FinishLoad
};

}  // namespace pksm::saves
