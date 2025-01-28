#pragma once

#include <memory>
#include <vector>
#include "titles/Title.hpp"

class ITitleDataProvider {
public:
    virtual ~ITitleDataProvider() = default;
    
    // Get the currently inserted game card title, or nullptr if none
    virtual titles::TitleRef GetGameCardTitle() const = 0;
    
    // Get list of installed titles
    virtual std::vector<titles::TitleRef> GetInstalledTitles() const = 0;
}; 