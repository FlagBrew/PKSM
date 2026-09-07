#pragma once

#include <span>

#include "sav/Sav.hpp"

// Item ids as the save's format stores them. Gen 1-3 items carry their own ids, which core's
// Item::id() and Sav::validItems() convert to the national index; the bag addresses items by
// the native id, as strings::ItemName and the sprites do
namespace pksm::bag {

u16 NativeItemId(const ::pksm::Item& item);

void SetNativeItemId(::pksm::Item& item, u16 id);

// The items the pouch can hold, in the game's order; empty for a pouch the save lacks
std::span<const int> NativeItemList(const ::pksm::Sav& sav, ::pksm::Sav::Pouch pouch);

}  // namespace pksm::bag
