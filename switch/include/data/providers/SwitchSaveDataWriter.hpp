#pragma once

#include "data/providers/interfaces/ISaveDataWriter.hpp"

class SwitchSaveDataWriter : public ISaveDataWriter {
public:
    SwitchSaveDataWriter() = default;
    PU_SMART_CTOR(SwitchSaveDataWriter)

    bool WriteSave(
        const pksm::titles::Title::Ref& title,
        const std::string& savePath,
        const AccountUid* userId,
        const u8* data,
        size_t size
    ) override;
};
