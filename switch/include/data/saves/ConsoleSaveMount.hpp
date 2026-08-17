#pragma once

#include <switch.h>

namespace pksm::saves {

// Mount a console save container as the "save" fsdev device, and release it.
// Shared by the save sources that read containers and the writer that
// persists back into them.
Result MountConsoleSave(FsFileSystem* fs, u64 titleId, AccountUid userId);
void UnmountConsoleSave();

}  // namespace pksm::saves
