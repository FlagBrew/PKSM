#include "data/providers/BagDataProvider.hpp"

#include "sav/Item.hpp"
#include "utils/CoreStrings.hpp"

namespace {

// Gen 1-3 items carry their own ids; id() would convert to the national index
u16 NativeItemId(const ::pksm::Item& item) {
    switch (item.generation()) {
        case ::pksm::Generation::ONE:
            return static_cast<const ::pksm::Item1&>(item).id1();
        case ::pksm::Generation::TWO:
            return static_cast<const ::pksm::Item2&>(item).id2();
        case ::pksm::Generation::THREE:
            return static_cast<const ::pksm::Item3&>(item).id3();
        default:
            return item.id();
    }
}

}  // namespace

BagDataProvider::BagDataProvider(SaveDataAccessor::Ref saveDataAccessor)
  : saveDataAccessor(std::move(saveDataAccessor)) {}

pksm::bag::BagData BagDataProvider::GetBag(const pksm::saves::SaveData::Ref& saveData) const {
    pksm::bag::BagData bag;
    ::pksm::Sav* sav = saveDataAccessor->savFor(saveData);
    if (!sav) {
        return bag;
    }
    bag.storageFormat = sav->generation();
    for (const auto& [pouch, capacity] : sav->pouches()) {
        pksm::bag::Pouch view{pouch, pksm::strings::PouchName(pouch, bag.storageFormat), static_cast<size_t>(capacity), {}};
        // Switch-era saves list every item a pouch can hold; owned ones carry a count
        for (int slot = 0; slot < capacity; slot++) {
            const auto item = sav->item(pouch, static_cast<u16>(slot));
            const u16 id = item ? NativeItemId(*item) : 0;
            if (id != 0 && item->count() > 0) {
                view.items.push_back({id, item->count(), pksm::strings::ItemName(id, bag.storageFormat)});
            }
        }
        bag.pouches.push_back(std::move(view));
    }
    return bag;
}
