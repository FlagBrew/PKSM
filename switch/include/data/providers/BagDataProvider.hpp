#pragma once

#include "data/providers/SaveDataAccessor.hpp"
#include "data/providers/interfaces/IBagDataProvider.hpp"

// Reads bag contents off the accessor's single owned Sav
class BagDataProvider : public IBagDataProvider {
private:
    SaveDataAccessor::Ref saveDataAccessor;

public:
    explicit BagDataProvider(SaveDataAccessor::Ref saveDataAccessor);
    PU_SMART_CTOR(BagDataProvider)

    pksm::bag::BagData GetBag(const pksm::saves::SaveData::Ref& saveData) const override;
    std::optional<pksm::bag::Pouch>
    SetCount(const pksm::saves::SaveData::Ref& saveData, ::pksm::Sav::Pouch pouch, u16 slot, u16 count) override;
    std::vector<pksm::bag::Slot>
    GetAddable(const pksm::saves::SaveData::Ref& saveData, ::pksm::Sav::Pouch pouch) const override;
    std::optional<pksm::bag::Pouch>
    Add(const pksm::saves::SaveData::Ref& saveData, ::pksm::Sav::Pouch pouch, u16 itemId, u16 count) override;
    std::optional<pksm::bag::Pouch>
    Move(const pksm::saves::SaveData::Ref& saveData, ::pksm::Sav::Pouch pouch, u16 fromSlot, u16 toSlot) override;
    std::optional<pksm::bag::Pouch>
    Sort(const pksm::saves::SaveData::Ref& saveData, ::pksm::Sav::Pouch pouch, pksm::bag::SortOrder order) override;
    std::optional<pksm::bag::Pouch>
    SetMarks(const pksm::saves::SaveData::Ref& saveData, ::pksm::Sav::Pouch pouch, u16 slot, bool isNew, bool favorite)
        override;
    std::optional<pksm::bag::Pouch> Register(
        const pksm::saves::SaveData::Ref& saveData,
        ::pksm::Sav::Pouch pouch,
        u16 slot,
        std::optional<u8> shortcut
    ) override;
    std::optional<pksm::bag::Pouch>
    Unregister(const pksm::saves::SaveData::Ref& saveData, ::pksm::Sav::Pouch pouch, u16 slot) override;
    std::vector<pksm::bag::Shortcut> GetShortcuts(const pksm::saves::SaveData::Ref& saveData) const override;
    std::optional<pksm::bag::BagData>
    SetPouchSort(const pksm::saves::SaveData::Ref& saveData, ::pksm::Sav::Pouch pouch, size_t option) override;
};
