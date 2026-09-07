#include "gui/shared/components/PouchColumn.hpp"

#include "gui/shared/UIConstants.hpp"

pksm::ui::PouchColumn::PouchColumn(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 titleX,
    const pu::i32 titleY,
    const std::vector<Entry>& pouches,
    input::FocusManager::Ref focusManager
)
  : Element(), x(x), y(y), titleX(titleX), titleY(titleY) {
    marker =
        pu::ui::elm::Rectangle::New(x - MARKER_GAP - MARKER_WIDTH, y, MARKER_WIDTH, POUCH_HEIGHT, global::TEXT_WHITE);
    for (size_t i = 0; i < pouches.size(); i++) {
        auto button = FocusableButton::New(
            x,
            PouchY(i),
            POUCH_WIDTH,
            POUCH_HEIGHT,
            pouches[i].name,
            pu::ui::Color(0, 0, 0, 70),
            pu::ui::Color(255, 255, 255, 80)
        );
        button->SetContentFont(global::MakeMediumFontName(global::FONT_SIZE_ACCOUNT_NAME));
        button->SetName("Pouch button: " + pouches[i].name);
        button->SetOnFocus([this, i]() {
            if (onPouchFocused) {
                onPouchFocused(i);
            }
        });
        button->SetOnClick([this]() {
            if (onPouchActivated) {
                onPouchActivated();
            }
        });
        focusManager->RegisterFocusable(button);
        buttons.push_back(button);
        // Drawn over the button, left of its centred name
        glyphs.push_back(SpriteImage::New(
            x + GLYPH_INSET,
            PouchY(i) + (POUCH_HEIGHT - GLYPH_SIZE) / 2,
            GLYPH_SIZE,
            GLYPH_SIZE,
            pouches[i].glyph
        ));

        auto title = pu::ui::elm::TextBlock::New(titleX, titleY, pouches[i].name);
        title->SetColor(global::TEXT_WHITE);
        title->SetFont(global::MakeHeavyFontName(global::FONT_SIZE_HEADER));
        title->SetVisible(false);
        titles.push_back(title);
    }
}

void pksm::ui::PouchColumn::Select(size_t index) {
    if (selected != NONE) {
        titles[selected]->SetVisible(false);
    }
    selected = index;
    marker->SetY(PouchY(index));
    titles[index]->SetVisible(true);
}

void pksm::ui::PouchColumn::RequestFocus(size_t index) {
    buttons[index]->RequestFocus();
}

void pksm::ui::PouchColumn::ShakeSelected(ShakeDirection direction) {
    if (selected != NONE) {
        buttons[selected]->shakeOutOfBounds(direction);
    }
}

void pksm::ui::PouchColumn::SetTitleVisible(bool visible) {
    titles[selected]->SetVisible(visible);
}

void pksm::ui::PouchColumn::SetDisabled(bool disabled) {
    for (auto& button : buttons) {
        button->SetDisabled(disabled);
    }
}

pu::i32 pksm::ui::PouchColumn::GetHeight() {
    return static_cast<pu::i32>(buttons.size()) * (POUCH_HEIGHT + POUCH_SPACING) - POUCH_SPACING;
}

void pksm::ui::PouchColumn::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    for (size_t i = 0; i < buttons.size(); i++) {
        buttons[i]->OnRender(drawer, buttons[i]->GetX(), buttons[i]->GetY());
        glyphs[i]->OnRender(drawer, glyphs[i]->GetX(), glyphs[i]->GetY());
    }
    if (selected != NONE) {
        if (titles[selected]->IsVisible()) {
            titles[selected]->OnRender(drawer, titles[selected]->GetX(), titles[selected]->GetY());
        }
        marker->OnRender(drawer, marker->GetX(), marker->GetY());
    }
}

void pksm::ui::PouchColumn::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    for (auto& button : buttons) {
        button->OnInput(keys_down, keys_up, keys_held, touch_pos);
    }
}
