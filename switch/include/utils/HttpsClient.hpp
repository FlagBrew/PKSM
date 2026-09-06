#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <switch.h>

namespace pksm::utils {

// ---- Minimal HTTPS GET over the console's own ssl service ----------------
// The body streams to a sink instead of buffering, so a sheet never sits in RAM.

bool EnsureNetworkServices(std::string& err);
void ReleaseNetworkServices();

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
);

}  // namespace pksm::utils
