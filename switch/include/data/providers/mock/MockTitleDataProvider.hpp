#pragma once

#include "data/providers/interfaces/ITitleDataProvider.hpp"

class MockTitleDataProvider : public ITitleDataProvider {
private:
    pksm::titles::Title::Ref mockCartridgeTitle;
    std::vector<pksm::titles::Title::Ref> mockInstalledTitles;

public:
    MockTitleDataProvider();

    pksm::titles::Title::Ref GetGameCardTitle() const override;
    std::vector<pksm::titles::Title::Ref> GetInstalledTitles() const override;
};