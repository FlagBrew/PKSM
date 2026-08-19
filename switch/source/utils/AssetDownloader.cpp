#include "utils/AssetDownloader.hpp"

#include <array>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>

#include "utils/HttpsClient.hpp"
#include "utils/Logger.hpp"

namespace pksm::utils {

namespace {

constexpr const char* SD_ASSETS_DIR = "sdmc:/switch/PKSM/assets";
constexpr const char* RELEASE_BASE =
    "https://github.com/Omar-Kay/PKResources/releases/download/switch-assets-v1/";

struct AssetSpec {
    AssetDownloader::Asset asset;
    const char* fileName;
    const char* sha256;  // lowercase hex, pinned to the release build
};

constexpr AssetSpec ASSETS[] = {
    {AssetDownloader::Asset::PokemonSprites, "pokesprites.pkss",
     "9a975fe84da9cfa3d809a1a63079f929cf662f7fc92a6627b8041eef0a12a012"},
    {AssetDownloader::Asset::BoxArt, "basprites.pkss",
     "142b932923851795155ffc551ff844ddb6415a21ab3f1d8d4e3cb79bdfd240f4"},
};
constexpr size_t ASSET_COUNT = sizeof(ASSETS) / sizeof(ASSETS[0]);

bool sdValid[ASSET_COUNT] = {};

std::string SdPath(const AssetSpec& spec) {
    return std::string(SD_ASSETS_DIR) + "/" + spec.fileName;
}

bool FileMatchesSha256(const std::string& path, const char* expectedHex) {
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) {
        return false;
    }
    Sha256Context ctx;
    sha256ContextCreate(&ctx);
    std::array<u8, 0x10000> buf;
    size_t got;
    while ((got = fread(buf.data(), 1, buf.size(), f)) > 0) {
        sha256ContextUpdate(&ctx, buf.data(), got);
    }
    const bool readOk = feof(f) != 0;
    fclose(f);
    if (!readOk) {
        return false;
    }
    u8 hash[SHA256_HASH_SIZE];
    sha256ContextGetHash(&ctx, hash);
    char hex[SHA256_HASH_SIZE * 2 + 1];
    for (size_t i = 0; i < SHA256_HASH_SIZE; i++) {
        snprintf(hex + i * 2, 3, "%02x", hash[i]);
    }
    return strcmp(hex, expectedHex) == 0;
}

bool DownloadAsset(const AssetSpec& spec, AssetDownloader::Progress& progress) {
    const std::string target = SdPath(spec);
    const std::string tmp = target + ".tmp";
    FILE* outFile = fopen(tmp.c_str(), "wb");
    if (!outFile) {
        LOG_ERROR("Cannot create " + tmp);
        return false;
    }

    size_t written = 0;
    GetResult result;
    std::string err;
    const bool ok = HttpsGet(
        std::string(RELEASE_BASE) + spec.fileName,
        [&](const u8* data, size_t size) {
            if (fwrite(data, 1, size, outFile) != size) {
                return false;
            }
            written += size;
            progress.received.store(written);
            if (result.contentLength) {
                progress.total.store(result.contentLength);
            }
            return true;
        },
        result,
        err
    );
    fclose(outFile);

    if (!ok) {
        LOG_ERROR("Download of " + std::string(spec.fileName) + " failed: " + err);
        std::remove(tmp.c_str());
        return false;
    }
    if (!FileMatchesSha256(tmp, spec.sha256)) {
        LOG_ERROR(std::string(spec.fileName) + " downloaded but does not match its pinned hash");
        std::remove(tmp.c_str());
        return false;
    }
    std::remove(target.c_str());
    if (std::rename(tmp.c_str(), target.c_str()) != 0) {
        LOG_ERROR("Cannot move " + tmp + " into place");
        std::remove(tmp.c_str());
        return false;
    }
    LOG_INFO("Downloaded and verified " + target + " (" + std::to_string(written) + " bytes)");
    return true;
}

}  // namespace

void AssetDownloader::Refresh() {
    for (size_t i = 0; i < ASSET_COUNT; i++) {
        sdValid[i] = FileMatchesSha256(SdPath(ASSETS[i]), ASSETS[i].sha256);
    }
}

std::string AssetDownloader::ResolvedPath(Asset asset) {
    for (size_t i = 0; i < ASSET_COUNT; i++) {
        if (ASSETS[i].asset == asset) {
            return SdPath(ASSETS[i]);
        }
    }
    return "";
}

bool AssetDownloader::NeedsDownload() {
    for (size_t i = 0; i < ASSET_COUNT; i++) {
        if (!sdValid[i]) {
            return true;
        }
    }
    return false;
}

bool AssetDownloader::DownloadAll(Progress& progress) {
    std::string err;
    if (!EnsureNetworkServices(err)) {
        LOG_ERROR(err);
        return false;
    }
    mkdir("sdmc:/switch", 0777);
    mkdir("sdmc:/switch/PKSM", 0777);
    mkdir(SD_ASSETS_DIR, 0777);

    size_t pending = 0;
    for (size_t i = 0; i < ASSET_COUNT; i++) {
        pending += sdValid[i] ? 0 : 1;
    }
    progress.fileCount.store(pending);
    bool all = true;
    size_t started = 0;
    for (size_t i = 0; i < ASSET_COUNT; i++) {
        if (sdValid[i]) {
            continue;
        }
        progress.fileIndex.store(++started);
        progress.received.store(0);
        progress.total.store(0);
        if (DownloadAsset(ASSETS[i], progress)) {
            sdValid[i] = true;
        } else {
            all = false;
        }
    }
    ReleaseNetworkServices();
    return all;
}

}  // namespace pksm::utils
