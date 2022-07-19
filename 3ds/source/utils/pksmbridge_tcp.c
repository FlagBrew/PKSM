/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2021 mrhappyasthma, Flagbrew
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

#include "pksmbridge_tcp.h"
#include "pksmbridge_api.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

/** Creates a socket and returns the file descriptor. If an error occurs, returns -1. */
static int createSocket(void)
{
    return socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
}

/** Creates a socket address with the provided port and address. */
static struct sockaddr_in createSocketAddress(uint16_t port, in_addr_t address)
{
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(port);
    servaddr.sin_addr.s_addr = address;
    return servaddr;
}

/** Sends chunks of `data` from a buffer to a socket. Returns the number of sent bytes. */
static int sendDataFromBufferToSocket(int sockfd, void* buffer, size_t size)
{
    size_t total = 0;
    size_t chunk = 1024;
    while (total < size)
    {
        size_t tosend = size - total > chunk ? chunk : size - total;
        int n         = send(sockfd, ((char*)buffer) + total, tosend, 0);
        if (n == -1)
        {
            break;
        }
        total += n;
    }
    return total;
}

/**
 * Reads chunks of data from a socket into the provided `buffer`. Returns the
 * number of read bytes.
 */
static int receiveDataFromSocketIntoBuffer(int sockfd, void* buffer, size_t size)
{
    size_t total = 0;
    size_t chunk = 1024;
    while (total < size)
    {
        size_t torecv = size - total > chunk ? chunk : size - total;
        int n         = recv(sockfd, ((char*)buffer) + total, torecv, 0);
        if (n <= 0)
        {
            break;
        }
        total += n;
    }
    return total;
}

/** Expects the `protocol_name` field from either pksmBridgeRequest or pksmBridgeResponse. */
static bool verifyPKSMBridgeHeader(char protocol_name[10])
{
    int result = strncmp(protocol_name, PKSM_BRIDGE_PROTOCOL_NAME, 10);
    return (result == 0) ? true : false;
}

/** A helper function to send a file over the PKSM Bridge piece-by-piece to avoid additional memory
 * allocations. */
static bool sendPKSMBridgeFileToSocket(int sockfd, struct pksmBridgeFile file)
{
    uint32_t sentBytes =
        sendDataFromBufferToSocket(sockfd, &file.checksumSize, sizeof(file.checksumSize));
    if (sentBytes != sizeof(file.checksumSize))
    {
        return false;
    }
    sentBytes = sendDataFromBufferToSocket(sockfd, file.checksum, file.checksumSize);
    if (sentBytes != file.checksumSize)
    {
        return false;
    }
    sentBytes = sendDataFromBufferToSocket(sockfd, &file.size, sizeof(file.size));
    if (sentBytes != sizeof(file.size))
    {
        return false;
    }
    sentBytes = sendDataFromBufferToSocket(sockfd, file.contents, file.size);
    if (sentBytes != file.size)
    {
        return false;
    }
    return true;
}

/** A helper function to receive file bytes over the PKSM Bridge. Callers are expected to `free()`
 * the `outFile`. */
static uint32_t receiveFileOverPKSMBridgeFromSocket(
    int sockfd, uint8_t** outFile, bool (*validateChecksumFunc)(struct pksmBridgeFile file))
{
    uint32_t checksumSize;
    uint32_t bytesRead =
        receiveDataFromSocketIntoBuffer(sockfd, &checksumSize, sizeof(checksumSize));
    if (bytesRead != sizeof(checksumSize))
    {
        return 0;
    }
    uint8_t* checksumBuffer = (uint8_t*)malloc(checksumSize);
    bytesRead               = receiveDataFromSocketIntoBuffer(sockfd, checksumBuffer, checksumSize);
    if (bytesRead != checksumSize)
    {
        free(checksumBuffer);
        return 0;
    }
    uint32_t fileSize;
    bytesRead = receiveDataFromSocketIntoBuffer(sockfd, &fileSize, sizeof(fileSize));
    if (bytesRead != sizeof(fileSize))
    {
        free(checksumBuffer);
        return 0;
    }
    uint8_t* fileBuffer = (uint8_t*)malloc(fileSize);
    bytesRead           = receiveDataFromSocketIntoBuffer(sockfd, fileBuffer, fileSize);
    if (bytesRead != fileSize)
    {
        free(fileBuffer);
        free(checksumBuffer);
        return 0;
    }

    // Construct a temporary `struct pksmBridgeFile` pointing to the corresponding buffers to pass
    // to the validation function.
    struct pksmBridgeFile file = {.checksumSize = checksumSize,
        .checksum                               = checksumBuffer,
        .size                                   = fileSize,
        .contents                               = fileBuffer};
    if (!validateChecksumFunc(file))
    {
        return 0;
    }
    free(checksumBuffer);

    *outFile = fileBuffer;
    return fileSize;
}

bool checkSupportedPKSMBridgeProtocolVersionForTCP(int version)
{
    // This logic should be updated if newer protocol versions are introduced.
    // For now, there is only a single protocol version.
    return (version == 1);
}

