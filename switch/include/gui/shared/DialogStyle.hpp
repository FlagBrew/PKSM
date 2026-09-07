#pragma once

#include <pu/Plutonium>
#include <string>
#include <vector>

namespace pksm::ui {

// The app's list dialog: a light panel with one row per option, each row's note on its right and
// the cursor row outlined. Applied to a Plutonium dialog before it shows
void StyleListDialog(pu::ui::Dialog::Ref& dialog, const std::vector<std::string>& notes);

}  // namespace pksm::ui
