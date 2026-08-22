/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2026 Bernardo Giordano, Admiral Fish, piepie62
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
#include "DataMutex.hpp"
#include "logging.hpp"
#include "thread.hpp"
#include "TransferProtocol.hpp"
#include <3ds.h>
#include <algorithm>
#include <atomic>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <map>
#include <memory>
#include <poll.h>
#include <string>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace
{
    constexpr int SERVER_PORT           = 8000;
    constexpr size_t MAX_REQUEST_SIZE   = 1024 * 1024;
    constexpr size_t MAX_UPLOAD_SIZE    = 32 * 1024 * 1024;
    constexpr size_t MAX_HEADER_SIZE    = 128 * 1024;
    constexpr size_t RECEIVE_CHUNK_SIZE = 32 * 1024;
    constexpr int RECEIVE_TIMEOUT_MS    = 15000;
    constexpr int POLL_SLICE_MS         = 1000;

    std::atomic_flag serverRunning = ATOMIC_FLAG_INIT;
    std::atomic<bool> serverReady  = false;
    s32 serverSocket               = -1;
    std::string serverAddress;

    struct HandlerRegistry
    {
        std::map<std::string, Server::HttpHandler> handlers;
        std::map<std::string, std::pair<std::string, Server::UploadHandler>> uploads;
    };

    DataMutex<HandlerRegistry> registry;

    LightEvent serverThreadDone;
    bool serverThreadStarted = false;

    std::string extractPath(const std::string& request)
    {
        size_t firstSpace = request.find(' ');
        if (firstSpace == std::string::npos)
        {
            return "";
        }
        size_t pathEnd = request.find(' ', firstSpace + 1);
        return pathEnd == std::string::npos
                 ? ""
                 : request.substr(firstSpace + 1, pathEnd - firstSpace - 1);
    }

    size_t parseContentLength(const std::string& headers)
    {
        std::string value = TransferProtocol::headerValue(headers, "Content-Length");
        if (value.empty())
        {
            return 0;
        }
        char* end            = nullptr;
        unsigned long parsed = strtoul(value.c_str(), &end, 10);
        return end == value.c_str() || *end != '\0' ? 0 : (size_t)parsed;
    }

    ssize_t receiveWithTimeout(s32 socket, char* buffer, size_t size, int& idleTime)
    {
        while (serverRunning.test())
        {
            pollfd descriptor{socket, POLLIN, 0};
            int ready = poll(&descriptor, 1, POLL_SLICE_MS);
            if (ready < 0)
            {
                return -1;
            }
            if (ready == 0)
            {
                idleTime += POLL_SLICE_MS;
                if (idleTime >= RECEIVE_TIMEOUT_MS)
                {
                    return 0;
                }
                continue;
            }
            idleTime = 0;
            return recv(socket, buffer, size, 0);
        }
        return -1;
    }

    bool sendAll(s32 socket, const void* data, size_t size)
    {
        const char* current = static_cast<const char*>(data);
        size_t sent         = 0;
        while (sent < size)
        {
            int result = send(socket, current + sent, size - sent, 0);
            if (result <= 0)
            {
                return false;
            }
            sent += (size_t)result;
        }
        return true;
    }

    void sendResponse(s32 socket, const Server::HttpResponse& response)
    {
        std::string status = response.statusCode == 200
                               ? "OK"
                               : (response.statusCode == 404 ? "Not Found" : "Error");
        std::string header = "HTTP/1.1 " + std::to_string(response.statusCode) + " " + status +
                             "\r\nContent-Type: " + response.contentType +
                             "\r\nContent-Length: " + std::to_string(response.body.size()) +
                             "\r\nConnection: close\r\n\r\n";
        sendAll(socket, header.data(), header.size());
        sendAll(socket, response.body.data(), response.body.size());
    }

    bool streamBody(s32 socket, const std::string& path, size_t contentLength, const char* initial,
        size_t initialSize)
    {
        FILE* output = fopen(path.c_str(), "wb");
        if (!output)
        {
            Logging::error("Failed to open wireless transfer temporary file: {}", path);
            return false;
        }

        size_t written = 0;
        if (initialSize > 0)
        {
            size_t toWrite = std::min(initialSize, contentLength);
            written        = fwrite(initial, 1, toWrite, output);
        }

        std::unique_ptr<char[]> buffer(new char[RECEIVE_CHUNK_SIZE]);
        int idleTime = 0;
        while (written < contentLength)
        {
            ssize_t received =
                receiveWithTimeout(socket, buffer.get(), RECEIVE_CHUNK_SIZE, idleTime);
            if (received <= 0)
            {
                break;
            }
            size_t toWrite = std::min((size_t)received, contentLength - written);
            size_t result  = fwrite(buffer.get(), 1, toWrite, output);
            written       += result;
            if (result != toWrite)
            {
                break;
            }
        }

        bool closed = fclose(output) == 0;
        return written == contentLength && closed;
    }

    void handleHttpRequest(s32 clientSocket, const std::string& peerAddress)
    {
        std::string data;
        data.reserve(4096);
        std::unique_ptr<char[]> buffer(new char[RECEIVE_CHUNK_SIZE]);
        size_t headerEnd = std::string::npos;
        int idleTime     = 0;

        while (data.size() <= MAX_HEADER_SIZE)
        {
            ssize_t received =
                receiveWithTimeout(clientSocket, buffer.get(), RECEIVE_CHUNK_SIZE, idleTime);
            if (received <= 0)
            {
                return;
            }
            data.append(buffer.get(), (size_t)received);
            headerEnd = data.find("\r\n\r\n");
            if (headerEnd != std::string::npos)
            {
                break;
            }
        }
        if (headerEnd == std::string::npos)
        {
            sendResponse(clientSocket,
                {400, "application/json", "{\"ok\":false,\"error\":\"Bad request\"}"});
            return;
        }

        std::string headers  = data.substr(0, headerEnd);
        std::string path     = extractPath(headers);
        size_t contentLength = parseContentLength(headers);
        size_t bodyStart     = headerEnd + 4;

        std::string temporaryPath;
        Server::UploadHandler uploadHandler;
        Server::HttpHandler handler;
        {
            auto handlers = registry.lock();
            auto upload   = handlers->uploads.find(path);
            if (upload != handlers->uploads.end())
            {
                temporaryPath = upload->second.first;
                uploadHandler = upload->second.second;
            }
            else
            {
                auto normal = handlers->handlers.find(path);
                if (normal != handlers->handlers.end())
                {
                    handler = normal->second;
                }
            }
        }

        if (uploadHandler)
        {
            if (contentLength == 0 || contentLength > MAX_UPLOAD_SIZE)
            {
                sendResponse(clientSocket,
                    {413, "application/json", "{\"ok\":false,\"error\":\"Payload too large\"}"});
                return;
            }
            size_t initialSize = data.size() - bodyStart;
            if (!streamBody(clientSocket, temporaryPath, contentLength, data.data() + bodyStart,
                    initialSize))
            {
                remove(temporaryPath.c_str());
                Logging::warning("Wireless transfer upload ended before its declared length");
                return;
            }
            Server::UploadRequest request{
                headers, temporaryPath, peerAddress, (uint64_t)contentLength};
            Server::HttpResponse response = uploadHandler(request);
            remove(temporaryPath.c_str());
            sendResponse(clientSocket, response);
            return;
        }

        if (!handler)
        {
            sendResponse(clientSocket, {404, "text/plain", ""});
            return;
        }
        if (contentLength > MAX_REQUEST_SIZE)
        {
            sendResponse(clientSocket,
                {413, "application/json", "{\"ok\":false,\"error\":\"Payload too large\"}"});
            return;
        }
        while (data.size() < bodyStart + contentLength)
        {
            ssize_t received =
                receiveWithTimeout(clientSocket, buffer.get(), RECEIVE_CHUNK_SIZE, idleTime);
            if (received <= 0)
            {
                break;
            }
            data.append(buffer.get(), (size_t)received);
        }
        sendResponse(clientSocket, handler(path, data));
    }

    bool acceptWouldBlock(int error)
    {
        return error == EAGAIN || error == EWOULDBLOCK || error == EINPROGRESS;
    }

    void networkLoop()
    {
        fcntl(serverSocket, F_SETFL, fcntl(serverSocket, F_GETFL, 0) | O_NONBLOCK);
        serverReady.store(true);

        while (serverRunning.test())
        {
            sockaddr_in clientAddress{};
            socklen_t clientLength = sizeof(clientAddress);
            s32 clientSocket       = accept(serverSocket, (sockaddr*)&clientAddress, &clientLength);
            if (clientSocket >= 0)
            {
                fcntl(clientSocket, F_SETFL, fcntl(clientSocket, F_GETFL, 0) & ~O_NONBLOCK);
                char peer[INET_ADDRSTRLEN] = {0};
                inet_ntop(AF_INET, &clientAddress.sin_addr, peer, sizeof(peer));
                handleHttpRequest(clientSocket, peer);
                close(clientSocket);
            }
            else if (!acceptWouldBlock(errno))
            {
                Logging::error("HTTP server accept failed with errno {}", errno);
            }
            svcSleepThread(100000000);
        }

        serverReady.store(false);
        LightEvent_Signal(&serverThreadDone);
    }
}

