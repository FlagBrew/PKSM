#pragma once

#include <cstddef>
#include <optional>
#include <switch.h>

namespace pksm::saves {

// GB/GBC NSO container: "SRAM" magic + variable-length header, then the raw
// save at the tail with its 40-hex SHA-1 immediately before it. (GBA is raw.)
struct NSOContainerRegion {
    size_t rawOffset = 0;
    size_t rawSize = 0;
    size_t shaOffset = 0;
};

// Locate the raw save; the stored SHA-1 must match its contents (fail-closed)
std::optional<NSOContainerRegion> ProbeNSOContainer(const u8* data, size_t size);

// Recompute the stored ASCII-hex SHA-1 after the raw save was replaced
void ResignNSOContainer(u8* data, const NSOContainerRegion& region);

}  // namespace pksm::saves
