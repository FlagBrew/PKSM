#include "data/saves/SaveValidationCache.hpp"

#include <algorithm>
#include <switch.h>

#include "data/saves/SaveValidator.hpp"
#include "utils/Logger.hpp"

namespace pksm::saves {

SaveValidationCache::~SaveValidationCache() {
    prewarmStop = true;
    if (prewarmThread.joinable()) {
        prewarmThread.join();
    }
}

bool SaveValidationCache::Validate(const std::string& path) {
    std::error_code ec;
    const auto mtime = std::filesystem::last_write_time(std::filesystem::path(path), ec);
    const auto size = ec ? 0 : std::filesystem::file_size(std::filesystem::path(path), ec);
    if (ec) {
        return false;
    }

    {
        std::lock_guard<std::mutex> lg(mutex);
        auto it = cache.find(path);
        if (it != cache.end() && it->second.mtime == mtime && it->second.size == size) {
            return it->second.valid;
        }
    }

    // Validation runs unlocked; it must not block the UI thread's cache hits
    const auto summary = SaveValidator::Validate(path);
    if (summary) {
        LOG_INFO("Validated save " + path + ": " + summary->Describe());
    } else {
        LOG_INFO("Rejected save candidate " + path + ": core cannot parse it");
    }

    std::lock_guard<std::mutex> lg(mutex);
    cache[path] = {mtime, size, summary.has_value()};
    return summary.has_value();
}

void SaveValidationCache::Record(
    const std::string& path,
    std::filesystem::file_time_type mtime,
    std::uintmax_t size,
    bool valid
) {
    std::lock_guard<std::mutex> lg(mutex);
    cache[path] = {mtime, size, valid};
}

void SaveValidationCache::Prewarm(std::function<std::vector<std::string>()> collectPaths) {
    if (prewarmThread.joinable()) {
        return;
    }
    prewarmThread = std::thread([this, collectPaths = std::move(collectPaths)]() {
        // An exception escaping a std::thread is std::terminate; a failed
        // prewarm must degrade to cold landings instead
        try {
            const u64 t0 = armGetSystemTick();

            auto paths = collectPaths();
            // Save families share files (one .srm can match several games)
            std::sort(paths.begin(), paths.end());
            paths.erase(std::unique(paths.begin(), paths.end()), paths.end());

            size_t validated = 0;
            for (const auto& path : paths) {
                if (prewarmStop) {
                    return;
                }
                Validate(path);
                validated++;
            }
            LOG_DEBUG(
                "Prewarmed save validation: " + std::to_string(validated) + " files in " +
                std::to_string(armTicksToNs(armGetSystemTick() - t0) / 1000000) + " ms"
            );
        } catch (const std::exception& e) {
            LOG_ERROR("Save prewarm aborted: " + std::string(e.what()));
        }
    });
}

}  // namespace pksm::saves
