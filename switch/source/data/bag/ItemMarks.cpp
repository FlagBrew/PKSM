#include "data/bag/ItemMarks.hpp"

namespace {

bool IsSwordShield(const ::pksm::Sav& sav) {
    return sav.version() == ::pksm::GameVersion::SW || sav.version() == ::pksm::GameVersion::SH;
}

}  // namespace

namespace pksm::bag {

bool KeepsNewMark(const ::pksm::Sav& sav) {
    switch (sav.generation()) {
        case ::pksm::Generation::SEVEN:
        case ::pksm::Generation::LGPE:
        case ::pksm::Generation::NINE:
            return true;
        case ::pksm::Generation::EIGHT:
            return IsSwordShield(sav);
        default:
            return false;
    }
}

bool KeepsFavorite(const ::pksm::Sav& sav) {
    return sav.generation() == ::pksm::Generation::NINE || IsSwordShield(sav);
}

ItemMarks MarksOf(const ::pksm::Sav& sav, const ::pksm::Item& item) {
    switch (item.generation()) {
        case ::pksm::Generation::SEVEN:
            return {static_cast<const ::pksm::Item7&>(item).newFlag(), false};
        case ::pksm::Generation::LGPE:
            return {static_cast<const ::pksm::Item7b&>(item).newFlag(), false};
        case ::pksm::Generation::EIGHT:
            if (IsSwordShield(sav)) {
                const auto& item8 = static_cast<const ::pksm::Item8&>(item);
                return {item8.newFlag(), item8.favoriteFlag()};
            }
            return {};
        case ::pksm::Generation::NINE: {
            const auto& item9 = static_cast<const ::pksm::Item9a&>(item);
            return {item9.newFlag(), item9.favoriteFlag()};
        }
        default:
            return {};
    }
}

void SetMarks(const ::pksm::Sav& sav, ::pksm::Item& item, ItemMarks marks) {
    switch (item.generation()) {
        case ::pksm::Generation::SEVEN:
            static_cast<::pksm::Item7&>(item).newFlag(marks.isNew);
            break;
        case ::pksm::Generation::LGPE:
            static_cast<::pksm::Item7b&>(item).newFlag(marks.isNew);
            break;
        case ::pksm::Generation::EIGHT:
            if (IsSwordShield(sav)) {
                auto& item8 = static_cast<::pksm::Item8&>(item);
                item8.newFlag(marks.isNew);
                item8.favoriteFlag(marks.favorite);
            }
            break;
        case ::pksm::Generation::NINE: {
            auto& item9 = static_cast<::pksm::Item9a&>(item);
            item9.newFlag(marks.isNew);
            item9.favoriteFlag(marks.favorite);
            break;
        }
        default:
            break;
    }
}

}  // namespace pksm::bag
