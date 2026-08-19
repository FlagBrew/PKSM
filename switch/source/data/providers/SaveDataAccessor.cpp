#include "data/providers/SaveDataAccessor.hpp"

#include <algorithm>

#include "data/saves/BoxListRefs.hpp"
#include "enums/GameVersion.hpp"
#include "enums/Gender.hpp"
#include "enums/Generation.hpp"
#include "utils/Logger.hpp"

namespace {

pksm::saves::Generation ToAppGeneration(pksm::Generation gen) {
    switch (gen) {
        case pksm::Generation::ONE:
            return pksm::saves::Generation::ONE;
        case pksm::Generation::TWO:
            return pksm::saves::Generation::TWO;
        case pksm::Generation::THREE:
            return pksm::saves::Generation::THREE;
        case pksm::Generation::FOUR:
            return pksm::saves::Generation::FOUR;
        case pksm::Generation::FIVE:
            return pksm::saves::Generation::FIVE;
        case pksm::Generation::SIX:
            return pksm::saves::Generation::SIX;
        case pksm::Generation::SEVEN:
            return pksm::saves::Generation::SEVEN;
        case pksm::Generation::LGPE:
            return pksm::saves::Generation::LGPE;
        case pksm::Generation::EIGHT:
            return pksm::saves::Generation::EIGHT;
        case pksm::Generation::NINE:
            return pksm::saves::Generation::NINE;
        default:
            return pksm::saves::Generation::EIGHT;
    }
}

pksm::saves::Gender ToAppGender(pksm::Gender gender) {
    switch (gender) {
        case pksm::Gender::Female:
            return pksm::saves::Gender::Female;
        case pksm::Gender::Male:
        default:
            return pksm::saves::Gender::Male;
    }
}

pksm::saves::GameVersion ToAppGameVersion(pksm::GameVersion version) {
    using GV = pksm::GameVersion;
    using AGV = pksm::saves::GameVersion;

    switch (version) {
        case GV::RD:
            return AGV::RD;
        case GV::GN:
            return AGV::GN;
        case GV::BU:
            return AGV::BU;
        case GV::YW:
            return AGV::YW;
        case GV::GD:
            return AGV::GD;
        case GV::SV:
            return AGV::SV;
        case GV::C:
            return AGV::C;
        case GV::R:
            return AGV::R;
        case GV::S:
            return AGV::S;
        case GV::E:
            return AGV::E;
        case GV::FR:
            return AGV::FR;
        case GV::LG:
            return AGV::LG;
        case GV::D:
            return AGV::D;
        case GV::P:
            return AGV::P;
        case GV::Pt:
            return AGV::Pt;
        case GV::HG:
            return AGV::HG;
        case GV::SS:
            return AGV::SS;
        case GV::W:
            return AGV::W;
        case GV::B:
            return AGV::B;
        case GV::W2:
            return AGV::W2;
        case GV::B2:
            return AGV::B2;
        case GV::X:
            return AGV::X;
        case GV::Y:
            return AGV::Y;
        case GV::OR:
            return AGV::OR;
        case GV::AS:
            return AGV::AS;
        case GV::SN:
            return AGV::SN;
        case GV::MN:
            return AGV::MN;
        case GV::US:
            return AGV::US;
        case GV::UM:
            return AGV::UM;
        case GV::GP:
            return AGV::GP;
        case GV::GE:
            return AGV::GE;
        case GV::SW:
            return AGV::SW;
        case GV::SH:
            return AGV::SH;
        case GV::PLA:
            return AGV::PLA;
        case GV::SL:
            return AGV::SL;
        case GV::VL:
            return AGV::VL;
        case GV::ZA:
            return AGV::ZA;
        default:
            return AGV::SW;
    }
}

}  // namespace

SaveDataAccessor::SaveDataAccessor(ISaveDataProvider::Ref saveProvider, ISaveDataWriter::Ref saveWriter)
  : saveProvider(std::move(saveProvider)), saveWriter(std::move(saveWriter)) {}

pksm::saves::SaveData::Ref SaveDataAccessor::getCurrentSaveData() const {
    return currentSave;
}

pksm::saves::SaveData::Ref SaveDataAccessor::BuildSaveData(const std::string& name) const {
    return pksm::saves::SaveData::New(
        name,
        ToAppGeneration(sav->generation()),
        ToAppGameVersion(sav->version()),
        sav->otName(),
        sav->TID(),
        sav->SID(),
        ToAppGender(sav->gender()),
        sav->badges(),
        static_cast<u16>(std::max(0, sav->dexSeen())),
        static_cast<u16>(std::max(0, sav->dexCaught())),
        static_cast<u16>(sav->availableSpecies().size()),
        static_cast<u16>(std::max(0, sav->currentGiftAmount())),
        sav->playedHours(),
        sav->playedMinutes(),
        sav->playedSeconds()
    );
}

bool SaveDataAccessor::loadSave(
    const pksm::titles::Title::Ref& title,
    const std::string& saveName,
    const AccountUid* userId
) {
    return applySaveLoadResult(title, saveName, userId, saveProvider->LoadSave(title, saveName, userId));
}

bool SaveDataAccessor::applySaveLoadResult(
    const pksm::titles::Title::Ref& title,
    const std::string& saveName,
    const AccountUid* userId,
    std::optional<pksm::saves::LoadedSave> loaded
) {
    // Success or failure, the previous save is gone either way - a failed
    // load must not leave stale data on screen
    if (!loaded) {
        sav.reset();
        savePath.clear();
        saveTitle = nullptr;
        hasSaveUserId = false;
        currentSave = nullptr;
        hasChanges = false;
        if (onSaveDataChanged) {
            onSaveDataChanged(nullptr);
        }
        return false;
    }

    sav = std::move(loaded->sav);
    savePath = loaded->path;
    saveTitle = title;
    hasSaveUserId = userId != nullptr;
    if (userId) {
        saveUserId = *userId;
    }
    currentSave = BuildSaveData(saveName);
    hasChanges = false;

    if (onSaveDataChanged) {
        onSaveDataChanged(currentSave);
    }
    return true;
}

bool SaveDataAccessor::saveChanges() {
    if (!sav) {
        return false;
    }
    if (!hasChanges) {
        return true;
    }

    pksm::saves::FinalizeBoxList(*sav);
    sav->finishEditing();
    const bool written = saveWriter->WriteSave(
        saveTitle,
        savePath,
        hasSaveUserId ? &saveUserId : nullptr,
        sav->rawData().get(),
        sav->getEntireLengthIncludingFooter()
    );
    sav->beginEditing();

    if (!written) {
        LOG_ERROR("Failed to write save file " + savePath);
        return false;
    }

    hasChanges = false;
    LOG_INFO("Saved changes to " + savePath);
    return true;
}

void SaveDataAccessor::unloadSave() {
    sav.reset();
    savePath.clear();
    saveTitle = nullptr;
    hasSaveUserId = false;
    currentSave = nullptr;
    hasChanges = false;
}

bool SaveDataAccessor::hasUnsavedChanges() const {
    return hasChanges;
}
