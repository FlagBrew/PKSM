/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2025 Bernardo Giordano, Admiral Fish, piepie62
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
 *       * Requiring preservation of specified reasonable legal notices or
 *         author attributions in that material or in the Appropriate Legal
 *         Notices displayed by works containing it.
 *       * Prohibiting misrepresentation of the origin of that material,
 *         or requiring that modified versions of such material be marked in
 *         reasonable ways as different from the original version.
 */

#include "logging.hpp"
#include "font.h"

#if defined(__3DS__)
#include <3ds.h>
#endif

#include "revision.h"
#include <chrono>
#include <cstring>
#include <iomanip>
#include <sstream>

namespace
{
    std::string applicationLogs;
    PrintConsole headerConsole; // Fixed header console
    PrintConsole logConsole;    // Scrolling log console
    std::chrono::steady_clock::time_point startTime;
}

void Logging::init()
{
    consoleInit(GFX_BOTTOM, &headerConsole);
    consoleInit(GFX_BOTTOM, &logConsole);

    // Configure header console to use just the top line
    consoleSetWindow(&headerConsole, 0, 0, 40, 1);

    // Configure log console to use the remaining space
    consoleSetWindow(&logConsole, 0, 1, 40, 29);

    ConsoleFont font;
    font.gfx         = (u8*)console_font_8x8;
    font.asciiOffset = 0;
    font.numChars    = 255;

    consoleSetFont(&headerConsole, &font);
    consoleSetFont(&logConsole, &font);

    startTime = std::chrono::steady_clock::now();

    std::string versionInfo = std::format(
        "PKSM v{:d}.{:d}.{:d}-{:s}", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO, GIT_REV);
    consoleSelect(&headerConsole);
    printf("\x1b[1;%dH" CONSOLE_YELLOW "%s" CONSOLE_RESET,
        40 - static_cast<int>(versionInfo.length()), versionInfo.c_str());
    info(versionInfo);
}

void Logging::startupLog(const std::string& category, const std::string& message)
{
    std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed             = currentTime - startTime;
    double seconds                                    = elapsed.count();

    std::ostringstream oss;
    oss << "[" << std::setw(3) << std::setfill(' ') << (int)seconds << "." << std::setfill('0')
        << std::setw(4) << (int)((seconds - (int)seconds) * 10000 + 0.5) << "]";
    std::string formattedText = oss.str() + " " + category + ": " + message;
    info(category + ": " + message);

    consoleSelect(&logConsole);
    printf("%s\n", formattedText.c_str());
}

void Logging::info(const std::string& message)
{
    log(LogLevel::INFO, message);
}

void Logging::warning(const std::string& message)
{
    log(LogLevel::WARNING, message);
}

void Logging::error(const std::string& message)
{
    log(LogLevel::ERROR, message);
}

void Logging::debug(const std::string& message)
{
    log(LogLevel::DEBUG, message);
}

void Logging::trace(const std::string& message)
{
    log(LogLevel::TRACE, message);
}

void Logging::log(LogLevel level, const std::string& message)
{
    std::stringstream ss;
    auto now        = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    ss << std::put_time(std::localtime(&now_time_t), "[%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << now_ms.count() << "] ";

    switch (level)
    {
        case LogLevel::TRACE:
            ss << " TRACE - ";
            break;
        case LogLevel::DEBUG:
            ss << " DEBUG - ";
            break;
        case LogLevel::INFO:
            ss << "  INFO - ";
            break;
        case LogLevel::WARNING:
            ss << "WARNING - ";
            break;
        case LogLevel::ERROR:
            ss << " ERROR - ";
            break;
    }

    applicationLogs += ss.str() + message + "\n";
}

std::string Logging::getLogs()
{
    return applicationLogs;
}