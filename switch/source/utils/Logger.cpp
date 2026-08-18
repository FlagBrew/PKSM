#include "utils/Logger.hpp"

#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <ctime>
#include <unistd.h>

// libnx heap bounds, set up at startup by the runtime
extern "C" {
extern char* fake_heap_start;
extern char* fake_heap_end;
}
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <switch.h>
#include <thread>
#include <vector>

namespace pksm::utils {

int Logger::OUTPUT_TO_FILE = 1;
int Logger::ADVANCED_LOGGING = 0;

namespace {

constexpr const char* LOG_DIR = "sdmc:/switch/PKSM";
constexpr const char* LOG_FILE = "sdmc:/switch/PKSM/pksm.log";
constexpr const char* LOG_FILE_OLD = "sdmc:/switch/PKSM/pksm.log.old";

// All logger state is TU-local: the header stays a pure API surface.
// g_log_mutex guards every flag and buffer below, and serializes console
// writes so concurrent LOG_* calls can't interleave.
std::mutex g_log_mutex;
std::condition_variable g_log_cv;
std::vector<std::string> g_pending_lines;
std::thread g_flush_thread;
bool g_initialized = false;
bool g_socket_initialized = false;
bool g_console_initialized = false;
bool g_flush_thread_running = false;
bool g_flush_thread_stop = false;
bool g_log_file_rotated = false;

void FlushPendingToFileLocked(std::vector<std::string>& out) {
    if (g_pending_lines.empty()) {
        return;
    }
    out.swap(g_pending_lines);
}

void FlushLinesToFile(const std::vector<std::string>& lines) {
    if (lines.empty()) {
        return;
    }

    try {
        std::filesystem::create_directories(LOG_DIR);
        std::ofstream out(LOG_FILE, std::ios::out | std::ios::app);
        if (!out.good()) {
            return;
        }

        for (const auto& line : lines) {
            out.write(line.data(), static_cast<std::streamsize>(line.size()));
        }
        out.flush();
    } catch (...) {
        // ignore file logging failures
    }
}

void FlushThreadMain() {
    std::unique_lock<std::mutex> lk(g_log_mutex);
    while (!g_flush_thread_stop) {
        g_log_cv.wait_for(lk, std::chrono::seconds(3));

        std::vector<std::string> local;
        FlushPendingToFileLocked(local);

        lk.unlock();
        FlushLinesToFile(local);
        lk.lock();
    }

    std::vector<std::string> local;
    FlushPendingToFileLocked(local);
    lk.unlock();
    FlushLinesToFile(local);
}

// Caller must hold g_log_mutex.
void EnsureFlushThreadStartedLocked() {
    if (g_flush_thread_running) {
        return;
    }

    try {
        std::filesystem::create_directories(LOG_DIR);
        if (!g_log_file_rotated) {
            // Keep the previous run's log so a crash log survives one
            // relaunch: users can reopen the app and still send it.
            std::error_code ec;
            std::filesystem::remove(LOG_FILE_OLD, ec);
            if (std::filesystem::exists(LOG_FILE, ec)) {
                std::filesystem::rename(LOG_FILE, LOG_FILE_OLD, ec);
            }
            std::ofstream out(LOG_FILE, std::ios::out | std::ios::trunc);
            g_log_file_rotated = true;
        }
    } catch (...) {
        // ignore file logging failures
    }

    g_flush_thread_stop = false;
    g_flush_thread_running = true;
    // The new thread blocks on g_log_mutex until our caller releases it.
    g_flush_thread = std::thread(FlushThreadMain);
}

void StopFlushThread() {
    {
        std::lock_guard<std::mutex> lg(g_log_mutex);
        if (!g_flush_thread_running) {
            return;
        }
        g_flush_thread_stop = true;
    }
    g_log_cv.notify_all();
    if (g_flush_thread.joinable()) {
        g_flush_thread.join();
    }
    std::lock_guard<std::mutex> lg(g_log_mutex);
    g_flush_thread_running = false;
}

}  // namespace

void Logger::Initialize() {
    std::lock_guard<std::mutex> lg(g_log_mutex);
    if (g_initialized) {
        return;
    }

#ifndef NDEBUG
    // Initialize socket for nxlink first
    Result rc = socketInitializeDefault();
    g_socket_initialized = R_SUCCEEDED(rc);

    // Enable console logging only when nxlink redirection succeeds.
    // Avoid consoleInit(NULL) fallback since it conflicts with SDL/Plutonium
    // and causes black screens/crashes when launching normally.
    g_console_initialized = false;
    if (g_socket_initialized) {
        g_console_initialized = nxlinkStdio() > 0;
        if (!g_console_initialized) {
            socketExit();
            g_socket_initialized = false;
        }
    }

    if (g_console_initialized) {
        setvbuf(stdout, NULL, _IONBF, 0);
    }
#endif

    g_initialized = true;

    if (OUTPUT_TO_FILE != 0) {
        EnsureFlushThreadStartedLocked();
    }
}

void Logger::Flush() {
    // Synchronous flush for moments the 3s background cadence would lose:
    // right before a likely crash or applet teardown
    std::vector<std::string> lines;
    {
        std::lock_guard<std::mutex> lg(g_log_mutex);
        if (!g_initialized || OUTPUT_TO_FILE == 0) {
            return;
        }
        FlushPendingToFileLocked(lines);
    }
    FlushLinesToFile(lines);
}

void Logger::Finalize() {
    StopFlushThread();

    std::lock_guard<std::mutex> lg(g_log_mutex);
    if (!g_initialized) {
        return;
    }

#ifndef NDEBUG
    if (g_socket_initialized) {
        socketExit();
    }
#endif
    g_socket_initialized = false;
    g_console_initialized = false;
    g_initialized = false;
}

void Logger::Debug(const std::string& message) {
    Log(Level::Debug, message);
}

void Logger::Info(const std::string& message) {
    Log(Level::Info, message);
}

void Logger::Warning(const std::string& message) {
    Log(Level::Warning, message);
}

void Logger::Error(const std::string& message) {
    Log(Level::Error, message);
}

void Logger::Log(Level level, const std::string& message) {
    Initialize();

    // Get current time (localtime_r: Log may run on multiple threads)
    auto now = std::time(nullptr);
    struct tm tm_buf;
    localtime_r(&now, &tm_buf);

    std::stringstream ss;
    ss << "[" << std::put_time(&tm_buf, "%H:%M:%S") << "] ";

    // Add log level
    switch (level) {
        case Level::Debug:
            ss << "[DEBUG] ";
            break;
        case Level::Info:
            ss << "[INFO] ";
            break;
        case Level::Warning:
            ss << "[WARNING] ";
            break;
        case Level::Error:
            ss << "[ERROR] ";
            break;
    }

    ss << message << "\n";
    const std::string line = ss.str();

    std::lock_guard<std::mutex> lg(g_log_mutex);

#ifndef NDEBUG
    if (g_console_initialized) {
        printf("%s", line.c_str());
        fflush(stdout);

        // Give a small delay after each log to ensure it's flushed
        if (g_socket_initialized) {
            svcSleepThread(1'000'000);  // 1ms delay
        }
    }
#endif

    if (OUTPUT_TO_FILE != 0) {
        EnsureFlushThreadStartedLocked();
        g_pending_lines.push_back(line);
        if (g_pending_lines.size() >= 128) {
            g_log_cv.notify_all();
        }
    }
}

void Logger::LogMemoryInfo() {
    u64 total = 0;
    u64 used = 0;

    // Get total memory
    Result rc = svcGetInfo(&total, InfoType_TotalMemorySize, CUR_PROCESS_HANDLE, 0);
    if (R_FAILED(rc)) {
        Error("Failed to get total memory size");
        return;
    }

    // Get used memory
    rc = svcGetInfo(&used, InfoType_UsedMemorySize, CUR_PROCESS_HANDLE, 0);
    if (R_FAILED(rc)) {
        Error("Failed to get used memory size");
        return;
    }

    // Calculate available memory
    u64 available = total - used;

    // svcGetInfo reports the reserved envelope, which is static once the
    // heap is claimed at boot; the sbrk high-water mark tracks how much of
    // the malloc heap has actually been claimed (mallinfo is unreliable here)
    const u64 heapTotal = static_cast<u64>(fake_heap_end - fake_heap_start);
    const u64 heapHighWater = static_cast<u64>(static_cast<char*>(sbrk(0)) - fake_heap_start);

    std::stringstream ss;
    ss << "Memory - Total: " << (total / 1024 / 1024) << "MB, "
       << "Used: " << (used / 1024 / 1024) << "MB, "
       << "Available: " << (available / 1024 / 1024) << "MB"
       << " | Heap total: " << (heapTotal / 1024 / 1024) << "MB, "
       << "high-water: " << (heapHighWater / 1024 / 1024) << "MB";
    Debug(ss.str());
}

}  // namespace pksm::utils
