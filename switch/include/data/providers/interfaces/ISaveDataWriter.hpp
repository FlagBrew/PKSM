#pragma once

#include <memory>
#include <string>
#include <switch.h>

#include "data/titles/Title.hpp"

class ISaveDataWriter {
public:
    PU_SMART_CTOR(ISaveDataWriter)
    virtual ~ISaveDataWriter() = default;

    // Write edited save bytes back to their source (SD file or console
    // container); the writer never interprets the bytes
    virtual bool WriteSave(
        const pksm::titles::Title::Ref& title,
        const std::string& savePath,
        const AccountUid* userId,
        const u8* data,
        size_t size
    ) = 0;
};
