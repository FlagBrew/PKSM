#include "utils/AssetDownloader.hpp"

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <functional>
#include <sys/stat.h>

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

std::string ToHex(Result rc) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%x", rc);
    return buf;
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

// ---- Minimal HTTPS GET over the console's own ssl service ----------------
// The body streams to a sink instead of buffering, so a sheet never sits in RAM.

struct UrlParts {
    std::string host;
    std::string path;
};

bool ParseHttpsUrl(const std::string& url, UrlParts& out) {
    constexpr const char* SCHEME = "https://";
    if (url.rfind(SCHEME, 0) != 0) {
        return false;
    }
    const size_t hostStart = strlen(SCHEME);
    const size_t pathStart = url.find('/', hostStart);
    out.host = url.substr(hostStart, pathStart - hostStart);
    out.path = (pathStart == std::string::npos) ? "/" : url.substr(pathStart);
    return !out.host.empty();
}

// Fresh inits are released after the downloads (~2MB of bsd buffers back to
// the applet heap); AlreadyInitialized means another owner, leave it alone
bool ownSocketService = false;
bool ownSslService = false;

bool EnsureNetworkServices(std::string& err) {
    const auto alreadyInit = R_VALUE(MAKERESULT(Module_Libnx, LibnxError_AlreadyInitialized));
    Result rc = socketInitializeDefault();
    if (R_FAILED(rc) && R_VALUE(rc) != alreadyInit) {
        err = "socketInitializeDefault failed: 0x" + ToHex(rc);
        return false;
    }
    ownSocketService = R_SUCCEEDED(rc);
    rc = sslInitialize(3);
    if (R_FAILED(rc) && R_VALUE(rc) != alreadyInit) {
        err = "sslInitialize failed: 0x" + ToHex(rc);
        return false;
    }
    ownSslService = R_SUCCEEDED(rc);
    return true;
}

void ReleaseNetworkServices() {
    if (ownSslService) {
        sslExit();
        ownSslService = false;
    }
    if (ownSocketService) {
        socketExit();
        ownSocketService = false;
    }
}

int ConnectTcp(const UrlParts& url, std::string& err) {
    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    addrinfo* result = nullptr;
    if (getaddrinfo(url.host.c_str(), "443", &hints, &result) != 0 || !result) {
        err = "getaddrinfo failed for " + url.host;
        return -1;
    }
    int sockfd = -1;
    for (addrinfo* rp = result; rp; rp = rp->ai_next) {
        sockfd = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd < 0) {
            continue;
        }
        // These timeouts only govern the connect phase; after the handshake
        // ssl:s applies its own 5-minute timeout to every read/write
        timeval tv{};
        tv.tv_sec = 10;
        if (::connect(sockfd, rp->ai_addr, static_cast<socklen_t>(rp->ai_addrlen)) == 0 &&
            setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == 0 &&
            setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == 0) {
            break;
        }
        ::close(sockfd);
        sockfd = -1;
    }
    freeaddrinfo(result);
    if (sockfd < 0) {
        err = "connect failed for " + url.host;
    }
    return sockfd;
}

struct GetResult {
    int statusCode = 0;
    std::string location;
    // contentLength stays 0 when the server declared none
    size_t contentLength = 0;
};

