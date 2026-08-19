#include "gui/shared/components/PokemonSummaryOverlay.hpp"

#include "gui/shared/UIConstants.hpp"
#include "gui/shared/components/GenderIcon.hpp"
#include "gui/shared/components/SpriteImage.hpp"

namespace {

// The 3DS app's shiny star, shared by every overlay instance and kept for
// the lifetime of the app
pu::sdl2::TextureHandle::Ref GetShinyIconTexture() {
    static pu::sdl2::TextureHandle::Ref star;
    if (!star) {
        pu::sdl2::Texture tex = pu::ui::render::LoadImage("romfs:/gfx/ui/icon_shiny.png");
        if (tex) {
            star = pu::sdl2::TextureHandle::New(tex);
        }
    }
    return star;
}

pu::ui::Color TypeColor(const ::pksm::Type type) {
    using C = pu::ui::Color;
    switch (static_cast<::pksm::Type::EnumType>(type)) {
        case ::pksm::Type::EnumType::Normal:
            return C(168, 168, 120, 255);
        case ::pksm::Type::EnumType::Fire:
            return C(240, 128, 48, 255);
        case ::pksm::Type::EnumType::Water:
            return C(104, 144, 240, 255);
        case ::pksm::Type::EnumType::Electric:
            return C(248, 208, 48, 255);
        case ::pksm::Type::EnumType::Grass:
            return C(120, 200, 80, 255);
        case ::pksm::Type::EnumType::Ice:
            return C(152, 216, 216, 255);
        case ::pksm::Type::EnumType::Fighting:
            return C(192, 48, 40, 255);
        case ::pksm::Type::EnumType::Poison:
            return C(160, 64, 160, 255);
        case ::pksm::Type::EnumType::Ground:
            return C(224, 192, 104, 255);
        case ::pksm::Type::EnumType::Flying:
            return C(168, 144, 240, 255);
        case ::pksm::Type::EnumType::Psychic:
            return C(248, 88, 136, 255);
        case ::pksm::Type::EnumType::Bug:
            return C(168, 184, 32, 255);
        case ::pksm::Type::EnumType::Rock:
            return C(184, 160, 56, 255);
        case ::pksm::Type::EnumType::Ghost:
            return C(112, 88, 152, 255);
        case ::pksm::Type::EnumType::Dragon:
            return C(112, 56, 248, 255);
        case ::pksm::Type::EnumType::Dark:
            return C(112, 88, 72, 255);
        case ::pksm::Type::EnumType::Steel:
            return C(184, 184, 208, 255);
        case ::pksm::Type::EnumType::Fairy:
            return C(238, 153, 172, 255);
        default:
            return C(120, 120, 120, 255);
    }
}

}  // namespace

