#include "data/providers/SwitchTitleDataProvider.hpp"

#include <algorithm>
#include <cstring>
#include <memory>
#include <sstream>
#include <switch.h>

#include "utils/Logger.hpp"

// Helper to check if a title ID belongs to a Pokémon game
bool SwitchTitleDataProvider::IsPokemonTitle(u64 titleId) const {
    return knownTitleNames.find(titleId) != knownTitleNames.end();
}

// Get a readable name for a title
std::string SwitchTitleDataProvider::GetTitleName(u64 titleId, NacpLanguageEntry* languageEntry) const {
    if (languageEntry && strlen(languageEntry->name) > 0) {
        return std::string(languageEntry->name);
    }

    // Check if we have a known name for this title
    auto it = knownTitleNames.find(titleId);
    if (it != knownTitleNames.end()) {
        return it->second;
    }

    // Fallback to title ID as hex
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "0x%016lX", titleId);
    return std::string(buffer);
}

// Load title icon into a texture
SDL_Texture* SwitchTitleDataProvider::LoadTitleIcon(NsApplicationControlData* nsacd, size_t iconSize) const {
    if (!nsacd || iconSize == 0) {
        // Return a default icon texture
        return pu::ui::render::LoadImage(FALLBACK_TITLE_ICON_PATH);
    }

    // Decode the JPEG directly from memory (no SD round-trip)
    SDL_Surface* surface = IMG_Load_RW(SDL_RWFromMem(nsacd->icon, iconSize), 1);
    if (!surface) {
        LOG_ERROR("Failed to decode title icon, using default");
        return pu::ui::render::LoadImage(FALLBACK_TITLE_ICON_PATH);
    }

    SDL_Texture* texture = pu::ui::render::ConvertToTexture(surface);
    if (!texture) {
        LOG_ERROR("Failed to convert title icon to texture, using default");
        return pu::ui::render::LoadImage(FALLBACK_TITLE_ICON_PATH);
    }

    return texture;
}

SwitchTitleDataProvider::SwitchTitleDataProvider(ISaveDataProvider::Ref saveDataProvider)
  : saveDataProvider(std::move(saveDataProvider)),
    gameCardTitle(nullptr),
    customTitleProvider(CustomTitleProvider::New()) {
    // Known Pokémon titles; doubles as the IsPokemonTitle filter list
    knownTitleNames = {
        {0x010003F003A34000, "Pokémon: Let's Go, Pikachu!"},
        {0x0100187003A36000, "Pokémon: Let's Go, Eevee!"},
        {0x0100ABF008968000, "Pokémon Sword"},
        {0x01008DB008C2C000, "Pokémon Shield"},
        {0x0100000011D90000, "Pokémon Brilliant Diamond"},
        {0x010018E011D92000, "Pokémon Shining Pearl"},
        {0x01001F5010DFA000, "Pokémon Legends: Arceus"},
        {0x0100A3D008C5C000, "Pokémon Scarlet"},
        {0x01008F6008C5E000, "Pokémon Violet"},
        {0x0100F43008C44000, "Pokémon Legends: Z-A"}
    };

    // Initialize emulator titles (these would typically be loaded from a config file)
    // For now, we'll keep them empty - they'll need to be discovered through a different mechanism
    emulatorTitles = {};

    // Bring up ns once for the provider's lifetime (icons + game card metadata)
    Result rc = nsInitialize();
    nsAvailable = R_SUCCEEDED(rc);
    if (!nsAvailable) {
        std::stringstream ss;
        ss << "Failed to initialize NS service: 0x" << std::hex << rc;
        LOG_ERROR(ss.str());
    }

    // Refresh game card and installed titles
    RefreshGameCardTitle();
}

SwitchTitleDataProvider::~SwitchTitleDataProvider() {
    if (nsAvailable) {
        nsExit();
    }
}

pksm::titles::Title::Ref SwitchTitleDataProvider::GetGameCardTitle() const {
    return gameCardTitle;
}

