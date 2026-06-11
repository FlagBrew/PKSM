#pragma once

#include <memory>
#include <vector>

#include "data/titles/Title.hpp"

class CustomTitleProvider {
public:
    PU_SMART_CTOR(CustomTitleProvider);
    CustomTitleProvider();
    virtual ~CustomTitleProvider() = default;

    // Get list of custom titles
    virtual std::vector<pksm::titles::Title::Ref> GetCustomTitles() const;

private:
    std::vector<pksm::titles::Title::Ref> customTitles;
};