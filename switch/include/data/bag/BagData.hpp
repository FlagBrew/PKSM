#pragma once

#include <string>
#include <vector>

#include "enums/Generation.hpp"
#include "sav/Sav.hpp"

// Generation-agnostic view of a save's bag; the UI renders it verbatim
namespace pksm::bag {

struct Slot {
    u16 itemId;  // Format-native, like strings::ItemName
    u16 count;
    std::string name;
};

struct Pouch {
    ::pksm::Sav::Pouch pouch;
    std::string name;
    size_t capacity;
    std::vector<Slot> items;  // Owned items in save order
};

struct BagData {
    ::pksm::Generation storageFormat;  // Keys item names and sprites
    std::vector<Pouch> pouches;
};

}  // namespace pksm::bag
