#pragma once

#include "sav/Item.hpp"
#include "sav/Sav.hpp"

// The marks a game keeps beside an item: the red dot until the player looks at it, and a
// favourite. The Sav tells Sword/Shield's Item8 from Arceus's Item8a, which both answer EIGHT
namespace pksm::bag {

struct ItemMarks {
    bool isNew = false;
    bool favorite = false;
};

bool KeepsNewMark(const ::pksm::Sav& sav);
bool KeepsFavorite(const ::pksm::Sav& sav);
ItemMarks MarksOf(const ::pksm::Sav& sav, const ::pksm::Item& item);
// Sets what the format keeps and leaves the rest
void SetMarks(const ::pksm::Sav& sav, ::pksm::Item& item, ItemMarks marks);

}  // namespace pksm::bag
