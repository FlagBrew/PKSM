#include "utils/SoftwareKeyboard.hpp"

#include <algorithm>
#include <cstdio>
#include <sstream>
#include <switch.h>
#include <vector>

#include "utils/Logger.hpp"

namespace {

// Clamp to a number of UTF-8 characters without splitting a sequence
std::string ClampToChars(const std::string& text, size_t maxChars) {
    size_t chars = 0;
    size_t bytes = 0;
    while (bytes < text.size() && chars < maxChars) {
        bytes++;
        while (bytes < text.size() && (static_cast<unsigned char>(text[bytes]) & 0xC0) == 0x80) {
            bytes++;
        }
        chars++;
    }
    return text.substr(0, bytes);
}

// swkbd's text-check callback takes no user data, so the active validator
// lives here (the UI is single-threaded, no call overlaps)
pksm::utils::KeyboardValidator activeValidator;

// A rejection message goes back to swkbd through the same buffer the text came in
SwkbdTextCheckResult ValidateText(char* tmp_string, size_t tmp_string_size) {
    const auto error = activeValidator(tmp_string);
    if (!error) {
        return SwkbdTextCheckResult_OK;
    }
    std::snprintf(tmp_string, tmp_string_size, "%s", error->c_str());
    return SwkbdTextCheckResult_Bad;
}

}  // namespace

namespace pksm::utils {

std::optional<std::string> ShowKeyboard(const std::string& headerText, const std::string& initialText,
    size_t maxLength, KeyboardValidator validator) {
    SwkbdConfig kbd;
    Result rc = swkbdCreate(&kbd, 0);
    if (R_FAILED(rc)) {
        std::stringstream ss;
        ss << "swkbdCreate failed: 0x" << std::hex << rc;
        LOG_ERROR(ss.str());
        return std::nullopt;
    }
    const std::string prefill = ClampToChars(initialText, maxLength);
    swkbdConfigMakePresetDefault(&kbd);
    swkbdConfigSetHeaderText(&kbd, headerText.c_str());
    swkbdConfigSetInitialText(&kbd, prefill.c_str());
    swkbdConfigSetStringLenMax(&kbd, static_cast<u32>(maxLength));

    // UTF-8 output: up to four bytes per character, plus the terminator
    size_t bufferSize = maxLength * 4 + 1;
    if (validator) {
        swkbdConfigSetTextCheckCallback(&kbd, ValidateText);
        activeValidator = std::move(validator);
        // The buffer must also fit a rejection message
        bufferSize = std::max<size_t>(bufferSize, 256);
    }
    std::vector<char> out(bufferSize, '\0');
    rc = swkbdShow(&kbd, out.data(), out.size());
    swkbdClose(&kbd);
    activeValidator = nullptr;

    // Cancellation surfaces as a failed result
    if (R_FAILED(rc)) {
        return std::nullopt;
    }
    return std::string(out.data());
}

}  // namespace pksm::utils
