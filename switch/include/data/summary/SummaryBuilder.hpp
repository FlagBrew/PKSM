#pragma once

#include "data/summary/SummaryData.hpp"
#include "pkx/PKX.hpp"

namespace pksm::summary {

// The single place that knows which summary fields exist in which format
SummaryData BuildSummary(const ::pksm::PKX& pk);

}  // namespace pksm::summary
