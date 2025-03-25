#include "gui/shared/components/BoxNavigationButton.hpp"

#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"

namespace pksm::ui {

BoxNavigationButton::BoxNavigationButton(const pu::i32 x, const pu::i32 y, const Direction direction)
  : Element(), x(x), y(y), isPressed(false), direction(direction) {
    // Create the chevron text block
    if (direction == Direction::Left) {
        chevronText = pu::ui::elm::TextBlock::New(0, 0, global::GetButtonGlyphString(global::ButtonGlyph::ChevronLeft));
        buttonText = pu::ui::elm::TextBlock::New(0, 0, global::GetButtonGlyphString(global::ButtonGlyph::L));
    } else {  // Right
        chevronText = pu::ui::elm::TextBlock::New(0, 0, global::GetButtonGlyphString(global::ButtonGlyph::ChevronRight));
        buttonText = pu::ui::elm::TextBlock::New(0, 0, global::GetButtonGlyphString(global::ButtonGlyph::R));
    }

    // Set colors and font
    chevronText->SetColor(DEFAULT_COLOR);
    buttonText->SetColor(DEFAULT_COLOR);

    // Use appropriate font
    chevronText->SetFont(global::MakeSwitchButtonFontName(global::FONT_SIZE_BOX_BUTTON));
    buttonText->SetFont(global::MakeSwitchButtonFontName(global::FONT_SIZE_BOX_BUTTON));

    // Update layout to position elements correctly
    UpdateLayout();

    LOG_DEBUG("BoxNavigationButton component initialization complete");
}

pu::i32 BoxNavigationButton::GetX() {
    return x;
}

pu::i32 BoxNavigationButton::GetY() {
    return y;
}

pu::i32 BoxNavigationButton::GetWidth() {
    if (direction == Direction::Left) {
        return chevronText->GetWidth() + ELEMENT_SPACING + buttonText->GetWidth();
    } else {
        return buttonText->GetWidth() + ELEMENT_SPACING + chevronText->GetWidth();
    }
}

pu::i32 BoxNavigationButton::GetHeight() {
    // Return the height of the taller element
    return std::max(chevronText->GetHeight(), buttonText->GetHeight());
}

void BoxNavigationButton::SetX(pu::i32 newX) {
    x = newX;
    UpdateLayout();
}

void BoxNavigationButton::SetY(pu::i32 newY) {
    y = newY;
    UpdateLayout();
}

void BoxNavigationButton::SetPosition(pu::i32 newX, pu::i32 newY) {
    x = newX;
    y = newY;
    UpdateLayout();
}

void BoxNavigationButton::SetColor(const pu::ui::Color& color) {
    chevronText->SetColor(color);
    buttonText->SetColor(color);
}

void BoxNavigationButton::SetPressed(bool pressed) {
    isPressed = pressed;

    // Update colors based on pressed state
    pu::ui::Color color = pressed ? PRESSED_COLOR : DEFAULT_COLOR;
    SetColor(color);
}

void BoxNavigationButton::UpdateLayout() {
    // Position the elements relative to the component's position
    pu::i32 currentX = x;

    if (direction == Direction::Left) {
        // For left direction: chevron + button
        chevronText->SetX(currentX);
        chevronText->SetY(y);

        currentX += chevronText->GetWidth() + ELEMENT_SPACING;

        buttonText->SetX(currentX);
        buttonText->SetY(y);
    } else {
        // For right direction: button + chevron
        buttonText->SetX(currentX);
        buttonText->SetY(y);

        currentX += buttonText->GetWidth() + ELEMENT_SPACING;

        chevronText->SetX(currentX);
        chevronText->SetY(y);
    }
}

void BoxNavigationButton::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Render the chevron and button text blocks
    chevronText->OnRender(drawer, chevronText->GetX(), chevronText->GetY());
    buttonText->OnRender(drawer, buttonText->GetX(), buttonText->GetY());
}

void BoxNavigationButton::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    // No input handling needed for this component
    // Button handling is done by the parent component
}

}  // namespace pksm::ui