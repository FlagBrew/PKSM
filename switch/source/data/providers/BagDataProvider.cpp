#include "data/providers/BagDataProvider.hpp"

#include <algorithm>
#include <unordered_set>

#include "sav/Item.hpp"
#include "sav/Sav1.hpp"
#include "sav/Sav2.hpp"
#include "sav/Sav3.hpp"
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

void SetNativeItemId(::pksm::Item& item, u16 id) {
    switch (item.generation()) {
        case ::pksm::Generation::ONE:
            static_cast<::pksm::Item1&>(item).id1(id);
            break;
        case ::pksm::Generation::TWO:
            static_cast<::pksm::Item2&>(item).id2(id);
            break;
        case ::pksm::Generation::THREE:
            static_cast<::pksm::Item3&>(item).id3(id);
            break;
        default:
            item.id(id);
            break;
    }
}

// The pouch's item list in the ids the format stores; core's validItems() speaks national ids,
// which Gen 1-3 saves do not
std::span<const int> NativeItemList(const ::pksm::Sav& sav, ::pksm::Sav::Pouch pouch) {
    const auto lists = [&]() {
        switch (sav.generation()) {
            case ::pksm::Generation::ONE:
                return static_cast<const ::pksm::Sav1&>(sav).validItems1();
            case ::pksm::Generation::TWO:
                return static_cast<const ::pksm::Sav2&>(sav).validItems2();
            case ::pksm::Generation::THREE:
                return static_cast<const ::pksm::Sav3&>(sav).validItems3();
            default:
                return sav.validItems();
        }
    }();
    for (const auto& [candidate, list] : lists) {
        if (candidate == pouch) {
            return list;
        }
    }
    return {};
}

// Quality and level boost, the two things a donut is chosen for
std::string DonutDetail(const ::pksm::SavZA::Donut& donut) {
    return "★" + std::to_string(donut.stars) + "  Lv +" + std::to_string(donut.levelBoost);
}

// One pouch as the save holds it. Gen 9 saves index slots by item id, so every item the pouch
// can hold has a slot and owned ones carry a count; the rest keep contiguous slot arrays
pksm::bag::Pouch ReadPouch(const ::pksm::Sav& sav, ::pksm::Sav::Pouch pouch, int capacity) {
    const auto storageFormat = sav.generation();
    pksm::bag::Pouch view{
        pouch, pksm::strings::PouchName(pouch, storageFormat), static_cast<size_t>(capacity), sav.maxCount(pouch), {}
    };
    view.indexedByItem = sav.pouchIndexedByItem(pouch);
    for (int slot = 0; slot < capacity; slot++) {
        const auto item = sav.item(pouch, static_cast<u16>(slot));
        if (!item) {
            break;
        }
        const u16 id = NativeItemId(*item);
        if (id != 0 && item->count() > 0) {
            view.items.push_back(
                {id, item->count(), pksm::strings::ItemName(id, storageFormat), {}, 0, static_cast<u16>(slot)}
            );
        }
    }
    return view;
}

int PouchCapacity(const ::pksm::Sav& sav, ::pksm::Sav::Pouch pouch) {
    for (const auto& [candidate, capacity] : sav.pouches()) {
        if (candidate == pouch) {
            return capacity;
        }
    }
    return 0;
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
        bag.pouches.push_back(ReadPouch(*sav, pouch, capacity));
    }
    // Legends Z-A keeps Mega Dimension donuts outside the item pouches
    if (sav->version() == ::pksm::GameVersion::ZA) {
        // No RTTI; core builds a SavZA for every ZA-version save, so the version settles the type
        const auto* za = static_cast<const ::pksm::SavZA*>(sav);
        if (za->hasDonuts()) {
            constexpr auto POUCH = ::pksm::Sav::Pouch::Donut;
            pksm::bag::Pouch view{POUCH, pksm::strings::PouchName(POUCH, bag.storageFormat), ::pksm::SavZA::DONUT_SLOTS, {}};
            for (const auto& donut : za->donuts()) {
                view.items.push_back(
                    {donut.id, 1, pksm::strings::DonutName(donut.id), DonutDetail(donut), donut.stars, 0}
                );
            }
            bag.pouches.push_back(std::move(view));
        }
    }
    return bag;
}