std::vector<pksm::titles::Title::Ref> SwitchTitleDataProvider::GetInstalledTitles(const AccountUid& userId) const {
    auto it = installedTitleCache.find(userId);
    if (it != installedTitleCache.end()) {
        return it->second;
    }

    // Not in cache, so refresh
    LOG_INFO("Titles not in cache, refreshing for user");
    RefreshInstalledTitles(userId);

    // Now check cache again
    it = installedTitleCache.find(userId);
    if (it != installedTitleCache.end()) {
        return it->second;
    }

    // Return empty vector if not found
    return {};
}

std::vector<pksm::titles::Title::Ref> SwitchTitleDataProvider::GetEmulatorTitles() const {
    return emulatorTitles;
}

std::vector<pksm::titles::Title::Ref> SwitchTitleDataProvider::GetCustomTitles() const {
    return customTitleProvider->GetCustomTitles();
}

void SwitchTitleDataProvider::RefreshGameCardTitle() {
    // Clear existing game card title
    gameCardTitle = nullptr;

    if (!nsAvailable) {
        LOG_WARNING("NS service unavailable, skipping game card detection");
        return;
    }

    // Check if a game card is inserted
    bool inserted;
    Result res = nsIsGameCardInserted(&inserted);
    if (R_FAILED(res) || !inserted) {
        LOG_INFO("No game card inserted");
        return;
    }

    // Get game card control data
    NsApplicationControlData* nsacd = (NsApplicationControlData*)malloc(sizeof(NsApplicationControlData));
    if (!nsacd) {
        LOG_ERROR("Failed to allocate memory for control data");
        return;
    }

    memset(nsacd, 0, sizeof(NsApplicationControlData));

    // Get the application ID of the game card
    u64 titleId = 0;
    s32 totalApplications = 0;
    res = nsListApplicationIdOnGameCard(&titleId, 1, &totalApplications);
    if (R_FAILED(res) || totalApplications == 0) {
        LOG_ERROR("Failed to get game card application ID");
        free(nsacd);
        return;
    }

    // Check if this is a Pokémon game
    if (!IsPokemonTitle(titleId)) {
        std::stringstream ss;
        ss << "Game card is not a Pokémon game (Title ID: 0x" << std::hex << titleId << ")";
        LOG_INFO(ss.str());
        free(nsacd);
        return;
    }

    // Get control data for the game card
    size_t outsize = 0;
    res = nsGetApplicationControlData(
        NsApplicationControlSource_Storage,
        titleId,
        nsacd,
        sizeof(NsApplicationControlData),
        &outsize
    );
    if (R_FAILED(res) || outsize < sizeof(nsacd->nacp)) {
        LOG_ERROR("Failed to get game card control data");
        free(nsacd);
        return;
    }

    // Get language entry
    NacpLanguageEntry* languageEntry = nullptr;
    res = nacpGetLanguageEntry(&nsacd->nacp, &languageEntry);
    if (R_FAILED(res) || !languageEntry) {
        LOG_ERROR("Failed to get language entry");
        free(nsacd);
        return;
    }

    // Get title name
    std::string titleName = GetTitleName(titleId, languageEntry);

    // Load icon
    SDL_Texture* iconTexture = LoadTitleIcon(nsacd, outsize - sizeof(nsacd->nacp));
    if (!iconTexture) {
        LOG_ERROR("Failed to load game card icon texture");
        free(nsacd);
        return;
    }

    // Create title object using the texture directly
    gameCardTitle = std::make_shared<pksm::titles::Title>(titleName, iconTexture, titleId);

    // Clean up
    free(nsacd);

    std::stringstream ss;
    ss << "Game card title loaded: " << titleName << " (0x" << std::hex << titleId << ")";
    LOG_INFO(ss.str());
}

