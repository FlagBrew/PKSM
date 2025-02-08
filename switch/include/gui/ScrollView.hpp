#pragma once
#include <pu/Plutonium>

#include "gui/IFocusable.hpp"

namespace pksm::ui {
class ScrollView : public pu::ui::elm::Element, public IFocusable {
public:
    ScrollView(const pu::i32 x, const pu::i32 y, const pu::i32 width, const pu::i32 height);
    PU_SMART_CTOR(ScrollView)

    // Core Element overrides
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos)
        override;

    // IFocusable implementation
    void SetFocused(bool focused) override;
    bool IsFocused() const override;

    // Scroll position management
    void SetContentHeight(pu::i32 height);
    pu::i32 GetContentHeight() const;
    void ScrollToOffset(pu::i32 offset, bool animated = false);
    pu::i32 GetScrollOffset() const;

    // Viewport management
    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;

    // Container delegation
    void Add(pu::ui::elm::Element::Ref elem);
    std::vector<pu::ui::elm::Element::Ref>& GetElements();
    void Clear();

protected:
    // Scroll behavior
    void UpdateScrollMomentum();
    void ClampScrollOffset();
    bool ShouldStartDragging(const pu::ui::TouchPoint& touch_pos) const;

private:
    // Container for elements
    pu::ui::Container::Ref container;

    // Focus state
    bool focused;

    // Scroll state
    pu::i32 scrollOffset;
    pu::i32 contentHeight;
    pu::i32 availableHeight;

    // Animation state
    bool isAnimatingScroll;
    pu::i32 animationStartOffset;
    pu::i32 animationTargetOffset;
    u64 animationStartTime;
    static constexpr u64 SCROLL_ANIMATION_DURATION = 300;  // Duration in milliseconds

    // Touch handling state
    bool isDragging;
    bool touchStartedOnChild;
    pu::i32 touchStartY;
    pu::i32 lastTouchY;
    pu::i32 scrollStartOffset;
    float scrollVelocity;
    bool hasMomentum;

    // Debounce state
    bool isScrollingProgrammatically;
    u64 lastProgrammaticScrollTime;

    // Constants (preserved from GameGrid)
    static constexpr pu::i32 DRAG_THRESHOLD = 5;
    static constexpr float SCROLL_FRICTION = 0.95f;
    static constexpr float MIN_SCROLL_VELOCITY = 0.1f;
    static constexpr u64 SCROLL_DEBOUNCE_TIME = 160;  // About 10 frames at 60fps
};
}  // namespace pksm::ui