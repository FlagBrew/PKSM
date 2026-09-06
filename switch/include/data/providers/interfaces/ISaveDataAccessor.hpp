#pragma once

#include <functional>
#include <memory>
#include <string>
#include <switch.h>

#include <optional>

#include "data/saves/LoadedSave.hpp"
#include "data/saves/SaveData.hpp"
#include "data/titles/Title.hpp"

class ISaveDataAccessor {
public:
    PU_SMART_CTOR(ISaveDataAccessor)

    virtual ~ISaveDataAccessor() = default;

    // Get the current save data
    virtual pksm::saves::SaveData::Ref getCurrentSaveData() const = 0;

    // Commit (or, on nullopt, clear) an externally-run load; UI thread only
    virtual bool applySaveLoadResult(
        const pksm::titles::Title::Ref& title,
        const std::string& saveName,
        const AccountUid* userId,
        std::optional<pksm::saves::LoadedSave> loaded
    ) = 0;

    // Set a callback for when the save data changes
    virtual void setOnSaveDataChanged(std::function<void(pksm::saves::SaveData::Ref)> callback) = 0;

    // Save any changes to the current save
    virtual bool saveChanges() = 0;

    // Release the loaded save (and any unsaved changes with it)
    virtual void unloadSave() = 0;

    // Check if there are unsaved changes
    virtual bool hasUnsavedChanges() const = 0;
};