#pragma once

#include <unordered_set>
#include <vector>

#include "data/bag/BagData.hpp"

namespace pksm::bag {

// A bag's (item, count) stacks as they were read, per pouch: a slot not among them is an edit
class EditedStacks {
private:
    std::vector<std::unordered_set<u32>> stacks;

public:
    void Reset(const BagData& bag);
    bool IsEdited(size_t pouchIndex, const Slot& slot) const;
};

}  // namespace pksm::bag
