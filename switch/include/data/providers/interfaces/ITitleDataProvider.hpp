#pragma once

#include <memory>
#include <switch.h>
#include <vector>

#include "data/titles/Title.hpp"

class ITitleDataProvider {
public:
    PU_SMART_CTOR(ITitleDataProvider)
    virtual ~ITitleDataProvider() = default;

    // Get the currently inserted game card title, or nullptr if none
    virtual pksm::titles::Title::Ref GetGameCardTitle() const = 0;

    // Get list of installed titles for a specific user
    virtual std::vector<pksm::titles::Title::Ref> GetInstalledTitles(const AccountUid& userId) const = 0;

    // Get list of emulator titles
    virtual std::vector<pksm::titles::Title::Ref> GetEmulatorTitles() const = 0;

    // Get list of custom titles
    virtual std::vector<pksm::titles::Title::Ref> GetCustomTitles() const = 0;
};