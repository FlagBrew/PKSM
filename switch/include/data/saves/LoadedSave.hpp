#pragma once

#include <memory>
#include <string>

#include "sav/Sav.hpp"

namespace pksm::saves {

// A save parsed by PKSM-Core together with where its bytes came from.
// Produced by the save provider, then owned by the save data accessor -
// exactly one live Sav exists at a time, and all edits go through it.
struct LoadedSave {
    std::unique_ptr<::pksm::Sav> sav;
    std::string path;
};

}  // namespace pksm::saves
