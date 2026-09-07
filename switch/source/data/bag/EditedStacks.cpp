#include "data/bag/EditedStacks.hpp"

namespace {

u32 StackKey(const pksm::bag::Slot& slot) {
    return (static_cast<u32>(slot.itemId) << 16) | slot.count;
}

}  // namespace

namespace pksm::bag {

void EditedStacks::Reset(const BagData& bag) {
    stacks.clear();
    for (const auto& pouch : bag.pouches) {
        auto& pouchStacks = stacks.emplace_back();
        for (const auto& slot : pouch.items) {
            pouchStacks.insert(StackKey(slot));
        }
    }
}

bool EditedStacks::IsEdited(size_t pouchIndex, const Slot& slot) const {
    return !stacks[pouchIndex].contains(StackKey(slot));
}

}  // namespace pksm::bag
