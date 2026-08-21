#pragma once

#include <functional>
#include <optional>
#include <string>

namespace pksm::utils {

// Returns a message to show over the still-open keyboard, or nullopt to accept
using KeyboardValidator = std::function<std::optional<std::string>(const std::string&)>;

// Blocking system keyboard for a single line; nullopt when cancelled.
// maxLength is in characters, not bytes.
std::optional<std::string> ShowKeyboard(const std::string& headerText, const std::string& initialText,
    size_t maxLength, KeyboardValidator validator = {});

}  // namespace pksm::utils
