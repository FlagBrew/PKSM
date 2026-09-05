#pragma once

#include <list>
#include <pu/Plutonium>
#include <string>
#include <unordered_map>

namespace pksm::utils {

// Rasterized text shared by everything that draws the same string in the same font and colour,
// kept in a bounded least-recently-used cache. Destroying a texture costs a third of a
// millisecond on the console, so a list that let every scrolled-past row keep its own text
// paid hundreds of them at once when it changed; here an eviction is one texture at a time,
// and the rows in view hold their entries alive whether or not the cache still does.
// Render thread only: the cache is unguarded static state.
class TextTextureCache {
public:
    // Texture for text in the named font; rasterized on a miss
    static pu::sdl2::TextureHandle::Ref Get(const std::string& font, const std::string& text, pu::ui::Color color);

    // Drop every cached texture (session teardown)
    static void Clear();

private:
    static constexpr size_t CAPACITY = 96;  // a few screens of two-texture rows

    struct Entry {
        std::string key;
        pu::sdl2::TextureHandle::Ref texture;
    };
    // Most recently used at the front; the map points into the list
    static std::list<Entry> order;
    static std::unordered_map<std::string, std::list<Entry>::iterator> index;
};

}  // namespace pksm::utils
