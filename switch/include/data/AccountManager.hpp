// AccountManager.hpp
#pragma once

#include <SDL2/SDL.h>
#include <functional>
#include <memory>
#include <mutex>
#include <pu/Plutonium>
#include <queue>
#include <string>
#include <switch.h>

#include "pu/pu_Include.hpp"

namespace pksm::data {

struct AccountUpdate;

class AccountManager {
public:
    using AccountSelectedCallback = std::function<void(AccountUid)>;

    AccountManager();
    ~AccountManager();

    Result Initialize();
    void Exit();

    void ShowAccountSelector();
    void ProcessPendingUpdates();

    std::string GetAccountUsername() const;
    std::shared_ptr<pu::sdl2::TextureHandle> GetCurrentAccountIcon() const { return currentIcon; }
    AccountUid GetCurrentAccount() const { return currentAccount; }
    void SetOnAccountSelected(AccountSelectedCallback callback) { onAccountSelectedCallback = callback; }

private:
    AccountUid currentAccount{};
    std::shared_ptr<pu::sdl2::TextureHandle> currentIcon;
    AccountSelectedCallback onAccountSelectedCallback;

    std::queue<AccountUpdate> pendingUpdates;
    std::mutex updateMutex;
    SDL_TimerID selectorTimer{0};
    bool isShowingSelector{false};

    void LoadCurrentAccountIcon();
    void ShowAccountSelectorImpl();
};

}  // namespace pksm::data