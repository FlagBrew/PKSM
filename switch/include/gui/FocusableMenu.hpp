#pragma once
#include <pu/Plutonium>

class FocusableMenu : public pu::ui::elm::Menu {
    private:
        bool focused;

    public:
        using Ref = std::shared_ptr<FocusableMenu>;
        
        FocusableMenu(const pu::i32 x, const pu::i32 y, const pu::i32 width, const pu::ui::Color items_clr, const pu::ui::Color items_focus_clr, const pu::i32 items_height, const u32 items_to_show);
        
        static Ref New(const pu::i32 x, const pu::i32 y, const pu::i32 width, const pu::ui::Color items_clr, const pu::ui::Color items_focus_clr, const pu::i32 items_height, const u32 items_to_show) {
            return std::make_shared<FocusableMenu>(x, y, width, items_clr, items_focus_clr, items_height, items_to_show);
        }
        
        void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;
        
        void SetFocused(bool focused);
        bool IsFocused() const;
}; 