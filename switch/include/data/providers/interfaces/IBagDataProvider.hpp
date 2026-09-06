#pragma once

#include <optional>
#include <pu/Plutonium>
#include <vector>

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

    // The pouch's item list minus what it holds, in the game's order, as rows without a count;
    // empty when saveData is not the live save
    virtual std::vector<pksm::bag::Slot>
    GetAddable(const pksm::saves::SaveData::Ref& saveData, ::pksm::Sav::Pouch pouch) const = 0;

    // Puts count of the item (format-native id) into the pouch: its own slot where the pouch
    // indexes by item, else the first free one. Returns the pouch as the save now holds it, or
    // nothing when the item is not the pouch's, already held, or the pouch is full
    virtual std::optional<pksm::bag::Pouch>
    Add(const pksm::saves::SaveData::Ref& saveData, ::pksm::Sav::Pouch pouch, u16 itemId, u16 count) = 0;
};
