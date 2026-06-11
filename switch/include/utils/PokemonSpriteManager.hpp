#pragma once

#include <SDL2/SDL.h>
#include <SDLHelper.hpp>
#include <map>
#include <memory>
#include <pu/Plutonium>
#include <set>
#include <string>

namespace pksm::utils {

class PokemonSpriteManager {
public:
    // Initialize with path to JSON metadata
    static bool Initialize(const std::string& jsonPath);

    // Get a texture handle for a specific Pokemon
    static pu::sdl2::TextureHandle::Ref GetPokemonSprite(u16 species, u8 form = 0, bool shiny = false);

    // Clean up resources
    static void Cleanup();

    // Clear the sprite cache completely
    static void ClearCache();

    // Release unused sprites from cache that aren't in the active list
    static void ReleaseUnusedSprites(const std::set<std::string>& activeKeys);

    // Generate a key for the sprite cache
    static std::string GenerateKey(u16 species, u8 form, bool shiny);

private:
    // Whether the manager has been initialized
    static bool initialized;

    // Cache of loaded textures - key is "id-form_id-shiny"
    static std::map<std::string, pu::sdl2::TextureHandle::Ref> spriteCache;

    // Mapping from pokemon data to file paths - key is "id-form_id-shiny"
    static std::map<std::string, std::string> spritePaths;

    // Parse JSON metadata
    static bool ParseJsonMetadata(const std::string& jsonPath);

    // Load a sprite from file
    static pu::sdl2::TextureHandle::Ref LoadSpriteFromFile(const std::string& filePath);
};

}  // namespace pksm::utils