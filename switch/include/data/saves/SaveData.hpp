#pragma once

#include <memory>
#include <optional>
#include <pu/pu_Include.hpp>
#include <string>
#include <switch.h>
#include <vector>

namespace pksm::saves {

enum class Generation { ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, LGPE, EIGHT, NINE };

enum class Gender { Male, Female };

enum class GameVersion {
    RD,
    GN,
    BU,
    YW,
    GD,
    SV,
    C,
    R,
    S,
    E,
    FR,
    LG,
    D,
    P,
    Pt,
    HG,
    SS,
    W,
    B,
    W2,
    B2,
    X,
    Y,
    OR,
    AS,
    SN,
    MN,
    US,
    UM,
    GP,
    GE,
    SW,
    SH,
    BD,
    SP,
    PLA,
    SL,
    VL
};

class SaveData {
public:
    PU_SMART_CTOR(SaveData)

    // Constructor
    SaveData(
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
    );

    // Getters
    const std::string& getName() const { return name; }
    Generation getGeneration() const { return generation; }
    GameVersion getVersion() const { return version; }
    const std::string& getOTName() const { return otName; }
    u16 getTID() const { return tid; }
    u16 getSID() const { return sid; }
    Gender getGender() const { return gender; }
    u8 getBadges() const { return badges; }
    u16 getDexSeen() const { return dexSeen; }
    u16 getDexCaught() const { return dexCaught; }
    u16 getWonderCards() const { return wonderCards; }
    u16 getPlayedHours() const { return playedHours; }
    u8 getPlayedMinutes() const { return playedMinutes; }
    u8 getPlayedSeconds() const { return playedSeconds; }

    // Formatted time string (e.g., "26h 15m")
    std::string getPlayedTimeString() const;

    // Formatted completion percentage (e.g., 0.48f for 48%)
    float getDexCompletionPercentage() const;

    // Helper method to convert Generation to string
    static std::string GenerationToString(Generation gen);

private:
    std::string name;
    Generation generation;
    GameVersion version;
    std::string otName;
    u16 tid;
    u16 sid;
    Gender gender;
    u8 badges;
    u16 dexSeen;
    u16 dexCaught;
    u16 wonderCards;
    u16 playedHours;
    u8 playedMinutes;
    u8 playedSeconds;
};

}  // namespace pksm::saves