#pragma once

#include <random>
#include <unordered_map>

#include "data/providers/CustomTitleProvider.hpp"
#include "data/providers/interfaces/ITitleDataProvider.hpp"

// Custom equality operator for AccountUid
inline bool operator==(const AccountUid& lhs, const AccountUid& rhs) {
    return lhs.uid[0] == rhs.uid[0] && lhs.uid[1] == rhs.uid[1];
}

// Custom hash function for AccountUid
struct AccountUidHash {
    size_t operator()(const AccountUid& uid) const {
        // Combine both u64 parts of the AccountUid using XOR
        return std::hash<u64>{}(uid.uid[0]) ^ (std::hash<u64>{}(uid.uid[1]) << 1);
    }
};

class MockTitleDataProvider : public ITitleDataProvider {
private:
    pksm::titles::Title::Ref mockCartridgeTitle;
    std::vector<pksm::titles::Title::Ref> mockInstalledTitles;
    std::vector<pksm::titles::Title::Ref> mockEmulatorTitles;
    CustomTitleProvider::Ref customTitleProvider;
    AccountUid initialUserId;
    mutable std::unordered_map<AccountUid, std::vector<pksm::titles::Title::Ref>, AccountUidHash> userSpecificTitles;
    mutable std::mt19937 rng;

    // Helper to generate random subset of titles for non-initial users
    std::vector<pksm::titles::Title::Ref> GenerateRandomTitleSubset(const std::vector<pksm::titles::Title::Ref>& source
    ) const;

public:
    explicit MockTitleDataProvider(const AccountUid& initialUserId);

    pksm::titles::Title::Ref GetGameCardTitle() const override;
    std::vector<pksm::titles::Title::Ref> GetInstalledTitles(const AccountUid& userId) const override;
    std::vector<pksm::titles::Title::Ref> GetEmulatorTitles() const override;
    std::vector<pksm::titles::Title::Ref> GetCustomTitles() const override;
};