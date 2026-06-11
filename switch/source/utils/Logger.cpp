#include "utils/Logger.hpp"

#include <cstdio>
#include <ctime>
#include <iomanip>
#include <sstream>

#ifndef NDEBUG

namespace pksm::utils {

bool Logger::initialized = false;
bool Logger::socket_initialized = false;
bool Logger::console_initialized = false;

void Logger::Initialize() {
    if (!initialized) {
        // Initialize socket for nxlink first
        Result rc = socketInitializeDefault();
        socket_initialized = R_SUCCEEDED(rc);

        if (socket_initialized) {
            // Try to redirect output to nxlink
            console_initialized = nxlinkStdio() > 0;
            if (!console_initialized) {
                // If nxlink redirection fails, try console
                consoleInit(NULL);
                console_initialized = true;
            }
        } else {
            // If socket fails, fallback to console
            consoleInit(NULL);
            console_initialized = true;
        }

        initialized = true;

        // Log initialization status
        if (socket_initialized) {
            LOG_INFO("Socket initialized successfully");
            if (console_initialized) {
                LOG_INFO("Console output redirected to nxlink");
            } else {
                LOG_WARNING("Failed to redirect console to nxlink");
            }
        } else {
            LOG_WARNING("Socket initialization failed, using console output");
        }
    }
}

void Logger::Finalize() {
    if (initialized) {
        if (socket_initialized) {
            socketExit();
            socket_initialized = false;
        }
        if (console_initialized) {
            consoleExit(NULL);
            console_initialized = false;
        }
        initialized = false;
    }
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
    if (!initialized || (!socket_initialized && !console_initialized)) {
        return;
    }

    // Get current time
    auto now = std::time(nullptr);
    auto tm = std::localtime(&now);

    std::stringstream ss;
    ss << "[" << std::put_time(tm, "%H:%M:%S") << "] ";

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

    ss << message << std::endl;
    printf("%s", ss.str().c_str());
    fflush(stdout);

    // Give a small delay after each log to ensure it's flushed
    if (socket_initialized) {
        svcSleepThread(1'000'000);  // 1ms delay
    }
}

void Logger::LogMemoryInfo() {
    if (!initialized) {
        return;
    }

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

    std::stringstream ss;
    ss << "Memory - Total: " << (total / 1024 / 1024) << "MB, "
       << "Used: " << (used / 1024 / 1024) << "MB, "
       << "Available: " << (available / 1024 / 1024) << "MB";
    Debug(ss.str());
}

}  // namespace pksm::utils

#endif  // NDEBUG