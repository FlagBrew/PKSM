#pragma once

#include <atomic>
#include <cstddef>
#include <string>
#include <switch.h>

namespace pksm::utils {

// Downloads the PKSS sprite sheets to SD, verified against the SHA-256
// hashes pinned here; romfs ships no sheets.
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

    // Fetch every missing/mismatched asset (tmp + verify + rename).
    // Blocking - run on a worker thread.
    static bool DownloadAll(Progress& progress);
};

}  // namespace pksm::utils
