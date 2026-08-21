#pragma once

#include "sav/Sav.hpp"

namespace pksm::saves {

// Some saves keep external references into their box storage (LGPE's party
// indexes the box list). These helpers move such references along with the
// entries they name; generation-agnostic for callers.

// Reference token attached to the list entry at listIndex, -1 if none;
// excludeToken keeps a held Pokémon's own ref from shadowing the occupant's
int ListRefAt(::pksm::Sav& sav, int listIndex, int excludeToken = -1);

// Point a previously taken reference token at the entry's new list index
void MoveListRef(::pksm::Sav& sav, int token, int listIndex);

// Drop a token whose entry left the save entirely; the referrer is repaired
void ClearListRef(::pksm::Sav& sav, int token);

// Drop a token without repairing the referrer (repair deferred to the caller)
void DetachListRef(::pksm::Sav& sav, int token);

// Drop every reference naming the entry at listIndex
void ClearListRefsAt(::pksm::Sav& sav, int listIndex);

// Compact the list and recount it before the save's bytes leave memory;
// no-op for saves without a box list
void FinalizeBoxList(::pksm::Sav& sav);

}  // namespace pksm::saves