std::optional<pksm::bag::Pouch> BagDataProvider::SetCount(
    const pksm::saves::SaveData::Ref& saveData,
    ::pksm::Sav::Pouch pouch,
    u16 slot,
    u16 count
) {
    ::pksm::Sav* sav = saveDataAccessor->savFor(saveData);
    const int capacity = sav ? PouchCapacity(*sav, pouch) : 0;
    if (slot >= capacity) {
        return std::nullopt;
    }
    if (count > 0 || sav->pouchIndexedByItem(pouch)) {
        // A slot that is the item's own just reads as unowned once emptied
        auto item = sav->item(pouch, slot);
        item->count(count);
        sav->item(*item, pouch, slot);
    } else {
        // Slot arrays stay contiguous: pull the later slots up and clear the last one
        for (int to = slot; to + 1 < capacity; to++) {
            sav->item(*sav->item(pouch, static_cast<u16>(to + 1)), pouch, static_cast<u16>(to));
        }
        // An empty item of any format converts to an empty one of this format, flags and all
        sav->item(::pksm::Item1{}, pouch, static_cast<u16>(capacity - 1));
    }
    saveDataAccessor->markDirty();
    return ReadPouch(*sav, pouch, capacity);
}

std::vector<pksm::bag::Slot>
BagDataProvider::GetAddable(const pksm::saves::SaveData::Ref& saveData, ::pksm::Sav::Pouch pouch) const {
    std::vector<pksm::bag::Slot> addable;
    ::pksm::Sav* sav = saveDataAccessor->savFor(saveData);
    if (!sav) {
        return addable;
    }
    std::unordered_set<u16> held;
    const int capacity = PouchCapacity(*sav, pouch);
    for (int slot = 0; slot < capacity; slot++) {
        const auto item = sav->item(pouch, static_cast<u16>(slot));
        if (!item) {
            break;
        }
        if (item->count() > 0) {
            held.insert(NativeItemId(*item));
        }
    }
    const auto storageFormat = sav->generation();
    for (const int id : NativeItemList(*sav, pouch)) {
        const u16 itemId = static_cast<u16>(id);
        if (id > 0 && !held.contains(itemId)) {
            addable.push_back({itemId, 0, pksm::strings::ItemName(itemId, storageFormat), {}, 0, 0});
        }
    }
    return addable;
}

std::optional<pksm::bag::Pouch>
BagDataProvider::Add(const pksm::saves::SaveData::Ref& saveData, ::pksm::Sav::Pouch pouch, u16 itemId, u16 count) {
    ::pksm::Sav* sav = saveDataAccessor->savFor(saveData);
    if (!sav || count == 0) {
        return std::nullopt;
    }
    const auto list = NativeItemList(*sav, pouch);
    const auto position = std::find(list.begin(), list.end(), itemId);
    if (position == list.end()) {
        return std::nullopt;
    }
    const int capacity = PouchCapacity(*sav, pouch);
    int target = -1;
    if (sav->pouchIndexedByItem(pouch)) {
        // Slots follow the list, so the item's own slot is its place in it; refused when held
        target = static_cast<int>(position - list.begin());
        const auto item = target < capacity ? sav->item(pouch, static_cast<u16>(target)) : nullptr;
        if (!item || NativeItemId(*item) != itemId || item->count() > 0) {
            return std::nullopt;
        }
    } else {
        // The first free slot of an array the game keeps contiguous; the held slots come first
        for (int slot = 0; slot < capacity; slot++) {
            const auto item = sav->item(pouch, static_cast<u16>(slot));
            if (!item) {
                break;
            }
            const u16 id = NativeItemId(*item);
            if (id == itemId && item->count() > 0) {
                return std::nullopt;
            }
            if (id == 0 || item->count() == 0) {
                target = slot;
                break;
            }
        }
    }
    if (target < 0) {
        return std::nullopt;
    }
    auto item = sav->item(pouch, static_cast<u16>(target));
    SetNativeItemId(*item, itemId);
    item->count(count);
    sav->item(*item, pouch, static_cast<u16>(target));
    saveDataAccessor->markDirty();
    return ReadPouch(*sav, pouch, capacity);
}
