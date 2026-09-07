#pragma once

#include <optional>
#include <string>
#include <vector>

#include "sav/Sav.hpp"
#include "sav/SavZA.hpp"

// The orders the games that sort their own pouches keep in the save, and how their items and
// donuts fall into them; the provider reads the slots and writes the settings
namespace pksm::bag {

// The bag-wide sort a game that orders its own pouches keeps, as its menu lists the options
// (see Sav::bagSort); 0 shows as the first
enum class BagSort : u8 { Type, Name, Newest, Favorites };
extern const std::vector<std::string> BAG_SORT_OPTIONS;  // by BagSort

// The sort in force; nothing where the player orders the slots
std::optional<BagSort> BagSortOf(const ::pksm::Sav& sav);

// The sort as the save numbers it
u8 BagSortValue(BagSort sort);

// What the game's order reads off an item: the marks any format keeps, the type order only Gen 9's
struct BagSortKey {
    int order = 0;  // the game's sort-by-type position
    bool isNew = false;
    bool favorite = false;
    std::string name;  // folded, for a name sort
};

BagSortKey BagSortKeyOf(const ::pksm::Sav& sav, const ::pksm::Item& item, const std::string& name, BagSort sort);

// Flagged items lead, then the type order; a name sort stands alone. Ties keep the order given
bool BagSortBefore(BagSort sort, const BagSortKey& a, const BagSortKey& b);

// Legends Z-A's donut pocket has its own sort, cycled in this order in-game; the save numbers
// the options differently (see SavZA::donutSort)
enum class DonutSort : u8 { Date, Flavor, Variety };
extern const std::vector<std::string> DONUT_SORT_OPTIONS;  // by DonutSort

// The sort in force; nothing before the DLC
std::optional<DonutSort> DonutSortOf(const ::pksm::SavZA& za);

// The sort as the save numbers it
u8 DonutSortValue(DonutSort sort);

// Puts the donuts in the order the game shows
void SortDonuts(std::vector<::pksm::SavZA::Donut>& donuts, DonutSort sort);

// ASCII-only fold for name order: the games ignore case, and an accented letter sorts as typed
std::string FoldCase(std::string text);

}  // namespace pksm::bag
