#pragma once

#include <cstddef>
#include <optional>
#include <switch.h>

namespace pksm::saves {

// The GB/GBC Nintendo Switch Online app wraps each game's raw cartridge
// SRAM in a small container: a "SRAM" magic, the emulator's git revision,
// an optional RTC block, then the ASCII-hex SHA-1 of the raw save, which
// runs from there to the end of the file. The header is variable-length,
// so the raw save is located from the tail: its size is a known GB save
// size and the 40 hex characters of its SHA-1 sit immediately before it.
// (The GBA app's cartridge.sram has no container - it is the raw save.)
struct NSOContainerRegion {
    size_t rawOffset = 0;
    size_t rawSize = 0;
    size_t shaOffset = 0;
};

// Identify a GB NSO container and locate its raw save. The stored SHA-1
// must match the raw contents; a container the app would itself reject is
// not a loadable save.
std::optional<NSOContainerRegion> ProbeNSOContainer(const u8* data, size_t size);

// Recompute the stored ASCII-hex SHA-1 after the raw save was replaced
void ResignNSOContainer(u8* data, const NSOContainerRegion& region);

}  // namespace pksm::saves
