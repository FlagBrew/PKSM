#include "gui/shared/components/BoxItem.hpp"

#include "gui/shared/components/GenderIcon.hpp"
#include "gui/shared/components/PartyBadgeIcon.hpp"
#include "utils/Logger.hpp"

namespace {

constexpr pu::i32 GENDER_ICON_SIZE = 26;
constexpr pu::i32 GENDER_ICON_MARGIN = 5;
constexpr pu::i32 PARTY_BADGE_SIZE = 30;
constexpr pu::i32 PARTY_BADGE_MARGIN = 5;

}  // namespace

pksm::ui::BoxItem::BoxItem(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    const utils::SpriteRef& sprite,
    const pu::i32 outlinePadding,
    const pu::ui::Color defaultBgColor,
    const pu::ui::Color selectedBgColor
)
  : Element(),
    ShakeableWithOutline(
        pksm::ui::PulsingOutline::New(
            x - outlinePadding,
            y - outlinePadding,
            width + (outlinePadding * 2),
            height + (outlinePadding * 2),
            pksm::ui::global::OUTLINE_COLOR,
            0,
            OUTLINE_BORDER_WIDTH
        )
    ),
    focused(false),
    selected(false),
    outlinePadding(outlinePadding),
    x(x),
    y(y),
    width(width),
    height(height),
    defaultBgColor(defaultBgColor),
    selectedBgColor(selectedBgColor),
    onTouchSelectCallback(nullptr),
    onSelectCallback(nullptr),
    touchHandler(),
    buttonHandler() {
    // Create container with elements
    container = pu::ui::Container::New(0, 0, width, height);
    background = pu::ui::elm::Rectangle::New(0, 0, width, height, defaultBgColor);
    this->image = pksm::ui::SpriteImage::New(0, 0, width, height, sprite);

    // Add elements to container
    container->Add(background);
    container->Add(this->image);

    // Badge positions are container-relative
    this->genderIcon = pu::ui::elm::Image::New(width - GENDER_ICON_SIZE - GENDER_ICON_MARGIN, GENDER_ICON_MARGIN, nullptr);
    this->genderIcon->SetVisible(false);
    container->Add(this->genderIcon);

    this->partyBadge = pu::ui::elm::Image::New(
        width - PARTY_BADGE_SIZE - PARTY_BADGE_MARGIN,
        height - PARTY_BADGE_SIZE - PARTY_BADGE_MARGIN,
        nullptr
    );
    this->partyBadge->SetVisible(false);
    container->Add(this->partyBadge);

    unusableShade = pu::ui::elm::Rectangle::New(0, 0, width, height, pu::ui::Color(0, 0, 0, 110));
    unusableShade->SetVisible(false);
    container->Add(unusableShade);

    // Create the regular outline
    outline = pksm::ui::RectangularOutline::New(
        x - outlinePadding,
        y - outlinePadding,
        width + (outlinePadding * 2),
        height + (outlinePadding * 2),
        outlineColor,
        0,
        OUTLINE_BORDER_WIDTH
    );

    // Set up touch handler callbacks
    touchHandler.SetOnTouchUpInside([this]() {
        if (!focused && onTouchSelectCallback) {
            onTouchSelectCallback();
            RequestFocus();
        } else if (focused && onSelectCallback) {
            onSelectCallback();
        }
    });

    // Register A button with the condition to only process when focused
    buttonHandler.RegisterButton(
        HidNpadButton_A,
        nullptr,
        [this]() {
            if (focused && onSelectCallback) {
                onSelectCallback();
            }
        },
        [this]() { return this->focused; }
    );

    // Set initial visibility
    outline->SetVisible(true);
    pulsingOutline->SetVisible(false);
}

pu::i32 pksm::ui::BoxItem::GetX() {
    return x;
}

pu::i32 pksm::ui::BoxItem::GetY() {
    return y;
}

pu::i32 pksm::ui::BoxItem::GetWidth() {
    return width;
}

pu::i32 pksm::ui::BoxItem::GetHeight() {
    return height;
}

void pksm::ui::BoxItem::SetImage(const utils::SpriteRef& sprite) {
    image->SetImage(sprite);
}

void pksm::ui::BoxItem::SetGender(pksm::Gender gender, bool visible) {
    if (!visible) {
        genderIcon->SetVisible(false);
        genderIcon->SetImage(nullptr);
        return;
    }
    // SetImage resets the element to the texture's natural size; re-apply the display size
    genderIcon->SetImage(GetGenderIconTexture(gender));
    genderIcon->SetWidth(GENDER_ICON_SIZE);
    genderIcon->SetHeight(GENDER_ICON_SIZE);
    genderIcon->SetVisible(true);
}

void pksm::ui::BoxItem::SetPartyNumber(u8 partyNumber) {
    auto texture = partyNumber > 0 ? GetPartyBadgeTexture(partyNumber) : nullptr;
    if (!texture) {
        partyBadge->SetVisible(false);
        partyBadge->SetImage(nullptr);
        return;
    }
    // Same natural-size reset as the gender badge
    partyBadge->SetImage(texture);
    partyBadge->SetWidth(PARTY_BADGE_SIZE);
    partyBadge->SetHeight(PARTY_BADGE_SIZE);
    partyBadge->SetVisible(true);
}

void pksm::ui::BoxItem::SetUnusable(bool unusable) {
    unusableShade->SetVisible(unusable);
}

void pksm::ui::BoxItem::SetSelected(bool select) {
    this->selected = select;
    background->SetColor(select ? selectedBgColor : defaultBgColor);
}

bool pksm::ui::BoxItem::IsSelected() const {
    return selected;
}

void pksm::ui::BoxItem::SetFocused(bool focus) {
    if (focus) {
        LOG_TRACE("[BoxItem] Setting focused to true");
        RequestSelection();
    } else {
        LOG_TRACE("[BoxItem] Setting focused to false");
    }

    this->focused = focus;
    outline->SetVisible(!focus);
    pulsingOutline->SetVisible(focus);
}

bool pksm::ui::BoxItem::IsFocused() const {
    return focused;
}

void pksm::ui::BoxItem::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Draw the appropriate outline first
    if (focused) {
        pulsingOutline->OnRender(drawer, x - outlinePadding, y - outlinePadding);
    } else {
        outline->OnRender(drawer, x - outlinePadding, y - outlinePadding);
    }

    // Draw container elements; OnRender alone draws regardless of SetVisible
    for (auto& element : container->GetElements()) {
        if (element->IsVisible()) {
            element->OnRender(drawer, x + element->GetX(), y + element->GetY());
        }
    }
}

void pksm::ui::BoxItem::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    // Pass input to container elements
    for (auto& element : container->GetElements()) {
        element->OnInput(keys_down, keys_up, keys_held, touch_pos);
    }

    // Process touch input using TouchInputHandler
    touchHandler.HandleInput(touch_pos, x, y, width, height);

    // Process button inputs
    buttonHandler.HandleInput(keys_down, keys_up, keys_held);
}