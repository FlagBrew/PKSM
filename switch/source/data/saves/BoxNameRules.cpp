#include "data/saves/BoxNameRules.hpp"

namespace pksm::saves {

// Gen 1 stores no box names (core synthesizes "Box N") and LGPE has no real
// boxes; both core setters are no-ops. Field capacities follow the core
// setters: 8 characters for Gens 2-5, 16 from Gen 6 on (SWSH, PLA, SV and
// Z-A all use the same 0x22-byte layout entries).
BoxNameRules BoxNameRulesFor(const ::pksm::Sav& sav) {
    switch (sav.generation()) {
        case ::pksm::Generation::TWO:
        case ::pksm::Generation::THREE:
        case ::pksm::Generation::FOUR:
        case ::pksm::Generation::FIVE:
            return {true, 8};
        case ::pksm::Generation::SIX:
        case ::pksm::Generation::SEVEN:
        case ::pksm::Generation::EIGHT:
        case ::pksm::Generation::NINE:
            return {true, 16};
        default:
            return {};
    }
}

}  // namespace pksm::saves
