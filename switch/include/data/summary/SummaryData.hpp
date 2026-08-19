#pragma once

#include <optional>
#include <string>
#include <vector>

#include "enums/Gender.hpp"
#include "enums/Type.hpp"

// Generation-agnostic view of one Pokémon's summary; the UI renders it
// verbatim and carries no generation knowledge.
namespace pksm::summary {

enum class Accent : u8 {
    None,
    Positive,  // Nature-raised stat label
    Negative,  // Nature-lowered stat label
    Highlight,  // Hyper-trained IV, hidden ability
};

struct Row {
    std::string label;
    std::string value;
    Accent accent = Accent::None;
    // When set, the value renders as a colored type chip instead of text
    std::optional<::pksm::Type> typeChip;
};

struct TypeBadge {
    ::pksm::Type type;  // Keys the chip color
    std::string name;  // Localized display name
};

struct StatCell {
    std::string text;
    // Shares one stored value with the row above (e.g. Gen 2's single Special DV)
    bool mergedWithAbove = false;
    Accent accent = Accent::None;
};

struct StatRow {
    std::string label;
    Accent labelAccent = Accent::None;
    std::vector<StatCell> cells;  // One per column header
};

struct StatTable {
    std::vector<std::string> columnHeaders;  // e.g. {"DV","Stat Exp","Stat"} - count varies by format
    std::vector<StatRow> rows;  // 5 or 6 rows depending on format
    std::string footnote;  // Explains table quirks (e.g. shared Gen 2 Special values); empty for none
};

struct SummaryData {
    // Header strip. Sprite is keyed by species/form/shiny.
    u16 species = 0;
    u8 form = 0;
    bool shiny = false;
    std::string speciesName;
    std::optional<::pksm::Gender> gender;  // nullopt: the format has no gender
    u8 level = 0;
    std::string formatBadge;  // "Gen 3", "LGPE", "PLA", ...
    bool pokerus = false;
    std::vector<TypeBadge> types;  // 1 or 2, deduped

    std::vector<Row> details;
    StatTable stats;
    std::vector<std::string> moves;  // 0-4, empty slots omitted
};

}  // namespace pksm::summary
