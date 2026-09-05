#pragma once

#include <string>

namespace pksm::saves {

// Port of JKSV's util::safeString; empty means JKSV falls back to a title-ID folder name
std::string JKSVSafeName(const std::string& name);

}  // namespace pksm::saves
