#pragma once

#include <string>

#include "enums/Ability.hpp"
#include "enums/Generation.hpp"
#include "enums/Language.hpp"
#include "enums/Move.hpp"
#include "enums/Nature.hpp"
#include "enums/Species.hpp"
#include "enums/Type.hpp"

// Localized display names from PKSM-Core's string tables (romfs:/i18n/);
// every lookup falls back to a readable placeholder, never an empty string
namespace pksm::strings {

// English for now; the one place that decides the display language
::pksm::Language CurrentLanguage();

std::string SpeciesName(::pksm::Species species);
std::string MoveName(::pksm::Move move);
std::string AbilityName(::pksm::Ability ability);
std::string NatureName(::pksm::Nature nature);
std::string TypeName(::pksm::Type type);
std::string ItemName(u16 itemId);

// itemId is format-native: Gen 2/3 use their own item indices
std::string ItemName(u16 itemId, ::pksm::Generation storageFormat);

}  // namespace pksm::strings
