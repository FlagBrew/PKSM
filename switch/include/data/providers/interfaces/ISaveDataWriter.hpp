#pragma once

#include <memory>
#include <string>
#include <switch.h>

#include "data/titles/Title.hpp"

class ISaveDataWriter {
public:
    PU_SMART_CTOR(ISaveDataWriter)
    virtual ~ISaveDataWriter() = default;

    // Write edited save bytes back to their source - an SD file, or a console
    // container (remounted with the title/user it was loaded for, written,
    // and committed). The writer never interprets the bytes.
    virtual bool WriteSave(
        const pksm::titles::Title::Ref& title,
        const std::string& savePath,
        const AccountUid* userId,
        const u8* data,
        size_t size
    ) = 0;
};
