#pragma once

#include <pu/ui/ui_Overlay.hpp>

#include "data/summary/SummaryData.hpp"

namespace pksm::ui {

// Renders one Pokémon's SummaryData verbatim; the builder decides which
// rows and accents exist per format - no generation knowledge here
class PokemonSummaryOverlay : public pu::ui::Overlay {
public:
    PokemonSummaryOverlay(const pu::i32 x, const pu::i32 y, const pu::i32 width, const pu::i32 height);
    PU_SMART_CTOR(PokemonSummaryOverlay)

    void SetData(summary::SummaryData data);

private:
    void Rebuild();

    summary::SummaryData data;
    bool hasData = false;

    static constexpr pu::ui::Color OVERLAY_BG = pu::ui::Color(0, 0, 0, 160);

    // Laid out in the app's 1920x1080 coordinate space
    static constexpr pu::i32 PANEL_Y = 60;
    static constexpr pu::i32 PANEL_W = 1560;
    static constexpr pu::i32 PANEL_H = 960;
    static constexpr pu::i32 PANEL_RADIUS = 38;

    pu::i32 panelX = 48;

    static constexpr pu::ui::Color SHADOW = pu::ui::Color(0, 0, 0, 90);

    static constexpr pu::ui::Color PANEL_BG = pu::ui::Color(246, 248, 246, 255);
    static constexpr pu::ui::Color PANEL_BORDER = pu::ui::Color(18, 60, 30, 255);
    static constexpr pu::ui::Color TITLE_BAR_BG = pu::ui::Color(22, 74, 36, 255);
    static constexpr pu::ui::Color TEXT_DARK = pu::ui::Color(8, 20, 10, 255);
    static constexpr pu::ui::Color TEXT_LIGHT = pu::ui::Color(255, 255, 255, 255);
    static constexpr pu::ui::Color TEXT_MUTED = pu::ui::Color(210, 226, 214, 255);
    static constexpr pu::ui::Color DIVIDER = pu::ui::Color(18, 60, 30, 55);
    static constexpr pu::ui::Color TEXT_FOOTNOTE = pu::ui::Color(18, 60, 30, 170);
    static constexpr pu::ui::Color CARD_BG = pu::ui::Color(255, 255, 255, 150);
    static constexpr pu::ui::Color CARD_BORDER = pu::ui::Color(18, 60, 30, 55);

    // Accent palette: nature-raised red, nature-lowered blue, highlight amber
    static constexpr pu::ui::Color ACCENT_POSITIVE = pu::ui::Color(196, 60, 66, 255);
    static constexpr pu::ui::Color ACCENT_NEGATIVE = pu::ui::Color(54, 110, 212, 255);
    static constexpr pu::ui::Color ACCENT_HIGHLIGHT = pu::ui::Color(200, 120, 0, 255);

    // Frame
    static constexpr pu::i32 PANEL_INSET = 6;  // Border thickness around the panel body
    static constexpr pu::i32 PAD = 48;  // Panel content padding

    // Title bar
    static constexpr pu::i32 TITLE_BAR_HEIGHT = 96;
    // Taller than the bar; overhangs it symmetrically, centered on the title line
    static constexpr pu::i32 SPRITE_SIZE = 124;
    static constexpr pu::i32 SPRITE_MARGIN = 16;
    static constexpr pu::i32 TITLE_TEXT_GAP = 14;  // Sprite to species name
    static constexpr pu::i32 GENDER_ICON_SIZE = 36;
    static constexpr pu::i32 GENDER_ICON_GAP = 20;  // Species name to badge
    static constexpr pu::i32 SHINY_ICON_SIZE = 30;
    static constexpr pu::i32 BADGE_GAP = 24;  // Between title-bar badges
    static constexpr pu::i32 DIVIDER_HEIGHT = 3;
    static constexpr pu::i32 DIVIDER_MARGIN_X = 24;
    static constexpr pu::i32 DIVIDER_GAP = 6;  // Title bar to divider

    // Cards
    static constexpr pu::i32 CONTENT_TOP_GAP = 23;  // Divider to cards
    static constexpr pu::i32 CARD_RADIUS = 26;
    static constexpr pu::i32 CARD_INSET = 4;  // Card border thickness
    static constexpr pu::i32 CARD_PAD_X = 18;  // Column content to card edge
    static constexpr pu::i32 CARD_PAD_TOP = 20;
    static constexpr pu::i32 CARD_BOTTOM_MARGIN = 70;  // Strip under the cards for the close hint

    // Columns and rows
    static constexpr pu::i32 LEFT_COL_W = 700;
    static constexpr pu::i32 COL_GAP = 90;
    static constexpr pu::i32 LABEL_COL_W = 315;  // Left card label column, values start after it
    static constexpr pu::i32 ROW_GAP = 14;
    static constexpr pu::i32 SECTION_GAP = 30;  // Stats to moves
    static constexpr pu::i32 SECTION_HEADER_GAP = 12;  // Section header to first row
    static constexpr pu::i32 STAT_LABEL_INSET = 16;
    static constexpr pu::i32 STAT_VALUE_COL_X = 280;  // First value column, from the right column edge
    static constexpr pu::i32 STAT_COL_STRIDE = 130;
    static constexpr pu::i32 STAT_ROW_GAP = 10;
    static constexpr pu::i32 MOVE_ROW_GAP = 12;

    // Type chips
    static constexpr pu::i32 CHIP_PAD_X = 14;
    static constexpr pu::i32 CHIP_PAD_Y = 5;
    static constexpr pu::i32 CHIP_GAP = 12;
};

}  // namespace pksm::ui
