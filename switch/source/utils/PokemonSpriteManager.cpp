#include "utils/PokemonSpriteManager.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

#include "utils/Logger.hpp"

namespace pksm::utils {

// Initialize static members
bool PokemonSpriteManager::initialized = false;
std::map<std::string, pu::sdl2::TextureHandle::Ref> PokemonSpriteManager::spriteCache;
std::map<std::string, std::string> PokemonSpriteManager::spritePaths;

bool PokemonSpriteManager::Initialize(const std::string& jsonPath) {
    // Don't initialize twice
    if (initialized) {
        return true;
    }

    LOG_DEBUG("Initializing Pokemon sprite manager from " + jsonPath);

    // Parse JSON metadata
    if (!ParseJsonMetadata(jsonPath)) {
        LOG_ERROR("Failed to parse sprite metadata: " + jsonPath);
        return false;
    }

    initialized = true;
    LOG_DEBUG("Pokemon sprite manager initialized successfully with " + std::to_string(spritePaths.size()) + " sprites");
    return true;
}

pu::sdl2::TextureHandle::Ref PokemonSpriteManager::GetPokemonSprite(u16 species, u8 form, bool shiny) {
    // If this is an empty slot, return nullptr
    if (species == 0) {
        return nullptr;
    }

    // Lazy initialization if not already initialized
    if (!initialized) {
        if (!Initialize("romfs:/gfx/pokesprites/pokesprite.json")) {
            LOG_ERROR("Failed to initialize Pokemon sprite manager during lazy initialization");
            return nullptr;
        }
    }

    // Generate the key for this Pokemon
    std::string key = GenerateKey(species, form, shiny);

    // First check if we've already loaded this sprite
    auto it = spriteCache.find(key);
    if (it != spriteCache.end()) {
        return it->second;
    }

    // If not in cache, look for the file path
    auto pathIt = spritePaths.find(key);
    if (pathIt != spritePaths.end()) {
        pu::sdl2::TextureHandle::Ref texture = LoadSpriteFromFile(pathIt->second);
        // Cache the texture for future use
        if (texture) {
            spriteCache[key] = texture;
        }
        return texture;
    }

    // Try fallback to normal form if form > 0
    if (form > 0) {
        key = GenerateKey(species, 0, shiny);
        pathIt = spritePaths.find(key);
        if (pathIt != spritePaths.end()) {
            pu::sdl2::TextureHandle::Ref texture = LoadSpriteFromFile(pathIt->second);
            if (texture) {
                spriteCache[key] = texture;
            }
            return texture;
        }
    }

    // Try fallback to non-shiny if shiny
    if (shiny) {
        key = GenerateKey(species, form, false);
        pathIt = spritePaths.find(key);
        if (pathIt != spritePaths.end()) {
            pu::sdl2::TextureHandle::Ref texture = LoadSpriteFromFile(pathIt->second);
            if (texture) {
                spriteCache[key] = texture;
            }
            return texture;
        }
    }

    // Try fallback to non-shiny, form 0
    if (shiny && form > 0) {
        key = GenerateKey(species, 0, false);
        pathIt = spritePaths.find(key);
        if (pathIt != spritePaths.end()) {
            pu::sdl2::TextureHandle::Ref texture = LoadSpriteFromFile(pathIt->second);
            if (texture) {
                spriteCache[key] = texture;
            }
            return texture;
        }
    }

    // Sprite not found
    LOG_ERROR(
        "Failed to find sprite for species " + std::to_string(species) + ", form " + std::to_string(form) + ", shiny " +
        std::to_string(shiny)
    );
    return nullptr;
}

void PokemonSpriteManager::Cleanup() {
    // Clear the cache to release texture memory
    spriteCache.clear();
    spritePaths.clear();
    initialized = false;
}

void PokemonSpriteManager::ClearCache() {
    // Just clear the sprite cache, not the path mapping
    LOG_DEBUG("Clearing sprite cache: " + std::to_string(spriteCache.size()) + " textures released");
    spriteCache.clear();
}

void PokemonSpriteManager::ReleaseUnusedSprites(const std::set<std::string>& activeKeys) {
    // Create a copy of the keys since we'll be modifying the map
    std::vector<std::string> allKeys;
    for (const auto& pair : spriteCache) {
        allKeys.push_back(pair.first);
    }

    // Remove keys that aren't in the active list
    for (const auto& key : allKeys) {
        if (activeKeys.find(key) == activeKeys.end()) {
            spriteCache.erase(key);
        }
    }
}

std::string PokemonSpriteManager::GenerateKey(u16 species, u8 form, bool shiny) {
    std::stringstream ss;
    ss << species << "-" << static_cast<int>(form) << "-" << (shiny ? "1" : "0");
    return ss.str();
}

bool PokemonSpriteManager::ParseJsonMetadata(const std::string& jsonPath) {
    LOG_DEBUG("Parsing sprite metadata from " + jsonPath);

    // Clear any existing data
    spritePaths.clear();

    try {
        // Open and parse the JSON file
        std::ifstream file(jsonPath);
        if (!file.is_open()) {
            LOG_ERROR("Failed to open sprite metadata file: " + jsonPath);
            return false;
        }

        // Parse JSON using nlohmann/json
        nlohmann::json data = nlohmann::json::parse(file);
        file.close();

        // Base path for sprites - all sprite files are in this directory
        std::string basePath = "romfs:/gfx/pokesprites/";

        // Process each Pokemon sprite entry
        for (const auto& entry : data["pokemon"]) {
            // Skip entries marked as "skip: true" if any
            if (entry.contains("skip") && entry["skip"].get<bool>()) {
                continue;
            }

            // Extract sprite data
            u16 species = entry["id"].get<u16>();
            u8 formId = entry.contains("form_id") ? entry["form_id"].get<u8>() : 0;
            bool isShiny = entry.contains("shiny") ? entry["shiny"].get<bool>() : false;

            // Get the file path
            if (!entry.contains("file_path")) {
                LOG_ERROR("Missing file_path in sprite entry for species " + std::to_string(species));
                continue;
            }

            std::string filePath = basePath + entry["file_path"].get<std::string>();

            // Generate key and store the sprite path
            std::string key = GenerateKey(species, formId, isShiny);
            spritePaths[key] = filePath;
        }

        LOG_DEBUG("Loaded " + std::to_string(spritePaths.size()) + " sprite paths from JSON");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Error parsing JSON: " + std::string(e.what()));
        return false;
    }
}

pu::sdl2::TextureHandle::Ref PokemonSpriteManager::LoadSpriteFromFile(const std::string& filePath) {
    // Load the texture using Plutonium's loading function
    SDL_Texture* texture = pu::ui::render::LoadImage(filePath);
    if (!texture) {
        LOG_ERROR("Failed to load sprite texture: " + filePath);
        return nullptr;
    }

    // Set texture blend mode for transparency
    SDL_SetTextureBlendMode(texture, sdl::BlendModeBlend());

    // Create and return a TextureHandle
    return pu::sdl2::TextureHandle::New(texture);
}

}  // namespace pksm::utils