#pragma once

#include <map>

#include "data/ISaveDataProvider.hpp"
#include "saves/Save.hpp"

class MockSaveDataProvider : public ISaveDataProvider {
private:
    // Map of title ID to list of save names
    std::map<u64, std::vector<pksm::saves::Save::Ref>> mockSaves;

public:
    MockSaveDataProvider();

    std::vector<pksm::saves::Save::Ref> GetSavesForTitle(const pksm::titles::Title::Ref& title) const override;
    bool LoadSave(const pksm::titles::Title::Ref& title, const std::string& saveName) override;
};