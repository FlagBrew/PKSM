#include "data/saves/NSOContainer.hpp"

#include <cctype>
#include <cstring>

namespace pksm::saves {

namespace {

constexpr size_t SHA_HEX_LEN = 40;
// Magic, version byte, git revision, SHA-1 hex: the raw save can never
// start earlier than this
constexpr size_t MIN_HEADER = 4 + 1 + 1 + SHA_HEX_LEN;

// GB/GBC cartridge SRAM sizes, largest first so a big save is never
// mistaken for a small one with a padded header
constexpr size_t GB_RAW_SIZES[] = {0x20000, 0x10000, 0x8000, 0x2000, 0x800};

void Sha1Hex(const u8* data, size_t size, char out[SHA_HEX_LEN]) {
    u8 digest[SHA1_HASH_SIZE];
    sha1CalculateHash(digest, data, size);
    static constexpr char HEX[] = "0123456789abcdef";
    for (size_t i = 0; i < SHA1_HASH_SIZE; i++) {
        out[i * 2] = HEX[digest[i] >> 4];
        out[i * 2 + 1] = HEX[digest[i] & 0xF];
    }
}

bool HexEqualsIgnoreCase(const u8* stored, const char* computed) {
    for (size_t i = 0; i < SHA_HEX_LEN; i++) {
        if (std::tolower(stored[i]) != computed[i]) {
            return false;
        }
    }
    return true;
}

}  // namespace

std::optional<NSOContainerRegion> ProbeNSOContainer(const u8* data, size_t size) {
    if (size < MIN_HEADER || std::memcmp(data, "SRAM", 4) != 0) {
        return std::nullopt;
    }
    for (const size_t rawSize : GB_RAW_SIZES) {
        if (size < rawSize + MIN_HEADER) {
            continue;
        }
        NSOContainerRegion region;
        region.rawSize = rawSize;
        region.rawOffset = size - rawSize;
        region.shaOffset = region.rawOffset - SHA_HEX_LEN;

        char computed[SHA_HEX_LEN];
        Sha1Hex(data + region.rawOffset, rawSize, computed);
        if (HexEqualsIgnoreCase(data + region.shaOffset, computed)) {
            return region;
        }
    }
    return std::nullopt;
}

void ResignNSOContainer(u8* data, const NSOContainerRegion& region) {
    char computed[SHA_HEX_LEN];
    Sha1Hex(data + region.rawOffset, region.rawSize, computed);
    std::memcpy(data + region.shaOffset, computed, SHA_HEX_LEN);
}

}  // namespace pksm::saves
