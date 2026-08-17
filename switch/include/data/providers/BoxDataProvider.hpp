#pragma once

#include <memory>

#include "data/providers/SaveDataAccessor.hpp"
#include "data/providers/interfaces/IBoxDataProvider.hpp"

// Reads box contents from the accessor's single owned Sav; read-only until
// the box editing work lands.
class BoxDataProvider : public IBoxDataProvider {
private:
    SaveDataAccessor::Ref saveDataAccessor;

    // Non-null only when `saveData` is the save the accessor currently owns
    ::pksm::Sav* CurrentSav(const pksm::saves::SaveData::Ref& saveData) const;

public:
    explicit BoxDataProvider(SaveDataAccessor::Ref saveDataAccessor);
    PU_SMART_CTOR(BoxDataProvider)

    // IBoxDataProvider implementation
    size_t GetBoxCount(const pksm::saves::SaveData::Ref& saveData) const override;
    pksm::ui::BoxData GetBoxData(const pksm::saves::SaveData::Ref& saveData, int boxIndex) const override;
    bool SetBoxData(const pksm::saves::SaveData::Ref& saveData, int boxIndex, const pksm::ui::BoxData& boxData)
        override;
    bool SetPokemonData(
        const pksm::saves::SaveData::Ref& saveData,
        int boxIndex,
        int slotIndex,
        const pksm::ui::BoxPokemonData& pokemonData
    ) override;
};
