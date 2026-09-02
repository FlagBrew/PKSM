#pragma once

#include <pu/Plutonium>

#include "data/bag/BagData.hpp"
#include "data/saves/SaveData.hpp"

class IBagDataProvider {
public:
    PU_SMART_CTOR(IBagDataProvider)
    virtual ~IBagDataProvider() = default;

    // Every pouch with the items it holds; empty when saveData is not the live save
    virtual pksm::bag::BagData GetBag(const pksm::saves::SaveData::Ref& saveData) const = 0;
};