void SwitchTitleDataProvider::RefreshInstalledTitles(const AccountUid& userId) const {
    // Clear existing titles for this user
    installedTitleCache.erase(userId);
    std::vector<pksm::titles::Title::Ref> userTitles;

    LOG_INFO("REFRESH TITLES");

    // Open save data info reader
    FsSaveDataInfoReader reader;
    Result res = fsOpenSaveDataInfoReader(&reader, FsSaveDataSpaceId_User);
    if (R_FAILED(res)) {
        LOG_ERROR("Failed to open save data info reader");
        return;
    }

    // Read save data info
    FsSaveDataInfo info;
    s64 total = 0;

    // Allocate memory for application control data
    NsApplicationControlData* nsacd = (NsApplicationControlData*)malloc(sizeof(NsApplicationControlData));
    if (!nsacd) {
        LOG_ERROR("Failed to allocate memory for application control data");
        fsSaveDataInfoReaderClose(&reader);
        return;
    }

    // Read save data info entries
    while (true) {
        res = fsSaveDataInfoReaderRead(&reader, &info, 1, &total);
        if (R_FAILED(res) || total == 0) {
            break;
        }

        // Check if this is account-specific save data
        if (info.save_data_type == FsSaveDataType_Account && info.uid.uid[0] == userId.uid[0] &&
            info.uid.uid[1] == userId.uid[1]) {
            u64 titleId = info.application_id;

            // Check if this is a Pokémon title with an actual save
            if (IsPokemonTitle(titleId)) {
                if (!saveDataProvider->HasConsoleSaveData(titleId, userId)) {
                    std::stringstream ss;
                    ss << "Skipping title 0x" << std::hex << titleId << ": empty save container";
                    LOG_INFO(ss.str());
                    continue;
                }
                // Get application control data; fall back to known name +
                // default icon if metadata is unavailable, rather than
                // silently dropping the title
                size_t outsize = 0;
                NsApplicationControlData* controlData = nullptr;
                NacpLanguageEntry* languageEntry = nullptr;
                memset(nsacd, 0, sizeof(NsApplicationControlData));

                if (nsAvailable) {
                    res = nsGetApplicationControlData(
                        NsApplicationControlSource_Storage,
                        titleId,
                        nsacd,
                        sizeof(NsApplicationControlData),
                        &outsize
                    );
                    if (R_SUCCEEDED(res) && outsize >= sizeof(nsacd->nacp)) {
                        controlData = nsacd;
                        if (R_FAILED(nacpGetLanguageEntry(&nsacd->nacp, &languageEntry))) {
                            languageEntry = nullptr;
                        }
                    } else {
                        std::stringstream ss;
                        ss << "No control data for title 0x" << std::hex << titleId << " (rc 0x" << res
                           << "), using fallbacks";
                        LOG_WARNING(ss.str());
                    }
                }

                std::string titleName = GetTitleName(titleId, languageEntry);
                SDL_Texture* iconTexture =
                    LoadTitleIcon(controlData, controlData ? outsize - sizeof(nsacd->nacp) : 0);
                auto title = std::make_shared<pksm::titles::Title>(titleName, iconTexture, titleId);
                userTitles.push_back(title);

                std::stringstream ss;
                ss << "Installed title found: " << titleName << " (0x" << std::hex << titleId << ")";
                LOG_INFO(ss.str());
            }
        }
    }

    // Cache the results
    installedTitleCache[userId] = userTitles;

    // Clean up
    free(nsacd);
    fsSaveDataInfoReaderClose(&reader);

    std::stringstream ss;
    ss << "Found " << userTitles.size() << " installed Pokémon titles for user";
    LOG_INFO(ss.str());
}

void SwitchTitleDataProvider::RefreshEmulatorTitles() {
    // For now, we'll need to implement this differently
    // Emulator titles would need to be discovered through a directory scan
    // This would typically look at known emulator save directories

    // For example, we could scan directories like:
    // - /switch/retroarch/saves
    // - /switch/desmume
    // - /switch/melonds
    // etc.

    // For now, we'll leave this empty as it requires knowledge of specific emulator paths
    emulatorTitles = {};
}