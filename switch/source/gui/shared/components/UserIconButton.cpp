#include "gui/shared/components/UserIconButton.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <chrono>
#include <thread>

#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"

namespace pksm::ui {

// Timer callback function - needs to be outside the class since it's a C-style callback
static Uint32 ExecuteCallback(Uint32 interval, void* param) {
    auto* data = static_cast<std::pair<SDL_TimerID*, std::function<void()>>*>(param);
    // Store local copies since we'll delete the data
    auto callback = data->second;
    auto timerIdPtr = data->first;

    // Clear and delete before executing callback to prevent race conditions
    SDL_TimerID timerId = *timerIdPtr;
    *timerIdPtr = 0;
    delete data;

    // Remove the timer first
    SDL_RemoveTimer(timerId);

    // Now execute the callback
    callback();
    return 0;  // Don't repeat the timer
}

UserIconButton::UserIconButton(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 diameter,
    data::AccountManager& accountManager
)
  : Element(),
    x(x),
    y(y),
    diameter(diameter),
    focused(false),
    accountManager(accountManager),
    maskedIconTexture(nullptr),
    clickTimer(0),
    isProcessingClick(false) {
    // Create username text element
    usernameText = pu::ui::elm::TextBlock::New(
        x + diameter + 10,  // Initial position to the right of icon with padding
        y,  // Start at same y as icon
        accountManager.GetAccountUsername()
    );
    usernameText->SetColor(pu::ui::Color(255, 255, 255, 255));
    usernameText->SetFont(pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_ACCOUNT_NAME));
    // Create pulsingOutline
    pulsingOutline = pksm::ui::CircularPulsingOutline::New(
        x - OUTLINE_PADDING,
        y - OUTLINE_PADDING,
        diameter + (OUTLINE_PADDING * 2),
        pu::ui::Color(0, 150, 255, 255),
        4
    );
    pulsingOutline->SetVisible(false);

    // Create outline
    outline = pksm::ui::CircularOutline::New(
        x - OUTLINE_PADDING,
        y - OUTLINE_PADDING,
        diameter + (OUTLINE_PADDING * 2),
        pu::ui::Color(70, 70, 70, 255),
        4
    );
    outline->SetVisible(true);

    // Update account info to initialize the UI
    UpdateAccountInfo();
}

UserIconButton::~UserIconButton() {
    if (maskedIconTexture) {
        SDL_DestroyTexture(maskedIconTexture);
        maskedIconTexture = nullptr;
    }
    if (clickTimer) {
        SDL_RemoveTimer(clickTimer);
        clickTimer = 0;
    }
}

void UserIconButton::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    if (focused) {
        pulsingOutline->OnRender(drawer, x - 2, y - 2);
    } else {
        outline->OnRender(drawer, x - 2, y - 2);
    }

    // Render text at its position relative to the icon
    usernameText->OnRender(drawer, x + diameter + USERNAME_PADDING, y + ((diameter - usernameText->GetHeight()) / 2));

    // Draw the masked icon if available
    if (maskedIconTexture) {
        SDL_Rect destRect = {x, y, diameter, diameter};
        SDL_RenderCopy(pu::ui::render::GetMainRenderer(), maskedIconTexture, NULL, &destRect);
    }
}

void UserIconButton::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    if (!touch_pos.IsEmpty()) {
        if (touch_pos.HitsRegion(x, y, diameter, diameter)) {
            if (onClickCallback && !isProcessingClick) {
                isProcessingClick = true;
                RequestFocus();
                // Remove any existing timer
                if (clickTimer) {
                    SDL_RemoveTimer(clickTimer);
                }
                // Create data bundle with timer ID pointer and callback
                auto* data =
                    new std::pair<SDL_TimerID*, std::function<void()>>(&clickTimer, [this, cb = onClickCallback]() {
                        cb();
                        isProcessingClick = false;
                    });
                clickTimer = SDL_AddTimer(4, ExecuteCallback, data);
            }
        }
    }
}

void UserIconButton::SetFocused(bool focus) {
    this->focused = focus;
    pulsingOutline->SetVisible(focus);
}

bool UserIconButton::IsFocused() const {
    return focused;
}

void UserIconButton::UpdateAccountInfo() {
    // Clean up existing texture if any
    if (maskedIconTexture) {
        SDL_DestroyTexture(maskedIconTexture);
        maskedIconTexture = nullptr;
    }

    // Update username text
    std::string username = accountManager.GetAccountUsername();
    usernameText->SetText(username);

    // Create new masked icon texture
    auto icon = accountManager.GetCurrentAccountIcon();
    if (icon) {
        // Create final texture for the masked result
        maskedIconTexture = SDL_CreateTexture(
            pu::ui::render::GetMainRenderer(),
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            diameter,
            diameter
        );

        if (maskedIconTexture) {
            // Set up blend mode for final texture
            SDL_SetTextureBlendMode(maskedIconTexture, SDL_BLENDMODE_BLEND);

            // Set render target to final texture
            SDL_Renderer* renderer = pu::ui::render::GetMainRenderer();
            SDL_Texture* prevTarget = SDL_GetRenderTarget(renderer);
            SDL_SetRenderTarget(renderer, maskedIconTexture);

            // Clear with full transparency
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);

            // Draw the icon first
            SDL_Rect destRect = {0, 0, diameter, diameter};
            SDL_RenderCopy(renderer, icon->Get(), NULL, &destRect);

            // Apply circular mask
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

            pu::i32 radius = diameter / 2;
            for (pu::i32 py = 0; py < diameter; py++) {
                for (pu::i32 px = 0; px < diameter; px++) {
                    const pu::i32 dx = px - radius;
                    const pu::i32 dy = py - radius;
                    bool inCircle = ((dx * dx) + (dy * dy)) <= (radius * radius);
                    if (!inCircle) {
                        // For anything outside the circle, set alpha = 0
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                        SDL_RenderDrawPoint(renderer, px, py);
                    }
                }
            }

            // Reset render target
            SDL_SetRenderTarget(renderer, prevTarget);
        }
    }
}

pu::i32 UserIconButton::GetX() {
    return x;
}

pu::i32 UserIconButton::GetY() {
    return y;
}

pu::i32 UserIconButton::GetWidth() {
    return diameter + 10 + usernameText->GetWidth();  // Icon + padding + text width
}

pu::i32 UserIconButton::GetHeight() {
    return diameter;  // Height is now just the icon diameter since text is beside it
}

void UserIconButton::SetOnClick(std::function<void()> callback) {
    onClickCallback = callback;
}

}  // namespace pksm::ui