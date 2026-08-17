#include "utils/CoreStrings.hpp"

#include "utils/i18n.hpp"

namespace pksm::strings {

::pksm::Language CurrentLanguage() {
    return ::pksm::Language::ENG;
}

std::string SpeciesName(::pksm::Species species) {
    const auto& name = i18n::species(CurrentLanguage(), species);
    if (name.empty()) {
        return "Species #" + std::to_string(static_cast<int>(static_cast<u16>(species)));
    }
    return name;
}

std::string MoveName(::pksm::Move move) {
    if (move == ::pksm::Move::None) {
        return "None";
    }
    const auto& name = i18n::move(CurrentLanguage(), move);
    if (name.empty()) {
        return "Move #" + std::to_string(static_cast<int>(static_cast<u16>(move)));
    }
    return name;
}

std::string AbilityName(::pksm::Ability ability) {
    const auto& name = i18n::ability(CurrentLanguage(), ability);
    if (name.empty()) {
        return "Ability #" + std::to_string(static_cast<int>(static_cast<u16>(ability)));
    }
    return name;
}

std::string NatureName(::pksm::Nature nature) {
    const auto& name = i18n::nature(CurrentLanguage(), nature);
    if (name.empty()) {
        return "Nature #" + std::to_string(static_cast<int>(static_cast<u8>(nature)));
    }
    return name;
}

std::string TypeName(::pksm::Type type) {
    const auto& name = i18n::type(CurrentLanguage(), type);
    if (name.empty()) {
        return "Type #" + std::to_string(static_cast<int>(static_cast<u8>(type)));
    }
    return name;
}

std::string ItemName(u16 itemId) {
    if (itemId == 0) {
        return "None";
    }
    const auto& name = i18n::item(CurrentLanguage(), itemId);
    if (name.empty()) {
        return "Item #" + std::to_string(itemId);
    }
    return name;
}

std::string ItemName(u16 itemId, ::pksm::Generation storageFormat) {
    if (itemId == 0) {
        return "None";
    }
    const std::string* name;
    if (storageFormat == ::pksm::Generation::TWO) {
        name = &i18n::item2(CurrentLanguage(), static_cast<u8>(itemId));
    } else if (storageFormat == ::pksm::Generation::THREE) {
        name = &i18n::item3(CurrentLanguage(), itemId);
    } else {
        name = &i18n::item(CurrentLanguage(), itemId);
    }
    if (name->empty()) {
        return "Item #" + std::to_string(itemId);
    }
    return *name;
}

}  // namespace pksm::strings
