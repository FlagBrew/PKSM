// UserIconButton.hpp
#pragma once

#include <SDL2/SDL.h>
#include <SDLHelper.hpp>
#include <pu/Plutonium>

#include "data/AccountManager.hpp"
#include "gui/shared/components/PulsingOutline.hpp"
#include "gui/shared/components/ShakeableWithOutline.hpp"
#include "gui/shared/components/StaticOutline.hpp"
#include "gui/shared/interfaces/IHelpProvider.hpp"
#include "input/ButtonInputHandler.hpp"
#include "input/TouchInputHandler.hpp"
#include "input/visual-feedback/interfaces/IFocusable.hpp"

namespace pksm::ui {

class UserIconButton : public pu::ui::elm::Element,
                       public IFocusable,
                       public IHelpProvider,
                       public ShakeableWithOutline {
public:
    static constexpr pu::i32 OUTLINE_PADDING = 2;
    static constexpr pu::i32 USERNAME_PADDING = 16;

    PU_SMART_CTOR(UserIconButton)

    UserIconButton(const pu::i32 x, const pu::i32 y, const pu::i32 diameter, data::AccountManager& accountManager);
    ~UserIconButton();

    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void
    OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;

    void SetFocused(bool focus) override;
    bool IsFocused() const override;
    void UpdateAccountInfo();

    // Disable/enable the button (affects input handling only)
    void SetDisabled(bool disabled);
    bool IsDisabled() const;

    // Set callbacks
    void SetOnCancel(std::function<void()> callback) { onCancelCallback = callback; }

    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;

    std::vector<HelpItem> GetHelpItems() const override;

private:
    pu::i32 x;
    pu::i32 y;
    pu::i32 diameter;
    bool focused;
    bool disabled;
    data::AccountManager& accountManager;
    SDL_Texture* maskedIconTexture;
    pu::ui::elm::TextBlock::Ref usernameText;
    pksm::ui::StaticOutlineBase::Ref outline;
    pksm::input::TouchInputHandler touchHandler;
    pksm::input::ButtonInputHandler buttonHandler;
    std::function<void()> onCancelCallback;

    static constexpr pu::ui::Color USER_ICON_BUTTON_OUTLINE_COLOR = pu::ui::Color(70, 70, 70, 255);
};

}  // namespace pksm::ui