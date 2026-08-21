#pragma once

#include <atomic>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace pksm::saves {

// Per-file core-validation verdicts, invalidated by mtime+size; guarded by
// its mutex (prewarm worker writes, UI reads)
class SaveValidationCache {
public:
    ~SaveValidationCache();

    // Core-parse the file, consulting the cache first
    bool Validate(const std::string& path);

    // Seed a verdict something else already parsed for
    void Record(const std::string& path, std::filesystem::file_time_type mtime, std::uintmax_t size, bool valid);

    // Validate collectPaths' files on a background thread; main-thread-only,
    // call once (collectPaths itself runs on the worker)
    void Prewarm(std::function<std::vector<std::string>()> collectPaths);

private:
    struct ValidatedFile {
        std::filesystem::file_time_type mtime;
        std::uintmax_t size = 0;
        bool valid = false;
    };
    std::unordered_map<std::string, ValidatedFile> cache;
    std::mutex mutex;
    std::thread prewarmThread;
    std::atomic<bool> prewarmStop{false};
};

}  // namespace pksm::saves
