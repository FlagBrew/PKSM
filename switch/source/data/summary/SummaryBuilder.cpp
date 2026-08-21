#include "data/summary/SummaryBuilder.hpp"

#include <algorithm>
#include <cmath>

#include "pkx/PA8.hpp"
#include "pkx/PB7.hpp"
#include "pkx/PK8.hpp"
#include "pkx/PK9.hpp"
#include "utils/CoreStrings.hpp"

namespace {

using pksm::summary::Accent;
using pksm::summary::StatCell;
using pksm::summary::StatRow;
using pksm::summary::SummaryData;

// The Legends formats report the mainline generation, so they are told
// apart by file extension
enum class Format { G1, G2, G3, G4, G5, G6, G7, LGPE, G8, PLA, G9, ZA };

Format DetectFormat(const ::pksm::PKX& pk) {
    switch (pk.generation()) {
        case ::pksm::Generation::ONE:
            return Format::G1;
        case ::pksm::Generation::TWO:
            return Format::G2;
        case ::pksm::Generation::THREE:
            return Format::G3;
        case ::pksm::Generation::FOUR:
            return Format::G4;
        case ::pksm::Generation::FIVE:
            return Format::G5;
        case ::pksm::Generation::SIX:
            return Format::G6;
        case ::pksm::Generation::SEVEN:
            return Format::G7;
        case ::pksm::Generation::LGPE:
            return Format::LGPE;
        case ::pksm::Generation::EIGHT:
            return pk.extension() == ".pa8" ? Format::PLA : Format::G8;
        case ::pksm::Generation::NINE:
            return pk.extension() == ".pa9" ? Format::ZA : Format::G9;
        default:
            return Format::G8;
    }
}

std::string FormatBadge(Format fmt) {
    switch (fmt) {
        case Format::G1:
            return "Gen 1";
        case Format::G2:
            return "Gen 2";
        case Format::G3:
            return "Gen 3";
        case Format::G4:
            return "Gen 4";
        case Format::G5:
            return "Gen 5";
        case Format::G6:
            return "Gen 6";
        case Format::G7:
            return "Gen 7";
        case Format::LGPE:
            return "LGPE";
        case Format::G8:
            return "Gen 8";
        case Format::PLA:
            return "PLA";
        case Format::G9:
            return "Gen 9";
        case Format::ZA:
            return "Z-A";
    }
    return "";
}

bool DvEra(Format fmt) {
    return fmt == Format::G1 || fmt == Format::G2;
}

std::string OrDash(const std::string& s) {
    return s.empty() ? "-" : s;
}

std::string PadDigits(u32 value, int width) {
    std::string s = std::to_string(value);
    while (static_cast<int>(s.size()) < width) {
        s.insert(s.begin(), '0');
    }
    return s;
}

u8 BaseStat(const ::pksm::PKX& pk, ::pksm::Stat stat) {
    switch (stat) {
        case ::pksm::Stat::HP:
            return pk.baseHP();
        case ::pksm::Stat::ATK:
            return pk.baseAtk();
        case ::pksm::Stat::DEF:
            return pk.baseDef();
        case ::pksm::Stat::SPD:
            return pk.baseSpe();
        case ::pksm::Stat::SPATK:
            return pk.baseSpa();
        default:
            return pk.baseSpd();
    }
}

// Canonical nature table order (Atk, Def, Spe, SpA, SpD) = core Stat enum order shifted by one
int NatureAmp(::pksm::Nature nature, ::pksm::Stat stat) {
    if (stat == ::pksm::Stat::HP) {
        return 0;
    }
    const u8 raised = static_cast<u8>(nature) / 5;
    const u8 lowered = static_cast<u8>(nature) % 5;
    if (raised == lowered) {
        return 0;
    }
    const u8 index = static_cast<u8>(stat) - 1;
    if (index == raised) {
        return 1;
    }
    if (index == lowered) {
        return -1;
    }
    return 0;
}

Accent NatureAccent(::pksm::Nature nature, ::pksm::Stat stat) {
    switch (NatureAmp(nature, stat)) {
        case 1:
            return Accent::Positive;
        case -1:
            return Accent::Negative;
        default:
            return Accent::None;
    }
}

// Gen 3+ formula, for formats whose core stat() only reads party bytes (0 for box Pokémon)
u16 ClassicStat(const ::pksm::PKX& pk, ::pksm::Stat stat, ::pksm::Nature nature) {
    const int base = BaseStat(pk, stat);
    const int calc = (2 * base + pk.iv(stat) + pk.ev(stat) / 4) * pk.level() / 100;
    if (stat == ::pksm::Stat::HP) {
        if (pk.species() == ::pksm::Species::Shedinja) {
            return 1;
        }
        return static_cast<u16>(calc + pk.level() + 10);
    }
    const int initial = calc + 5;
    switch (NatureAmp(nature, stat)) {
        case 1:
            return static_cast<u16>(110 * initial / 100);
        case -1:
            return static_cast<u16>(90 * initial / 100);
        default:
            return static_cast<u16>(initial);
    }
}

// Ported from PKHeX 26.07.07 PA8.cs LoadStats/GetGanbaruStat/GetStatHp/GetStat + IGanbaru.cs
u16 PlaStat(const ::pksm::PA8& pk, ::pksm::Stat stat, ::pksm::Nature nature) {
    static constexpr u8 GANBARU_MULTIPLIER[11] = {0, 2, 3, 4, 7, 8, 9, 14, 15, 16, 25};

    const int base = BaseStat(pk, stat);
    const int iv = pk.hyperTrain(stat) ? 31 : pk.iv(stat);
    const int bias = iv >= 31 ? 3 : (iv >= 26 ? 2 : (iv >= 20 ? 1 : 0));
    const int multIndex = std::min(pk.effortLevel(stat) + bias, 10);
    const int gvPart =
        static_cast<int>(std::lround((std::sqrt(static_cast<double>(base)) * GANBARU_MULTIPLIER[multIndex] +
                                      pk.level()) /
                                     2.5));

    if (stat == ::pksm::Stat::HP) {
        const int hp = static_cast<int>(((pk.level() / 100.0f + 1.0f) * base) + pk.level());
        return static_cast<u16>(gvPart + hp);
    }
    const int initial = static_cast<int>(((pk.level() / 50.0f + 1.0f) * base) / 1.5f);
    switch (NatureAmp(nature, stat)) {
        case 1:
            return static_cast<u16>(gvPart + 110 * initial / 100);
        case -1:
            return static_cast<u16>(gvPart + 90 * initial / 100);
        default:
            return static_cast<u16>(gvPart + initial);
    }
}

// Per PKHeX 26.07.07 ITeraType.GetTeraType: 19 = no override, junk guards to
// Normal. Stellar has no Type enum value, so it returns nullopt.
std::optional<::pksm::Type> TeraType(const ::pksm::PK9& pk) {
    constexpr u8 OVERRIDE_NONE = 19;
    constexpr u8 STELLAR = 99;
    constexpr u8 MAX_TYPE = 17;

    const u8 override_ = pk.teraTypeOverride();
    u8 effective;
    if (override_ <= MAX_TYPE || override_ == STELLAR) {
        effective = override_;
    } else if (override_ != OVERRIDE_NONE) {
        effective = 0;
    } else {
        effective = pk.teraTypeOriginal() <= STELLAR ? pk.teraTypeOriginal() : 0;
    }
    if (effective == STELLAR) {
        return std::nullopt;
    }
    if (effective > MAX_TYPE) {
        effective = 0;
    }
    return ::pksm::Type{effective};
}

// Keyed by the Pokémon's origin, matching the games and PKHeX; GO/unset
// origins fall back to the storage format.
bool SixDigitTrainerId(const ::pksm::PKX& pk, Format fmt) {
    switch (pk.originGen()) {
        case ::pksm::Generation::SEVEN:
        case ::pksm::Generation::LGPE:
        case ::pksm::Generation::EIGHT:
        case ::pksm::Generation::NINE:
            return true;
        case ::pksm::Generation::UNUSED:
            return fmt == Format::G7 || fmt == Format::LGPE || fmt == Format::G8 || fmt == Format::PLA ||
                fmt == Format::G9 || fmt == Format::ZA;
        default:
            return false;
    }
}

}  // namespace

