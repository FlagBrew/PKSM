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

/**
 * This file defines the PKSM Bridge protocol API.
 *
 * Example data flow:
 *
 *     Client -> Send `pksmBridgeRequest` to server and await acknowledgement of supported version
 *
 *     Server: Checks the protocol version from the request to see if it is supported.
 *
 *             <- Server send `pksmBridgeResponse`. If version is supported, the value matches
 *                one sent from the client. If unsupported, then the value is
 *                PKSM_BRIDGE_UNSUPPORTED_PROTOCOL_VERSION.
 *
 *     Client: Parses the `pksmBridgeResponse` to see if the server acknowledged and will send a
 *             file payload.
 *
 *                (if the version requested by the client is supported)
 *             <- Server sends the `pksmBridgeFile`
 *
 *     Client: Conditionally receives the file payload, if the version was supported.
 */
#ifndef PKSMBRIDGE_API_H
#define PKSMBRIDGE_API_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/** A string literal used to identify PKSM Bridge messages. */
static const char PKSM_BRIDGE_PROTOCOL_NAME[] = "PKSMBRIDGE";

/** The latest protocol version. */
static const int PKSM_BRIDGE_LATEST_PROTOCOL_VERSION = 1;

/** A sentinel value indicating an unsupported version number. */
extern const int PKSM_BRIDGE_UNSUPPORTED_PROTOCOL_VERSION;

struct pksmBridgeRequest
{
    /**
     * Used as a sentinel value to identify the request type.
     * Contains `PKSM_BRIDGE_PROTOCOL_NAME`, without the null terminator.
     */
    char protocol_name[10];

    /** The requested protocol version to use. */
    int8_t protocol_version;
};

struct pksmBridgeResponse
{
    /**
     * Used as a sentinel value to identify the request type.
     * Contains `PKSM_BRIDGE_PROTOCOL_NAME`, without the null terminator.
     */
    char protocol_name[10];

    /**
     * If the requested protocol_version is supported by the server, this value will
     * contain the same value to confirm support. If the requested version is not
     * supported, it will contain `PKSM_BRIDGE_UNSUPPORTED_PROTOCOL_VERSION`.
     */
    int8_t protocol_version;
};

struct pksmBridgeFile
{
    /** The size of the checksum. */
    uint32_t checksumSize;

    /**
     * The checksum of `contents`. Used to verify data integrity after the transmission.
     * The v1 protocol uses SHA256.
     */
    uint8_t* checksum;

    /** The size of the file in bytes. */
    uint32_t size;

    /** A pointer to the file contents. The size of these contents is specified in `size`. */
    uint8_t* contents;
};

enum pksmBridgeError
{
    /** Indicates that no error occurred (i.e. success). */
    PKSM_BRIDGE_ERROR_NONE = 0,
    /** The requested PKSM Bridge protocol version is not supported. */
    PKSM_BRIDGE_ERROR_UNSUPPORTED_PROTCOL_VERSION = -1,
    /** A connection error occurred and the PKSM Bridge could not be established. */
    PKSM_BRIDGE_ERROR_CONNECTION_ERROR = -2,
    /** The protocol encountered an error while reading data. */
    PKSM_BRIDGE_DATA_READ_FAILURE = -3,
    /** The protocol encountered an error while writing data. */
    PKSM_BRIDGE_DATA_WRITE_FAILURE = -4,
    /**
     * The file received from the PKSM Bridge contains unexpected bytes.
     * This likely indicates that a checksum failed.
     */
    PKSM_BRIDGE_DATA_FILE_CORRUPTED = -5,
    /**
     * A generic error indicating that an unexpected message was received that is not part
     * of the PKSM Bridge protocol.
     */
    PKSM_BRIDGE_ERROR_UNEXPECTED_MESSAGE = -99999,
};

/**
 * Create a PKSMBridge request with the specified protocol version.
 *
 * @param protocol_version The protocol version to specify in the request.
 *
 * @return The created bridge request.
 */
struct pksmBridgeRequest createPKSMBridgeRequest(int protocol_version);

/**
 * Create a PKSMBridge response corresponding to the given request.
 *
 * @param request The request that was received over the bridge.
 * @param supportedProtocolVersionsFunc A pointer to a function that validates the
 *                                      support protocol versions by the client
 *                                      implementing the PKSM Bridge protocol. It
 *                                      takes one argument `version`, which is the
 *                                      requested version number from the request.
 *                                      It s a boolean indicating whether or not
 *                                      the `version` is supported by this client.
 *
 * @return The created bridge response.
 */
struct pksmBridgeResponse createPKSMBridgeResponseForRequest(
    struct pksmBridgeRequest request, bool (*supportedProtocolVersionsFunc)(int version));

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PKSMBRIDGE_API_H