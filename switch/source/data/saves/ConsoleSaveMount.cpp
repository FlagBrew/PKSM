#include "data/saves/ConsoleSaveMount.hpp"

#include <sstream>

#include "utils/Logger.hpp"

namespace pksm::saves {

Result MountConsoleSave(FsFileSystem* fs, u64 titleId, AccountUid userId) {
    // Defensively clear any leftover mount under the same device name
    fsdevUnmountDevice("save");

    Result res = fsOpen_SaveData(fs, titleId, userId);
    if (R_SUCCEEDED(res)) {
        // fsdev closes fs itself on failure (libnx fs_dev.h) - do NOT fsFsClose here
        int mountResult = fsdevMountDevice("save", *fs);
        if (mountResult == -1) {
            LOG_ERROR("Failed to mount save filesystem device");
            return MAKERESULT(Module_Libnx, LibnxError_IoError);
        }
    } else {
        std::stringstream ss;
        ss << "Failed to open save data filesystem: 0x" << std::hex << res;
        LOG_ERROR(ss.str());
    }

    return res;
}

void UnmountConsoleSave() {
    fsdevUnmountDevice("save");
}

}  // namespace pksm::saves
