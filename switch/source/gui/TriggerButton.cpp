#include "gui/TriggerButton.hpp"
#include "gui/UIConstants.hpp"

namespace pksm::ui {
    TriggerButton::TriggerButton(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        const pu::i32 cornerRadius,
        const Side side,
        const pu::ui::Color& backgroundColor,
        const pu::ui::Color& textColor
    ) : Element(),
        x(x), y(y),
        width(width), height(height),
        cornerRadius(cornerRadius),
        side(side),
        backgroundColor(backgroundColor),
        textColor(textColor)
    {
        // Create text block
        textBlock = pu::ui::elm::TextBlock::New(x, y, "");
        textBlock->SetColor(textColor);
        textBlock->SetFont(UIConstants::MakeSwitchButtonFontName(UIConstants::FONT_SIZE_BUTTON));
        switch (side) {
            case Side::Left:
                textBlock->SetText("\ue0f6");
                break;
            case Side::Right:
                textBlock->SetText("\ue0f7");
                break;
        }
        
        // Position text and create initial texture
        UpdateTextPosition();
    }

    TriggerButton::~TriggerButton() {}

    void TriggerButton::SetBackgroundColor(const pu::ui::Color& color) {
        backgroundColor = color;
    }

    void TriggerButton::SetTextColor(const pu::ui::Color& color) {
        textColor = color;
        textBlock->SetColor(color);
    }

    void TriggerButton::UpdateTextPosition() {
        // Center text in button
        pu::i32 textX = x + (width - textBlock->GetWidth()) / 2;
        textBlock->SetX(textX);
    }

    void TriggerButton::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
        drawer->RenderRoundedRectangleFill(backgroundColor, x, y, width, height, cornerRadius);

        // Render text
        if (textBlock) {
            textBlock->OnRender(drawer, textBlock->GetX(), textBlock->GetY());
        }
    }
}