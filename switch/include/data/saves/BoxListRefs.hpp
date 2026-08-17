#pragma once

#include "sav/Sav.hpp"

namespace pksm::saves {

// Some saves keep external references into their box storage: LGPE's party
// is six indices into the same 1000-entry list the box editor moves entries
// within. Editors keep such references attached to the Pokémon they name by
// taking the reference token where an entry is picked up and moving it to
// wherever that entry finally lands. Generation-agnostic for callers; the
// per-save knowledge lives here.

// Reference token attached to the list entry at listIndex, -1 if none.
// excludeToken skips that reference: a held Pokémon's own (transiently
// stale) ref must not shadow the occupant's during a swap.
int ListRefAt(::pksm::Sav& sav, int listIndex, int excludeToken = -1);

// Point a previously taken reference token at the entry's new list index
void MoveListRef(::pksm::Sav& sav, int token, int listIndex);

}  // namespace pksm::saves
