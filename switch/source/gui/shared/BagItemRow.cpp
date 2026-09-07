#include "gui/shared/components/BagItemRow.hpp"

#include <utility>

#include "gui/shared/UIConstants.hpp"
#include "utils/ItemSpriteManager.hpp"
#include "utils/TextTextureCache.hpp"

namespace {

const std::string& RowFont() {
    static const std::string font =
        pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_TRIGGER_BUTTON_NAVIGATION);
    return font;
}

// The button font carries the console's glyphs and falls back to the shared font for letters
const std::string& PillFont() {
    static const std::string font =
        pksm::ui::global::MakeSwitchButtonFontName(pksm::ui::global::FONT_SIZE_ACCOUNT_NAME);
    return font;
}

const std::string& PillHeartFont() {
    static const std::string font = pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_ACCOUNT_NAME);
    return font;
}

bool TextureSize(const pu::sdl2::TextureHandle::Ref& texture, int& width, int& height) {
    return texture && texture->Get() && SDL_QueryTexture(texture->Get(), nullptr, nullptr, &width, &height) == 0;
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

void pksm::ui::BagItemRow::SetItem(
    u32 spriteKey,
    const std::string& itemName,
    const std::string& detail,
    bool isNew,
    bool favorite,
    const std::string& shortcut
) {
    this->spriteKey = spriteKey;
    name = itemName;
    this->detail = detail;
    this->isNew = isNew;
    this->favorite = favorite;
    this->shortcut = shortcut;
    edited = false;
    Release();
}

void pksm::ui::BagItemRow::Release() {
    sprite->SetImage({});
    spriteResolved = false;
    nameTexture = nullptr;
    detailTexture = nullptr;
    newMarkTexture = nullptr;
    favoriteMarkTexture = nullptr;
    shortcutTexture = nullptr;
    pillHeartTexture = nullptr;
}

void pksm::ui::BagItemRow::SetDetail(const std::string& detail, bool edited) {
    this->detail = detail;
    this->edited = edited;
    detailTexture = nullptr;
}

void pksm::ui::BagItemRow::SwapContent(BagItemRow& other) {
    std::swap(spriteKey, other.spriteKey);
    std::swap(name, other.name);
    std::swap(detail, other.detail);
    std::swap(isNew, other.isNew);
    std::swap(favorite, other.favorite);
    std::swap(shortcut, other.shortcut);
    std::swap(edited, other.edited);
    std::swap(spriteResolved, other.spriteResolved);
    std::swap(nameTexture, other.nameTexture);
    std::swap(detailTexture, other.detailTexture);
    std::swap(newMarkTexture, other.newMarkTexture);
    std::swap(favoriteMarkTexture, other.favoriteMarkTexture);
    std::swap(shortcutTexture, other.shortcutTexture);
    std::swap(pillHeartTexture, other.pillHeartTexture);
    std::swap(sprite, other.sprite);  // same place in every row, so the image goes with the key
}

void pksm::ui::BagItemRow::SetSelected(bool select) {
    selected = select;
    UpdateBackground();
}

void pksm::ui::BagItemRow::SetLifted(bool lift) {
    lifted = lift;
    UpdateBackground();
}

void pksm::ui::BagItemRow::UpdateBackground() {
    background->SetColor(lifted ? LIFTED_BG_COLOR : selected ? SELECTED_BG_COLOR : DEFAULT_BG_COLOR);
}

void pksm::ui::BagItemRow::SetFocused(bool focus) {
    focused = focus;
    pulsingOutline->SetVisible(focus);
}

bool pksm::ui::BagItemRow::IsFocused() const {
    return focused;
}

pu::i32 pksm::ui::BagItemRow::DrawText(
    pu::ui::render::Renderer::Ref& drawer,
    pu::sdl2::TextureHandle::Ref& texture,
    const std::string& text,
    pu::ui::Color color,
    pu::i32 textX,
    pu::i32 rowY,
    bool alignRight
) {
    if (text.empty()) {
        return 0;  // a row need not have a detail
    }
    if (!texture) {
        texture = utils::TextTextureCache::Get(RowFont(), text, color);
    }
    int textWidth = 0;
    int textHeight = 0;
    if (!texture->Get() || SDL_QueryTexture(texture->Get(), nullptr, nullptr, &textWidth, &textHeight) != 0) {
        return 0;
    }
    if (alignRight) {
        textX = textX + width - PADDING - textWidth;
    }
    drawer->RenderTexture(texture->Get(), textX, rowY + (height - textHeight) / 2);
    return textWidth;
}

pu::i32 pksm::ui::BagItemRow::DrawShortcut(pu::ui::render::Renderer::Ref& drawer, pu::i32 pillX, pu::i32 rowY) {
    if (!shortcutTexture) {
        shortcutTexture = utils::TextTextureCache::Get(PillFont(), shortcut, FAVORITE_MARK_COLOR);
    }
    if (!pillHeartTexture) {
        pillHeartTexture = utils::TextTextureCache::Get(PillHeartFont(), "♥", PILL_CAP_INK);
    }
    int labelWidth = 0;
    int labelHeight = 0;
    int heartWidth = 0;
    int heartHeight = 0;
    if (!TextureSize(shortcutTexture, labelWidth, labelHeight) ||
        !TextureSize(pillHeartTexture, heartWidth, heartHeight)) {
        return 0;
    }
    const pu::i32 pillWidth = PILL_CAP + PILL_PAD_LEFT + labelWidth + PILL_PAD_RIGHT;
    const pu::i32 pillY = rowY + (height - PILL_HEIGHT) / 2;
    const pu::i32 radius = PILL_HEIGHT / 2;
    // A dark pill; the cap round on the left, straight where the label starts. A rounded fill
    // does not close a half-height radius into a true semicircle, so the cap is a circle and a
    // rect and the border nested outlines
    drawer->RenderRoundedRectangleFill(PILL_CAP_INK, pillX, pillY, pillWidth, PILL_HEIGHT, radius);
    drawer->RenderCircleFill(FAVORITE_MARK_COLOR, pillX + radius, pillY + radius, radius);
    drawer->RenderRectangleFill(FAVORITE_MARK_COLOR, pillX + radius, pillY, PILL_CAP - radius, PILL_HEIGHT);
    for (pu::i32 i = 0; i < PILL_BORDER; i++) {
        drawer->RenderRoundedRectangle(
            FAVORITE_MARK_COLOR, pillX + i, pillY + i, pillWidth - 2 * i, PILL_HEIGHT - 2 * i, radius - i
        );
    }
    drawer->RenderTexture(
        pillHeartTexture->Get(), pillX + (PILL_CAP - heartWidth) / 2, pillY + (PILL_HEIGHT - heartHeight) / 2
    );
    drawer->RenderTexture(
        shortcutTexture->Get(), pillX + PILL_CAP + PILL_PAD_LEFT, pillY + (PILL_HEIGHT - labelHeight) / 2
    );
    return pillWidth;
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
    pu::i32 cursor = x + (PADDING * 2) + SPRITE_SIZE;
    cursor += DrawText(drawer, nameTexture, name, global::TEXT_WHITE, cursor, y);
    // The game's marks trail the name: its red dot for an item not looked at, a heart for a favourite
    if (isNew) {
        cursor += MARK_GAP + DrawText(drawer, newMarkTexture, "●", NEW_MARK_COLOR, cursor + MARK_GAP, y);
    }
    if (favorite) {
        cursor += MARK_GAP + DrawText(drawer, favoriteMarkTexture, "♥", FAVORITE_MARK_COLOR, cursor + MARK_GAP, y);
    }
    if (!shortcut.empty()) {
        DrawShortcut(drawer, cursor + MARK_GAP, y);
    }
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
