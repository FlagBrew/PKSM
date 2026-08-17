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

// Drop a previously taken reference token: its entry left the save entirely
// (e.g. released), so the referrer is repaired rather than retargeted
void ClearListRef(::pksm::Sav& sav, int token);

// Drop a token without repairing the referrer's order: for mid-carry
// detachment, when the entry re-enters the party by other means and the
// repair is deferred until the hand empties
void DetachListRef(::pksm::Sav& sav, int token);

// Drop every reference naming the entry at listIndex, for when that entry is
// removed in place without ever being picked up
void ClearListRefsAt(::pksm::Sav& sav, int listIndex);

// List bookkeeping owed right before the save's bytes leave memory:
// compact the list and recount its length (the 3DS app's storage-exit
// routine). No-op for saves without a box list.
void FinalizeBoxList(::pksm::Sav& sav);

}  // namespace pksm::saves
