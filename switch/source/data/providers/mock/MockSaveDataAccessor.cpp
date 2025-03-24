#include "data/providers/mock/MockSaveDataAccessor.hpp"

#include <array>
#include <chrono>
#include <functional>  // For std::hash

#include "utils/Logger.hpp"

// Array of possible trainer names for mock saves
static const std::array<std::string, 10> TRAINER_NAMES =
    {"Red", "Blue", "Green", "Yellow", "Gold", "Silver", "Crystal", "Ruby", "Sapphire", "Emerald"};

MockSaveDataAccessor::MockSaveDataAccessor()
  : currentSave(nullptr), hasChanges(false), rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
    pksm::utils::Logger::Debug("Initializing MockSaveDataAccessor...");
    initializeMockSaveGenerators();
}

pksm::saves::SaveData::Ref MockSaveDataAccessor::getCurrentSaveData() const {
    return currentSave;
}

bool MockSaveDataAccessor::saveChanges() {
    pksm::utils::Logger::Debug("Saving changes to mock save...");
    hasChanges = false;
    return true;
}

bool MockSaveDataAccessor::hasUnsavedChanges() const {
    return hasChanges;
}

std::mt19937 MockSaveDataAccessor::createSeededRNG(u64 titleId, const std::string& saveName) const {
    // Combine title ID and save name to create a seed
    std::string seedStr = std::to_string(titleId) + saveName;

    // Use std::hash to create a seed from the combined string
    std::hash<std::string> hasher;
    std::size_t hashValue = hasher(seedStr);
    std::seed_seq seed{static_cast<unsigned int>(hashValue)};

    // Create and return a seeded random number generator
    return std::mt19937(seed);
}

bool MockSaveDataAccessor::loadMockSave(const pksm::titles::Title::Ref& title, const std::string& saveName) {
    pksm::utils::Logger::Debug("Loading mock save for title: " + title->getName() + ", save: " + saveName);

    u64 titleId = title->getTitleId();

    // Create a seeded RNG for this title/save combination
    std::mt19937 seededRng = createSeededRNG(titleId, saveName);

    auto it = mockSaveGenerators.find(titleId);

    if (it != mockSaveGenerators.end()) {
        // Use the generator for this title ID with the seeded RNG
        currentSave = it->second(saveName);
    } else {
        // Use a generic generator with the seeded RNG
        std::uniform_int_distribution<u16> tidDist(0, 65535);
        std::uniform_int_distribution<u16> sidDist(0, 65535);
        std::uniform_int_distribution<u8> genderDist(0, 1);
        std::uniform_int_distribution<u8> badgesDist(0, 8);
        std::uniform_int_distribution<u16> dexSeenDist(50, 400);
        std::uniform_int_distribution<u16> hoursDist(0, 300);  // Limit to 300 hours
        std::uniform_int_distribution<u8> minutesDist(1, 59);  // At least 1 minute
        std::uniform_int_distribution<u8> secondsDist(0, 59);

        // Generate dexSeen first using the seeded RNG
        u16 dexSeen = dexSeenDist(seededRng);
        // Then generate dexCaught to be at most dexSeen
        std::uniform_int_distribution<u16> dexCaughtDist(20, dexSeen);
        u16 dexCaught = dexCaughtDist(seededRng);

        std::uniform_int_distribution<u16> wonderCardsDist(0, 10);

        // Use the seeded RNG for all random values
        currentSave = std::make_shared<pksm::saves::SaveData>(
            saveName,
            pksm::saves::Generation::EIGHT,  // Default to Gen 8
            pksm::saves::GameVersion::SW,  // Default to Sword
            generateRandomTrainerName(seededRng),
            tidDist(seededRng),
            sidDist(seededRng),
            genderDist(seededRng) ? pksm::saves::Gender::Male : pksm::saves::Gender::Female,
            badgesDist(seededRng),
            dexSeen,
            dexCaught,
            wonderCardsDist(seededRng),
            hoursDist(seededRng),
            minutesDist(seededRng),
            secondsDist(seededRng)
        );
    }

    // Notify listeners that save data has changed
    if (onSaveDataChanged) {
        onSaveDataChanged(currentSave);
    }

    return true;
}

