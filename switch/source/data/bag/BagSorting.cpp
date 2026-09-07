#include "data/bag/BagSorting.hpp"

#include <algorithm>
#include <array>
#include <cctype>

namespace {

constexpr std::array<u8, 3> DONUT_SORT_VALUES{2, 0, 1};  // by DonutSort

}  // namespace

namespace pksm::bag {

const std::vector<std::string> BAG_SORT_OPTIONS{"By Type", "By Name", "Newest First", "Favorites First"};

std::optional<BagSort> BagSortOf(const ::pksm::Sav& sav) {
    const auto setting = sav.bagSort();
    if (!setting) {
        return std::nullopt;
    }
    return static_cast<BagSort>(std::clamp<u8>(*setting, 1, BAG_SORT_OPTIONS.size()) - 1);
}

u8 BagSortValue(BagSort sort) {
    return static_cast<u8>(sort) + 1;
}

BagSortKey BagSortKeyOf(const ::pksm::Sav& sav, const ::pksm::Item& item, const std::string& name, BagSort sort) {
    BagSortKey key;
    if (item.generation() == ::pksm::Generation::NINE) {
        const auto& item9 = static_cast<const ::pksm::Item9a&>(item);
        key.order = sav.itemSortOrder(item9.id());
        key.isNew = item9.newFlag();
        key.favorite = item9.favoriteFlag();
        if (sort == BagSort::Name) {
            key.name = FoldCase(name);
        }
    }
    return key;
}

bool BagSortBefore(BagSort sort, const BagSortKey& a, const BagSortKey& b) {
    switch (sort) {
        case BagSort::Name:
            return a.name < b.name;
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
}

const std::vector<std::string> DONUT_SORT_OPTIONS{"Date Made", "Flavor Score", "Variety"};

std::optional<DonutSort> DonutSortOf(const ::pksm::SavZA& za) {
    const auto setting = za.donutSort();
    if (!setting) {
        return std::nullopt;
    }
    const auto found = std::find(DONUT_SORT_VALUES.begin(), DONUT_SORT_VALUES.end(), *setting);
    if (found == DONUT_SORT_VALUES.end()) {
        return DonutSort::Date;
    }
    return static_cast<DonutSort>(found - DONUT_SORT_VALUES.begin());
}

u8 DonutSortValue(DonutSort sort) {
    return DONUT_SORT_VALUES[static_cast<size_t>(sort)];
}

void SortDonuts(std::vector<::pksm::SavZA::Donut>& donuts, DonutSort sort) {
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

std::string FoldCase(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return std::tolower(c); });
    return text;
}

}  // namespace pksm::bag
