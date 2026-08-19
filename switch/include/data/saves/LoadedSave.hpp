#pragma once

#include <memory>
#include <string>
#include <vector>

#include "sav/Sav.hpp"

namespace pksm::saves {

// Exactly one live Sav exists at a time, and all edits go through it
struct LoadedSave {
    std::unique_ptr<::pksm::Sav> sav;
    std::string path;
};

// Mounts happen on the UI thread - mount registration must not race the
// logger thread's path lookups; a worker tries candidates, first parse wins.
struct PendingLoad {
    std::vector<std::string> candidates;
    // Overrides the winning path (nsosave: carries mount identity for write-back)
    std::string recordedPath;
    std::string description;  // log context, e.g. "JKSV save"
    bool mounted = false;     // holds "save:"; released by FinishLoad
};

}  // namespace pksm::saves
