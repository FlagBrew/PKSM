#pragma once

#include <atomic>
#include <cstddef>
#include <string>
#include <switch.h>

namespace pksm::utils {

// Downloads the PKSS sprite sheets from the PKResources release and keeps
// them on SD, verified against SHA-256 hashes pinned here - the 3DS app's
// asset model, romfs ships no sheets. The boot gate refuses to continue
// until every asset is verified on SD, so consumers may trust
// ResolvedPath unconditionally. Mirrors PokemonSpriteManager's
// static-singleton shape: state is file-local, callers see only the
// policy.
class AssetDownloader {
public:
    enum class Asset { PokemonSprites, BoxArt };

    // Hash-check the SD copies once; ResolvedPath answers from this scan
    static void Refresh();

    // Where the asset lives on SD (the boot gate guarantees validity)
    static std::string ResolvedPath(Asset asset);

    static bool NeedsDownload();

    // Live progress for the boot UI, written by the download thread
    struct Progress {
        std::atomic<size_t> fileIndex{0};
        std::atomic<size_t> fileCount{0};
        std::atomic<size_t> received{0};
        std::atomic<size_t> total{0};
    };

    // Fetch every missing/mismatched asset into the SD dir (tmp + verify +
    // rename). Blocking - run on a worker thread. Returns true when every
    // asset ended up verified on SD.
    static bool DownloadAll(Progress& progress);
};

}  // namespace pksm::utils
