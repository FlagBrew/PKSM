#include "data/saves/BoxNameRules.hpp"

#include <array>

#include "utils/utils.hpp"

namespace {

// What the save would store: round-trip through the same StringUtils calls
// each generation's boxName setter/getter pair makes.
std::string StoredBoxName(const ::pksm::Sav& sav, const std::string& name) {
    // Largest field: 17 UCS-2 code units (Gen 6 on)
    std::array<u8, 34> buf{};
    switch (sav.generation()) {
        case ::pksm::Generation::TWO: {
            const ::pksm::Language lang = sav.language();
            const int len = lang == ::pksm::Language::KOR ? 17 : 9;
            StringUtils::setString2(buf.data(), name, 0, len, lang, len);
            return StringUtils::getString2(buf.data(), 0, len, lang);
        }
        case ::pksm::Generation::THREE: {
            const bool jp = sav.language() == ::pksm::Language::JPN;
            StringUtils::setString3(buf.data(), name, 0, 8, jp, 9);
            return StringUtils::getString3(buf.data(), 0, 9, jp);
        }
        case ::pksm::Generation::FOUR:
            StringUtils::setString4(buf.data(), StringUtils::transString45(name), 0, 9);
            return StringUtils::transString45(StringUtils::getString4(buf.data(), 0, 9));
        case ::pksm::Generation::FIVE:
            StringUtils::setString(buf.data(), StringUtils::transString45(name), 0, 9, u'\uFFFF', 0);
            return StringUtils::transString45(StringUtils::getString(buf.data(), 0, 9, u'\uFFFF'));
        case ::pksm::Generation::SIX:
        case ::pksm::Generation::SEVEN:
            StringUtils::setString(buf.data(), StringUtils::transString67(name), 0, 17);
            return StringUtils::transString67(StringUtils::getString(buf.data(), 0, 17));
        case ::pksm::Generation::EIGHT:
        case ::pksm::Generation::NINE:
            StringUtils::setString(buf.data(), name, 0, 17);
            return StringUtils::getString(buf.data(), 0, 17);
        default:
            // Not renamable; nothing can be lost
            return name;
    }
}

}  // namespace

namespace pksm::saves {

// Gen 1 stores no box names and LGPE has no real boxes; both core setters are no-ops
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

std::optional<std::string> FirstUnstorableBoxNameChar(const ::pksm::Sav& sav, const std::string& name) {
    const std::u32string typed = StringUtils::UTF8toUTF32(name);
    const std::u32string stored = StringUtils::UTF8toUTF32(StoredBoxName(sav, name));
    // Unstorable characters truncate the stored name (charmap gens 2-4) or store
    // as U+FFFD (UCS-2 gens 5 on); fullwidth transliteration keeps length, so it passes.
    size_t bad = std::u32string::npos;
    if (stored.size() < typed.size()) {
        bad = stored.size();
    } else {
        for (size_t i = 0; i < typed.size(); i++) {
            if (stored[i] == StringUtils::CODEPOINT_INVALID && typed[i] != stored[i]) {
                bad = i;
                break;
            }
        }
    }
    if (bad == std::u32string::npos) {
        return std::nullopt;
    }
    return StringUtils::UTF32toUTF8(typed.substr(bad, 1));
}

}  // namespace pksm::saves
