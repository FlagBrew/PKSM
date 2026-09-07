#include "data/providers/BagDataProvider.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <unordered_set>

#include "data/bag/NativeItems.hpp"
#include "sav/Item.hpp"
#include "sav/SavZA.hpp"
#include "utils/CoreStrings.hpp"

namespace {

// Quality and level boost, the two things a donut is chosen for
std::string DonutDetail(const ::pksm::SavZA::Donut& donut) {
    return "★" + std::to_string(donut.stars) + "  Lv +" + std::to_string(donut.levelBoost);
}

// ASCII-only fold for name order: the games ignore case, and an accented letter sorts as typed
std::string FoldCase(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return std::tolower(c); });
    return text;
}

// The bag-wide sort a game that orders its own pouches keeps, as its menu lists the options
// (see Sav::bagSort); 0 shows as the first
enum class BagSort : u8 { Type, Name, Newest, Favorites };
const std::vector<std::string> BAG_SORT_OPTIONS{"By Type", "By Name", "Newest First", "Favorites First"};

std::optional<BagSort> BagSortOf(const ::pksm::Sav& sav) {
    const auto setting = sav.bagSort();
    if (!setting) {
        return std::nullopt;
    }
    return static_cast<BagSort>(std::clamp<u8>(*setting, 1, BAG_SORT_OPTIONS.size()) - 1);
}

// Legends Z-A's donut pocket has its own sort, cycled in this order in-game; the save numbers
// the options differently (see SavZA::donutSort)
enum class DonutSort : u8 { Date, Flavor, Variety };
const std::vector<std::string> DONUT_SORT_OPTIONS{"Date Made", "Flavor Score", "Variety"};
constexpr std::array<u8, 3> DONUT_SORT_VALUES{2, 0, 1};  // by DonutSort

DonutSort DonutSortOf(u8 value) {
    const auto found = std::find(DONUT_SORT_VALUES.begin(), DONUT_SORT_VALUES.end(), value);
    if (found == DONUT_SORT_VALUES.end()) {
        return DonutSort::Date;
    }
    return static_cast<DonutSort>(found - DONUT_SORT_VALUES.begin());
}

// The donut pocket as the save holds it, in the order the game shows
pksm::bag::Pouch ReadDonuts(const ::pksm::SavZA& za, ::pksm::Generation storageFormat) {
    constexpr auto POUCH = ::pksm::Sav::Pouch::Donut;
    pksm::bag::Pouch view{POUCH, pksm::strings::PouchName(POUCH, storageFormat), ::pksm::SavZA::DONUT_SLOTS, {}};
    auto donuts = za.donuts();
    if (const auto setting = za.donutSort()) {
        const DonutSort sort = DonutSortOf(*setting);
        view.sortOptions = DONUT_SORT_OPTIONS;
        view.sortOption = static_cast<size_t>(sort);
        // Newest first within every order; the score is the star rating, the variety the kind
        using Donut = ::pksm::SavZA::Donut;
        std::stable_sort(donuts.begin(), donuts.end(), [sort](const Donut& a, const Donut& b) {
            if (sort == DonutSort::Flavor && a.stars != b.stars) {
                return a.stars > b.stars;
            }
            if (sort == DonutSort::Variety && a.id != b.id) {
                return a.id < b.id;
            }
            return a.made > b.made;
        });
    }
    for (const auto& donut : donuts) {
        view.items.push_back({donut.id, 1, pksm::strings::DonutName(donut.id), DonutDetail(donut), donut.stars, 0});
    }
    return view;
}

