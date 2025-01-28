#pragma once

#include "data/ISaveDataProvider.hpp"
#include <map>

class MockSaveDataProvider : public ISaveDataProvider {
private:
    // Map of title ID to list of save names
    std::map<u64, std::vector<std::string>> mockSaves;

public:
    MockSaveDataProvider();
    
    std::vector<std::string> GetSavesForTitle(const titles::TitleRef& title) const override;
    bool LoadSave(const titles::TitleRef& title, const std::string& saveName) override;
}; 