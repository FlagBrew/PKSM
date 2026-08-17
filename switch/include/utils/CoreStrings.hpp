#pragma once

#include <string>

#include "enums/Ability.hpp"
#include "enums/Generation.hpp"
#include "enums/Language.hpp"
#include "enums/Move.hpp"
#include "enums/Nature.hpp"
#include "enums/Species.hpp"
#include "enums/Type.hpp"

// Localized display names from PKSM-Core's string tables (romfs:/i18n/,
// loaded lazily by the core on first use). Every lookup falls back to a
// readable placeholder rather than an empty string.
namespace pksm::strings {

// The app currently presents in English; following the system language is a
// planned follow-up, and this is the one place that decides.
::pksm::Language CurrentLanguage();

std::string SpeciesName(::pksm::Species species);
std::string MoveName(::pksm::Move move);
std::string AbilityName(::pksm::Ability ability);
std::string NatureName(::pksm::Nature nature);
std::string TypeName(::pksm::Type type);
std::string ItemName(u16 itemId);

// The raw held-item index is format-native: Gen 2 and Gen 3 formats store
// their own item indices, later formats store national ones
std::string ItemName(u16 itemId, ::pksm::Generation storageFormat);

}  // namespace pksm::strings
