#pragma once

#include <optional>
#include <pu/Plutonium>

#include "data/bag/BagData.hpp"
#include "data/saves/SaveData.hpp"

class IBagDataProvider {
public:
    PU_SMART_CTOR(IBagDataProvider)
    virtual ~IBagDataProvider() = default;

    // Every pouch with the items it holds; empty when saveData is not the live save
    virtual pksm::bag::BagData GetBag(const pksm::saves::SaveData::Ref& saveData) const = 0;

    // Writes a slot's quantity into the live save, 0 removing the item (later rows close the
    // gap where the game keeps pouches contiguous). Returns the pouch as the save now holds it,
    // or nothing when saveData is not the live save or slot is past the pouch
    virtual std::optional<pksm::bag::Pouch> SetCount(
        const pksm::saves::SaveData::Ref& saveData,
        ::pksm::Sav::Pouch pouch,
        u16 slot,
        u16 count
    ) = 0;
};
