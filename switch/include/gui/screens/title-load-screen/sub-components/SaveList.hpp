#pragma once
#include <functional>
#include <pu/Plutonium>
#include <vector>

#include "gui/shared/components/FocusableMenu.hpp"
#include "saves/Save.hpp"

namespace pksm::ui {

class SaveList : public FocusableMenu {
private:
    std::vector<saves::Save::Ref> saves;  // Store actual save objects
    std::function<void()> onSelectionChangedCallback;

public:
    SaveList(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::ui::Color items_clr,
        const pu::ui::Color items_focus_clr,
        const pu::i32 items_height,
        const u32 items_to_show
    );
    PU_SMART_CTOR(SaveList)

    // Set save data
    void SetDataSource(const std::vector<saves::Save::Ref>& saves);

    // Get currently selected save
    saves::Save::Ref GetSelectedSave() const;

    // Get selected save text (for display)
    std::string GetSelectedSaveText() const;

    // Selection changed callback
    void SetOnSelectionChanged(std::function<void()> callback);
};

}  // namespace pksm::ui