// One pouch as the save holds it. Gen 9 saves index slots by item id, so every item the pouch
// can hold has a slot and owned ones carry a count, and the game shows them sorted, by type until
// the player chooses otherwise; the rest keep contiguous slot arrays in the order the player made
pksm::bag::Pouch ReadPouch(const ::pksm::Sav& sav, ::pksm::Sav::Pouch pouch, int capacity) {
    const auto storageFormat = sav.generation();
    pksm::bag::Pouch view{
        pouch, pksm::strings::PouchName(pouch, storageFormat), static_cast<size_t>(capacity), sav.maxCount(pouch), {}
    };
    view.indexedByItem = sav.pouchIndexedByItem(pouch);
    struct Row {
        pksm::bag::Slot slot;
        int order;  // the game's sort-by-type position
        bool isNew;
        bool favorite;
        std::string key;  // the name folded, for a name sort
    };
    std::vector<Row> rows;
    std::optional<BagSort> setting;
    if (view.indexedByItem) {
        setting = BagSortOf(sav);
    }
    const BagSort sort = setting.value_or(BagSort::Type);
    for (int slot = 0; slot < capacity; slot++) {
        const auto item = sav.item(pouch, static_cast<u16>(slot));
        if (!item) {
            break;
        }
        const u16 id = pksm::bag::NativeItemId(*item);
        if (id != 0 && item->count() > 0) {
            Row row{{id, item->count(), pksm::strings::ItemName(id, storageFormat), {}, 0, static_cast<u16>(slot)}, 0};
            if (item->generation() == ::pksm::Generation::NINE) {
                const auto& item9 = static_cast<const ::pksm::Item9a&>(*item);
                row.order = sav.itemSortOrder(id);
                row.isNew = item9.isNew();
                row.favorite = item9.isFavorite();
                if (sort == BagSort::Name) {
                    row.key = FoldCase(row.slot.name);
                }
            }
            rows.push_back(std::move(row));
        }
    }
    if (setting) {
        view.sortOptions = BAG_SORT_OPTIONS;
        view.sortOption = static_cast<size_t>(*setting);
    }
    if (view.indexedByItem) {
        // Flagged items lead, then the type order; a name sort stands alone. Ties keep the id order
        std::stable_sort(rows.begin(), rows.end(), [sort](const Row& a, const Row& b) {
            switch (sort) {
                case BagSort::Name:
                    return a.key < b.key;
                case BagSort::Newest:
                    if (a.isNew != b.isNew) {
                        return a.isNew;
                    }
                    break;
                case BagSort::Favorites:
                    if (a.favorite != b.favorite) {
                        return a.favorite;
                    }
                    break;
                case BagSort::Type:
                    break;
            }
            return a.order < b.order;
        });
    }
    view.items.reserve(rows.size());
    for (auto& row : rows) {
        view.items.push_back(std::move(row.slot));
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
            bag.pouches.push_back(ReadDonuts(*za, bag.storageFormat));
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
            held.insert(pksm::bag::NativeItemId(*item));
        }
    }
    const auto storageFormat = sav->generation();
    for (const int id : pksm::bag::NativeItemList(*sav, pouch)) {
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
    const auto list = pksm::bag::NativeItemList(*sav, pouch);
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
        if (!item || pksm::bag::NativeItemId(*item) != itemId || item->count() > 0) {
            return std::nullopt;
        }
    } else {
        // The first free slot of an array the game keeps contiguous; the held slots come first
        for (int slot = 0; slot < capacity; slot++) {
            const auto item = sav->item(pouch, static_cast<u16>(slot));
            if (!item) {
                break;
            }
            const u16 id = pksm::bag::NativeItemId(*item);
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
    pksm::bag::SetNativeItemId(*item, itemId);
    item->count(count);
    sav->item(*item, pouch, static_cast<u16>(target));
    saveDataAccessor->markDirty();
    return ReadPouch(*sav, pouch, capacity);
}

std::optional<pksm::bag::Pouch>
BagDataProvider::Move(const pksm::saves::SaveData::Ref& saveData, ::pksm::Sav::Pouch pouch, u16 fromSlot, u16 toSlot) {
    ::pksm::Sav* sav = saveDataAccessor->savFor(saveData);
    const int capacity = sav ? PouchCapacity(*sav, pouch) : 0;
    if (fromSlot >= capacity || toSlot >= capacity || sav->pouchIndexedByItem(pouch)) {
        return std::nullopt;
    }
    if (fromSlot != toSlot) {
        const auto moving = sav->item(pouch, fromSlot);
        const int step = toSlot > fromSlot ? 1 : -1;
        for (int slot = fromSlot; slot != toSlot; slot += step) {
            sav->item(*sav->item(pouch, static_cast<u16>(slot + step)), pouch, static_cast<u16>(slot));
        }
        sav->item(*moving, pouch, toSlot);
        saveDataAccessor->markDirty();
    }
    return ReadPouch(*sav, pouch, capacity);
}

std::optional<pksm::bag::Pouch> BagDataProvider::Sort(
    const pksm::saves::SaveData::Ref& saveData,
    ::pksm::Sav::Pouch pouch,
    pksm::bag::SortOrder order
) {
    ::pksm::Sav* sav = saveDataAccessor->savFor(saveData);
    const int capacity = sav ? PouchCapacity(*sav, pouch) : 0;
    if (capacity == 0 || sav->pouchIndexedByItem(pouch)) {
        return std::nullopt;
    }
    struct Entry {
        std::unique_ptr<::pksm::Item> item;
        u16 id;
        std::string name;  // folded for the name order
        int slot;
    };
    std::vector<Entry> owned;  // in save order, so equal keys keep it
    int last = -1;             // the last slot in use; what the packed items leave behind is cleared
    const auto storageFormat = sav->generation();
    const bool byName = order == pksm::bag::SortOrder::Name;
    for (int slot = 0; slot < capacity; slot++) {
        auto item = sav->item(pouch, static_cast<u16>(slot));
        if (!item) {
            break;
        }
        const u16 id = pksm::bag::NativeItemId(*item);
        if (id != 0 && item->count() > 0) {
            std::string key = byName ? FoldCase(pksm::strings::ItemName(id, storageFormat)) : "";
            owned.push_back({std::move(item), id, std::move(key), slot});
            last = slot;
        }
    }
    std::stable_sort(owned.begin(), owned.end(), [order](const Entry& a, const Entry& b) {
        switch (order) {
            case pksm::bag::SortOrder::Name:
                return a.name < b.name;
            case pksm::bag::SortOrder::Number:
                return a.id < b.id;
            case pksm::bag::SortOrder::Quantity:
                return a.item->count() > b.item->count();
        }
        return false;
    });
    // Already in order and packed: nothing to write, so no unsaved change either
    bool moved = last + 1 != static_cast<int>(owned.size());
    for (size_t slot = 0; slot < owned.size() && !moved; slot++) {
        moved = owned[slot].slot != static_cast<int>(slot);
    }
    if (moved) {
        for (size_t slot = 0; slot < owned.size(); slot++) {
            sav->item(*owned[slot].item, pouch, static_cast<u16>(slot));
        }
        for (int slot = static_cast<int>(owned.size()); slot <= last; slot++) {
            sav->item(::pksm::Item1{}, pouch, static_cast<u16>(slot));
        }
        saveDataAccessor->markDirty();
    }
    return ReadPouch(*sav, pouch, capacity);
}

std::optional<pksm::bag::BagData>
BagDataProvider::SetPouchSort(const pksm::saves::SaveData::Ref& saveData, ::pksm::Sav::Pouch pouch, size_t option) {
    ::pksm::Sav* sav = saveDataAccessor->savFor(saveData);
    if (!sav) {
        return std::nullopt;
    }
    if (pouch == ::pksm::Sav::Pouch::Donut) {
        auto* za = sav->version() == ::pksm::GameVersion::ZA ? static_cast<::pksm::SavZA*>(sav) : nullptr;
        if (!za || !za->donutSort() || option >= DONUT_SORT_VALUES.size()) {
            return std::nullopt;
        }
        za->donutSort(DONUT_SORT_VALUES[option]);
    } else {
        if (!sav->bagSort() || option >= BAG_SORT_OPTIONS.size()) {
            return std::nullopt;
        }
        sav->bagSort(static_cast<u8>(option) + 1);
    }
    saveDataAccessor->markDirty();
    return GetBag(saveData);
}
