#pragma once

#include <string>

namespace pksm::saves {

// The folder name JKSV uses for a title: a port of JKSV's util::safeString.
// Codepoints JKSV forbids in FAT names become spaces, e is substituted for
// e-acute, and any other non-ASCII codepoint makes JKSV fall back to naming
// the folder with the title ID instead - signalled here by an empty return.
std::string JKSVSafeName(const std::string& name);

}  // namespace pksm::saves
