#pragma once

#include <functional>
#include <memory>
#include <string>

#include "data/providers/interfaces/ISaveDataAccessor.hpp"
#include "data/providers/interfaces/ISaveDataProvider.hpp"
#include "data/providers/interfaces/ISaveDataWriter.hpp"
#include "data/saves/SaveData.hpp"
#include "data/titles/Title.hpp"

// Owns the single live PKSM-Core save. Sourcing the bytes (emulator files,
// console containers, Checkpoint backups) is the save provider's job; every
// read and write of the loaded save goes through this one owner, so there is
// never a second Sav for the same file to clobber it with.
class SaveDataAccessor : public ISaveDataAccessor {
private:
    ISaveDataProvider::Ref saveProvider;
    ISaveDataWriter::Ref saveWriter;

    // The one live save and where it came from; title and user are kept so a
    // console save (unmounted after loading) can be remounted for write-back
    std::unique_ptr<::pksm::Sav> sav;
    std::string savePath;
    pksm::titles::Title::Ref saveTitle;
    AccountUid saveUserId{};
    bool hasSaveUserId = false;

    // UI-facing summary of `sav`
    pksm::saves::SaveData::Ref currentSave;

    std::function<void(pksm::saves::SaveData::Ref)> onSaveDataChanged;
    bool hasChanges = false;

    pksm::saves::SaveData::Ref BuildSaveData(const std::string& name) const;

public:
    SaveDataAccessor(ISaveDataProvider::Ref saveProvider, ISaveDataWriter::Ref saveWriter);
    PU_SMART_CTOR(SaveDataAccessor)

    // Borrowed view of the live save; ownership stays here
    ::pksm::Sav* currentSav() const { return sav.get(); }

    // Editors call this after mutating the Sav so saveChanges knows there is work
    void markDirty() { hasChanges = true; }

    // ISaveDataAccessor interface implementation
    pksm::saves::SaveData::Ref getCurrentSaveData() const override;
    bool loadSave(const pksm::titles::Title::Ref& title, const std::string& saveName, const AccountUid* userId = nullptr)
        override;
    void setOnSaveDataChanged(std::function<void(pksm::saves::SaveData::Ref)> callback) override {
        onSaveDataChanged = callback;
    }
    bool saveChanges() override;
    void unloadSave() override;
    bool hasUnsavedChanges() const override;
};
