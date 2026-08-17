#pragma once

#include "data/summary/SummaryData.hpp"
#include "pkx/PKX.hpp"

namespace pksm::summary {

// Builds the display model for one Pokémon. The single place that knows
// which summary fields exist in which format; everything it omits is either
// absent from that game or fabricated by the core's uniform interface.
SummaryData BuildSummary(const ::pksm::PKX& pk);

}  // namespace pksm::summary
