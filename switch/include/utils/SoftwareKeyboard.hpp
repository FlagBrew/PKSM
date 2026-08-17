#pragma once

#include <optional>
#include <string>

namespace pksm::utils {

// Blocking system software keyboard for a short single line of text.
// Returns the entered UTF-8 text, or nullopt when the user cancelled.
// maxLength is in characters; the keyboard refuses further input beyond it.
std::optional<std::string>
ShowKeyboard(const std::string& headerText, const std::string& initialText, size_t maxLength);

}  // namespace pksm::utils
