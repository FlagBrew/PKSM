#include "data/AccountManager.hpp"

#include <mutex>
#include <queue>

#include "utils/Logger.hpp"

namespace pksm::data {

struct AccountUpdate {
    AccountUid newAccount;
    bool needsIconReload;
};

AccountManager::AccountManager() : pendingUpdates(), updateMutex() {}

AccountManager::~AccountManager() {
    Exit();
}

Result AccountManager::Initialize() {
    Result res = accountInitialize(AccountServiceType_Administrator);
    if (R_SUCCEEDED(res)) {
        // Get initial account if available
        s32 accountCount;
        AccountUid accounts[ACC_USER_LIST_SIZE];
        res = accountListAllUsers(accounts, ACC_USER_LIST_SIZE, &accountCount);
        if (R_SUCCEEDED(res) && accountCount > 0) {
            currentAccount = accounts[0];
            LoadCurrentAccountIcon();
        }
    }
    return res;
}

void AccountManager::Exit() {
    currentIcon.reset();
    accountExit();
}

std::string AccountManager::GetAccountUsername() const {
    std::string username;
    AccountProfile profile;
    AccountProfileBase profileBase;

    if (R_SUCCEEDED(accountGetProfile(&profile, currentAccount)) &&
        R_SUCCEEDED(accountProfileGet(&profile, nullptr, &profileBase))) {
        username = std::string(profileBase.nickname);
    }
    accountProfileClose(&profile);
    return username;
}

void AccountManager::LoadCurrentAccountIcon() {
    AccountProfile profile;
    if (R_SUCCEEDED(accountGetProfile(&profile, currentAccount))) {
        u32 imageSize;
        if (R_SUCCEEDED(accountProfileGetImageSize(&profile, &imageSize))) {
            std::vector<u8> buffer(imageSize);
            u32 realSize;
            if (R_SUCCEEDED(accountProfileLoadImage(&profile, buffer.data(), imageSize, &realSize))) {
                // Create SDL_Surface from the raw image data
                SDL_Surface* surface = IMG_Load_RW(SDL_RWFromMem(buffer.data(), realSize), 1);
                if (surface) {
                    // Set transparency color key (black)
                    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0, 0));
                    // Convert surface to Plutonium texture
                    currentIcon = std::make_shared<pu::sdl2::TextureHandle>(pu::ui::render::ConvertToTexture(surface));
                }
            }
        }
        accountProfileClose(&profile);
    }
}

void AccountManager::ProcessPendingUpdates() {
    std::lock_guard<std::mutex> lock(updateMutex);
    while (!pendingUpdates.empty()) {
        const auto& update = pendingUpdates.front();
        if (update.needsIconReload) {
            LoadCurrentAccountIcon();
        }
        if (onAccountSelectedCallback) {
            onAccountSelectedCallback(update.newAccount);
        }
        pendingUpdates.pop();
    }
}

void AccountManager::ShowAccountSelector() {
    LibAppletArgs args;
    libappletArgsCreate(&args, 0x10000);
    u8 st_in[0xA0] = {0};
    u8 st_out[0x18] = {0};
    size_t repsz;

    if (R_SUCCEEDED(libappletLaunch(AppletId_LibraryAppletPlayerSelect, &args, st_in, 0xA0, st_out, 0x18, &repsz))) {
        u64 res = *(u64*)st_out;
        if (res == 0) {
            AccountUid newAccount = *(AccountUid*)&st_out[8];
            if (newAccount.uid[0] != currentAccount.uid[0] || newAccount.uid[1] != currentAccount.uid[1]) {
                currentAccount = newAccount;
                // Queue the update instead of processing immediately
                std::lock_guard<std::mutex> lock(updateMutex);
                pendingUpdates.push({newAccount, true});
            }
        }
    }
}

}  // namespace pksm::data