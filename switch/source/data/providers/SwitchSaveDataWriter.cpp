#include "data/providers/SwitchSaveDataWriter.hpp"

#include <fstream>

#include "data/saves/ConsoleSaveMount.hpp"
#include "utils/Logger.hpp"

bool SwitchSaveDataWriter::WriteSave(
    const pksm::titles::Title::Ref& title,
    const std::string& savePath,
    const AccountUid* userId,
    const u8* data,
    size_t size
) {
    if (!data || size == 0) {
        return false;
    }

    const bool isConsoleSave = savePath.rfind("save:/", 0) == 0;
    if (!isConsoleSave && savePath.rfind("sdmc:", 0) != 0) {
        LOG_ERROR("Write-back is not supported for " + savePath);
        return false;
    }

    if (isConsoleSave) {
        if (!title || !userId) {
            LOG_ERROR("Console save write-back needs the title and user it was loaded for");
            return false;
        }
        FsFileSystem fs;
        if (R_FAILED(pksm::saves::MountConsoleSave(&fs, title->getTitleId(), *userId))) {
            LOG_ERROR("Cannot mount console save for write-back: " + title->getName());
            return false;
        }
    }

    bool written = false;
    {
        std::ofstream out(savePath, std::ios::binary | std::ios::trunc);
        if (out.good()) {
            out.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));
            written = out.good();
        }
    }

    if (isConsoleSave) {
        if (written) {
            // Console saves are journaled; nothing persists without the commit
            Result rc = fsdevCommitDevice("save");
            if (R_FAILED(rc)) {
                LOG_ERROR("Failed to commit console save device for " + savePath);
                written = false;
            }
        }
        pksm::saves::UnmountConsoleSave();
    }

    if (!written) {
        LOG_ERROR("Failed to write " + savePath);
    }
    return written;
}