void MockSaveDataAccessor::initializeMockSaveGenerators() {
    // Add generators for different game types

    // Let's Go Pikachu/Eevee
    mockSaveGenerators[0x00175200] = [this](const std::string& saveName) -> pksm::saves::SaveData::Ref {
        // Create a seeded RNG for this title/save combination
        std::mt19937 seededRng = createSeededRNG(0x00175200, saveName);

        std::uniform_int_distribution<u8> versionDist(0, 1);
        std::uniform_int_distribution<u16> tidDist(0, 65535);
        std::uniform_int_distribution<u16> sidDist(0, 65535);
        std::uniform_int_distribution<u8> genderDist(0, 1);
        std::uniform_int_distribution<u8> badgesDist(0, 8);
        std::uniform_int_distribution<u16> dexSeenDist(100, 200);
        std::uniform_int_distribution<u16> hoursDist(0, 300);  // Limit to 300 hours
        std::uniform_int_distribution<u8> minutesDist(1, 59);  // At least 1 minute
        std::uniform_int_distribution<u8> secondsDist(0, 59);

        // Generate dexSeen first
        u16 dexSeen = dexSeenDist(seededRng);
        // Then generate dexCaught to be at most dexSeen
        std::uniform_int_distribution<u16> dexCaughtDist(50, dexSeen);
        u16 dexCaught = dexCaughtDist(seededRng);

        return std::make_shared<pksm::saves::SaveData>(
            saveName,
            pksm::saves::Generation::LGPE,
            versionDist(seededRng) ? pksm::saves::GameVersion::GP : pksm::saves::GameVersion::GE,
            generateRandomTrainerName(seededRng),
            tidDist(seededRng),
            sidDist(seededRng),
            genderDist(seededRng) ? pksm::saves::Gender::Male : pksm::saves::Gender::Female,
            badgesDist(seededRng),
            dexSeen,
            dexCaught,
            0,  // No Wonder Cards in LGPE
            hoursDist(seededRng),
            minutesDist(seededRng),
            secondsDist(seededRng)
        );
    };

    // Let's Go Eevee
    mockSaveGenerators[0x00175201] = [this](const std::string& saveName) -> pksm::saves::SaveData::Ref {
        // Create a seeded RNG for this title/save combination
        std::mt19937 seededRng = createSeededRNG(0x00175201, saveName);

        std::uniform_int_distribution<u16> tidDist(0, 65535);
        std::uniform_int_distribution<u16> sidDist(0, 65535);
        std::uniform_int_distribution<u8> genderDist(0, 1);
        std::uniform_int_distribution<u8> badgesDist(0, 8);
        std::uniform_int_distribution<u16> dexSeenDist(100, 200);
        std::uniform_int_distribution<u16> hoursDist(0, 300);  // Limit to 300 hours
        std::uniform_int_distribution<u8> minutesDist(1, 59);  // At least 1 minute
        std::uniform_int_distribution<u8> secondsDist(0, 59);

        // Generate dexSeen first
        u16 dexSeen = dexSeenDist(seededRng);
        // Then generate dexCaught to be at most dexSeen
        std::uniform_int_distribution<u16> dexCaughtDist(50, dexSeen);
        u16 dexCaught = dexCaughtDist(seededRng);

        return std::make_shared<pksm::saves::SaveData>(
            saveName,
            pksm::saves::Generation::LGPE,
            pksm::saves::GameVersion::GE,
            generateRandomTrainerName(seededRng),
            tidDist(seededRng),
            sidDist(seededRng),
            genderDist(seededRng) ? pksm::saves::Gender::Male : pksm::saves::Gender::Female,
            badgesDist(seededRng),
            dexSeen,
            dexCaught,
            0,  // No Wonder Cards in LGPE
            hoursDist(seededRng),
            minutesDist(seededRng),
            secondsDist(seededRng)
        );
    };

    // Sword
    mockSaveGenerators[0x00175202] = [this](const std::string& saveName) -> pksm::saves::SaveData::Ref {
        // Create a seeded RNG for this title/save combination
        std::mt19937 seededRng = createSeededRNG(0x00175202, saveName);

        std::uniform_int_distribution<u16> tidDist(0, 65535);
        std::uniform_int_distribution<u16> sidDist(0, 65535);
        std::uniform_int_distribution<u8> genderDist(0, 1);
        std::uniform_int_distribution<u8> badgesDist(0, 8);
        std::uniform_int_distribution<u16> dexSeenDist(200, 400);
        std::uniform_int_distribution<u16> wonderCardsDist(0, 10);
        std::uniform_int_distribution<u16> hoursDist(0, 300);  // Limit to 300 hours
        std::uniform_int_distribution<u8> minutesDist(1, 59);  // At least 1 minute
        std::uniform_int_distribution<u8> secondsDist(0, 59);

        // Generate dexSeen first
        u16 dexSeen = dexSeenDist(seededRng);
        // Then generate dexCaught to be at most dexSeen
        std::uniform_int_distribution<u16> dexCaughtDist(100, dexSeen);
        u16 dexCaught = dexCaughtDist(seededRng);

        return std::make_shared<pksm::saves::SaveData>(
            saveName,
            pksm::saves::Generation::EIGHT,
            pksm::saves::GameVersion::SW,
            generateRandomTrainerName(seededRng),
            tidDist(seededRng),
            sidDist(seededRng),
            genderDist(seededRng) ? pksm::saves::Gender::Male : pksm::saves::Gender::Female,
            badgesDist(seededRng),
            dexSeen,
            dexCaught,
            wonderCardsDist(seededRng),
            hoursDist(seededRng),
            minutesDist(seededRng),
            secondsDist(seededRng)
        );
    };

    // Shield
    mockSaveGenerators[0x00175203] = [this](const std::string& saveName) -> pksm::saves::SaveData::Ref {
        // Create a seeded RNG for this title/save combination
        std::mt19937 seededRng = createSeededRNG(0x00175203, saveName);

        std::uniform_int_distribution<u16> tidDist(0, 65535);
        std::uniform_int_distribution<u16> sidDist(0, 65535);
        std::uniform_int_distribution<u8> genderDist(0, 1);
        std::uniform_int_distribution<u8> badgesDist(0, 8);
        std::uniform_int_distribution<u16> dexSeenDist(200, 400);
        std::uniform_int_distribution<u16> wonderCardsDist(0, 10);
        std::uniform_int_distribution<u16> hoursDist(0, 300);  // Limit to 300 hours
        std::uniform_int_distribution<u8> minutesDist(1, 59);  // At least 1 minute
        std::uniform_int_distribution<u8> secondsDist(0, 59);

        // Generate dexSeen first
        u16 dexSeen = dexSeenDist(seededRng);
        // Then generate dexCaught to be at most dexSeen
        std::uniform_int_distribution<u16> dexCaughtDist(100, dexSeen);
        u16 dexCaught = dexCaughtDist(seededRng);

        return std::make_shared<pksm::saves::SaveData>(
            saveName,
            pksm::saves::Generation::EIGHT,
            pksm::saves::GameVersion::SH,
            generateRandomTrainerName(seededRng),
            tidDist(seededRng),
            sidDist(seededRng),
            genderDist(seededRng) ? pksm::saves::Gender::Male : pksm::saves::Gender::Female,
            badgesDist(seededRng),
            dexSeen,
            dexCaught,
            wonderCardsDist(seededRng),
            hoursDist(seededRng),
            minutesDist(seededRng),
            secondsDist(seededRng)
        );
    };

    // Brilliant Diamond
    mockSaveGenerators[0x00164900] = [this](const std::string& saveName) -> pksm::saves::SaveData::Ref {
        // Create a seeded RNG for this title/save combination
        std::mt19937 seededRng = createSeededRNG(0x00164900, saveName);

        std::uniform_int_distribution<u16> tidDist(0, 65535);
        std::uniform_int_distribution<u16> sidDist(0, 65535);
        std::uniform_int_distribution<u8> genderDist(0, 1);
        std::uniform_int_distribution<u8> badgesDist(0, 8);
        std::uniform_int_distribution<u16> dexSeenDist(150, 300);
        std::uniform_int_distribution<u16> wonderCardsDist(0, 5);
        std::uniform_int_distribution<u16> hoursDist(0, 300);  // Limit to 300 hours
        std::uniform_int_distribution<u8> minutesDist(1, 59);  // At least 1 minute
        std::uniform_int_distribution<u8> secondsDist(0, 59);

        // Generate dexSeen first
        u16 dexSeen = dexSeenDist(seededRng);
        // Then generate dexCaught to be at most dexSeen
        std::uniform_int_distribution<u16> dexCaughtDist(75, dexSeen);
        u16 dexCaught = dexCaughtDist(seededRng);

        return std::make_shared<pksm::saves::SaveData>(
            saveName,
            pksm::saves::Generation::EIGHT,
            pksm::saves::GameVersion::BD,
            generateRandomTrainerName(seededRng),
            tidDist(seededRng),
            sidDist(seededRng),
            genderDist(seededRng) ? pksm::saves::Gender::Male : pksm::saves::Gender::Female,
            badgesDist(seededRng),
            dexSeen,
            dexCaught,
            wonderCardsDist(seededRng),
            hoursDist(seededRng),
            minutesDist(seededRng),
            secondsDist(seededRng)
        );
    };

    // Shining Pearl
    mockSaveGenerators[0x00175204] = [this](const std::string& saveName) -> pksm::saves::SaveData::Ref {
        // Create a seeded RNG for this title/save combination
        std::mt19937 seededRng = createSeededRNG(0x00175204, saveName);

        std::uniform_int_distribution<u16> tidDist(0, 65535);
        std::uniform_int_distribution<u16> sidDist(0, 65535);
        std::uniform_int_distribution<u8> genderDist(0, 1);
        std::uniform_int_distribution<u8> badgesDist(0, 8);
        std::uniform_int_distribution<u16> dexSeenDist(150, 300);
        std::uniform_int_distribution<u16> wonderCardsDist(0, 5);
        std::uniform_int_distribution<u16> hoursDist(0, 300);  // Limit to 300 hours
        std::uniform_int_distribution<u8> minutesDist(1, 59);  // At least 1 minute
        std::uniform_int_distribution<u8> secondsDist(0, 59);

        // Generate dexSeen first
        u16 dexSeen = dexSeenDist(seededRng);
        // Then generate dexCaught to be at most dexSeen
        std::uniform_int_distribution<u16> dexCaughtDist(75, dexSeen);
        u16 dexCaught = dexCaughtDist(seededRng);

        return std::make_shared<pksm::saves::SaveData>(
            saveName,
            pksm::saves::Generation::EIGHT,
            pksm::saves::GameVersion::SP,
            generateRandomTrainerName(seededRng),
            tidDist(seededRng),
            sidDist(seededRng),
            genderDist(seededRng) ? pksm::saves::Gender::Male : pksm::saves::Gender::Female,
            badgesDist(seededRng),
            dexSeen,
            dexCaught,
            wonderCardsDist(seededRng),
            hoursDist(seededRng),
            minutesDist(seededRng),
            secondsDist(seededRng)
        );
    };

    // Legends Arceus
    mockSaveGenerators[0x00175E00] = [this](const std::string& saveName) -> pksm::saves::SaveData::Ref {
        // Create a seeded RNG for this title/save combination
        std::mt19937 seededRng = createSeededRNG(0x00175E00, saveName);

        std::uniform_int_distribution<u16> tidDist(0, 65535);
        std::uniform_int_distribution<u16> sidDist(0, 65535);
        std::uniform_int_distribution<u8> genderDist(0, 1);
        std::uniform_int_distribution<u16> dexSeenDist(150, 250);
        std::uniform_int_distribution<u16> hoursDist(0, 300);  // Limit to 300 hours
        std::uniform_int_distribution<u8> minutesDist(1, 59);  // At least 1 minute
        std::uniform_int_distribution<u8> secondsDist(0, 59);

        // Generate dexSeen first
        u16 dexSeen = dexSeenDist(seededRng);
        // Then generate dexCaught to be at most dexSeen
        std::uniform_int_distribution<u16> dexCaughtDist(100, dexSeen);
        u16 dexCaught = dexCaughtDist(seededRng);

        return std::make_shared<pksm::saves::SaveData>(
            saveName,
            pksm::saves::Generation::EIGHT,
            pksm::saves::GameVersion::PLA,
            generateRandomTrainerName(seededRng),
            tidDist(seededRng),
            sidDist(seededRng),
            genderDist(seededRng) ? pksm::saves::Gender::Male : pksm::saves::Gender::Female,
            0,  // No badges in PLA
            dexSeen,
            dexCaught,
            0,  // No Wonder Cards in PLA
            hoursDist(seededRng),
            minutesDist(seededRng),
            secondsDist(seededRng)
        );
    };

    // Scarlet
    mockSaveGenerators[0x00175F00] = [this](const std::string& saveName) -> pksm::saves::SaveData::Ref {
        // Create a seeded RNG for this title/save combination
        std::mt19937 seededRng = createSeededRNG(0x00175F00, saveName);

        std::uniform_int_distribution<u16> tidDist(0, 65535);
        std::uniform_int_distribution<u16> sidDist(0, 65535);
        std::uniform_int_distribution<u8> genderDist(0, 1);
        std::uniform_int_distribution<u8> badgesDist(0, 8);
        std::uniform_int_distribution<u16> dexSeenDist(300, 450);
        std::uniform_int_distribution<u16> wonderCardsDist(0, 15);
        std::uniform_int_distribution<u16> hoursDist(0, 300);  // Limit to 300 hours
        std::uniform_int_distribution<u8> minutesDist(1, 59);  // At least 1 minute
        std::uniform_int_distribution<u8> secondsDist(0, 59);

        // Generate dexSeen first
        u16 dexSeen = dexSeenDist(seededRng);
        // Then generate dexCaught to be at most dexSeen
        std::uniform_int_distribution<u16> dexCaughtDist(200, dexSeen);
        u16 dexCaught = dexCaughtDist(seededRng);

        return std::make_shared<pksm::saves::SaveData>(
            saveName,
            pksm::saves::Generation::NINE,
            pksm::saves::GameVersion::SL,
            generateRandomTrainerName(seededRng),
            tidDist(seededRng),
            sidDist(seededRng),
            genderDist(seededRng) ? pksm::saves::Gender::Male : pksm::saves::Gender::Female,
            badgesDist(seededRng),
            dexSeen,
            dexCaught,
            wonderCardsDist(seededRng),
            hoursDist(seededRng),
            minutesDist(seededRng),
            secondsDist(seededRng)
        );
    };

    // Violet
    mockSaveGenerators[0x00164800] = [this](const std::string& saveName) -> pksm::saves::SaveData::Ref {
        // Create a seeded RNG for this title/save combination
        std::mt19937 seededRng = createSeededRNG(0x00164800, saveName);

        std::uniform_int_distribution<u16> tidDist(0, 65535);
        std::uniform_int_distribution<u16> sidDist(0, 65535);
        std::uniform_int_distribution<u8> genderDist(0, 1);
        std::uniform_int_distribution<u8> badgesDist(0, 8);
        std::uniform_int_distribution<u16> dexSeenDist(300, 450);
        std::uniform_int_distribution<u16> wonderCardsDist(0, 15);
        std::uniform_int_distribution<u16> hoursDist(0, 300);  // Limit to 300 hours
        std::uniform_int_distribution<u8> minutesDist(1, 59);  // At least 1 minute
        std::uniform_int_distribution<u8> secondsDist(0, 59);

        // Generate dexSeen first
        u16 dexSeen = dexSeenDist(seededRng);
        // Then generate dexCaught to be at most dexSeen
        std::uniform_int_distribution<u16> dexCaughtDist(200, dexSeen);
        u16 dexCaught = dexCaughtDist(seededRng);

        return std::make_shared<pksm::saves::SaveData>(
            saveName,
            pksm::saves::Generation::NINE,
            pksm::saves::GameVersion::VL,
            generateRandomTrainerName(seededRng),
            tidDist(seededRng),
            sidDist(seededRng),
            genderDist(seededRng) ? pksm::saves::Gender::Male : pksm::saves::Gender::Female,
            badgesDist(seededRng),
            dexSeen,
            dexCaught,
            wonderCardsDist(seededRng),
            hoursDist(seededRng),
            minutesDist(seededRng),
            secondsDist(seededRng)
        );
    };

    // Add more game-specific generators as needed...
}

std::string MockSaveDataAccessor::generateRandomTrainerName() const {
    std::uniform_int_distribution<size_t> dist(0, TRAINER_NAMES.size() - 1);
    return TRAINER_NAMES[dist(rng)];
}

std::string MockSaveDataAccessor::generateRandomTrainerName(std::mt19937& generator) const {
    std::uniform_int_distribution<size_t> dist(0, TRAINER_NAMES.size() - 1);
    return TRAINER_NAMES[dist(generator)];
}