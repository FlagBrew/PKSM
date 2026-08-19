#include "utils/BoxArtSheet.hpp"

#include <SDL2/SDL_image.h>
#include <algorithm>
#include <cstring>
#include <switch.h>

#include "utils/Logger.hpp"

namespace pksm::utils {

BoxArtSheet::~BoxArtSheet() {
    if (file) {
        fclose(file);
    }
}

bool BoxArtSheet::Load(const std::string& path) {
    file = fopen(path.c_str(), "rb");
    if (!file) {
        LOG_ERROR("Cannot open box-art sheet " + path);
        return false;
    }
    fseek(file, 0, SEEK_END);
    const long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    constexpr long MAX_SHEET_SIZE = 0x4000000;  // 64MB, far above any real sheet
    u8 header[0x20];
    if (size < long(sizeof(header)) || size > MAX_SHEET_SIZE) {
        LOG_ERROR("Box-art sheet has absurd size " + std::to_string(size));
        return false;
    }
    if (fread(header, 1, sizeof(header), file) != sizeof(header)) {
        LOG_ERROR("Short read of box-art sheet header");
        return false;
    }

    u16 version;
    u8 payloadType;
    u32 entryCount, indexOffset;
    if (memcmp(header, "PKSS", 4) != 0) {
        LOG_ERROR("Box-art sheet has no PKSS magic");
        return false;
    }
    memcpy(&version, &header[0x04], 2);
    memcpy(&payloadType, &header[0x06], 1);
    memcpy(&entryCount, &header[0x08], 4);
    memcpy(&indexOffset, &header[0x0C], 4);
    if (version != 2 || payloadType != 1) {
        LOG_ERROR("Unsupported box-art sheet variant");
        return false;
    }
    const u64 fileSize = static_cast<u64>(size);
    if (static_cast<u64>(indexOffset) + static_cast<u64>(entryCount) * sizeof(IndexEntry) > fileSize) {
        LOG_ERROR("Box-art sheet header disagrees with its size");
        return false;
    }

    index.resize(entryCount);
    if (fseek(file, indexOffset, SEEK_SET) != 0 ||
        fread(index.data(), sizeof(IndexEntry), entryCount, file) != entryCount) {
        LOG_ERROR("Short read of box-art index");
        index.clear();
        return false;
    }
    u64 prevKey = 0;
    for (const IndexEntry& e : index) {
        // Strict ascent doubles as a nonzero-key check: key 0 can't beat
        // the initial prevKey
        if (e.key <= prevKey ||
            static_cast<u64>(e.dataOffset) + e.dataSize > fileSize) {
            LOG_ERROR("Box-art index entry out of bounds");
            index.clear();
            return false;
        }
        prevKey = e.key;
    }

    LOG_DEBUG(
        "Box-art sheet loaded: " + std::to_string(entryCount) + " entries, " + std::to_string(size) + " bytes"
    );
    return true;
}

SDL_Texture* BoxArtSheet::CreateTexture(u64 titleId) {
    const auto it = std::lower_bound(
        index.begin(),
        index.end(),
        titleId,
        [](const IndexEntry& e, u64 k) { return e.key < k; }
    );
    if (it == index.end() || it->key != titleId) {
        return nullptr;
    }

    std::vector<u8> blob(it->dataSize);
    if (fseek(file, it->dataOffset, SEEK_SET) != 0 ||
        fread(blob.data(), 1, blob.size(), file) != blob.size()) {
        LOG_ERROR("Short read of box-art blob for title " + std::to_string(titleId));
        return nullptr;
    }

    SDL_Surface* surface = IMG_Load_RW(SDL_RWFromMem(blob.data(), blob.size()), 1);
    if (!surface) {
        LOG_ERROR("Failed to decode box art for title " + std::to_string(titleId));
        return nullptr;
    }
    SDL_Texture* texture = pu::ui::render::ConvertToTexture(surface);
    if (!texture) {
        LOG_ERROR("Failed to convert box art to texture for title " + std::to_string(titleId));
    }
    return texture;
}

}  // namespace pksm::utils
