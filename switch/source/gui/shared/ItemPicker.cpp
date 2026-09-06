#include "gui/shared/components/ItemPicker.hpp"

#include <algorithm>
#include <cctype>

#include "gui/shared/UIConstants.hpp"

namespace {

// ASCII-only fold: names are English for now, and an accented letter matches as typed
std::string Lower(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return std::tolower(c); });
    return text;
}

}  // namespace

pksm::ui::ItemPicker::ItemPicker(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 listY,
    const pu::i32 width,
    const pu::i32 listHeight,
    const pu::i32 rowHeight,
    input::FocusManager::Ref focusManager
)
  : Element(), x(x), y(y), width(width), height(listY - y + listHeight) {
    title = pu::ui::elm::TextBlock::New(x, y, "");
    title->SetColor(global::TEXT_WHITE);
    title->SetFont(global::MakeHeavyFontName(global::FONT_SIZE_HEADER));
    status = pu::ui::elm::TextBlock::New(x, y + STATUS_Y, "");
    status->SetColor(global::TEXT_WHITE);
    status->SetFont(global::MakeMediumFontName(global::FONT_SIZE_ACCOUNT_NAME));
    notice = pu::ui::elm::TextBlock::New(x, listY + NOTICE_Y, "");
    notice->SetColor(global::TEXT_WHITE);
    notice->SetFont(global::MakeMediumFontName(global::FONT_SIZE_TRIGGER_BUTTON_NAVIGATION));
    notice->SetVisible(false);

    list = BagItemList::New(x, listY, width, listHeight, rowHeight, focusManager);
    list->SetName("ItemPicker list");
    list->EstablishOwningRelationship();
    SetVisible(false);
}

void pksm::ui::ItemPicker::Open(
    const std::string& pouchName,
    std::vector<bag::Slot> candidates,
    ::pksm::Generation storageFormat,
    ::pksm::Sav::Pouch pouch,
    const std::string& emptyNotice
) {
    this->candidates = std::move(candidates);
    this->storageFormat = storageFormat;
    this->pouch = pouch;
    this->emptyNotice = emptyNotice;
    search.clear();
    title->SetText("Add to " + pouchName);
    open = true;  // before focus moves: the screen reads it from the focus-change callback
    SetVisible(true);
    Refresh();
    list->RequestFocus();  // even with no rows, so the view underneath is not left focused
}

void pksm::ui::ItemPicker::Close() {
    open = false;
    SetVisible(false);
    candidates.clear();
    matches.clear();
    search.clear();
    list->SetDataSource(matches, storageFormat, pouch);  // releases the rows' textures
}

void pksm::ui::ItemPicker::SetSearch(const std::string& text) {
    search = text;
    Refresh();
    list->RequestFocus();  // a search that finds rows again gives them the cursor back
}

void pksm::ui::ItemPicker::ShakeSelected() {
    if (auto row = list->GetItemAtIndex(list->GetSelectedIndex())) {
        row->shakeOutOfBounds(ShakeDirection::RIGHT);
    }
}

void pksm::ui::ItemPicker::Refresh() {
    matches.clear();
    const std::string needle = Lower(search);
    for (const auto& candidate : candidates) {
        if (needle.empty() || Lower(candidate.name).find(needle) != std::string::npos) {
            matches.push_back(candidate);
        }
    }
    list->SetDataSource(matches, storageFormat, pouch);
    list->SetVisible(!matches.empty());

    // Empty text is never set: the notice and status hide instead
    if (candidates.empty()) {
        notice->SetText(emptyNotice);
    } else if (matches.empty()) {
        notice->SetText("No items match \"" + search + "\"");
    }
    notice->SetVisible(matches.empty());
    if (!candidates.empty()) {
        const std::string total = std::to_string(candidates.size());
        status->SetText(
            search.empty() ? total + " to add"
                           : std::to_string(matches.size()) + " of " + total + " match \"" + search + "\""
        );
        status->SetX(x + width - status->GetWidth());
    }
    status->SetVisible(!candidates.empty());
}

std::optional<pksm::bag::Slot> pksm::ui::ItemPicker::GetSelected() const {
    const size_t index = list->GetSelectedIndex();
    if (index < matches.size()) {
        return matches[index];
    }
    return std::nullopt;
}

void pksm::ui::ItemPicker::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    if (!open) {
        return;
    }
    title->OnRender(drawer, title->GetX(), title->GetY());
    if (status->IsVisible()) {
        status->OnRender(drawer, status->GetX(), status->GetY());
    }
    if (notice->IsVisible()) {
        notice->OnRender(drawer, notice->GetX(), notice->GetY());
    }
    if (list->IsVisible()) {
        list->OnRender(drawer, list->GetX(), list->GetY());
    }
}

void pksm::ui::ItemPicker::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    if (open && list->IsVisible()) {
        list->OnInput(keys_down, keys_up, keys_held, touch_pos);
    }
}
