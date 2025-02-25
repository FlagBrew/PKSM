#pragma once

#include <functional>
#include <memory>
#include <string>

#include "data/saves/SaveData.hpp"

class ISaveDataAccessor {
public:
    PU_SMART_CTOR(ISaveDataAccessor)

    virtual ~ISaveDataAccessor() = default;

    // Get the current save data
    virtual pksm::saves::SaveData::Ref getCurrentSaveData() const = 0;

    // Set a callback for when the save data changes
    virtual void setOnSaveDataChanged(std::function<void(pksm::saves::SaveData::Ref)> callback) = 0;

    // Save any changes to the current save
    virtual bool saveChanges() = 0;

    // Check if there are unsaved changes
    virtual bool hasUnsavedChanges() const = 0;
};