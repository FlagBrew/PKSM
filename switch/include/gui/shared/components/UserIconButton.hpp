// UserIconButton.hpp
#pragma once

#include <SDL2/SDL.h>
#include <pu/Plutonium>

#include "data/AccountManager.hpp"
#include "gui/shared/components/PulsingOutline.hpp"
#include "gui/shared/components/StaticOutline.hpp"
#include "input/visual-feedback/interfaces/IFocusable.hpp"

namespace pksm::ui {

class UserIconButton : public pu::ui::elm::Element, public IFocusable {
public:
    static constexpr pu::i32 OUTLINE_PADDING = 2;
    static constexpr pu::i32 USERNAME_PADDING = 16;

    PU_SMART_CTOR(UserIconButton)

    UserIconButton(const pu::i32 x, const pu::i32 y, const pu::i32 diameter, data::AccountManager& accountManager);
    ~UserIconButton();

    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos)
        override;

    void SetFocused(bool focus) override;
    bool IsFocused() const override;

    void SetOnClick(std::function<void()> callback);
    void UpdateAccountInfo();

    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;

private:
    pu::i32 x;
    pu::i32 y;
    pu::i32 diameter;
    bool focused;
    std::function<void()> onClickCallback;
    data::AccountManager& accountManager;
    bool isProcessingClick;  // Flag to prevent double execution

    SDL_Texture* maskedIconTexture;  // Store the masked icon texture
    pu::ui::elm::TextBlock::Ref usernameText;
    pksm::ui::PulsingOutlineBase::Ref pulsingOutline;
    pksm::ui::StaticOutlineBase::Ref outline;
};

}  // namespace pksm::ui