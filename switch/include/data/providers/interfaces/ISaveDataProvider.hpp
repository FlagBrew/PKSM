#pragma once

#include <memory>
#include <string>
#include <vector>

#include "data/saves/Save.hpp"
#include "data/titles/Title.hpp"

class ISaveDataProvider {
public:
    PU_SMART_CTOR(ISaveDataProvider)
    virtual ~ISaveDataProvider() = default;

    // Get list of available saves for a given title
    virtual std::vector<pksm::saves::Save::Ref> GetSavesForTitle(const pksm::titles::Title::Ref& title) const = 0;

    // Load a specific save file
    virtual bool LoadSave(const pksm::titles::Title::Ref& title, const std::string& saveName) = 0;
};