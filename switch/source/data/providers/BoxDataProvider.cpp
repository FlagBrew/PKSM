#include "data/providers/BoxDataProvider.hpp"

#include <algorithm>
#include <cctype>
#include <string>

#include "utils/Logger.hpp"

namespace {

// The UI grid is always 6x5
constexpr int GRID_SLOTS = 30;

// International Gen 1/2 saves hold 20 Pokémon per box; the games show them
// 4 wide, so keep the outer grid columns empty and center each row
bool SaveUsesTwentySlotBoxes(const ::pksm::Sav& sav) {
    const int boxes = sav.maxBoxes();
    return boxes > 0 && (sav.maxSlot() / boxes) == 20;
}

int ToGridSlot(int saveSlot) {
    return (saveSlot / 4) * 6 + 1 + (saveSlot % 4);
}

}  // namespace

BoxDataProvider::BoxDataProvider(SaveDataAccessor::Ref saveDataAccessor)
  : saveDataAccessor(std::move(saveDataAccessor)) {}

::pksm::Sav* BoxDataProvider::CurrentSav(const pksm::saves::SaveData::Ref& saveData) const {
    if (!saveData || saveData != saveDataAccessor->getCurrentSaveData()) {
        return nullptr;
    }
    return saveDataAccessor->currentSav();
}

size_t BoxDataProvider::GetBoxCount(const pksm::saves::SaveData::Ref& saveData) const {
    auto* sav = CurrentSav(saveData);
    return sav ? static_cast<size_t>(std::max(0, sav->maxBoxes())) : 0;
}

pksm::ui::BoxData BoxDataProvider::GetBoxData(const pksm::saves::SaveData::Ref& saveData, int boxIndex) const {
    pksm::ui::BoxData boxData("Box " + std::to_string(boxIndex + 1));
    boxData.resize(GRID_SLOTS);

    auto* sav = CurrentSav(saveData);
    if (!sav || boxIndex < 0 || boxIndex >= sav->maxBoxes()) {
        return boxData;
    }

    try {
        // Some saves have no box names by design (e.g. LGPE)
        std::string name = sav->boxName(static_cast<u8>(boxIndex));
        name.erase(std::remove(name.begin(), name.end(), '\0'), name.end());
        const bool blank = std::all_of(name.begin(), name.end(), [](unsigned char c) {
            return std::isspace(c) != 0;
        });
        if (!name.empty() && !blank) {
            boxData.name = name;
        }

        const bool twentySlot = SaveUsesTwentySlotBoxes(*sav);
        const int slotsPerBox = twentySlot ? 20 : GRID_SLOTS;
        // The last box may be partial (LGPE's 1000-slot list ends mid-box)
        const int slotsInBox = std::min(slotsPerBox, sav->maxSlot() - boxIndex * slotsPerBox);

        for (int slot = 0; slot < slotsInBox; slot++) {
            auto pk = sav->pkm(static_cast<u8>(boxIndex), static_cast<u8>(slot));
            if (!pk) {
                continue;
            }
            if (pk->isEncrypted()) {
                pk->decrypt();
            }
            const u16 species = static_cast<u16>(pk->species());
            if (species == 0) {
                continue;
            }
            const u16 form = pk->alternativeForm();
            const int gridSlot = twentySlot ? ToGridSlot(slot) : slot;
            boxData[gridSlot] =
                pksm::ui::BoxPokemonData(species, form > 0xFF ? 0 : static_cast<u8>(form), pk->shiny());
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Core threw while reading box " + std::to_string(boxIndex) + ": " + e.what());
    }

    return boxData;
}

bool BoxDataProvider::SetBoxData(
    const pksm::saves::SaveData::Ref& saveData,
    int boxIndex,
    const pksm::ui::BoxData& boxData
) {
    LOG_ERROR("Box editing is not implemented yet (box " + std::to_string(boxIndex) + ")");
    return false;
}

bool BoxDataProvider::SetPokemonData(
    const pksm::saves::SaveData::Ref& saveData,
    int boxIndex,
    int slotIndex,
    const pksm::ui::BoxPokemonData& pokemonData
) {
    LOG_ERROR(
        "Box editing is not implemented yet (box " + std::to_string(boxIndex) + ", slot " +
        std::to_string(slotIndex) + ")"
    );
    return false;
}
