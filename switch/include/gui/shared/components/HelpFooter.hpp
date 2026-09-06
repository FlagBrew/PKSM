#pragma once

#include <pu/Plutonium>
#include <string>
#include <vector>

#include "gui/shared/UIConstants.hpp"
#include "gui/shared/components/HelpItem.hpp"

namespace pksm::ui {

class HelpFooter : public pu::ui::elm::Element {
public:
    static constexpr pu::i32 FOOTER_HEIGHT = 80;
    static constexpr pu::i32 DIVIDER_MARGIN = 28;  // Margin from screen edge to divider
    static constexpr pu::i32 DIVIDER_HEIGHT = 2;
    static constexpr pu::i32 INTERNAL_PADDING = 16;  // Padding from divider to content
    static constexpr pu::i32 BUTTON_SPACING = 8;
    static constexpr pu::i32 BUTTON_ACTION_SPACING = 20;
    static constexpr pu::i32 HELP_ITEM_SPACING = 28;
    static constexpr u32 GLYPH_Y_OFFSET = -4;

    HelpFooter(const pu::i32 x, const pu::i32 y, const pu::i32 width);
    PU_SMART_CTOR(HelpFooter)

    // Element implementation
    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void
    OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;

    // Help item management
    void SetHelpItems(const std::vector<HelpItem>& items);

private:
    pu::i32 x;
    pu::i32 y;
    pu::i32 width;
    std::vector<HelpItem> helpItems;

    // UI Elements
    pu::ui::elm::Rectangle::Ref divider;
    pu::ui::elm::TextBlock::Ref minusButtonText;
    pu::ui::elm::TextBlock::Ref helpText;
    std::vector<pu::ui::elm::TextBlock::Ref> dynamicHelpTexts;

    // Helper methods
    void UpdateDynamicHelpTexts();
    void PositionElements();
};

}  // namespace pksm::ui