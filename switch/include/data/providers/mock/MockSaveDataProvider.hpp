#pragma once

#include <optional>
#include <pu/Plutonium>
#include <unordered_map>
#include <vector>

#include "data/providers/interfaces/ISaveDataProvider.hpp"
#include "data/saves/Save.hpp"

class MockSaveDataProvider : public ISaveDataProvider {
private:
    std::unordered_map<u64, std::vector<pksm::saves::Save::Ref>> mockConsoleSaves;
    std::unordered_map<u64, std::vector<pksm::saves::Save::Ref>> mockEmulatorSaves;
    std::unordered_map<u64, std::vector<pksm::saves::Save::Ref>> mockCustomSaves;
    AccountUid initialUserId;

    // Helper to create user-specific save names
    std::vector<pksm::saves::Save::Ref>
    GetUserSpecificSaves(const std::vector<pksm::saves::Save::Ref>& baseSaves, bool isOtherUser) const;

public:
    explicit MockSaveDataProvider(const AccountUid& initialUserId);

    std::vector<pksm::saves::Save::Ref> GetSavesForTitle(
        const pksm::titles::Title::Ref& title,
        const std::optional<AccountUid>& currentUser = std::nullopt
    ) const override;

    bool LoadSave(const pksm::titles::Title::Ref& title, const std::string& saveName, const AccountUid* userId = nullptr)
        override;
};