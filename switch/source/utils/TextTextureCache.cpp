#include "utils/TextTextureCache.hpp"

namespace pksm::utils {

std::list<TextTextureCache::Entry> TextTextureCache::order;
std::unordered_map<std::string, std::list<TextTextureCache::Entry>::iterator> TextTextureCache::index;

pu::sdl2::TextureHandle::Ref
TextTextureCache::Get(const std::string& font, const std::string& text, pu::ui::Color color) {
    // The colour is a fixed-width suffix, so its bytes matching the separator cannot mislead
    std::string key = font;
    key += '\x1f';
    key += text;
    key += '\x1f';
    key += static_cast<char>(color.r);
    key += static_cast<char>(color.g);
    key += static_cast<char>(color.b);
    key += static_cast<char>(color.a);
    if (auto found = index.find(key); found != index.end()) {
        order.splice(order.begin(), order, found->second);
        return found->second->texture;
    }
    auto texture = pu::sdl2::TextureHandle::New(pu::ui::render::RenderText(font, text, color));
    if (!texture->Get()) {
        return texture;  // a rasterization that failed under memory pressure is retried next draw, not pinned
    }
    order.push_front({key, texture});
    index[key] = order.begin();
    if (order.size() > CAPACITY) {
        index.erase(order.back().key);
        order.pop_back();
    }
    return texture;
}

void TextTextureCache::Clear() {
    index.clear();
    order.clear();
}

}  // namespace pksm::utils
