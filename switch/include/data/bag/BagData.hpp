#pragma once

#include <string>
#include <vector>

#include "enums/Generation.hpp"
#include "sav/Sav.hpp"

// Generation-agnostic view of a save's bag; the UI renders it verbatim
namespace pksm::bag {

struct Slot {
    u16 itemId;  // Format-native, like strings::ItemName; a donut's index for Pouch::Donut
    u16 count;
    std::string name;
    std::string detail;  // Shown instead of ×count when set (donuts: quality and level boost)
    u8 variant = 0;      // Art variant where one id has several looks (a donut's stars)
    u16 slot = 0;        // The save's slot this row came from, what an edit addresses
};

struct Pouch {
    ::pksm::Sav::Pouch pouch;
    std::string name;
    size_t capacity;
    u16 maxCount = 0;         // Largest quantity one slot holds in-game; 1 means one of each, not a stack
    std::vector<Slot> items;  // Owned items in save order
    bool indexedByItem = false;  // One slot per item: rows never move, and full means every item is held
};

struct BagData {
    ::pksm::Generation storageFormat;  // Keys item names and sprites
    std::vector<Pouch> pouches;
};

}  // namespace pksm::bag
