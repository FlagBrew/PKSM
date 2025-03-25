#pragma once

#include <functional>
#include <memory>
#include <string>

#include "data/saves/SaveData.hpp"
#include "data/titles/Title.hpp"

class ISaveDataAccessor {
public:
    PU_SMART_CTOR(ISaveDataAccessor)

    virtual ~ISaveDataAccessor() = default;

    // Get the current save data
    virtual pksm::saves::SaveData::Ref getCurrentSaveData() const = 0;

    // Load a save data from a title and save name
    virtual bool loadSave(const pksm::titles::Title::Ref title, const std::string saveName) = 0;

    // Set a callback for when the save data changes
    virtual void setOnSaveDataChanged(std::function<void(pksm::saves::SaveData::Ref)> callback) = 0;

    // Save any changes to the current save
    virtual bool saveChanges() = 0;

    // Check if there are unsaved changes
    virtual bool hasUnsavedChanges() const = 0;
};