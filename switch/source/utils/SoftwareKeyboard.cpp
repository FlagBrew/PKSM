#include "utils/SoftwareKeyboard.hpp"

#include <sstream>
#include <switch.h>
#include <vector>

#include "utils/Logger.hpp"

namespace {

// Clamp to a number of UTF-8 characters without splitting a sequence; a
// stored value can exceed the keyboard's cap (other tools write longer
// than this app does)
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

}  // namespace

namespace pksm::utils {

std::optional<std::string>
ShowKeyboard(const std::string& headerText, const std::string& initialText, size_t maxLength) {
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
    std::vector<char> out(maxLength * 4 + 1, '\0');
    rc = swkbdShow(&kbd, out.data(), out.size());
    swkbdClose(&kbd);

    // Cancellation surfaces as a failed result
    if (R_FAILED(rc)) {
        return std::nullopt;
    }
    return std::string(out.data());
}

}  // namespace pksm::utils
