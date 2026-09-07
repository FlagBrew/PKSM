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

    // Moves the item at fromSlot to toSlot, the slots between closing up behind it. Returns the
    // pouch as the save now holds it, or nothing when saveData is not the live save, the pouch
    // indexes by item (its slots never move), or a slot is past the pouch
    virtual std::optional<pksm::bag::Pouch>
    Move(const pksm::saves::SaveData::Ref& saveData, ::pksm::Sav::Pouch pouch, u16 fromSlot, u16 toSlot) = 0;

    // Rewrites the pouch's items in the given order, packed from its first slot. Returns the
    // pouch as the save now holds it, or nothing when saveData is not the live save or the
    // pouch indexes by item
    virtual std::optional<pksm::bag::Pouch>
    Sort(const pksm::saves::SaveData::Ref& saveData, ::pksm::Sav::Pouch pouch, pksm::bag::SortOrder order) = 0;

    // Sets the slot's marks where the format keeps them (BagData::keepsNewMark, keepsFavorite).
    // Returns the pouch as the save now holds it, or nothing when saveData is not the live save,
    // the format has no such marks, or slot is past the pouch
    virtual std::optional<pksm::bag::Pouch> SetMarks(
        const pksm::saves::SaveData::Ref& saveData,
        ::pksm::Sav::Pouch pouch,
        u16 slot,
        bool isNew,
        bool favorite
    ) = 0;

    // Registers the key item to the shortcut slot, replacing what it held, or to the first free
    // one when none is given (BagData::shortcuts). Returns the pouch as the save now holds it, or
    // nothing when saveData is not the live save, the game has no shortcut, the pouch is not the
    // key items, or no slot is free
    virtual std::optional<pksm::bag::Pouch> Register(
        const pksm::saves::SaveData::Ref& saveData,
        ::pksm::Sav::Pouch pouch,
        u16 slot,
        std::optional<u8> shortcut
    ) = 0;

    // Takes the key item off its shortcut slot; later slots close up, as the games keep them
    virtual std::optional<pksm::bag::Pouch>
    Unregister(const pksm::saves::SaveData::Ref& saveData, ::pksm::Sav::Pouch pouch, u16 slot) = 0;

    // The shortcut slots as the save holds them now
    virtual std::vector<pksm::bag::Shortcut> GetShortcuts(const pksm::saves::SaveData::Ref& saveData) const = 0;

    // Puts the pouch in one of its sortOptions and returns the bag as it now reads, since some
    // games keep one setting for every pouch; nothing when saveData is not the live save or the
    // pouch has no options
    virtual std::optional<pksm::bag::BagData>
    SetPouchSort(const pksm::saves::SaveData::Ref& saveData, ::pksm::Sav::Pouch pouch, size_t option) = 0;
};
