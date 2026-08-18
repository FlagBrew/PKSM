#include "data/providers/SwitchTitleDataProvider.hpp"

#include <cstring>
#include <memory>
#include <sstream>
#include <switch.h>

#include "data/emulator/EmulatorGameCatalog.hpp"
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
    gameCardTitle(nullptr) {
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

    // Emulator and Custom tiles both come from the bundled catalog
    RefreshCatalogTitles();

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
    return customTitles;
}

bool SwitchTitleDataProvider::RefreshGameCardTitle() {
    if (!nsAvailable) {
        return false;
    }

    // Lightweight probe first: this runs on a poll, so only rebuild the
    // title (metadata fetch + texture upload) when the card actually changed
    u64 titleId = 0;
    bool inserted = false;
    if (R_SUCCEEDED(nsIsGameCardInserted(&inserted)) && inserted) {
        s32 totalApplications = 0;
        if (R_FAILED(nsListApplicationIdOnGameCard(&titleId, 1, &totalApplications)) || totalApplications == 0) {
            titleId = 0;
        }
    }

    if (titleId == currentCardId) {
        return false;
    }
    currentCardId = titleId;
    gameCardTitle = nullptr;

    if (titleId == 0) {
        LOG_INFO("No game card inserted");
        return true;
    }

    // Check if this is a Pokémon game
    if (!IsPokemonTitle(titleId)) {
        std::stringstream ss;
        ss << "Game card is not a Pokémon game (Title ID: 0x" << std::hex << titleId << ")";
        LOG_INFO(ss.str());
        return true;
    }

    // Get game card control data
    NsApplicationControlData* nsacd = (NsApplicationControlData*)malloc(sizeof(NsApplicationControlData));
    if (!nsacd) {
        LOG_ERROR("Failed to allocate memory for control data");
        return true;
    }

    memset(nsacd, 0, sizeof(NsApplicationControlData));
    Result res;

    // Get control data for the game card; fall back to the known name and
    // default icon on failure rather than dropping the card
    size_t outsize = 0;
    res = nsGetApplicationControlData(
        NsApplicationControlSource_Storage,
        titleId,
        nsacd,
        sizeof(NsApplicationControlData),
        &outsize
    );
    NsApplicationControlData* controlData = nullptr;
    NacpLanguageEntry* languageEntry = nullptr;
    if (R_SUCCEEDED(res) && outsize >= sizeof(nsacd->nacp)) {
        controlData = nsacd;
        if (R_FAILED(nacpGetLanguageEntry(&nsacd->nacp, &languageEntry))) {
            languageEntry = nullptr;
        }
    } else {
        LOG_WARNING("No control data for game card, using fallbacks");
    }

    std::string titleName = GetTitleName(titleId, languageEntry);
    SDL_Texture* iconTexture = LoadTitleIcon(controlData, controlData ? outsize - sizeof(nsacd->nacp) : 0);
    gameCardTitle = std::make_shared<pksm::titles::Title>(titleName, iconTexture, titleId);

    free(nsacd);

    std::stringstream ss;
    ss << "Game card title loaded: " << titleName << " (0x" << std::hex << titleId << ")";
    LOG_INFO(ss.str());
    return true;
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

void SwitchTitleDataProvider::RefreshCatalogTitles() {
    emulatorTitles.clear();
    customTitles.clear();

    // The save layer decides which catalog games have save candidates; the
    // title layer only turns those answers into tiles. A game with both
    // discovered and configured saves gets a tile on each tab, and each
    // tile's context routes it to its own save listing.
    const auto games = pksm::data::emulator::EmulatorGameCatalog::LoadFromDataJson();
    for (const auto& game : games) {
        if (saveDataProvider->HasDiscoveredEmulatorSaves(game.titleId)) {
            emulatorTitles.push_back(
                pksm::titles::Title::New(game.name, game.iconPath, game.titleId, pksm::titles::TitleContext::Emulator)
            );
        }
        if (saveDataProvider->HasConfiguredEmulatorSaves(game.titleId)) {
            customTitles.push_back(
                pksm::titles::Title::New(game.name, game.iconPath, game.titleId, pksm::titles::TitleContext::Custom)
            );
        }
    }

    std::stringstream ss;
    ss << "Catalog tiles: " << emulatorTitles.size() << " with discovered saves, " << customTitles.size()
       << " with configured saves";
    LOG_INFO(ss.str());
}