#pragma once

#include <random>
#include <unordered_map>

#include "data/providers/CustomTitleProvider.hpp"
#include "data/providers/interfaces/ISaveDataProvider.hpp"
#include "data/providers/interfaces/ITitleDataProvider.hpp"
#include "utils/AccountUtil.hpp"

class SwitchTitleDataProvider : public ITitleDataProvider {
private:
    // Placeholder shown when a title's icon can't be fetched or decoded
    static constexpr const char* FALLBACK_TITLE_ICON_PATH = "romfs:/gfx/fallback_title_icon.jpg";

    // Save layer; owns the "does this container hold save data" policy
    ISaveDataProvider::Ref saveDataProvider;

    // Cache for loaded titles
    pksm::titles::Title::Ref gameCardTitle;
    mutable std::unordered_map<AccountUid, std::vector<pksm::titles::Title::Ref>, AccountUidHash> installedTitleCache;
    std::vector<pksm::titles::Title::Ref> emulatorTitles;
    CustomTitleProvider::Ref customTitleProvider;

    // Map of known title IDs to their names
    std::unordered_map<u64, std::string> knownTitleNames;

    // Whether nsInitialize succeeded (held for the provider's lifetime)
    bool nsAvailable = false;

    // Application id of the currently probed game card (0 = none); lets
    // RefreshGameCardTitle detect hotplug without rebuilding textures
    u64 currentCardId = 0;

    // Helper methods
    SDL_Texture* LoadTitleIcon(NsApplicationControlData* nsacd, size_t iconSize) const;
    bool IsPokemonTitle(u64 titleId) const;
    std::string GetTitleName(u64 titleId, NacpLanguageEntry* languageEntry) const;

public:
    explicit SwitchTitleDataProvider(ISaveDataProvider::Ref saveDataProvider);
    ~SwitchTitleDataProvider() override;
    PU_SMART_CTOR(SwitchTitleDataProvider)

    // Implementation of ITitleDataProvider
    pksm::titles::Title::Ref GetGameCardTitle() const override;
    std::vector<pksm::titles::Title::Ref> GetInstalledTitles(const AccountUid& userId) const override;
    std::vector<pksm::titles::Title::Ref> GetEmulatorTitles() const override;
    std::vector<pksm::titles::Title::Ref> GetCustomTitles() const override;

    // Update methods to refresh data
    bool RefreshGameCardTitle() override;
    void RefreshInstalledTitles(const AccountUid& userId) const;
    void RefreshEmulatorTitles();
};