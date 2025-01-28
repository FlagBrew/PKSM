#pragma once

#include "data/ITitleDataProvider.hpp"

class MockTitleDataProvider : public ITitleDataProvider {
private:
    titles::TitleRef mockCartridgeTitle;
    std::vector<titles::TitleRef> mockInstalledTitles;

public:
    MockTitleDataProvider();
    
    titles::TitleRef GetGameCardTitle() const override;
    std::vector<titles::TitleRef> GetInstalledTitles() const override;
}; 