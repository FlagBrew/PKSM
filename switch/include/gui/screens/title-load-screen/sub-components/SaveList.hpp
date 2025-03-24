#pragma once
#include <functional>
#include <pu/Plutonium>
#include <vector>

#include "gui/shared/components/FocusableMenu.hpp"
#include "gui/shared/interfaces/IHelpProvider.hpp"
#include "saves/Save.hpp"

namespace pksm::ui {

class SaveList : public FocusableMenu, public IHelpProvider {
private:
    // Constants for SaveList configuration
    static constexpr pu::i32 SAVE_ITEM_HEIGHT = 48;
    static constexpr u32 SAVE_LIST_MAX_VISIBLE_ITEMS = 5;
    static constexpr pu::ui::Color SAVE_LIST_BACKGROUND_COLOR = pu::ui::Color(0, 0, 0, 200);
    static constexpr pu::ui::Color SAVE_LIST_SELECTION_COLOR = pu::ui::Color(0, 150, 255, 255);
    static constexpr pu::i32 SCROLLBAR_WIDTH = 16;

    std::vector<saves::Save::Ref> saves;  // Store actual save objects
    std::function<void()> onSelectionChangedCallback;

public:
    SaveList(const pu::i32 x, const pu::i32 y, const pu::i32 width);
    PU_SMART_CTOR(SaveList)

    // Get component height
    pu::i32 GetHeight() const;

    // Get maximum height needed for the save list
    static constexpr pu::i32 GetMaxHeight() { return SAVE_ITEM_HEIGHT * SAVE_LIST_MAX_VISIBLE_ITEMS; }

    // Set save data
    void SetDataSource(const std::vector<saves::Save::Ref>& saves);

    // Get currently selected save
    saves::Save::Ref GetSelectedSave() const;

    // Get selected save text (for display)
    std::string GetSelectedSaveText() const;

    // Selection changed callback
    void SetOnSelectionChanged(std::function<void()> callback);

    std::vector<HelpItem> GetHelpItems() const override;
};

}  // namespace pksm::ui