#pragma once

#include <memory>
#include <optional>
#include <string>
#include <switch.h>
#include <vector>

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

    // Load a specific save file, with optional user ID for installed titles
    virtual bool LoadSave(
        const pksm::titles::Title::Ref& title,
        const std::string& saveName,
        const AccountUid* userId = nullptr
    ) = 0;
};