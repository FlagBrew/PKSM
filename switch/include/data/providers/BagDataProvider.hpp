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
};
