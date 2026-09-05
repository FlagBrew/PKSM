#include "utils/PouchGlyphs.hpp"

namespace pksm::utils {

namespace {

// Sheet keys, the contract with pkss-tool/makepouchsheet.py
enum Glyph : u32 {
    MEDICINE = 0,
    BALL = 1,
    BERRY = 2,
    ITEMS = 3,
    TM = 4,
    MEGA = 5,
    TREASURE = 6,
    KEY = 7,
    DONUT = 8,
    BATTLE = 9,
    MATERIALS = 10,
    PICNIC = 11,
    INGREDIENTS = 12,
    MAIL = 13,
    Z_CRYSTAL = 14,
    ROTOM = 15,
    PC_ITEMS = 16,
};

u32 GlyphFor(::pksm::Sav::Pouch pouch, ::pksm::Generation storageFormat) {
    using Pouch = ::pksm::Sav::Pouch;
    switch (pouch) {
        case Pouch::NormalItem:
            return ITEMS;
        case Pouch::KeyItem:
            return KEY;
        case Pouch::TM:
            return TM;
        case Pouch::Mail:
            return MAIL;
        case Pouch::Medicine:
            return MEDICINE;
        case Pouch::Berry:
            return BERRY;
        case Pouch::Ball:
        case Pouch::CatchingItem:
            return BALL;
        case Pouch::Battle:
            return BATTLE;
        case Pouch::Candy:  // Materials in Scarlet/Violet, candies elsewhere
            return storageFormat == ::pksm::Generation::NINE ? MATERIALS : ITEMS;
        case Pouch::ZCrystals:  // Power-Ups in Let's Go
            return storageFormat == ::pksm::Generation::LGPE ? MEGA : Z_CRYSTAL;
        case Pouch::Treasure:
            return TREASURE;
        case Pouch::Ingredient:  // Picnic Items in Scarlet/Violet
            return storageFormat == ::pksm::Generation::NINE ? PICNIC : INGREDIENTS;
        case Pouch::PCItem:
            return PC_ITEMS;
        case Pouch::RotomPower:
            return ROTOM;
        case Pouch::MegaStones:
            return MEGA;
        case Pouch::Recipe:  // Legends Arceus crafting, no glyph of its own yet
            return INGREDIENTS;
        case Pouch::Donut:
            return DONUT;
    }
    return ITEMS;
}

}  // namespace

SpriteSheet& PouchGlyphs::Sheet() {
    // Deliberately leaked for the same reason as PokemonSpriteManager's sheet
    static SpriteSheet* sheet = new SpriteSheet();
    return *sheet;
}

bool PouchGlyphs::Initialize(const std::string& sheetPath) {
    return Sheet().Load(sheetPath);
}

SpriteRef PouchGlyphs::Get(::pksm::Sav::Pouch pouch, ::pksm::Generation storageFormat) {
    return Sheet().Get(GlyphFor(pouch, storageFormat));
}

void PouchGlyphs::ClearCache() {
    Sheet().ReleaseSprites();
}

}  // namespace pksm::utils
