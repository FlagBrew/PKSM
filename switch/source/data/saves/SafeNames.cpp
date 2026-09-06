#include "data/saves/SafeNames.hpp"

#include <algorithm>
#include <iterator>
#include <switch.h>

namespace pksm::saves {

std::string JKSVSafeName(const std::string& name) {
    static constexpr u32 verboten[] =
        {',', '/', '\\', '<', '>', ':', '"', '|', '?', '*', 0x2122 /*TM*/, 0xA9 /*(c)*/, 0xAE /*(r)*/};

    std::string ret;
    for (size_t i = 0; i < name.size();) {
        const u8 lead = static_cast<u8>(name[i]);
        size_t len = 1;
        u32 codepoint = lead;
        if (lead >= 0xF0) {
            len = 4;
        } else if (lead >= 0xE0) {
            len = 3;
        } else if (lead >= 0xC0) {
            len = 2;
        }
        if (len > 1) {
            if (i + len > name.size()) {
                return "";
            }
            codepoint = lead & (0x7F >> len);
            for (size_t j = 1; j < len; j++) {
                codepoint = (codepoint << 6) | (static_cast<u8>(name[i + j]) & 0x3F);
            }
        }
        i += len;

        if (codepoint == 0xE9) {
            codepoint = 'e';
        }
        if (std::find(std::begin(verboten), std::end(verboten), codepoint) != std::end(verboten)) {
            ret += ' ';
        } else if (codepoint <= 30 || codepoint >= 127) {
            return "";
        } else {
            ret += static_cast<char>(codepoint);
        }
    }

    while (!ret.empty() && (ret.back() == ' ' || ret.back() == '.')) {
        ret.pop_back();
    }
    return ret;
}

}  // namespace pksm::saves
