#pragma once

#include <string>
#include <switch.h>

namespace pksm::utils {

class Logger {
public:
    enum class Level { Debug, Info, Warning, Error };

#ifdef NDEBUG
    static inline void Initialize() {}
    static inline void Finalize() {}
    static inline void Debug(const std::string&) {}
    static inline void Info(const std::string&) {}
    static inline void Warning(const std::string&) {}
    static inline void Error(const std::string&) {}
#else
    static void Initialize();
    static void Finalize();
    static void Debug(const std::string& message);
    static void Info(const std::string& message);
    static void Warning(const std::string& message);
    static void Error(const std::string& message);
    static void LogMemoryInfo();

private:
    static void Log(Level level, const std::string& message);
    static bool initialized;
    static bool socket_initialized;
    static bool console_initialized;
#endif
};

}  // namespace pksm::utils

#ifdef NDEBUG
#define LOG_DEBUG(msg) ((void)0)
#define LOG_INFO(msg) ((void)0)
#define LOG_WARNING(msg) ((void)0)
#define LOG_ERROR(msg) ((void)0)
#define LOG_MEMORY() ((void)0)
#else
#define LOG_DEBUG(msg) utils::Logger::Debug(msg)
#define LOG_INFO(msg) utils::Logger::Info(msg)
#define LOG_WARNING(msg) utils::Logger::Warning(msg)
#define LOG_ERROR(msg) utils::Logger::Error(msg)
#define LOG_MEMORY() utils::Logger::LogMemoryInfo()
#endif