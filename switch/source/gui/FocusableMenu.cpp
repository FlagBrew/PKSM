#include "gui/FocusableMenu.hpp"

FocusableMenu::FocusableMenu(const pu::i32 x, const pu::i32 y, const pu::i32 width, const pu::ui::Color items_clr, const pu::ui::Color items_focus_clr, const pu::i32 items_height, const u32 items_to_show)
    : Menu(x, y, width, items_clr, items_focus_clr, items_height, items_to_show), focused(false) {
}

void FocusableMenu::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
    if (focused) {
        Menu::OnInput(keys_down, keys_up, keys_held, touch_pos);
    }
}

void FocusableMenu::SetFocused(bool focused) {
    this->focused = focused;
}

bool FocusableMenu::IsFocused() const {
    return focused;
} 