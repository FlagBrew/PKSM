#pragma once

#include <optional>
#include <string>
#include <switch.h>
#include <unordered_map>
#include <vector>

#include "data/emulator/EmulatorGameCatalog.hpp"
#include "data/saves/LoadedSave.hpp"
#include "data/saves/Save.hpp"

// Saves in the NSO emulator apps' own console containers, keyed by catalog
// game. UI thread only: listing and resolving mount the shared "save" device.
class NsoContainerSource {
public:
    // The catalog outlives this source; matching scanned files needs it
    explicit NsoContainerSource(const std::vector<pksm::data::emulator::EmulatorGameEntry>& games) : games(games) {}

    // Cached saves for the title; scans once per account
    std::vector<pksm::saves::Save::Ref> List(u64 titleId, const std::optional<AccountUid>& user);

    // Parse an nsosave: path and mount its container for reading
    std::optional<pksm::saves::PendingLoad> Resolve(const std::string& nsoPath, const AccountUid& userId);

private:
    void Scan(const AccountUid& userId);

    const std::vector<pksm::data::emulator::EmulatorGameEntry>& games;
    std::optional<AccountUid> scanUser;
    std::unordered_map<u64, std::vector<pksm::saves::Save::Ref>> saves;
};