bool HttpsGet(
    const std::string& url,
    const std::function<bool(const u8*, size_t)>& sink,
    GetResult& out,
    std::string& err,
    int redirectDepth = 0
) {
    if (redirectDepth > 5) {
        err = "Too many HTTP redirects";
        return false;
    }
    UrlParts parts;
    if (!ParseHttpsUrl(url, parts)) {
        err = "Not an https URL: " + url;
        return false;
    }

    const int sockfd = ConnectTcp(parts, err);
    if (sockfd < 0) {
        return false;
    }

    SslContext sslContext{};
    SslConnection sslConn{};
    bool tlsReady = false;
    auto fail = [&](const std::string& what, Result rc) {
        err = what + ": 0x" + ToHex(rc);
        if (tlsReady) {
            sslConnectionClose(&sslConn);
            sslContextClose(&sslContext);
        }
        ::close(sockfd);
        return false;
    };

    Result rc = sslCreateContext(&sslContext, SslVersion_Auto);
    if (R_FAILED(rc)) {
        return fail("sslCreateContext failed", rc);
    }
    rc = sslContextCreateConnection(&sslContext, &sslConn);
    if (R_FAILED(rc)) {
        sslContextClose(&sslContext);
        ::close(sockfd);
        err = "sslContextCreateConnection failed: 0x" + ToHex(rc);
        return false;
    }
    tlsReady = true;
    rc = sslConnectionSetHostName(&sslConn, parts.host.c_str(), static_cast<u32>(parts.host.size() + 1));
    if (R_FAILED(rc)) {
        return fail("sslConnectionSetHostName failed", rc);
    }
    rc = sslConnectionSetOption(&sslConn, SslOptionType_DoNotCloseSocket, true);
    if (R_FAILED(rc)) {
        return fail("sslConnectionSetOption failed", rc);
    }
    if (socketSslConnectionSetSocketDescriptor(&sslConn, sockfd) < 0 && errno != ENOENT) {
        return fail("socketSslConnectionSetSocketDescriptor failed", 0);
    }
    rc = sslConnectionDoHandshake(&sslConn, nullptr, nullptr, nullptr, 0);
    if (R_FAILED(rc)) {
        return fail("TLS handshake failed", rc);
    }

    const std::string request =
        "GET " + parts.path + " HTTP/1.1\r\n"
        "Host: " + parts.host + "\r\n"
        "User-Agent: PKSM-Switch\r\n"
        "Accept: */*\r\n"
        "Connection: close\r\n\r\n";
    size_t sent = 0;
    while (sent < request.size()) {
        u32 chunk = 0;
        rc = sslConnectionWrite(
            &sslConn,
            request.data() + sent,
            static_cast<u32>(std::min<size_t>(request.size() - sent, 0x4000)),
            &chunk
        );
        if (R_FAILED(rc) || chunk == 0) {
            return fail("ssl write failed", rc);
        }
        sent += chunk;
    }

    auto parseHeader = [&](const std::string& header) {
        const size_t space = header.find(' ');
        out.statusCode = (space == std::string::npos) ? 0 : atoi(header.c_str() + space + 1);
        std::string lower(header);
        std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) {
            return std::tolower(c);
        });
        auto headerValue = [&](const char* name) -> std::string {
            const size_t pos = lower.find(std::string("\r\n") + name + ":");
            if (pos == std::string::npos) {
                return "";
            }
            const size_t valStart = header.find(':', pos + 2) + 1;
            const size_t valEnd = header.find("\r\n", valStart);
            std::string v = header.substr(valStart, valEnd - valStart);
            while (!v.empty() && v.front() == ' ') {
                v.erase(v.begin());
            }
            return v;
        };
        out.location = headerValue("location");
        out.contentLength = strtoul(headerValue("content-length").c_str(), nullptr, 10);
    };

    // A declared Content-Length bounds the transfer so a misbehaving
    // server cannot stream until the SD fills
    constexpr size_t MAX_HEADER_SIZE = 0x4000;
    std::string header;
    bool headerDone = false;
    bool sinkOk = true;
    size_t bodyWritten = 0;
    std::array<u8, 0x4000> buf;
    while (true) {
        u32 read = 0;
        rc = sslConnectionRead(&sslConn, buf.data(), static_cast<u32>(buf.size()), &read);
        if (R_FAILED(rc)) {
            return fail("ssl read failed", rc);
        }
        if (read == 0) {
            break;
        }
        size_t bodyStart = 0;
        if (!headerDone) {
            header.append(reinterpret_cast<const char*>(buf.data()), read);
            const size_t marker = header.find("\r\n\r\n");
            if (marker == std::string::npos) {
                if (header.size() > MAX_HEADER_SIZE) {
                    err = "HTTP headers too large";
                    break;
                }
                continue;
            }
            headerDone = true;
            // The bytes past the marker in this very chunk are body
            bodyStart = read - (header.size() - (marker + 4));
            header.resize(marker);
            parseHeader(header);
            if (out.statusCode != 200) {
                break;
            }
        }
        size_t bodyLen = read - bodyStart;
        if (out.contentLength > 0) {
            bodyLen = std::min(bodyLen, out.contentLength - bodyWritten);
        }
        if (bodyLen > 0 && !sink(buf.data() + bodyStart, bodyLen)) {
            sinkOk = false;
            err = "Writing the download to SD failed";
            break;
        }
        bodyWritten += bodyLen;
        if (out.contentLength > 0 && bodyWritten >= out.contentLength) {
            break;
        }
    }
    sslConnectionClose(&sslConn);
    sslContextClose(&sslContext);
    ::close(sockfd);

    if (!headerDone) {
        if (err.empty()) {
            err = "Connection closed before HTTP headers";
        }
        return false;
    }
    if (out.statusCode >= 301 && out.statusCode <= 308 && !out.location.empty()) {
        std::string next = out.location;
        if (next[0] == '/') {
            next = "https://" + parts.host + next;
        }
        return HttpsGet(next, sink, out, err, redirectDepth + 1);
    }
    if (!sinkOk) {
        return false;
    }
    if (out.statusCode != 200) {
        err = "HTTP status " + std::to_string(out.statusCode);
        return false;
    }
    return true;
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
