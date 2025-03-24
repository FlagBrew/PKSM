#pragma once

#include <functional>
#include <map>
#include <memory>
#include <random>
#include <string>

#include "data/providers/interfaces/ISaveDataAccessor.hpp"
#include "data/saves/SaveData.hpp"
#include "data/titles/Title.hpp"

class MockSaveDataAccessor : public ISaveDataAccessor {
private:
    pksm::saves::SaveData::Ref currentSave;
    std::function<void(pksm::saves::SaveData::Ref)> onSaveDataChanged;
    bool hasChanges;

    // Random number generator for mock data
    mutable std::mt19937 rng;

    // Map of title ID to save data generator functions
    // Each generator takes a save name and returns a SaveData
    std::map<u64, std::function<pksm::saves::SaveData::Ref(const std::string&)>> mockSaveGenerators;

    // Initialize the mock save generators
    void initializeMockSaveGenerators();

    // Generate a random trainer name
    std::string generateRandomTrainerName() const;

    // Generate a random trainer name with a specific RNG
    std::string generateRandomTrainerName(std::mt19937& generator) const;

    // Create a seeded random number generator based on title ID and save name
    std::mt19937 createSeededRNG(u64 titleId, const std::string& saveName) const;

public:
    MockSaveDataAccessor();
    virtual ~MockSaveDataAccessor() = default;

    // ISaveDataAccessor interface implementation
    pksm::saves::SaveData::Ref getCurrentSaveData() const override;
    void setOnSaveDataChanged(std::function<void(pksm::saves::SaveData::Ref)> callback) override {
        onSaveDataChanged = callback;
    }
    bool saveChanges() override;
    bool hasUnsavedChanges() const override;

    // Load a mock save for the given title and save name
    bool loadMockSave(const pksm::titles::Title::Ref& title, const std::string& saveName);
};