#pragma once

#include "data/providers/CustomTitleProvider.hpp"
#include "data/providers/interfaces/ITitleDataProvider.hpp"

class MockTitleDataProvider : public ITitleDataProvider {
private:
    pksm::titles::Title::Ref mockCartridgeTitle;
    std::vector<pksm::titles::Title::Ref> mockInstalledTitles;
    std::vector<pksm::titles::Title::Ref> mockEmulatorTitles;
    CustomTitleProvider::Ref customTitleProvider;

public:
    MockTitleDataProvider();

    pksm::titles::Title::Ref GetGameCardTitle() const override;
    std::vector<pksm::titles::Title::Ref> GetInstalledTitles() const override;
    std::vector<pksm::titles::Title::Ref> GetEmulatorTitles() const override;
    std::vector<pksm::titles::Title::Ref> GetCustomTitles() const override;
};