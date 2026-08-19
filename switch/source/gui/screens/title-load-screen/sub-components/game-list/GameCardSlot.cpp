#include "gui/screens/title-load-screen/sub-components/game-list/GameCardSlot.hpp"

#include "utils/Logger.hpp"

pksm::ui::GameCardSlot::GameCardSlot(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 cardSize,
    const pu::i32 outlinePadding
)
  : ShakeableWithOutline(pksm::ui::PulsingOutline::New(
        x - outlinePadding,
        y - outlinePadding,
        cardSize + (outlinePadding * 2),
        cardSize + (outlinePadding * 2),
        pksm::ui::global::OUTLINE_COLOR,
        cardSize * FRAME_CORNER_RADIUS / FRAME_CANVAS
    )),
    x(x),
    y(y),
    cardSize(cardSize),
    outlinePadding(outlinePadding),
    focused(false),
    selected(false),
    title(nullptr),
    touchHandler(),
    buttonHandler() {
    frameTexture = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage("romfs:/gfx/ui/game_card_frame.png"));
    emptyFrameTexture =
        pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage("romfs:/gfx/ui/game_card_frame_empty.png"));

    // Empty-state hint, centered in the frame's label window
    emptyText = pu::ui::elm::TextBlock::New(0, 0, "No Game Card");
    emptyText->SetColor(pu::ui::Color(170, 170, 170, 255));
    emptyText->SetFont(pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_TRIGGER_BUTTON_NAVIGATION));
    emptyText->SetX(x + Scaled(WINDOW_X) + (Scaled(WINDOW_WIDTH) - emptyText->GetWidth()) / 2);
    emptyText->SetY(y + Scaled(WINDOW_Y) + (Scaled(WINDOW_HEIGHT) - emptyText->GetHeight()) / 2);

    touchHandler.SetOnTouchUpInside([this]() {
        LOG_DEBUG("[GameCardSlot] Touch Up Inside");
        if (!focused && onTouchSelectCallback) {
            onTouchSelectCallback();
            RequestFocus();
        } else if (focused && onSelectCallback) {
            onSelectCallback();
        }
    });

    buttonHandler.RegisterButton(
        HidNpadButton_A,
        nullptr,
        [this]() {
            if (onSelectCallback) {
                onSelectCallback();
            }
        },
        [this]() { return this->focused; }
    );
}

void pksm::ui::GameCardSlot::SetTitle(pksm::titles::Title::Ref newTitle) {
    title = newTitle;
    const std::string name = title ? "GameCard Slot Element " + title->getName() : "GameCard Slot Element (empty)";
    ISelectable::SetName(name);
    IFocusable::SetName(name);
}

void pksm::ui::GameCardSlot::SetSelected(bool select) {
    this->selected = select;
}

bool pksm::ui::GameCardSlot::IsSelected() const {
    return selected;
}

void pksm::ui::GameCardSlot::SetFocused(bool focus) {
    LOG_DEBUG("[GameCardSlot] Setting focused to " + std::string(focus ? "true" : "false"));
    if (focus) {
        RequestSelection();
    }
    this->focused = focus;
    pulsingOutline->SetVisible(focus && selected);
}

bool pksm::ui::GameCardSlot::IsFocused() const {
    return focused;
}

void pksm::ui::GameCardSlot::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Empty state: ghosted frame + hint text (the slot is not selectable)
    if (!title) {
        if (emptyFrameTexture && emptyFrameTexture->Get()) {
            drawer->RenderTexture(
                emptyFrameTexture->Get(),
                x,
                y,
                pu::ui::render::TextureRenderOptions{
                    EMPTY_FRAME_ALPHA,
                    cardSize,
                    cardSize,
                    pu::ui::render::TextureRenderOptions::NoRotation
                }
            );
        }
        emptyText->OnRender(drawer, emptyText->GetX(), emptyText->GetY());
        return;
    }

    const u8 mod = selected ? 255 : DIM_COLOR_MOD;

    // Game icon first, so it shows through the frame's transparent window
    if (title && title->getIcon() && title->getIcon()->Get()) {
        SDL_Texture* iconTex = title->getIcon()->Get();
        SDL_SetTextureColorMod(iconTex, mod, mod, mod);
        drawer->RenderTexture(
            iconTex,
            x + Scaled(WINDOW_X),
            y + Scaled(WINDOW_Y),
            pu::ui::render::TextureRenderOptions{
                pu::ui::render::TextureRenderOptions::NoAlpha,
                Scaled(WINDOW_WIDTH),
                Scaled(WINDOW_HEIGHT),
                pu::ui::render::TextureRenderOptions::NoRotation
            }
        );
        SDL_SetTextureColorMod(iconTex, 255, 255, 255);
    }

    // Cartridge frame on top
    if (frameTexture && frameTexture->Get()) {
        SDL_Texture* frameTex = frameTexture->Get();
        SDL_SetTextureColorMod(frameTex, mod, mod, mod);
        drawer->RenderTexture(
            frameTex,
            x,
            y,
            pu::ui::render::TextureRenderOptions{
                pu::ui::render::TextureRenderOptions::NoAlpha,
                cardSize,
                cardSize,
                pu::ui::render::TextureRenderOptions::NoRotation
            }
        );
        SDL_SetTextureColorMod(frameTex, 255, 255, 255);
    }

    if (focused && selected) {
        pulsingOutline->OnRender(drawer, x - outlinePadding, y - outlinePadding);
    }
}

void pksm::ui::GameCardSlot::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    if (!title) {
        return;
    }
    touchHandler.HandleInput(touch_pos, GetX(), GetY(), GetWidth(), GetHeight());
    buttonHandler.HandleInput(keys_down, keys_up, keys_held);
}
