#pragma once

#include <functional>
#include <memory>
#include <pu/Plutonium>
#include <vector>

#include "input/visual-feedback/FocusManager.hpp"
#include "input/visual-feedback/interfaces/IFocusable.hpp"
#include "titles/Title.hpp"

namespace pksm::ui {

class IGameList : public pu::ui::elm::Element, public IFocusable {
public:
    PU_SMART_CTOR(IGameList)
    virtual ~IGameList() = default;

    // Element interface
    virtual pu::i32 GetX() override = 0;
    virtual pu::i32 GetY() override = 0;
    virtual pu::i32 GetWidth() override = 0;
    virtual pu::i32 GetHeight() override = 0;
    virtual void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override = 0;
    virtual void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos)
        override = 0;

    // IFocusable interface
    virtual void SetFocused(bool focused) override = 0;
    virtual bool IsFocused() const override = 0;
    virtual void SetFocusManager(std::shared_ptr<input::FocusManager> manager) = 0;

    // Common game list interface
    virtual void SetDataSource(const std::vector<titles::Title::Ref>& titles) = 0;
    virtual titles::Title::Ref GetSelectedTitle() const = 0;
    virtual void SetOnSelectionChanged(std::function<void()> callback) = 0;
    virtual void SetOnTouchSelect(std::function<void()> callback) = 0;
    virtual bool ShouldResignUpFocus() const = 0;
    virtual bool ShouldResignDownFocus() const = 0;
    virtual float GetSelectionHorizontalPosition() const = 0;
};

}  // namespace pksm::ui