namespace pksm::ui {

PokemonSummaryOverlay::PokemonSummaryOverlay(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height
)
  : pu::ui::Overlay(x, y, width, height, OVERLAY_BG) {
    this->SetRadius(0);
    this->SetMaxFadeAlpha(200);
    this->SetFadeAlphaVariation(18);
    this->panelX = (this->GetWidth() - PANEL_W) / 2;
}

void PokemonSummaryOverlay::SetData(summary::SummaryData newData) {
    this->data = std::move(newData);
    this->hasData = true;
    this->Rebuild();
}

void PokemonSummaryOverlay::Rebuild() {
    this->Clear();
    if (!this->hasData) {
        return;
    }

    const std::string heavyFont = global::MakeHeavyFontName(global::FONT_SIZE_BUTTON);
    const std::string mediumFont = global::MakeMediumFontName(global::FONT_SIZE_BUTTON);
    const std::string switchBtnFont = global::MakeSwitchButtonFontName(global::FONT_SIZE_BUTTON);
    const std::string badgeFont = global::MakeMediumFontName(global::FONT_SIZE_BOX_SPACES_BUTTON);

    const auto accentColor = [this](summary::Accent accent, const pu::ui::Color& fallback) {
        switch (accent) {
            case summary::Accent::Positive:
                return ACCENT_POSITIVE;
            case summary::Accent::Negative:
                return ACCENT_NEGATIVE;
            case summary::Accent::Highlight:
                return ACCENT_HIGHLIGHT;
            default:
                return fallback;
        }
    };

    // Panel frame: drop shadow, border, body
    auto shadow = pu::ui::elm::Rectangle::New(panelX + 10, PANEL_Y + 14, PANEL_W, PANEL_H, SHADOW, PANEL_RADIUS);
    this->Add(shadow);
    auto panelBorder = pu::ui::elm::Rectangle::New(panelX, PANEL_Y, PANEL_W, PANEL_H, PANEL_BORDER, PANEL_RADIUS);
    auto panelBg = pu::ui::elm::Rectangle::New(
        panelX + PANEL_INSET,
        PANEL_Y + PANEL_INSET,
        PANEL_W - 2 * PANEL_INSET,
        PANEL_H - 2 * PANEL_INSET,
        PANEL_BG,
        PANEL_RADIUS - PANEL_INSET
    );
    this->Add(panelBorder);
    this->Add(panelBg);

    // Title bar: sprite, species (+ shiny star), gender badge, PKRS badge,
    // format badge and level on the right
    const pu::i32 barY = PANEL_Y + PANEL_INSET;
    auto titleBar = pu::ui::elm::Rectangle::New(
        panelX + PANEL_INSET,
        barY,
        PANEL_W - 2 * PANEL_INSET,
        TITLE_BAR_HEIGHT,
        TITLE_BAR_BG,
        PANEL_RADIUS - PANEL_INSET
    );
    this->Add(titleBar);

    auto sprite = SpriteImage::New(
        panelX + SPRITE_MARGIN,
        barY + (TITLE_BAR_HEIGHT - SPRITE_SIZE) / 2,
        SPRITE_SIZE,
        SPRITE_SIZE,
        data.species,
        data.form,
        data.shiny
    );
    this->Add(sprite);

    auto speciesText =
        pu::ui::elm::TextBlock::New(sprite->GetX() + SPRITE_SIZE + TITLE_TEXT_GAP, 0, data.speciesName);
    speciesText->SetColor(TEXT_LIGHT);
    speciesText->SetFont(heavyFont);
    speciesText->SetY(barY + (TITLE_BAR_HEIGHT - speciesText->GetHeight()) / 2);
    this->Add(speciesText);

    // Badges align to the species text's own center, not the bar's - the
    // text's font metrics pad its height, so bar-centering drifts optically
    const pu::i32 titleCenterY = speciesText->GetY() + speciesText->GetHeight() / 2;
    pu::i32 titleX = speciesText->GetX() + speciesText->GetWidth() + GENDER_ICON_GAP;
    if (data.gender.has_value()) {
        auto genderIcon = pu::ui::elm::Image::New(
            titleX,
            titleCenterY - GENDER_ICON_SIZE / 2,
            GetGenderIconTexture(*data.gender)
        );
        genderIcon->SetWidth(GENDER_ICON_SIZE);
        genderIcon->SetHeight(GENDER_ICON_SIZE);
        this->Add(genderIcon);
        titleX += GENDER_ICON_SIZE + BADGE_GAP;
    }
    if (data.shiny) {
        auto shinyIcon =
            pu::ui::elm::Image::New(titleX, titleCenterY - SHINY_ICON_SIZE / 2, GetShinyIconTexture());
        shinyIcon->SetWidth(SHINY_ICON_SIZE);
        shinyIcon->SetHeight(SHINY_ICON_SIZE);
        this->Add(shinyIcon);
        titleX += SHINY_ICON_SIZE + BADGE_GAP;
    }
    if (data.pokerus) {
        auto pkrsText = pu::ui::elm::TextBlock::New(titleX, 0, "PKRS");
        pkrsText->SetColor(ACCENT_HIGHLIGHT);
        pkrsText->SetFont(badgeFont);
        pkrsText->SetY(titleCenterY - pkrsText->GetHeight() / 2);
        this->Add(pkrsText);
    }

    auto levelText = pu::ui::elm::TextBlock::New(0, 0, "Lvl. " + std::to_string(data.level));
    levelText->SetColor(TEXT_LIGHT);
    levelText->SetFont(heavyFont);
    levelText->SetX(panelX + PANEL_W - PAD - levelText->GetWidth());
    levelText->SetY(barY + (TITLE_BAR_HEIGHT - levelText->GetHeight()) / 2);
    this->Add(levelText);

    auto badgeText = pu::ui::elm::TextBlock::New(0, 0, data.formatBadge);
    badgeText->SetColor(TEXT_MUTED);
    badgeText->SetFont(badgeFont);
    badgeText->SetX(levelText->GetX() - BADGE_GAP - badgeText->GetWidth());
    badgeText->SetY(barY + (TITLE_BAR_HEIGHT - badgeText->GetHeight()) / 2);
    this->Add(badgeText);

    const pu::i32 dividerY = barY + TITLE_BAR_HEIGHT + DIVIDER_GAP;
    auto headerDivider = pu::ui::elm::Rectangle::New(
        panelX + DIVIDER_MARGIN_X,
        dividerY,
        PANEL_W - 2 * DIVIDER_MARGIN_X,
        DIVIDER_HEIGHT,
        DIVIDER,
        2
    );
    this->Add(headerDivider);

    // Two content cards side by side, a hint strip below them
    const pu::i32 contentTop = dividerY + DIVIDER_HEIGHT + CONTENT_TOP_GAP;
    const pu::i32 cardH = PANEL_Y + PANEL_H - contentTop - CARD_BOTTOM_MARGIN;
    const pu::i32 leftX = panelX + PAD;
    const pu::i32 rightX = leftX + LEFT_COL_W + COL_GAP;

    const auto addCard = [&](const pu::i32 colX, const pu::i32 colW) {
        auto border = pu::ui::elm::Rectangle::New(
            colX - CARD_PAD_X,
            contentTop,
            colW + 2 * CARD_PAD_X,
            cardH,
            CARD_BORDER,
            CARD_RADIUS
        );
        auto body = pu::ui::elm::Rectangle::New(
            colX - CARD_PAD_X + CARD_INSET,
            contentTop + CARD_INSET,
            colW + 2 * (CARD_PAD_X - CARD_INSET),
            cardH - 2 * CARD_INSET,
            CARD_BG,
            CARD_RADIUS - CARD_INSET
        );
        this->Add(border);
        this->Add(body);
    };
    addCard(leftX, LEFT_COL_W);
    const pu::i32 rightColW = panelX + PANEL_W - PAD - rightX;
    addCard(rightX, rightColW);

    // Left card: type chips, then the builder's detail rows
    pu::i32 leftY = contentTop + CARD_PAD_TOP;

    // Draws one colored type chip centered on a row of labelHeight; returns
    // its width
    const auto drawChip =
        [&](const pu::i32 x, const pu::i32 rowY, const pu::i32 labelHeight, const std::string& text,
            const pu::ui::Color& bg) {
            auto tb = pu::ui::elm::TextBlock::New(0, 0, text);
            tb->SetColor(TEXT_LIGHT);
            tb->SetFont(heavyFont);
            const pu::i32 w = tb->GetWidth() + 2 * CHIP_PAD_X;
            const pu::i32 h = tb->GetHeight() + 2 * CHIP_PAD_Y;
            const pu::i32 y = rowY + (labelHeight - h) / 2;
            this->Add(pu::ui::elm::Rectangle::New(x, y, w, h, bg, h / 2));
            tb->SetX(x + CHIP_PAD_X);
            tb->SetY(y + CHIP_PAD_Y);
            this->Add(tb);
            return w;
        };

    {
        auto lbl = pu::ui::elm::TextBlock::New(leftX, leftY, "Type");
        lbl->SetColor(TEXT_DARK);
        lbl->SetFont(heavyFont);
        this->Add(lbl);

        pu::i32 chipX = leftX + LABEL_COL_W;
        for (const auto& badge : data.types) {
            chipX += drawChip(chipX, leftY, lbl->GetHeight(), badge.name, TypeColor(badge.type)) + CHIP_GAP;
        }
        leftY += lbl->GetHeight() + ROW_GAP;
    }

    for (const auto& row : data.details) {
        auto lbl = pu::ui::elm::TextBlock::New(leftX, leftY, row.label);
        lbl->SetColor(TEXT_DARK);
        lbl->SetFont(heavyFont);
        this->Add(lbl);

        if (row.typeChip.has_value()) {
            drawChip(leftX + LABEL_COL_W, leftY, lbl->GetHeight(), row.value, TypeColor(*row.typeChip));
        } else {
            auto val = pu::ui::elm::TextBlock::New(leftX + LABEL_COL_W, leftY, row.value);
            val->SetColor(accentColor(row.accent, TEXT_DARK));
            val->SetFont(mediumFont);
            this->Add(val);
        }

        leftY += lbl->GetHeight() + ROW_GAP;
    }

    // Right card: the stat table (variable column count, merged cells drawn
    // once centered across the sharing rows), then moves
    std::string statsTitle = "Stats";
    if (!data.stats.columnHeaders.empty()) {
        statsTitle += " (";
        for (size_t i = 0; i < data.stats.columnHeaders.size(); i++) {
            statsTitle += (i == 0 ? "" : " / ") + data.stats.columnHeaders[i];
        }
        statsTitle += ")";
    }
    auto statsHeader = pu::ui::elm::TextBlock::New(rightX, contentTop + CARD_PAD_TOP, statsTitle);
    statsHeader->SetColor(TEXT_DARK);
    statsHeader->SetFont(heavyFont);
    this->Add(statsHeader);

    const pu::i32 statsTop = statsHeader->GetY() + statsHeader->GetHeight() + SECTION_HEADER_GAP;
    pu::i32 rowHeight = 0;
    pu::i32 statY = statsTop;
    for (size_t rowIndex = 0; rowIndex < data.stats.rows.size(); rowIndex++) {
        const auto& row = data.stats.rows[rowIndex];

        auto lbl = pu::ui::elm::TextBlock::New(rightX + STAT_LABEL_INSET, statY, row.label);
        lbl->SetColor(accentColor(row.labelAccent, TEXT_DARK));
        lbl->SetFont(mediumFont);
        this->Add(lbl);
        if (rowHeight == 0) {
            rowHeight = lbl->GetHeight() + STAT_ROW_GAP;
        }

        for (size_t col = 0; col < row.cells.size(); col++) {
            const auto& cell = row.cells[col];
            if (cell.mergedWithAbove) {
                continue;  // The row above drew the shared value
            }
            const bool sharedWithBelow = rowIndex + 1 < data.stats.rows.size() &&
                col < data.stats.rows[rowIndex + 1].cells.size() &&
                data.stats.rows[rowIndex + 1].cells[col].mergedWithAbove;

            auto tb = pu::ui::elm::TextBlock::New(
                rightX + STAT_VALUE_COL_X + static_cast<pu::i32>(col) * STAT_COL_STRIDE,
                sharedWithBelow ? statY + rowHeight / 2 : statY,
                cell.text
            );
            tb->SetColor(accentColor(cell.accent, TEXT_DARK));
            tb->SetFont(mediumFont);
            this->Add(tb);
        }

        statY += rowHeight;
    }

    if (!data.stats.footnote.empty()) {
        auto footnote = pu::ui::elm::TextBlock::New(rightX + STAT_LABEL_INSET, statY, data.stats.footnote);
        footnote->SetColor(TEXT_FOOTNOTE);
        footnote->SetFont(badgeFont);
        this->Add(footnote);
        statY += footnote->GetHeight() + STAT_ROW_GAP;
    }

    auto movesHeader = pu::ui::elm::TextBlock::New(rightX, statY + SECTION_GAP, "Moves");
    movesHeader->SetColor(TEXT_DARK);
    movesHeader->SetFont(heavyFont);
    this->Add(movesHeader);

    pu::i32 moveY = movesHeader->GetY() + movesHeader->GetHeight() + SECTION_HEADER_GAP;
    for (const auto& move : data.moves) {
        auto moveText = pu::ui::elm::TextBlock::New(rightX + STAT_LABEL_INSET, moveY, "- " + move);
        moveText->SetColor(TEXT_DARK);
        moveText->SetFont(mediumFont);
        this->Add(moveText);
        moveY += moveText->GetHeight() + MOVE_ROW_GAP;
    }

    // Close hint, centered in the strip between the cards and the panel edge
    auto hint = pu::ui::elm::TextBlock::New(0, 0, global::GetButtonGlyphString(global::ButtonGlyph::B) + "  Back");
    hint->SetColor(TEXT_DARK);
    hint->SetFont(switchBtnFont);
    const pu::i32 cardBottom = contentTop + cardH;
    const pu::i32 panelInnerBottom = PANEL_Y + PANEL_H - PANEL_INSET;
    hint->SetX(panelX + PANEL_W - PAD - hint->GetWidth());
    hint->SetY(cardBottom + (panelInnerBottom - cardBottom - hint->GetHeight()) / 2);
    this->Add(hint);
}

}  // namespace pksm::ui
