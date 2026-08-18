#pragma once

#include <functional>
#include <optional>
#include <string>

namespace pksm::utils {

// Rejects keyboard input before the keyboard confirms it: returns a message
// to show over the still-open keyboard, or nullopt to accept the text
using KeyboardValidator = std::function<std::optional<std::string>(const std::string&)>;

// Blocking system software keyboard for a short single line of text.
// Returns the entered UTF-8 text, or nullopt when the user cancelled.
// maxLength is in characters; the keyboard refuses further input beyond it.
std::optional<std::string> ShowKeyboard(const std::string& headerText, const std::string& initialText,
    size_t maxLength, KeyboardValidator validator = {});

}  // namespace pksm::utils
