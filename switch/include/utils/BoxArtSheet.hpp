#pragma once

#include <SDL2/SDL.h>
#include <cstdio>
#include <pu/Plutonium>
#include <string>
#include <vector>

namespace pksm::utils {

// PKSS v2 box-art sheet: verbatim JPEG blobs behind a sorted u64 title-id
// index (format spec lives with the out-of-repo pkss tool).
class BoxArtSheet {
public:
    BoxArtSheet() = default;
    BoxArtSheet(const BoxArtSheet&) = delete;
    BoxArtSheet& operator=(const BoxArtSheet&) = delete;
    ~BoxArtSheet();

    bool Load(const std::string& path);

    // Caller owns the returned texture; nullptr when missing or undecodable
    SDL_Texture* CreateTexture(u64 titleId);

private:
    struct IndexEntry {
        u64 key;
        u32 dataOffset;
        u32 dataSize;
    };
    static_assert(sizeof(IndexEntry) == 16);

    std::vector<IndexEntry> index;
    FILE* file = nullptr;
};

}  // namespace pksm::utils