void Server::registerHandler(const std::string& path, Server::HttpHandler handler)
{
    registry.lock()->handlers[path] = std::move(handler);
    Logging::info("Registered HTTP handler for path {}", path);
}

void Server::registerUploadHandler(
    const std::string& path, const std::string& temporaryPath, Server::UploadHandler handler)
{
    registry.lock()->uploads[path] = {temporaryPath, std::move(handler)};
    Logging::info("Registered streaming HTTP handler for path {}", path);
}

void Server::unregisterHandler(const std::string& path)
{
    auto handlers = registry.lock();
    handlers->handlers.erase(path);
    handlers->uploads.erase(path);
    Logging::info("Unregistered HTTP handler for path {}", path);
}

bool Server::isRunning()
{
    return serverReady.load();
}

std::string Server::getAddress()
{
    return serverAddress;
}

void Server::init()
{
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (serverSocket < 0)
    {
        Logging::startupLog(
            "log", "Failed to create socket with error {}: {}", errno, strerror(errno));
        return;
    }

    sockaddr_in address{};
    address.sin_family      = AF_INET;
    address.sin_port        = htons(SERVER_PORT);
    address.sin_addr.s_addr = gethostid();

    if (bind(serverSocket, (sockaddr*)&address, sizeof(address)) != 0)
    {
        Logging::startupLog("log", "Failed to bind to port {} with error {}", SERVER_PORT, errno);
        close(serverSocket);
        serverSocket = -1;
        return;
    }
    if (listen(serverSocket, 5) != 0)
    {
        Logging::startupLog(
            "log", "Failed to listen on socket with error {}: {}", errno, strerror(errno));
        close(serverSocket);
        serverSocket = -1;
        return;
    }

    char ip[INET_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET, &address.sin_addr, ip, sizeof(ip));
    serverAddress = ip;

    serverRunning.test_and_set();
    LightEvent_Init(&serverThreadDone, RESET_STICKY);
    serverThreadStarted = Threads::create(networkLoop);
    if (!serverThreadStarted)
    {
        serverRunning.clear();
        close(serverSocket);
        serverSocket = -1;
        Logging::startupLog("log", "Failed to start the HTTP server thread");
        return;
    }
    Logging::startupLog("log", "HTTP server started on http://{}:{}", ip, SERVER_PORT);
}

void Server::exit()
{
    serverRunning.clear();
    if (serverThreadStarted)
    {
        LightEvent_Wait(&serverThreadDone);
        serverThreadStarted = false;
    }
    if (serverSocket >= 0)
    {
        close(serverSocket);
        serverSocket = -1;
    }
    {
        auto handlers = registry.lock();
        handlers->handlers.clear();
        handlers->uploads.clear();
    }
    serverAddress.clear();
    Logging::trace("HTTP server stopped");
}
