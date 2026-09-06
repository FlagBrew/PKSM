#include "gui/shared/components/BagItemRow.hpp"

#include "gui/shared/UIConstants.hpp"
#include "utils/ItemSpriteManager.hpp"
#include "utils/TextTextureCache.hpp"

namespace {

const std::string& RowFont() {
    static const std::string font =
        pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_TRIGGER_BUTTON_NAVIGATION);
    return font;
}

}  // namespace

pksm::ui::BagItemRow::BagItemRow(const pu::i32 x, const pu::i32 y, const pu::i32 width, const pu::i32 height)
  : Element(),
    ShakeableWithOutline(
        pksm::ui::PulsingOutline::New(
            x - OUTLINE_PADDING,
            y - OUTLINE_PADDING,
            width + (OUTLINE_PADDING * 2),
            height + (OUTLINE_PADDING * 2),
            pksm::ui::global::OUTLINE_COLOR,
            0,
            OUTLINE_BORDER_WIDTH
        )
    ),
    x(x),
    y(y),
    width(width),
    height(height) {
    background = pu::ui::elm::Rectangle::New(0, 0, width, height, DEFAULT_BG_COLOR);
    sprite = SpriteImage::New(PADDING, (height - SPRITE_SIZE) / 2, SPRITE_SIZE, SPRITE_SIZE, utils::SpriteRef{});

    touchHandler.SetOnTouchUpInside([this]() {
        if (!focused && onTouchSelectCallback) {
            onTouchSelectCallback();
            RequestFocus();
        }
    });

    pulsingOutline->SetVisible(false);
}

pu::i32 pksm::ui::BagItemRow::GetX() {
    return x;
}

pu::i32 pksm::ui::BagItemRow::GetY() {
    return y;
}

pu::i32 pksm::ui::BagItemRow::GetWidth() {
    return width;
}

pu::i32 pksm::ui::BagItemRow::GetHeight() {
    return height;
}

void pksm::ui::BagItemRow::SetItem(u32 spriteKey, const std::string& itemName, const std::string& detail) {
    this->spriteKey = spriteKey;
    name = itemName;
    this->detail = detail;
    edited = false;
    Release();
}

void pksm::ui::BagItemRow::Release() {
    sprite->SetImage({});
    spriteResolved = false;
    nameTexture = nullptr;
    detailTexture = nullptr;
}

void pksm::ui::BagItemRow::SetDetail(const std::string& detail, bool edited) {
    this->detail = detail;
    this->edited = edited;
    detailTexture = nullptr;
}

void pksm::ui::BagItemRow::SetSelected(bool select) {
    selected = select;
    background->SetColor(select ? SELECTED_BG_COLOR : DEFAULT_BG_COLOR);
}

void pksm::ui::BagItemRow::SetFocused(bool focus) {
    focused = focus;
    pulsingOutline->SetVisible(focus);
}

bool pksm::ui::BagItemRow::IsFocused() const {
    return focused;
}

void pksm::ui::BagItemRow::DrawText(
    pu::ui::render::Renderer::Ref& drawer,
    pu::sdl2::TextureHandle::Ref& texture,
    const std::string& text,
    pu::ui::Color color,
    pu::i32 rowX,
    pu::i32 rowY,
    bool alignRight
) {
    if (text.empty()) {
        return;  // a row need not have a detail
    }
    if (!texture) {
        texture = utils::TextTextureCache::Get(RowFont(), text, color);
    }
    int textWidth = 0;
    int textHeight = 0;
    if (!texture->Get() || SDL_QueryTexture(texture->Get(), nullptr, nullptr, &textWidth, &textHeight) != 0) {
        return;
    }
    const pu::i32 textX = alignRight ? rowX + width - PADDING - textWidth : rowX + (PADDING * 2) + SPRITE_SIZE;
    drawer->RenderTexture(texture->Get(), textX, rowY + (height - textHeight) / 2);
}

void pksm::ui::BagItemRow::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    if (focused) {
        pulsingOutline->OnRender(drawer, x - OUTLINE_PADDING, y - OUTLINE_PADDING);
    }
    background->OnRender(drawer, x, y);
    if (!spriteResolved) {
        sprite->SetImage(utils::ItemSpriteManager::GetSprite(spriteKey));
        spriteResolved = true;
    }
    sprite->OnRender(drawer, x + sprite->GetX(), y + sprite->GetY());
    DrawText(drawer, nameTexture, name, global::TEXT_WHITE, x, y, false);
    DrawText(drawer, detailTexture, detail, edited ? EDITED_TEXT_COLOR : global::TEXT_WHITE, x, y, true);
}

void pksm::ui::BagItemRow::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    touchHandler.HandleInput(touch_pos, x, y, width, height);
}
