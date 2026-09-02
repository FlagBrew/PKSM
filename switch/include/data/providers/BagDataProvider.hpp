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
};
