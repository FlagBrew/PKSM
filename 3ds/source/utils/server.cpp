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

#include "server.hpp"
#include "logging.hpp"
#include "thread.hpp"
#include <3ds.h>
#include <cstring>

// Socket headers
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace
{
    // Socket server variables
    static const int SERVER_PORT   = 8000;
    std::atomic_flag serverRunning = ATOMIC_FLAG_INIT;
    s32 serverSocket               = -1;

    static void handleHttpRequest(s32 clientSocket)
    {
        char buffer[1024] = {0};
        recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        // Simple HTTP request parsing
        if (strstr(buffer, "GET /logs HTTP") != nullptr)
        {
            std::string applicationLogs = Logging::getLogs();
            // Serve logs at /logs endpoint
            std::string header = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n"
                                 "Content-Length: " +
                                 std::to_string(applicationLogs.length()) + "\r\n\r\n";

            // Send header and logs
            send(clientSocket, header.c_str(), header.length(), 0);
            send(clientSocket, applicationLogs.c_str(), applicationLogs.length(), 0);
        }
        else
        {
            // 404 for all other endpoints
            std::string response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    }

    static void networkLoop()
    {
        struct sockaddr_in clientAddr;
        u32 clientLen = sizeof(clientAddr);

        // Set server socket to non-blocking
        fcntl(serverSocket, F_SETFL, fcntl(serverSocket, F_GETFL, 0) | O_NONBLOCK);

        while (serverRunning.test_and_set())
        {
            // Accept connection (non-blocking)
            s32 clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);

            if (clientSocket >= 0)
            {
                // Set client socket to blocking for simpler sending
                fcntl(clientSocket, F_SETFL, fcntl(clientSocket, F_GETFL, 0) & ~O_NONBLOCK);

                // Handle the request
                handleHttpRequest(clientSocket);

                // Close the connection
                close(clientSocket);
            }
            else if (errno != EAGAIN)
            {
                // FIXME do something
            }

            // Prevent 100% CPU usage
            svcSleepThread(100000000); // 100ms
        }
    }
}

void Server::init()
{
    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (serverSocket < 0)
    {
        Logging::startupLog("log", "Failed to create socket with error: " + std::to_string(errno));
        return;
    }

    // Set up server address
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_port        = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = gethostid(); // Use the device's IP

    // Bind socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0)
    {
        Logging::startupLog("log", "Failed to bind to port " + std::to_string(SERVER_PORT) +
                                       " with error: " + std::to_string(errno));
        close(serverSocket);
        serverSocket = -1;
        return;
    }

    // Start listening
    if (listen(serverSocket, 5) != 0)
    {
        Logging::startupLog(
            "log", "Failed to listen on socket with error: " + std::to_string(errno));
        close(serverSocket);
        serverSocket = -1;
        return;
    }

    // Convert IP to string for log message
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(serverAddr.sin_addr), ipStr, INET_ADDRSTRLEN);

    // Start server thread
    serverRunning.test_and_set();
    Threads::create(networkLoop);
    Logging::startupLog("log",
        std::string("Listening on http://") + ipStr + ":" + std::to_string(SERVER_PORT) + "/logs");
}

void Server::exit()
{
    serverRunning.clear();

    if (serverSocket >= 0)
    {
        close(serverSocket);
        serverSocket = -1;
    }

    Logging::trace("HTTP log server stopped");
}