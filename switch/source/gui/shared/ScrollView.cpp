#include "gui/shared/components/ScrollView.hpp"

#include <cmath>

pksm::ui::ScrollView::ScrollView(const pu::i32 x, const pu::i32 y, const pu::i32 width, const pu::i32 height)
  : Element(),
    container(pu::ui::Container::New(x, y, width, height)),
    focused(false),
    scrollOffset(0),
    contentHeight(0),
    availableHeight(0),
    isAnimatingScroll(false),
    animationStartOffset(0),
    animationTargetOffset(0),
    animationStartTime(0),
    isDragging(false),
    touchStartedOnChild(false),
    touchStartY(0),
    lastTouchY(0),
    scrollStartOffset(0),
    scrollVelocity(0.0f),
    hasMomentum(false),
    isScrollingProgrammatically(false),
    lastProgrammaticScrollTime(0) {}

pu::i32 pksm::ui::ScrollView::GetX() {
    return container->GetX();
}

pu::i32 pksm::ui::ScrollView::GetY() {
    return container->GetY();
}

pu::i32 pksm::ui::ScrollView::GetWidth() {
    return container->GetWidth();
}

pu::i32 pksm::ui::ScrollView::GetHeight() {
    return container->GetHeight();
}

void pksm::ui::ScrollView::SetContentHeight(pu::i32 height) {
    contentHeight = height;
    ClampScrollOffset();
}

pu::i32 pksm::ui::ScrollView::GetContentHeight() const {
    return contentHeight;
}

void pksm::ui::ScrollView::ScrollToOffset(pu::i32 offset, bool animated) {
    // Clamp the target offset first
    pu::i32 maxScroll = std::max<pu::i32>(0, contentHeight - GetHeight());
    pu::i32 targetOffset = std::max<pu::i32>(0, std::min<pu::i32>(offset, maxScroll));

    if (animated) {
        // Start animation
        isAnimatingScroll = true;
        animationStartOffset = scrollOffset;
        animationTargetOffset = targetOffset;
        animationStartTime = SDL_GetTicks64();

        // Stop any existing momentum
        hasMomentum = false;
        scrollVelocity = 0.0f;
    } else {
        // Immediate scroll
        scrollOffset = targetOffset;
        isAnimatingScroll = false;
    }

    // Set debounce state
    isScrollingProgrammatically = true;
    lastProgrammaticScrollTime = SDL_GetTicks64();
}

pu::i32 pksm::ui::ScrollView::GetScrollOffset() const {
    return scrollOffset;
}

void pksm::ui::ScrollView::Add(pu::ui::elm::Element::Ref elem) {
    container->Add(elem);
}

std::vector<pu::ui::elm::Element::Ref>& pksm::ui::ScrollView::GetElements() {
    return container->GetElements();
}

void pksm::ui::ScrollView::Clear() {
    container->Clear();
}

void pksm::ui::ScrollView::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Update animation if active
    if (isAnimatingScroll) {
        u64 currentTime = SDL_GetTicks64();
        u64 elapsedTime = currentTime - animationStartTime;

        if (elapsedTime >= SCROLL_ANIMATION_DURATION) {
            // Animation complete
            scrollOffset = animationTargetOffset;
            isAnimatingScroll = false;
        } else {
            // Calculate progress (0.0 to 1.0) using easing
            float progress = static_cast<float>(elapsedTime) / SCROLL_ANIMATION_DURATION;
            float easedProgress = 1.0f - (1.0f - progress) * (1.0f - progress);  // Ease out quad

            // Update scroll position
            pu::i32 delta = animationTargetOffset - animationStartOffset;
            scrollOffset = animationStartOffset + static_cast<pu::i32>(delta * easedProgress);
        }
    }
    // Update momentum scrolling if active
    else if (hasMomentum) {
        UpdateScrollMomentum();
    }

    // Set up clipping rect
    SDL_Rect clipRect = {
        static_cast<pu::i32>(GetX()),
        static_cast<pu::i32>(GetY()),
        static_cast<pu::i32>(GetWidth()),
        static_cast<pu::i32>(GetHeight())
    };
    SDL_RenderSetClipRect(pu::ui::render::GetMainRenderer(), &clipRect);

    // Calculate visible region bounds
    pu::i32 visibleTop = GetY();
    pu::i32 visibleBottom = visibleTop + GetHeight();

    // Render all children with scroll offset applied, skipping those out of view
    for (auto& child : GetElements()) {
        pu::i32 childTop = child->GetY() - scrollOffset;
        pu::i32 childBottom = childTop + child->GetHeight();

        // Skip rendering if completely outside visible area
        if (childBottom < visibleTop || childTop > visibleBottom) {
            continue;
        }

        child->OnRender(drawer, child->GetX(), childTop);
    }

    // Disable clipping
    SDL_RenderSetClipRect(pu::ui::render::GetMainRenderer(), nullptr);
}

