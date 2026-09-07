#pragma once

#include <functional>
#include <optional>
#include <string>

#include "data/bag/BagData.hpp"
#include "data/providers/interfaces/IBagDataProvider.hpp"
#include "data/providers/interfaces/ISaveDataAccessor.hpp"
#include "gui/shared/ChoiceDialog.hpp"
#include "gui/shared/components/BagItemList.hpp"

namespace pksm::layout {

// The right stick on a key item where a game has a shortcut button and no favourite: registers
// into the first free slot, replaces on single-slot games, unregisters, or asks which slot
// through a list dialog when more than one thing could happen. Keeps the bag's slot list and the
// row list's glyphs in step with the save
class BagShortcuts {
public:
    struct Hooks {
        std::function<int(const pksm::ui::ChoiceDialog&)> requestChoice;
        std::function<void(pksm::bag::Pouch)> applyPouch;  // the pouch as the save now holds it
        std::function<void()> shakeCursor;                  // nothing to do, or every slot taken
        std::function<void()> beforeDialog;                 // the dialog swallows a held release
    };

    BagShortcuts(
        pksm::bag::BagData& bag,
        pksm::ui::BagItemList::Ref itemList,
        IBagDataProvider::Ref bagDataProvider,
        ISaveDataAccessor::Ref saveDataAccessor,
        Hooks hooks
    );

    bool CanRegister(const pksm::bag::Pouch& pouch, const pksm::bag::Slot* slot) const;
    // What a press would do to the row, for the help overlay; empty where the stick does nothing
    std::string Action(const pksm::bag::Pouch& pouch, const pksm::bag::Slot* slot) const;
    void Toggle(const pksm::bag::Pouch& pouch, const pksm::bag::Slot& slot);
    // Re-reads the slots after any change to the key pouch and pushes the glyphs
    void Refresh();
    void PushGlyphs();

private:
    pksm::bag::BagData& bag;
    pksm::ui::BagItemList::Ref itemList;
    IBagDataProvider::Ref bagDataProvider;
    ISaveDataAccessor::Ref saveDataAccessor;
    Hooks hooks;

    // Only one thing can happen: a lone slot takes or replaces, an empty list takes the first slot
    bool Direct() const;
    void Apply(std::optional<pksm::bag::Pouch> updated);
};

}  // namespace pksm::layout
