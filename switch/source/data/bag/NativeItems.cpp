#include "data/bag/NativeItems.hpp"

#include "sav/Sav1.hpp"
#include "sav/Sav2.hpp"
#include "sav/Sav3.hpp"

namespace pksm::bag {

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

}  // namespace pksm::bag