bool pksm::ui::ScrollView::ShouldStartDragging(const pu::ui::TouchPoint& touch_pos) const {
    return std::abs(touch_pos.y - touchStartY) > DRAG_THRESHOLD;
}

void pksm::ui::ScrollView::SetFocused(bool focused) {
    this->focused = focused;
    if (!focused) {
        // Clear any ongoing scroll state when losing focus
        isDragging = false;
        hasMomentum = false;
        scrollVelocity = 0.0f;
    }
}

bool pksm::ui::ScrollView::IsFocused() const {
    return focused;
}

void pksm::ui::ScrollView::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    // Check if we should clear the programmatic scrolling state
    if (isScrollingProgrammatically) {
        u64 currentTime = SDL_GetTicks64();
        if (currentTime - lastProgrammaticScrollTime >= SCROLL_DEBOUNCE_TIME) {
            isScrollingProgrammatically = false;
        }
    }

    if (!touch_pos.IsEmpty()) {
        // Check if touch hits any child first
        bool hitChild = false;
        pu::ui::TouchPoint adjusted_pos = touch_pos;
        adjusted_pos.y += scrollOffset;

        for (auto& child : GetElements()) {
            if (adjusted_pos.HitsRegion(child->GetX(), child->GetY(), child->GetWidth(), child->GetHeight())) {
                hitChild = true;

                if (focused) {
                    // Don't start dragging if we're in the debounce period
                    if (!isDragging && !isScrollingProgrammatically) {
                        if (touchStartY == 0) {  // New touch
                            touchStartY = touch_pos.y;
                            lastTouchY = touch_pos.y;
                            scrollStartOffset = scrollOffset;
                            touchStartedOnChild = true;
                        } else if (ShouldStartDragging(touch_pos)) {
                            // Convert to drag if moved enough
                            isDragging = true;
                            hasMomentum = false;
                            scrollVelocity = 0.0f;
                        } else if (touch_pos.y == touchStartY) {  // Clean tap
                            child->OnInput(keys_down, keys_up, keys_held, adjusted_pos);
                        }
                    }
                } else {
                    // When not focused, just forward touches to children
                    child->OnInput(keys_down, keys_up, keys_held, adjusted_pos);
                }
                break;
            }
        }

        if (focused && !isScrollingProgrammatically) {  // Only handle scrollview area drags when
                                                        // focused and not in debounce
            // If we didn't hit a child but hit the scrollview area, prepare for potential drag
            if (!hitChild && touch_pos.HitsRegion(GetX(), GetY(), GetWidth(), GetHeight())) {
                if (!isDragging && touchStartY == 0) {  // New touch
                    touchStartY = touch_pos.y;
                    lastTouchY = touch_pos.y;
                    scrollStartOffset = scrollOffset;
                    touchStartedOnChild = false;
                } else if (!isDragging && ShouldStartDragging(touch_pos)) {
                    isDragging = true;
                    hasMomentum = false;
                    scrollVelocity = 0.0f;
                }
            }

            if (isDragging) {
                // Continue drag
                pu::i32 deltaY = lastTouchY - touch_pos.y;
                scrollOffset = scrollStartOffset + (touchStartY - touch_pos.y);

                // Calculate velocity for momentum
                scrollVelocity = static_cast<float>(deltaY);

                // Update last touch position
                lastTouchY = touch_pos.y;

                // Ensure we stay in bounds
                ClampScrollOffset();
            }
        }
    } else {
        // Touch released
        if (isDragging) {
            isDragging = false;
            // Start momentum if we have enough velocity
            if (std::abs(scrollVelocity) > MIN_SCROLL_VELOCITY) {
                hasMomentum = true;
            }
        }
        // Reset touch tracking
        touchStartY = 0;
        touchStartedOnChild = false;
    }
}

void pksm::ui::ScrollView::UpdateScrollMomentum() {
    // Apply velocity to scroll position
    scrollOffset += static_cast<pu::i32>(scrollVelocity);

    // Apply friction
    scrollVelocity *= SCROLL_FRICTION;

    // Stop momentum if velocity is too low
    if (std::abs(scrollVelocity) < MIN_SCROLL_VELOCITY) {
        hasMomentum = false;
        scrollVelocity = 0.0f;
    }

    // Ensure we stay in bounds
    ClampScrollOffset();
}

void pksm::ui::ScrollView::ClampScrollOffset() {
    // Calculate max scroll
    pu::i32 maxScroll = std::max<pu::i32>(0, contentHeight - GetHeight());

    scrollOffset = std::max<pu::i32>(0, std::min<pu::i32>(scrollOffset, maxScroll));

    // If we hit bounds, stop momentum
    if (scrollOffset == 0 || scrollOffset == maxScroll) {
        hasMomentum = false;
        scrollVelocity = 0.0f;
    }
}