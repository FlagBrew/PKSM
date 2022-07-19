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

#ifndef PKSMBRIDGE_TCP_H
#define PKSMBRIDGE_TCP_H

#include "pksmbridge_api.h"
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * Returns a boolean indicating whether or not the `version` is supported by this PKSM Bridge
 * TCP implementation.
 */
bool checkSupportedPKSMBridgeProtocolVersionForTCP(int version);

/**
 * Send a file over the PKSM Bridge using TCP.
 *
 * @warning This is a blocking operation.
 *
 * @param port The TCP port to use for the PKSM Bridge connection.
 * @param address The IP address to use for the PKSM Bridge connection.
 * @param file The file to send over the PKSM Bridge connection.
 *
 * @return If the file was sent successfully, returns `PKSM_BRIDGE_ERROR_NONE`. If an error
 *         occurred, returns the corresponding `enum pksmBridgeError` error value.
 */
enum pksmBridgeError sendFileOverPKSMBridgeViaTCP(
    uint16_t port, struct in_addr address, struct pksmBridgeFile file);

/**
 * Receive a file over the PKSM Bridge using TCP.
 *
 * @warning This is a blocking operation.
 *
 * @note Callers should check for an error in the return value before attempting to access
 *       any of the out parameters. The out parameters are not assigned unless the operation
 *       succeeds.
 *
 * @param port The TCP port to use for the PKSM Bridge connection.
 * @param outAddress (out) Returns the IP address that was used for the PKSM Bridge connection.
 * @param outFile (out) The file that was received over the PKSM Bridge connection. The client
 *                is responsible for calling `free()` on the data returned via `outFile`.
 * @param outFileSize (out) The size of the `outFile` in bytes.
 * @param validateChecksumFunc A function pointer to the function that should be used to evaluate
 *                             the file's checksum. It takes one argument `file`, which is the
 *                             received file from the PKSM Bridge. It is expected to return a
 *                             boolean indicating whether or not the checksum in
 *                             `file.checksum` matches one computed from the `file.contents`.
 *
 * @return If the file was received successfully, returns `PKSM_BRIDGE_ERROR_NONE`. If an error
 *         occurred, returns the corresponding `enum pksmBridgeError` error value.
 */
enum pksmBridgeError receiveFileOverPKSMBridgeViaTCP(uint16_t port, struct in_addr* outAddress,
    uint8_t** outFile, uint32_t* outFileSize,
    bool (*validateChecksumFunc)(struct pksmBridgeFile file));

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PKSMBRIDGE_TCP_H