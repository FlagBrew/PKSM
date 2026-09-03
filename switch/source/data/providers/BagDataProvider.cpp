#include "data/providers/BagDataProvider.hpp"

#include "sav/Item.hpp"
#include "sav/SavZA.hpp"
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

// Quality and level boost, the two things a donut is chosen for
std::string DonutDetail(const ::pksm::SavZA::Donut& donut) {
    return "★" + std::to_string(donut.stars) + "  Lv +" + std::to_string(donut.levelBoost);
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
    // Legends Z-A keeps Mega Dimension donuts outside the item pouches
    if (sav->version() == ::pksm::GameVersion::ZA) {
        // No RTTI; core builds a SavZA for every ZA-version save, so the version settles the type
        const auto* za = static_cast<const ::pksm::SavZA*>(sav);
        if (za->hasDonuts()) {
            constexpr auto POUCH = ::pksm::Sav::Pouch::Donut;
            pksm::bag::Pouch view{POUCH, pksm::strings::PouchName(POUCH, bag.storageFormat), ::pksm::SavZA::DONUT_SLOTS, {}};
            for (const auto& donut : za->donuts()) {
                view.items.push_back({donut.id, 1, pksm::strings::DonutName(donut.id), DonutDetail(donut), donut.stars});
            }
            bag.pouches.push_back(std::move(view));
        }
    }
    return bag;
}
