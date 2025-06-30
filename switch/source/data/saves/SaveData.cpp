#include "data/saves/SaveData.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace pksm::saves {

SaveData::SaveData(
    const std::string& name,
    Generation generation,
    GameVersion version,
    const std::string& otName,
    u16 tid,
    u16 sid,
    Gender gender,
    u8 badges,
    u16 dexSeen,
    u16 dexCaught,
    u16 wonderCards,
    u16 playedHours,
    u8 playedMinutes,
    u8 playedSeconds
)
  : name(name),
    generation(generation),
    version(version),
    otName(otName),
    tid(tid),
    sid(sid),
    gender(gender),
    badges(badges),
    dexSeen(dexSeen),
    dexCaught(dexCaught),
    wonderCards(wonderCards),
    playedHours(playedHours),
    playedMinutes(playedMinutes),
    playedSeconds(playedSeconds) {}

std::string SaveData::getPlayedTimeString() const {
    // Ensure hours don't exceed 300
    u16 displayHours = std::min(playedHours, static_cast<u16>(300));

    std::stringstream ss;
    // Don't use leading zeros for hours
    ss << displayHours << ":" << std::setfill('0') << std::setw(2) << static_cast<int>(playedMinutes) << ":"
       << std::setfill('0') << std::setw(2) << static_cast<int>(playedSeconds);
    return ss.str();
}

float SaveData::getDexCompletionPercentage() const {
    if (dexSeen == 0) {
        return 0.0f;
    }

    // Ensure dexCaught is not greater than dexSeen for percentage calculation
    u16 adjustedCaught = std::min(dexCaught, dexSeen);
    return (static_cast<float>(adjustedCaught) / static_cast<float>(dexSeen)) * 100.0f;
}

std::string SaveData::GenerationToString(Generation gen) {
    switch (gen) {
        case Generation::ONE:
            return "I";
        case Generation::TWO:
            return "II";
        case Generation::THREE:
            return "III";
        case Generation::FOUR:
            return "IV";
        case Generation::FIVE:
            return "V";
        case Generation::SIX:
            return "VI";
        case Generation::SEVEN:
            return "VII";
        case Generation::LGPE:
            return "LGPE";
        case Generation::EIGHT:
            return "VIII";
        case Generation::NINE:
            return "IX";
        default:
            return "Unknown";
    }
}

}  // namespace pksm::saves