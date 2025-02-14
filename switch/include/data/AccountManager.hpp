// AccountManager.hpp
#pragma once

#include <SDL2/SDL.h>
#include <functional>
#include <pu/Plutonium>
#include <string>
#include <switch.h>

#include "pu/pu_Include.hpp"

namespace pksm::data {

class AccountManager {
public:
    using OnAccountSelectedCallback = std::function<void(AccountUid)>;

    AccountManager();
    ~AccountManager();

    Result Initialize();
    void Exit();

    AccountUid GetCurrentAccount() const { return currentAccount; }
    std::string GetAccountUsername() const;
    pu::sdl2::TextureHandle::Ref GetAccountIcon() const { return currentIcon; }

    void ShowAccountSelector();
    void SetOnAccountSelected(OnAccountSelectedCallback callback) { onAccountSelectedCallback = callback; }

private:
    AccountUid currentAccount{};
    OnAccountSelectedCallback onAccountSelectedCallback;
    pu::sdl2::TextureHandle::Ref currentIcon;

    void LoadCurrentAccountIcon();
};

}  // namespace pksm::data