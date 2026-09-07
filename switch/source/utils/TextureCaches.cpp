#include "utils/TextureCaches.hpp"

#include "utils/AssetDownloader.hpp"
#include "utils/ItemSpriteManager.hpp"
#include "utils/PokemonSpriteManager.hpp"
#include "utils/PouchGlyphs.hpp"
#include "utils/TextTextureCache.hpp"
#include "utils/TextureGraveyard.hpp"

namespace pksm::utils {

bool TextureCaches::Initialize() {
    return PokemonSpriteManager::Initialize(AssetDownloader::ResolvedPath(AssetDownloader::Asset::PokemonSprites)) &&
        ItemSpriteManager::Initialize(AssetDownloader::ResolvedPath(AssetDownloader::Asset::ItemSprites)) &&
        PouchGlyphs::Initialize(AssetDownloader::ResolvedPath(AssetDownloader::Asset::PouchGlyphs));
}

TextureCaches::Cleared TextureCaches::Clear() {
    return {
        PokemonSpriteManager::ClearCache(),
        ItemSpriteManager::ClearCache(),
        PouchGlyphs::ClearCache(),
        TextTextureCache::Clear(),
    };
}

}  // namespace pksm::utils
