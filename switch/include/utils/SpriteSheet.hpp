#pragma once

#include <SDL2/SDL.h>
#include <list>
#include <string>
#include <unordered_map>
#include <pu/Plutonium>
#include <vector>

namespace pksm::utils {

// One sprite as the UI consumes it: a texture and the region of it to draw
struct SpriteRef {
    pu::sdl2::TextureHandle::Ref texture;
    SDL_Rect src{0, 0, 0, 0};

    explicit operator bool() const { return texture != nullptr; }
};

// PKSS spritesheet: LZ4-compressed RGBA pages behind a sorted index (spec lives with the pkss tool).
// Per-sprite textures: texture memory is malloc heap, where large allocations fail silently in applet mode.
class SpriteSheet {
public:
    bool Load(const std::string& path);
    SpriteRef Get(u32 key);

    // Drop cached textures; sprites still on screen survive through their shared handles
    void ReleaseSprites();

    static constexpr u32 Key(u16 species, u8 form, bool shiny) {
        return (u32(species) << 12) | (u32(form) << 2) | (shiny ? 1 : 0);
    }

private:
    struct IndexEntry {
        u32 key;
        u16 page;
        u8 col, row;
    };
    static_assert(sizeof(IndexEntry) == 8);

    struct PageInfo {
        u32 dataOffset;
        u32 compressedSize;
        u32 rawSize;
    };

    struct CachedSprite {
        pu::sdl2::TextureHandle::Ref texture;
        std::list<u32>::iterator lruIt;
    };
    // ~7MB of 67x67 textures: a dozen warm boxes' worth
    static constexpr size_t MAX_CACHED_SPRITES = 384;

    const IndexEntry* FindKey(u32 key) const;
    bool DecompressPage(u16 page);

    std::vector<u8> file;
    std::vector<IndexEntry> index;
    std::vector<PageInfo> pageTable;
    std::vector<u8> staging;
    s32 stagedPage = -1;
    std::unordered_map<u32, CachedSprite> sprites;
    std::list<u32> lru;  // front = most recent
    u16 spriteWidth = 0;
    u16 spriteHeight = 0;
    u16 cellStride = 0;
    u16 pageWidth = 0;
    u16 pageHeight = 0;
};

}  // namespace pksm::utils
