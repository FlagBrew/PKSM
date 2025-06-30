#include "gui/screens/main-menu/sub-components/menu-grid/MenuButton.hpp"

#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"

namespace pksm::ui {

MenuButton::MenuButton(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    const std::string& text,
    const std::string& iconName
)
  : Element(),
    ShakeableWithOutline(
        PulsingOutline::New(
            x - OUTLINE_PADDING,
            y - OUTLINE_PADDING,
            width + (OUTLINE_PADDING * 2),
            height + (OUTLINE_PADDING * 2),
            pksm::ui::global::OUTLINE_COLOR,
            CORNER_RADIUS
        )
    ),
    x(x),
    y(y),
    width(width),
    height(height),
    focused(false),
    text(text),
    iconName(iconName),
    touchHandler(),
    buttonHandler() {
    LOG_DEBUG("Initializing MenuButton...");

    // Load icon texture
    pu::sdl2::Texture iconTexture = pu::ui::render::LoadImage("romfs:/gfx/ui/" + iconName + ".png");
    if (!iconTexture) {
        LOG_ERROR("Failed to load icon texture: " + iconName);
    } else {
        // Set blend mode for the texture
        SDL_SetTextureBlendMode(iconTexture, pksm::sdl::BlendModeBlend());
        icon = pu::sdl2::TextureHandle::New(iconTexture);
        LOG_DEBUG("Icon loaded successfully: " + iconName);
    }

    // Create text block
    textBlock = pu::ui::elm::TextBlock::New(0, 0, text);
    textBlock->SetFont(pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_BUTTON));
    textBlock->SetColor(TEXT_COLOR);

    // Center text below icon
    const pu::i32 textX = x + ((width - textBlock->GetWidth()) / 2);
    const pu::i32 textY = y + ICON_SIZE + TEXT_MARGIN;
    textBlock->SetX(textX);
    textBlock->SetY(textY);

    // Setup touch handler callbacks
    touchHandler.SetOnTouchUpInside([this]() {
        LOG_DEBUG("[MenuButton] Touch Up Inside");
        if (!focused && onTouchSelectCallback) {
            LOG_DEBUG("[MenuButton] Requesting focus");
            onTouchSelectCallback();
            RequestFocus();
        } else if (focused && onSelectCallback) {
            LOG_DEBUG("[MenuButton] Executing select callback");
            onSelectCallback();
        }
    });

    // Register A button with focus check
    buttonHandler.RegisterButton(
        HidNpadButton_A,
        nullptr,  // No press visual feedback needed
        [this]() {
            if (onSelectCallback) {
                LOG_DEBUG("[MenuButton] A button pressed, executing select callback");
                onSelectCallback();
            }
        },
        [this]() { return this->focused; }  // Only process when focused
    );

    LOG_DEBUG("MenuButton initialization complete");
}

pu::i32 MenuButton::GetX() {
    return x;
}

pu::i32 MenuButton::GetY() {
    return y;
}

pu::i32 MenuButton::GetWidth() {
    return width;
}

pu::i32 MenuButton::GetHeight() {
    return height;
}

void MenuButton::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Draw rounded rectangle background
    drawer->RenderRoundedRectangleFill(BACKGROUND_COLOR, x, y, width, height, CORNER_RADIUS);

    // Draw icon centered horizontally and vertically in the space above text
    if (icon) {
        const pu::i32 textHeight = textBlock->GetHeight();
        const pu::i32 totalContentHeight = ICON_SIZE + TEXT_MARGIN + textHeight;
        const pu::i32 startY = y + ((height - totalContentHeight) / 2);

        const pu::i32 iconX = x + ((width - ICON_SIZE) / 2);
        const pu::i32 iconY = startY;

        pu::ui::render::TextureRenderOptions opts;
        opts.width = ICON_SIZE;
        opts.height = ICON_SIZE;
        opts.alpha_mod = focused ? FOCUSED_ALPHA : UNFOCUSED_ALPHA;
        drawer->RenderTexture(icon->Get(), iconX, iconY, opts);

        // Update text position to be below icon
        textBlock->SetY(startY + ICON_SIZE + TEXT_MARGIN);
    }

    // Draw text with adjusted opacity
    const pu::ui::Color textColor = focused ? ui::global::TEXT_WHITE :  // Full opacity when focused
        TEXT_COLOR;  // Default translucent color
    textBlock->SetColor(textColor);

    // Use the base render alpha to control overall opacity during rendering
    drawer->SetBaseRenderAlpha(focused ? FOCUSED_ALPHA : UNFOCUSED_ALPHA);
    textBlock->OnRender(drawer, textBlock->GetX(), textBlock->GetY());
    drawer->ResetBaseRenderAlpha();

    // Draw outline if focused
    if (focused) {
        pulsingOutline->OnRender(drawer, x - OUTLINE_PADDING, y - OUTLINE_PADDING);
    }
}

void MenuButton::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    // Process touch inputs using the touch handler
    touchHandler.HandleInput(touch_pos, x, y, width, height);

    // Process button inputs using the button handler
    buttonHandler.HandleInput(keys_down, keys_up, keys_held);
}

void MenuButton::SetFocused(bool focused) {
    LOG_DEBUG("[MenuButton] Setting focused: " + std::to_string(focused));
    this->focused = focused;
    pulsingOutline->SetVisible(focused);
}

bool MenuButton::IsFocused() const {
    return focused;
}

void MenuButton::SetX(const pu::i32 x) {
    this->x = x;

    // Update outline position
    pulsingOutline->SetX(x - OUTLINE_PADDING);

    // Update text position
    const pu::i32 textX = x + ((width - textBlock->GetWidth()) / 2);
    textBlock->SetX(textX);
}

void MenuButton::SetY(const pu::i32 y) {
    this->y = y;

    // Update outline position
    pulsingOutline->SetY(y - OUTLINE_PADDING);

    // Update text position
    const pu::i32 textY = y + ICON_SIZE + TEXT_MARGIN;
    textBlock->SetY(textY);
}

}  // namespace pksm::ui