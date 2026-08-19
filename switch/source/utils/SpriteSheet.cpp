#include "utils/SpriteSheet.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <switch.h>

#include "lz4.h"
#include "utils/Logger.hpp"
#include "utils/SDLHelper.hpp"

namespace pksm::utils {

bool SpriteSheet::Load(const std::string& path) {
    const u64 t0 = armGetSystemTick();

    FILE* f = fopen(path.c_str(), "rb");
    if (!f) {
        LOG_ERROR("Cannot open spritesheet " + path);
        return false;
    }
    fseek(f, 0, SEEK_END);
    const long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    constexpr long MAX_SHEET_SIZE = 0x4000000;  // 64MB, far above any real sheet
    if (size < 0x20 || size > MAX_SHEET_SIZE) {
        fclose(f);
        LOG_ERROR("Spritesheet has absurd size " + std::to_string(size));
        return false;
    }
    file.resize(size);
    const size_t got = fread(file.data(), 1, size, f);
    fclose(f);
    if (got != static_cast<size_t>(size)) {
        LOG_ERROR("Short read of spritesheet " + path);
        return false;
    }

    u16 version, pageCount;
    u8 pixelFormat, compression;
    u32 spriteCount, indexOffset, pageTableOffset;
    if (memcmp(file.data(), "PKSS", 4) != 0) {
        LOG_ERROR("Spritesheet has no PKSS magic");
        return false;
    }
    memcpy(&version, &file[0x04], 2);
    memcpy(&pixelFormat, &file[0x06], 1);
    memcpy(&compression, &file[0x07], 1);
    memcpy(&spriteWidth, &file[0x08], 2);
    memcpy(&spriteHeight, &file[0x0A], 2);
    memcpy(&cellStride, &file[0x0C], 2);
    memcpy(&pageWidth, &file[0x0E], 2);
    memcpy(&pageHeight, &file[0x10], 2);
    memcpy(&pageCount, &file[0x12], 2);
    memcpy(&spriteCount, &file[0x14], 4);
    memcpy(&indexOffset, &file[0x18], 4);
    memcpy(&pageTableOffset, &file[0x1C], 4);
    if (version != 1 || pixelFormat != 0 || compression != 1) {
        LOG_ERROR("Unsupported spritesheet variant");
        return false;
    }
    const u64 fileSize = static_cast<u64>(size);
    const u64 pageBytes = static_cast<u64>(pageWidth) * pageHeight * 4;
    if (spriteWidth == 0 || spriteHeight == 0 || cellStride == 0 || pageWidth == 0 || pageHeight == 0 ||
        static_cast<u64>(indexOffset) + static_cast<u64>(spriteCount) * 8 > fileSize ||
        static_cast<u64>(pageTableOffset) + static_cast<u64>(pageCount) * 12 > fileSize) {
        LOG_ERROR("Spritesheet header disagrees with its size");
        return false;
    }

    index.resize(spriteCount);
    memcpy(index.data(), &file[indexOffset], spriteCount * sizeof(IndexEntry));
    for (const IndexEntry& e : index) {
        if (e.page >= pageCount || u32(e.col) * cellStride + spriteWidth > pageWidth ||
            u32(e.row) * cellStride + spriteHeight > pageHeight) {
            LOG_ERROR("Spritesheet index entry out of bounds");
            index.clear();
            return false;
        }
    }
    pageTable.resize(pageCount);
    for (u16 p = 0; p < pageCount; p++) {
        memcpy(&pageTable[p], &file[pageTableOffset + p * 12], sizeof(PageInfo));
        if (static_cast<u64>(pageTable[p].dataOffset) + pageTable[p].compressedSize > fileSize ||
            pageTable[p].rawSize != pageBytes) {
            LOG_ERROR("Spritesheet page table out of bounds");
            index.clear();
            pageTable.clear();
            return false;
        }
    }

    // The one page-sized buffer every decompression reuses, allocated once
    staging.resize(pageBytes);

    LOG_DEBUG(
        "Spritesheet loaded: " + std::to_string(spriteCount) + " sprites, " + std::to_string(pageCount) +
        " pages, " + std::to_string(size) + " bytes, " +
        std::to_string(armTicksToNs(armGetSystemTick() - t0) / 1000000) + " ms"
    );
    return true;
}

const SpriteSheet::IndexEntry* SpriteSheet::FindKey(u32 key) const {
    const IndexEntry* begin = index.data();
    const IndexEntry* end = begin + index.size();
    const IndexEntry* it =
        std::lower_bound(begin, end, key, [](const IndexEntry& e, u32 k) { return e.key < k; });
    return (it != end && it->key == key) ? it : nullptr;
}

bool SpriteSheet::DecompressPage(u16 page) {
    if (stagedPage == page) {
        return true;
    }
    const PageInfo& info = pageTable[page];
    const int decoded = LZ4_decompress_safe(
        reinterpret_cast<const char*>(&file[info.dataOffset]),
        reinterpret_cast<char*>(staging.data()),
        info.compressedSize,
        staging.size()
    );
    if (decoded != static_cast<int>(info.rawSize)) {
        LOG_ERROR("Sprite page " + std::to_string(page) + " failed to decompress");
        stagedPage = -1;
        return false;
    }
    stagedPage = page;
    return true;
}

SpriteRef SpriteSheet::Get(u32 key) {
    auto cached = sprites.find(key);
    if (cached != sprites.end()) {
        lru.splice(lru.begin(), lru, cached->second.lruIt);
        return {cached->second.texture, {0, 0, spriteWidth, spriteHeight}};
    }

    const IndexEntry* entry = FindKey(key);
    if (!entry || !DecompressPage(entry->page)) {
        return {};
    }

    SDL_Texture* texture = SDL_CreateTexture(
        pu::ui::render::GetMainRenderer(),
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STATIC,
        spriteWidth,
        spriteHeight
    );
    if (!texture) {
        LOG_ERROR("Sprite texture creation failed: " + std::string(SDL_GetError()));
        return {};
    }
    const size_t cellOffset = (size_t(entry->row) * cellStride * pageWidth + size_t(entry->col) * cellStride) * 4;
    if (SDL_UpdateTexture(texture, nullptr, staging.data() + cellOffset, pageWidth * 4) != 0) {
        LOG_ERROR("Sprite upload failed: " + std::string(SDL_GetError()));
        SDL_DestroyTexture(texture);
        return {};
    }
    SDL_SetTextureBlendMode(texture, sdl::BlendModeBlend());

    if (sprites.size() >= MAX_CACHED_SPRITES) {
        sprites.erase(lru.back());
        lru.pop_back();
    }
    lru.push_front(key);
    auto handle = pu::sdl2::TextureHandle::New(texture);
    sprites.emplace(key, CachedSprite{handle, lru.begin()});
    return {handle, {0, 0, spriteWidth, spriteHeight}};
}

void SpriteSheet::ReleaseSprites() {
    const size_t released = sprites.size();
    sprites.clear();
    lru.clear();
    stagedPage = -1;
    LOG_DEBUG("Released " + std::to_string(released) + " cached sprites");
}

}  // namespace pksm::utils
