#pragma once

#include <memory>
#include <optional>
#include <string>
#include <switch.h>
#include <vector>

#include "data/saves/LoadedSave.hpp"
#include "data/saves/Save.hpp"
#include "data/titles/Title.hpp"

class ISaveDataProvider {
public:
    PU_SMART_CTOR(ISaveDataProvider)
    virtual ~ISaveDataProvider() = default;

    // Get list of available saves for a given title and optional user
    virtual std::vector<pksm::saves::Save::Ref> GetSavesForTitle(
        const pksm::titles::Title::Ref& title,
        const std::optional<AccountUid>& currentUser = std::nullopt
    ) const = 0;

    // Load a specific save, with optional user ID for installed titles.
    // Resolves the name to its source (emulator file, console container,
    // Checkpoint backup), parses it with core, and hands the Sav over -
    // ownership of the loaded save lives in the save data accessor.
    virtual std::optional<pksm::saves::LoadedSave> LoadSave(
        const pksm::titles::Title::Ref& title,
        const std::string& saveName,
        const AccountUid* userId = nullptr
    ) = 0;

    // Whether a console save container for this title/user holds actual save
    // data. Containers are created the moment a profile launches a game, so an
    // existing-but-empty container must not count as a loadable save.
    virtual bool HasConsoleSaveData(u64 titleId, const AccountUid& userId) const {
        return true;
    }

    // Whether the save-location scan found any file for a catalog game, so
    // the title layer can decide which games earn an Emulator-tab tile
    // without owning save-location knowledge
    virtual bool HasDiscoveredEmulatorSaves(u64 titleId) const { return false; }

    // Whether the user's save config lists an existing file for a catalog
    // game; decides which games earn a Custom-tab tile
    virtual bool HasConfiguredEmulatorSaves(u64 titleId) const { return false; }
};