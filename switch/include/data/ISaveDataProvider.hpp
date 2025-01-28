#pragma once

#include <memory>
#include <string>
#include <vector>
#include "titles/Title.hpp"

class ISaveDataProvider {
public:
    virtual ~ISaveDataProvider() = default;
    
    // Get list of available saves for a given title
    virtual std::vector<std::string> GetSavesForTitle(const titles::TitleRef& title) const = 0;
    
    // Load a specific save file
    virtual bool LoadSave(const titles::TitleRef& title, const std::string& saveName) = 0;
}; 