enum pksmBridgeError sendFileOverPKSMBridgeViaTCP(
    uint16_t port, struct in_addr address, struct pksmBridgeFile file)
{
    int fd = createSocket();
    if (fd < 0)
    {
        return PKSM_BRIDGE_ERROR_CONNECTION_ERROR;
    }

    struct sockaddr_in servaddr = createSocketAddress(port, address.s_addr);
    if (connect(fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        close(fd);
        return PKSM_BRIDGE_ERROR_CONNECTION_ERROR;
    }

    // Expect a pksmBridgeRequest, which specifies the protocol version.
    struct pksmBridgeRequest request;
    int bytesRead = receiveDataFromSocketIntoBuffer(fd, &request, sizeof(request));
    if (bytesRead != sizeof(request))
    {
        close(fd);
        return PKSM_BRIDGE_DATA_READ_FAILURE;
    }
    if (!verifyPKSMBridgeHeader(request.protocol_name))
    {
        close(fd);
        return PKSM_BRIDGE_ERROR_UNEXPECTED_MESSAGE;
    }

    // Construct and send the pksmBridgeResponse.
    struct pksmBridgeResponse response =
        createPKSMBridgeResponseForRequest(request, &checkSupportedPKSMBridgeProtocolVersionForTCP);
    int sentBytes = sendDataFromBufferToSocket(fd, &response, sizeof(response));
    if (sentBytes != sizeof(response))
    {
        close(fd);
        return PKSM_BRIDGE_DATA_WRITE_FAILURE;
    }
    if (response.protocol_version == PKSM_BRIDGE_UNSUPPORTED_PROTOCOL_VERSION)
    {
        close(fd);
        return PKSM_BRIDGE_ERROR_UNSUPPORTED_PROTCOL_VERSION;
    }

    // Send the pksmBridgeFile, since the protocol version was confirmed.
    bool success = sendPKSMBridgeFileToSocket(fd, file);
    close(fd);
    return success ? PKSM_BRIDGE_ERROR_NONE : PKSM_BRIDGE_DATA_WRITE_FAILURE;
}

enum pksmBridgeError receiveFileOverPKSMBridgeViaTCP(uint16_t port, struct in_addr* outAddress,
    uint8_t** outFile, uint32_t* outFileSize,
    bool (*validateChecksumFunc)(struct pksmBridgeFile file))
{
    int fd = createSocket();
    if (fd < 0)
    {
        return PKSM_BRIDGE_ERROR_CONNECTION_ERROR;
    }

    struct sockaddr_in servaddr = createSocketAddress(port, INADDR_ANY);
    if (bind(fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        close(fd);
        return PKSM_BRIDGE_ERROR_CONNECTION_ERROR;
    }
    if (listen(fd, /*backlog=*/1) < 0)
    {
        close(fd);
        return PKSM_BRIDGE_ERROR_CONNECTION_ERROR;
    }

    int fdconn;
    int addrlen = sizeof(servaddr);
    if ((fdconn = accept(fd, (struct sockaddr*)&servaddr, (socklen_t*)&addrlen)) < 0)
    {
        close(fd);
        return PKSM_BRIDGE_ERROR_CONNECTION_ERROR;
    }
    close(fd);

    // Send a pksmBridgeRequest, requesting a specific protocol version.
    struct pksmBridgeRequest request = createPKSMBridgeRequest(PKSM_BRIDGE_LATEST_PROTOCOL_VERSION);
    uint32_t sentBytes = sendDataFromBufferToSocket(fdconn, &request, sizeof(request));
    if (sentBytes != sizeof(request))
    {
        close(fdconn);
        return PKSM_BRIDGE_DATA_WRITE_FAILURE;
    }

    // Expect a pksmBridgeResponse and see if the protocol version was supported.
    struct pksmBridgeResponse response;
    uint32_t readBytes = receiveDataFromSocketIntoBuffer(fdconn, &response, sizeof(response));
    if (readBytes != sizeof(response))
    {
        close(fdconn);
        return PKSM_BRIDGE_DATA_READ_FAILURE;
    }
    if (!verifyPKSMBridgeHeader(response.protocol_name))
    {
        close(fdconn);
        return PKSM_BRIDGE_ERROR_UNEXPECTED_MESSAGE;
    }
    if (response.protocol_version == PKSM_BRIDGE_UNSUPPORTED_PROTOCOL_VERSION)
    {
        close(fdconn);
        return PKSM_BRIDGE_ERROR_UNSUPPORTED_PROTCOL_VERSION;
    }

    // Expect a pksmBridgeFile since the protocol version was confirmed.
    uint8_t* fileBuffer = NULL;
    uint32_t fileSize =
        receiveFileOverPKSMBridgeFromSocket(fdconn, &fileBuffer, validateChecksumFunc);
    close(fdconn);

    if (fileSize == 0 || fileBuffer == NULL)
    {
        return PKSM_BRIDGE_DATA_FILE_CORRUPTED;
    }

    if (outAddress != NULL)
    {
        *outAddress = servaddr.sin_addr;
    }
    *outFileSize = fileSize;
    *outFile     = fileBuffer;
    return PKSM_BRIDGE_ERROR_NONE;
}