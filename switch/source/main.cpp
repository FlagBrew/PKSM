#include <pu/Plutonium>
#include <sstream>
#include <switch.h>

#include "PKSMApplication.hpp"
#include "utils/Logger.hpp"

int main(int argc, char* argv[]) {
    try {
        // Initialize and run application
        auto app = pksm::PKSMApplication::Initialize();
        if (!app) {
            pksm::utils::Logger::Error("Application failed to initialize; exiting");
            pksm::utils::Logger::Finalize();
            return 1;
        }
        // The fade-in presents the first frames; flush before the main loop takes over
        app->FadeIn();
        pksm::utils::Logger::Flush();
        app->Show();

        // Cleanup
        pksm::utils::Logger::Finalize();
        return 0;
    } catch (const std::exception& e) {
        // Make sure we log any fatal errors
        pksm::utils::Logger::Error("Fatal error: " + std::string(e.what()));
        try {
            pksm::utils::Logger::LogMemoryInfo();
        } catch (...) {
        }
        pksm::utils::Logger::Finalize();
        return 1;
    }
}