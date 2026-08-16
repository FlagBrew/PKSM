#pragma once

#include <string>

namespace pksm::utils {

class Logger {
public:
    enum class Level { Debug, Info, Warning, Error };

    static void Initialize();
    static void Finalize();
    static void Debug(const std::string& message);
    static void Info(const std::string& message);
    static void Warning(const std::string& message);
    static void Error(const std::string& message);
    static void LogMemoryInfo();

    // Runtime knobs: file logging to sdmc:/switch/PKSM/pksm.log (works in
    // release builds too; previous run kept as pksm.log.old), and
    // extra-verbose logging for input feedback.
    static int OUTPUT_TO_FILE;
    static int ADVANCED_LOGGING;

private:
    static void Log(Level level, const std::string& message);
};

}  // namespace pksm::utils

#define LOG_DEBUG(msg) ::pksm::utils::Logger::Debug(msg)
#define LOG_INFO(msg) ::pksm::utils::Logger::Info(msg)
#define LOG_WARNING(msg) ::pksm::utils::Logger::Warning(msg)
#define LOG_ERROR(msg) ::pksm::utils::Logger::Error(msg)
#define LOG_MEMORY() ::pksm::utils::Logger::LogMemoryInfo()
