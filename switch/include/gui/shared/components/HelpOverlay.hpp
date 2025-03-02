#pragma once

#include <pu/ui/ui_Overlay.hpp>

#include "gui/shared/components/HelpFooter.hpp"

namespace pksm::ui {

class HelpOverlay : public pu::ui::Overlay {
public:
    static constexpr pu::i32 OVERLAY_PADDING = 40;  // Left/right padding
    static constexpr pu::i32 OVERLAY_TOP_MARGIN = 120;  // Space from top of overlay
    static constexpr pu::i32 ITEM_VERTICAL_SPACING = 30;  // Space between help items
    static constexpr pu::i32 BUTTON_ACTION_SPACING = 10;  // Space between button and its description
    static constexpr pu::i32 GLYPH_Y_OFFSET = -4;  // Fine-tune vertical alignment of button glyphs

    HelpOverlay(const pu::i32 x, const pu::i32 y, const pu::i32 width, const pu::i32 height);
    PU_SMART_CTOR(HelpOverlay)

    void SetHelpItems(const std::vector<HelpItem>& items);
    void Show();
    void Hide();

private:
    void UpdateHelpTexts();
    std::string GetButtonGlyph(HelpButton button) const;

    std::vector<HelpItem> helpItems;
    std::vector<pu::ui::elm::TextBlock::Ref> helpTexts;
};

}  // namespace pksm::ui