#include "utils/HttpsClient.hpp"

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace pksm::utils {

namespace {

std::string ToHex(Result rc) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%x", rc);
    return buf;
}

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

}  // namespace

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

bool HttpsGet(
    const std::string& url,
    const std::function<bool(const u8*, size_t)>& sink,
    GetResult& out,
    std::string& err,
    int redirectDepth
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

}  // namespace pksm::utils