namespace pksm::summary {

SummaryData BuildSummary(const ::pksm::PKX& pk) {
    SummaryData data;
    const Format fmt = DetectFormat(pk);

    // Header strip
    data.species = static_cast<u16>(pk.species());
    const u16 form = pk.alternativeForm();
    data.form = form > 0xFF ? 0 : static_cast<u8>(form);
    data.shiny = pk.shiny();
    data.speciesName = strings::SpeciesName(pk.species());
    if (fmt != Format::G1 && pk.egg()) {
        data.speciesName += " (Egg)";
    }
    // Gen 1 has no gender; the core's value is the Gen 2 transfer derivation
    if (fmt != Format::G1) {
        data.gender = pk.gender();
    }
    data.level = pk.level();
    data.formatBadge = FormatBadge(fmt);
    // Active infection only - the strain byte stays set after curing
    data.pokerus = fmt != Format::G1 && pk.pkrsDays() != 0;
    data.types.push_back({pk.type1(), strings::TypeName(pk.type1())});
    if (pk.type2() != pk.type1()) {
        data.types.push_back({pk.type2(), strings::TypeName(pk.type2())});
    }

    // Gen 9/PLA mint a separate stat nature; the other formats apply nature() itself
    ::pksm::Nature statNature = pk.nature();
    if (fmt == Format::G9) {
        statNature = static_cast<const ::pksm::PK9&>(pk).statNature();
    } else if (fmt == Format::PLA) {
        statNature = static_cast<const ::pksm::PA8&>(pk).statNature();
    }

    // Detail rows, whitelisted per format - the core fabricates values for
    // concepts a game doesn't have (Gen 1/2 nature, Gen 1's catch-rate "held item")
    const auto addRow = [&](const std::string& label, const std::string& value, Accent accent = Accent::None) {
        data.details.push_back({label, value, accent, std::nullopt});
    };
    const auto addTypeRow = [&](const std::string& label, ::pksm::Type type) {
        data.details.push_back({label, strings::TypeName(type), Accent::None, type});
    };

    addRow("Nickname", OrDash(pk.nickname()));
    addRow("OT", OrDash(pk.otName()));
    if (!DvEra(fmt)) {
        addRow("Nature", strings::NatureName(pk.nature()));
        // LGPE/PLA store an ability byte their gameplay never uses; shown for PKHeX parity
        addRow(
            "Ability",
            strings::AbilityName(pk.ability()),
            pk.abilityNumber() == 4 ? Accent::Highlight : Accent::None
        );
    }
    const bool hasHeldItem = fmt != Format::G1 && fmt != Format::LGPE && fmt != Format::PLA;
    if (hasHeldItem) {
        addRow("Item", strings::ItemName(pk.heldItem(), pk.generation()));
    }
    if (fmt == Format::LGPE) {
        addRow("CP", std::to_string(static_cast<const ::pksm::PB7&>(pk).CP()));
    }
    // The extension gate keeps the cast safe if a format lands in DetectFormat's default
    if (fmt == Format::G8 && pk.extension() == ".pk8") {
        const auto& pk8 = static_cast<const ::pksm::PK8&>(pk);
        addRow(
            "Dynamax Lv",
            std::to_string(pk8.dynamaxLevel()) + (pk8.canGiga() ? " (G-Max)" : "")
        );
    }
    if (fmt == Format::G9) {
        const auto tera = TeraType(static_cast<const ::pksm::PK9&>(pk));
        if (tera) {
            addTypeRow("Tera Type", *tera);
        } else {
            addRow("Tera Type", "Stellar", Accent::Highlight);
        }
    }

    if (DvEra(fmt)) {
        // 16-bit ID, no secret ID in these games
        addRow("TID", std::to_string(pk.TID()));
    } else if (SixDigitTrainerId(pk, fmt)) {
        const u32 composite = (static_cast<u32>(pk.SID()) << 16) | pk.TID();
        addRow("TID/SID", PadDigits(composite % 1000000, 6) + "/" + PadDigits(composite / 1000000, 4));
    } else {
        addRow("TID/SID", std::to_string(pk.TID()) + "/" + std::to_string(pk.SID()));
    }
    if (!DvEra(fmt)) {
        addRow("PSV/TSV", std::to_string(pk.PSV()) + "/" + std::to_string(pk.TSV()));
    }
    if (fmt != Format::G1) {
        const bool otOnly =
            fmt == Format::G2 || fmt == Format::G3 || fmt == Format::G4 || fmt == Format::G5;
        addRow("Friendship", std::to_string(otOnly ? pk.otFriendship() : pk.currentFriendship()));
    }
    // Hidden Power exists Gen 2-7; type only (power is fixed from Gen 6 on)
    const bool hasHiddenPower = fmt == Format::G2 || fmt == Format::G3 || fmt == Format::G4 ||
        fmt == Format::G5 || fmt == Format::G6 || fmt == Format::G7;
    if (hasHiddenPower) {
        addTypeRow("Hidden Power", pk.hpType());
    }

    // Stat table
    const auto addStatRow = [&](const std::string& label,
                                ::pksm::Stat stat,
                                bool mergedDvExp,
                                bool withStatColumn,
                                bool natureAccents) {
        StatRow row;
        row.label = label;
        row.labelAccent = natureAccents ? NatureAccent(statNature, stat) : Accent::None;
        StatCell ivCell{std::to_string(pk.iv(stat)), mergedDvExp,
                        pk.hyperTrain(stat) ? Accent::Highlight : Accent::None};
        // Middle column: stat exp (Gens 1/2), AVs (LGPE), effort levels (PLA), EVs elsewhere
        const u16 middle = fmt == Format::PLA
            ? static_cast<const ::pksm::PA8&>(pk).effortLevel(stat)
            : pk.secondaryStatCalc(stat);
        row.cells.push_back(ivCell);
        row.cells.push_back({std::to_string(middle), mergedDvExp, Accent::None});
        if (withStatColumn) {
            u16 statValue;
            if (fmt == Format::G9) {
                statValue = ClassicStat(pk, stat, statNature);
            } else if (fmt == Format::PLA) {
                statValue = PlaStat(static_cast<const ::pksm::PA8&>(pk), stat, statNature);
            } else {
                statValue = pk.stat(stat);
            }
            row.cells.push_back({std::to_string(statValue), false, Accent::None});
        }
        data.stats.rows.push_back(std::move(row));
    };

    if (fmt == Format::G1) {
        // Speed before Special is the era's own ordering
        data.stats.columnHeaders = {"DV", "Exp", "Stat"};
        addStatRow("HP", ::pksm::Stat::HP, false, true, false);
        addStatRow("Attack", ::pksm::Stat::ATK, false, true, false);
        addStatRow("Defense", ::pksm::Stat::DEF, false, true, false);
        addStatRow("Speed", ::pksm::Stat::SPD, false, true, false);
        addStatRow("Special", ::pksm::Stat::SPATK, false, true, false);
    } else if (fmt == Format::G2) {
        // One Special DV/Exp feeds two separately computed stats; the merged cells say so
        data.stats.columnHeaders = {"DV", "Exp", "Stat"};
        addStatRow("HP", ::pksm::Stat::HP, false, true, false);
        addStatRow("Attack", ::pksm::Stat::ATK, false, true, false);
        addStatRow("Defense", ::pksm::Stat::DEF, false, true, false);
        addStatRow("Sp. Atk", ::pksm::Stat::SPATK, false, true, false);
        addStatRow("Sp. Def", ::pksm::Stat::SPDEF, true, true, false);
        addStatRow("Speed", ::pksm::Stat::SPD, false, true, false);
        data.stats.footnote = "Sp. Atk & Sp. Def share the one Special DV/Exp";
    } else {
        if (fmt == Format::LGPE) {
            data.stats.columnHeaders = {"IV", "AV", "Stat"};
        } else if (fmt == Format::PLA) {
            data.stats.columnHeaders = {"IV", "EL", "Stat"};
        } else if (fmt == Format::ZA) {
            // No verified stat formula for Z-A box data yet
            data.stats.columnHeaders = {"IV", "EV"};
        } else {
            data.stats.columnHeaders = {"IV", "EV", "Stat"};
        }
        const bool withStat = fmt != Format::ZA;
        addStatRow("HP", ::pksm::Stat::HP, false, withStat, true);
        addStatRow("Attack", ::pksm::Stat::ATK, false, withStat, true);
        addStatRow("Defense", ::pksm::Stat::DEF, false, withStat, true);
        addStatRow("Sp. Atk", ::pksm::Stat::SPATK, false, withStat, true);
        addStatRow("Sp. Def", ::pksm::Stat::SPDEF, false, withStat, true);
        addStatRow("Speed", ::pksm::Stat::SPD, false, withStat, true);
    }

    // Moves, empty slots omitted
    for (u8 i = 0; i < 4; i++) {
        const auto move = pk.move(i);
        if (move != ::pksm::Move::None) {
            data.moves.push_back(strings::MoveName(move));
        }
    }

    return data;
}

}  // namespace pksm::summary
