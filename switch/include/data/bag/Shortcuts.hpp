#pragma once

#include <vector>

#include "data/bag/BagData.hpp"
#include "sav/Sav.hpp"

// The slots a game's shortcut button reaches, in the save's order, labelled as the game names them
namespace pksm::bag {

std::vector<Shortcut> ShortcutsOf(const ::pksm::Sav& sav);
// The slot holding the item, negative for none
int ShortcutOf(const ::pksm::Sav& sav, u16 itemId);

}  // namespace pksm::bag
