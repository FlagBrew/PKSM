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

#include "pksmbridge_api.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

const int PKSM_BRIDGE_UNSUPPORTED_PROTOCOL_VERSION = -1;

struct pksmBridgeRequest createPKSMBridgeRequest(int protocol_version)
{
    struct pksmBridgeRequest request;
    memcpy(request.protocol_name, PKSM_BRIDGE_PROTOCOL_NAME,
        sizeof(request.protocol_name) / sizeof(request.protocol_name[0]));
    request.protocol_version = protocol_version;
    return request;
}

struct pksmBridgeResponse createPKSMBridgeResponseForRequest(
    struct pksmBridgeRequest request, bool (*supportedProtocolVersionsFunc)(int version))
{
    struct pksmBridgeResponse response;
    memcpy(response.protocol_name, PKSM_BRIDGE_PROTOCOL_NAME,
        sizeof(response.protocol_name) / sizeof(response.protocol_name[0]));
    response.protocol_version = request.protocol_version;
    if (!supportedProtocolVersionsFunc(request.protocol_version))
    {
        response.protocol_version = PKSM_BRIDGE_UNSUPPORTED_PROTOCOL_VERSION;
    }
    return response;
}