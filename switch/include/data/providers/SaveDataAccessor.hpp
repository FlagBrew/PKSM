#pragma once

#include <functional>
#include <memory>
#include <string>

#include "data/providers/interfaces/ISaveDataAccessor.hpp"
#include "data/providers/interfaces/ISaveDataProvider.hpp"
#include "data/saves/SaveData.hpp"
#include "data/titles/Title.hpp"

// Owns the single live PKSM-Core save. Sourcing the bytes (emulator files,
// console containers, Checkpoint backups) is the save provider's job; every
// read and write of the loaded save goes through this one owner, so there is
// never a second Sav for the same file to clobber it with.
class SaveDataAccessor : public ISaveDataAccessor {
private:
    ISaveDataProvider::Ref saveProvider;

    // The one live save and where it came from
    std::unique_ptr<::pksm::Sav> sav;
    std::string savePath;

    // UI-facing summary of `sav`
    pksm::saves::SaveData::Ref currentSave;

    std::function<void(pksm::saves::SaveData::Ref)> onSaveDataChanged;
    bool hasChanges = false;

    pksm::saves::SaveData::Ref BuildSaveData(const std::string& name) const;

public:
    explicit SaveDataAccessor(ISaveDataProvider::Ref saveProvider);
    PU_SMART_CTOR(SaveDataAccessor)

    // Borrowed view of the live save; ownership stays here
    ::pksm::Sav* currentSav() const { return sav.get(); }

    // ISaveDataAccessor interface implementation
    pksm::saves::SaveData::Ref getCurrentSaveData() const override;
    bool loadSave(const pksm::titles::Title::Ref& title, const std::string& saveName, const AccountUid* userId = nullptr)
        override;
    void setOnSaveDataChanged(std::function<void(pksm::saves::SaveData::Ref)> callback) override {
        onSaveDataChanged = callback;
    }
    bool saveChanges() override;
    bool hasUnsavedChanges() const override;
};
